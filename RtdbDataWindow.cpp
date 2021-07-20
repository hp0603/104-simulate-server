#include "RtdbDataWindow.h"
#include <QTextEdit>
#include <QGridLayout>
#include <QTableWidget> 
#include <QTimer>
#include "../iec/IEC104Slave.h"

RtdbDataWindow::RtdbDataWindow(QWidget* parent)
	:QWidget(parent)
{
	QGridLayout* grid = new QGridLayout;
	tablew = new QTableWidget(); 
	grid->addWidget(tablew);
	setLayout(grid);


	m_txt = "0";
	fillHeader(tablew);
	initdata(tablew);
	QTimer* timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(1000);
	
	
}


RtdbDataWindow::~RtdbDataWindow()
{
}

void RtdbDataWindow::update( )
{
	 

	//if (m_txt == "0")
	//{
	//	m_txt = "1";
	//}
	//else
	//{
	//	m_txt = "0";
	//}
	float fval = IEC104Slave::sfval;
	for (int row = 0; row < 1000; row++)
	{
		QTableWidgetItem* item2 = tablew->item(row, 2);
		item2->setText(m_txt);
	}
	m_tick++;

	m_yctxt = QString::number(fval);
	for (int row = 0; row < 0x1000; row++)
	{
	//int row = 0;
		QTableWidgetItem* item2 = tablew->item(1000 + row, 2);
		item2->setText(m_yctxt);
	}



}
void RtdbDataWindow::initdata(QTableWidget* tb)
{
	for (int row=0;row<1000;row++)
	{
		int rowCount = tb->rowCount();
		tb->insertRow(rowCount);
		QTableWidgetItem* item0 = new QTableWidgetItem(QString::fromLocal8Bit("遥信")) ;
		QTableWidgetItem* item1 = new QTableWidgetItem(QString::number(row));
		QTableWidgetItem* item2 = new QTableWidgetItem("1"); 
		tb->setItem(row, 0, item0); 
		tb->setItem(row, 1, item1);
		tb->setItem(row, 2, item2);
	}

	for (int row = 1000; row < 1000+0x1000; row++)
	{
		int rownum = 0x4001 + row;
		int rowCount = tb->rowCount();
		tb->insertRow(rowCount);
		QTableWidgetItem* item0 = new QTableWidgetItem(QString::fromLocal8Bit("遥测"));
		QTableWidgetItem* item1 = new QTableWidgetItem(QString::number(rownum));
		QTableWidgetItem* item2 = new QTableWidgetItem("1");
		tb->setItem(row, 0, item0);
		tb->setItem(row, 1, item1);
		tb->setItem(row, 2, item2);
	}


}

void RtdbDataWindow::fillHeader(QTableWidget* fillheader)
{
	fillheader->setColumnCount(3);
	QStringList headerString;
	headerString << QString::fromLocal8Bit("类型")
		<< QString::fromLocal8Bit("信息体地址")
		<< QString::fromLocal8Bit("数值");
	fillheader->setHorizontalHeaderLabels(headerString);
}