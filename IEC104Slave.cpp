#include "IEC104Slave.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QDebug>
#include <QTimer>

//static float sfval;
IEC104Slave::IEC104Slave(int index, int port, QObject *parent)
	: QObject(parent)
{
	tcpSocket = nullptr;
	dwTicket = 0;
	this->index = index;
	setup.port = port;
	//setup.port = 2404;
	setup.commonaddress = 1;
	masterSN = slaveSN = 0;
	OkToSend = false;
	tcpServer = new QTcpServer(this);
	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
	//start();
	initTestData();
	QTimer* timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(3000);
}

IEC104Slave::~IEC104Slave()
{
	APDU apdu;
	apdu.apci.format = U_FORMAT;
	apdu.apci.func = STOPDTACT;
	write(apdu);
	OkToSend = false;
	tcpSocket->close();
}

void IEC104Slave::newConnection()
{
	qDebug() << "IEC104Slave newconnection.";
	tcpSocket = tcpServer->nextPendingConnection();
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(tcpSocket, SIGNAL(connected()), this, SLOT(connected()));

}

void IEC104Slave::start()
{
	if (!tcpServer->listen(QHostAddress::Any,setup.port))
	{
		qDebug() << "IEC104Slave could not start.";	
		QString ba("IEC104Slave could not start.");
		emit iecMessageQ(index, ba, false);//red
	}
	else
	{
		qDebug() << "IEC104Slave started.";
		QString ba("IEC104Slave started.");
		emit iecMessageQ(index, ba, true);//blue
	}

}

void IEC104Slave::stop()
{
	tcpSocket->close();
}

void IEC104Slave::readyRead()
{
	QByteArray ba;
	ba=tcpSocket->readAll();
	emit iecMessage(index, ba,false);

	APDU apdu,res;
	InformationObject inf;
	apdu.set((BYTE*)ba.data());
	if (!apdu.valid())
		return;
	switch (apdu.apci.format)
	{
	case I_FORMAT:
		masterSN++;
		switch (apdu.asdu.dui.ident)
		{
		case C_SC_NA_1: // YK
		case C_DC_NA_1: //  
		case C_RC_NA_1: //  
		case C_SC_TA_1: //  
		case C_DC_TA_1: //  
		case C_RC_TA_1: //  
			if (apdu.asdu.io.size() != 1)
				break; 
			emit iecCommand(apdu);
			res.setAPCI(I_FORMAT);
			res.setDUI(setup.commonaddress, apdu.asdu.dui.ident, COT_ACTIVATIONCONFIRM);
			inf = apdu.asdu.io[0];
			res.addIO(inf);
			write(res);
			res.clear();
			res.setAPCI(I_FORMAT);
			res.setDUI(setup.commonaddress, apdu.asdu.dui.ident, COT_ACTIVATIONTERMINATION);
			inf = apdu.asdu.io[0];
			res.addIO(inf);
			write(res);
			break;
		case C_IC_NA_1: // Interrogation
			res.setAPCI(I_FORMAT);
			res.setDUI(setup.commonaddress, C_IC_NA_1, COT_ACTIVATIONCONFIRM);
			inf.address = 0;
			inf.qoi = QOI_STATION_INTERROGATION;
			res.addIO(inf);
			write(res);//确认
			sendInterrogation();//组
			res.clear();
			res.setAPCI(I_FORMAT);
			res.setDUI(setup.commonaddress, C_IC_NA_1, COT_ACTIVATIONTERMINATION);
			inf.clear();
			inf.address = 0;
			inf.qoi = QOI_STATION_INTERROGATION;
			res.addIO(inf);
			write(res);//终止
			break;
		case C_CI_NA_1: // dd
			break;
		case C_CS_NA_1: // 时钟同步.
			res.setAPCI(I_FORMAT);
			res.setDUI(setup.commonaddress, C_CS_NA_1, COT_ACTIVATIONCONFIRM);
			inf.address = 0;
			inf.cp56time = currentTime();
			res.addIO(inf);
			write(res);
			break;
		}
		break;
	case S_FORMAT:
		break;
	case U_FORMAT:
		res.apci.format = U_FORMAT;
		switch (apdu.apci.func)
		{
		case STARTDTACT:
			res.apci.func = STARTDTCON;
			OkToSend = true;
			write(res, false);
			spool();
			break;
		case STOPDTACT:
			res.apci.func = STOPDTCON;
			write(res,false);
			OkToSend = false;
			break;
		case TESTFRACT:
			res.apci.func = TESTFRCON;
			write(res);
			break;
		}
		break;
	}

}

void IEC104Slave::connected()
{
	qDebug() << "IEC104Naster connected.";
}

void IEC104Slave::write(APDU& apdu, bool checkspool)
{
	BYTE data[255];
	int len;
	if (checkspool && !OkToSend)
	{
		spool(apdu);
		return;
	}
	if (apdu.apci.format == I_FORMAT)
	{
		apdu.apci.ssn = slaveSN++;
		apdu.apci.rsn = masterSN;
		if (slaveSN > 32767)
			slaveSN = 0;
	}
	len = apdu.get(data);
	QByteArray ba;
	if (len)
	{
		ba.setRawData((char*)data, len);
		emit iecMessage(index, ba, true);
		tcpSocket->write(ba);
		//Sleep(30); // Waiting for 30 msec so the messages don't get too close. Special response to IC?
	}
}
float IEC104Slave::sfval = 9;
void IEC104Slave::initTestData()
{
	IECValue v;
	int addr = 0x4001;
	float val = 1.00;
	//全局变量 
	sfval = val;
	//YC
	for (int i=0;i<0x1000;i++)
	{
		v.id = addr+i;
		v.type = M_ME_NC_1;
		v.fVal = val;
		stored.push_back(v);
	}
	//YX
	//addr = 0x1;
	//int nval = 1;
	//for (int i = 0; i < 1000; i++)
	//{
	//	v.id = addr + i;
	//	v.type = M_SP_NA_1;
	//	v.nVal = nval;
	//	stored.push_back(v);
	//}

	QVector<IECValue>::iterator it; 
	qDebug() << "==============";
	for (it = stored.begin(); it != stored.end(); it++)
	{
		IECValue& v = *it;
		qDebug() << v.type;
	}

}

void IEC104Slave::Tufa()
{
	static int vspi = 1;
	for (int i=0;i<100;i++)
	{
		int addr = 0x01+i;
		int spi = vspi;
		//message
		APDU apdu;
		ASDU asdu;
		InformationObject inf;
		apdu.setAPCI(I_FORMAT);
		apdu.setDUI(setup.commonaddress, M_SP_NA_1, COT_SPONTANEOUS);
		inf.address = addr;
		inf.siq.SPI = spi;
		inf.cp56time = currentTime();
		apdu.addIO(inf);

		// Send message
		write(apdu);

	}
	if (vspi == 1)
		vspi = 0;
	else
		vspi = 1;

}
void IEC104Slave::update()
{
	return;

	dwTicket++;
	mutex.lock();
	QVector<IECValue>::iterator it;
	for (it = stored.begin(); it != stored.end(); it++)
	{
		IECValue& v = *it;
		v.fVal = 1 + dwTicket;
	}
	mutex.unlock();
	if (tcpSocket != NULL)  //多个链接，怎么释放；；1  13  为什么不按照顺序发送？
	{
		if (tcpSocket->state() == QAbstractSocket::ConnectedState)
		{
			Tufa();
		}
	}
}
void IEC104Slave::sendInterrogation()
{ 
	mutex.lock();
	int n;
	APDU apdu;
	InformationObject inf;

	// simple message

	QVector<IECValue>::iterator it= stored.begin();
	n = 0;
	while (it != stored.end())
	{
		if (it->type == M_SP_NA_1)
		{
			//qDebug() << "111111111111111";
			if (n == 0)
			{
				apdu.clear();
				apdu.setAPCI(I_FORMAT);
				apdu.setDUI(setup.commonaddress, M_SP_NA_1, COT_INTERROGATION);
			}
			inf.clear();
			inf.address = it->id;
			inf.siq.SPI = it->nVal;
			//翻转
			it->nVal = it->nVal == 1?0 : 1; 

			apdu.addIO(inf);
			n++;
			if (n == 10)
			{
				write(apdu);
				n = 0;
			}
		}
		++it;
	}
	if ((n>0) && (n<10))
		write(apdu);

	// Send doble meldinger  //double message
	it = stored.begin();
	n = 0;
	while (it != stored.end())
	{
		if (it->type == M_DP_NA_1)
		{
			if (n == 0)
			{
				apdu.clear();
				apdu.setAPCI(I_FORMAT);
				apdu.setDUI(setup.commonaddress, M_DP_NA_1, COT_INTERROGATION);
			}
			inf.clear();
			inf.address = it->id;
			inf.diq.DPI = it->nVal;
			apdu.addIO(inf);
			n++;
			if (n == 10)
			{
				write(apdu);
				n = 0;
			}
		}
		++it;
	}
	if ((n>0) && (n<10))
		write(apdu);

	// Send normalised minger
	it = stored.begin();
	n = 0;
	while (it != stored.end())
	{
		if (it->type == M_ME_NA_1)
		{
			if (n == 0)
			{
				apdu.clear();
				apdu.setAPCI(I_FORMAT);
				apdu.setDUI(setup.commonaddress, M_ME_NA_1, COT_INTERROGATION);
			}
			inf.clear();
			inf.address = it->id;
			inf.nva = it->nVal;
			apdu.addIO(inf);
			n++;
			if (n == 10)
			{
				write(apdu);
				n = 0;
			}
		}
		++it;
	}
	if ((n>0) && (n<10))
		write(apdu);

	// Send floating point minger
	it = stored.begin();
	n = 0;
	while (it != stored.end())
	{
		if (it->type == M_ME_NC_1)
		{
			//qDebug() << "YC";
			if (n == 0)
			{
				apdu.clear();
				apdu.setAPCI(I_FORMAT);
				apdu.setDUI(setup.commonaddress, M_ME_NC_1, COT_INTERROGATION);
			}
			inf.clear();
			inf.address = it->id;
			inf.value = it->fVal;
			//翻转+1
			it->fVal = it->fVal + 1.00;
			//全局变量
			sfval = it->fVal;  

			apdu.addIO(inf);
			n++;
			if (n == 10)
			{
				write(apdu);
				n = 0;
			}
		}
		++it;
	}
	if ((n>0) && (n<10))
		write(apdu);
	mutex.unlock();
}

void IEC104Slave::setVal(int ident, int addr, float val)
{
	QVector<IECValue>::iterator sit;
	QList<APDU>::iterator it;
	APDU apdu;
	ASDU asdu;
	InformationObject inf;

	// Lagre verdien
	sit = stored.begin();
	while (sit != stored.end())
	{
		if (sit->id == addr)
		{
			if (sit->fVal == val)
				return;
			sit->fVal = val;
			break;
		}
		++sit;
	}
	if (sit == stored.end())
	{
		IECValue v;
		v.id = addr;
		v.type = M_ME_NC_1;
		v.fVal = val;
		stored.push_back(v);
	}

	//  Make the message.
	apdu.setAPCI(I_FORMAT);
	apdu.setDUI(setup.commonaddress, ident, COT_SPONTANEOUS);
	inf.address = addr;
	inf.value = val;
	inf.cp56time = currentTime();
	apdu.addIO(inf);

	// Check if this message is in the spoolQue
	it = spoolQue.begin();
	while (it != spoolQue.end())
	{
		if ((it->asdu.dui.ident == ident) && (it->asdu.dui.qualifier.Number == 1))
		{
			if (it->asdu.io[0].address == addr)
			{
				spoolQue.erase(it);
				break;
			}
		}
		++it;
	}
	// Send message
	write(apdu);
}

void IEC104Slave::setNva(int ident, int addr, int nva)
{
	QVector<IECValue>::iterator sit;
	QList<APDU>::iterator it;
	APDU apdu;
	ASDU asdu;
	InformationObject inf;

	// Lagre verdien
	sit = stored.begin();
	while (sit != stored.end())
	{
		if (sit->id == addr)
		{
			if (sit->nVal == nva)
				return;
			sit->nVal = nva;
			break;
		}
		++sit;
	}
	if (sit == stored.end())
	{
		IECValue v;
		v.id = addr;
		v.type = M_ME_NA_1;
		v.nVal = nva;
		stored.push_back(v);
	}

	// 
	apdu.setAPCI(I_FORMAT);
	apdu.setDUI(setup.commonaddress, ident, COT_SPONTANEOUS);
	inf.address = addr;
	inf.nva = nva;
	inf.cp56time = currentTime();
	apdu.addIO(inf);

	// Check if this message is in the spoolQue
	it = spoolQue.begin();
	while (it != spoolQue.end())
	{
		if ((it->asdu.dui.ident == ident) && (it->asdu.dui.qualifier.Number == 1))
		{
			if (it->asdu.io[0].address == addr)
			{
				spoolQue.erase(it);
				break;
			}
		}
		++it;
	}

	// Send message
	write(apdu);
}

void IEC104Slave::setSpi(int ident, int addr, int spi)
{
	bool exist = false;
	QVector<IECValue>::iterator sit;
	APDU apdu;
	ASDU asdu;
	InformationObject inf;

	//  
	sit = stored.begin();
	while (sit != stored.end())
	{
		if (sit->id == addr)
		{
			if (sit->nVal == spi)
				return;
			sit->nVal = spi;
			break;
		}
		++sit;
	}
	if (sit == stored.end())
	{
		IECValue v;
		v.id = addr;
		v.type = M_SP_NA_1;
		v.nVal = spi;
		stored.push_back(v);
	}

	//message
	apdu.setAPCI(I_FORMAT);
	apdu.setDUI(setup.commonaddress, ident, COT_SPONTANEOUS);
	inf.address = addr;
	inf.siq.SPI = spi;
	inf.cp56time = currentTime();
	apdu.addIO(inf);

	// Send message
	write(apdu);
}

void IEC104Slave::setDpi(int ident, int addr, int dpi)
{
	bool exist = false;
	QVector<IECValue>::iterator sit;
	APDU apdu;
	ASDU asdu;
	InformationObject inf;

	// Lagre verdien
	sit = stored.begin();
	while (sit != stored.end())
	{
		if (sit->id == addr)
		{
			if (sit->nVal == dpi)
				return;
			sit->nVal = dpi;
			break;
		}
		++sit;
	}
	if (sit == stored.end())
	{
		IECValue v;
		v.id = addr;
		v.type = M_DP_NA_1;
		v.nVal = dpi;
		stored.push_back(v);
	}
	 
	apdu.setAPCI(I_FORMAT);
	apdu.setDUI(setup.commonaddress, ident, COT_SPONTANEOUS);
	inf.address = addr;
	inf.diq.DPI = dpi;
	inf.cp56time = currentTime();
	apdu.addIO(inf);
	 
	write(apdu);
}

void IEC104Slave::spool()
{
	if (!OkToSend)
		return;

	QList<APDU>::iterator it = spoolQue.begin();
	while (it != spoolQue.end())
	{
		write(*it);
		++it;
	}
	spoolQue.clear();
}

void IEC104Slave::spool(APDU& apdu)
{
	spoolQue.push_back(apdu);
}
