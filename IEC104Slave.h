#pragma once

#include "../iec/APDU.h"
#include <QObject>
#include <QVector>
#include <QMutex>

class QTcpServer;
class QTcpSocket;

struct IEC104Setup
{
	int commonaddress;
	int port;
};

struct IECValue
{
	int id;
	int type; // ASDU type
	union
	{
		float fVal;
		int		nVal;
	};
};

class IEC104Slave : public QObject
{
	Q_OBJECT

public:
	explicit IEC104Slave(int index, int port, QObject *parent = 0);
	virtual ~IEC104Slave();
	void start();
	void stop();

signals:
	void iecMessage(int index, const QByteArray&, bool);
	void iecMessageQ(int index, const QString&, bool);
	void iecCommand(const APDU&);
public slots:
	void newConnection();
	void readyRead();
	void connected();
	void update(); 
	void Tufa();

	public:
	static float sfval;
	static bool bval;
private:
	int index;
	IEC104Setup setup;
	QTcpServer * tcpServer;
	QTcpSocket * tcpSocket;
	unsigned int masterSN; //Number of sequences from master
	unsigned int slaveSN; // Next sequencenumber for message to master
	QVector<IECValue> stored;
	QList<APDU> spoolQue;
	bool OkToSend;

	void write(APDU&, bool checkspool = true);
	void sendInterrogation();
	void setSpi(int ident, int addr, int spi);
	void setDpi(int ident, int addr, int dpi);
	void setNva(int ident, int addr, int nva);
	void setVal(int ident, int addr, float val);
	void spool();
	void spool(APDU&apdu); 
	void initTestData();
	QMutex mutex;
	int dwTicket;
};
