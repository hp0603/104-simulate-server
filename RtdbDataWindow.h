#pragma once

#include <QWidget>
#include <QString>
#include <QByteArray>

class QTableWidget;
class IEC104Slave;
class RtdbDataWindow : public QWidget
{
	Q_OBJECT

public slots:
	void update();

public:
	RtdbDataWindow(QWidget* parent=0);
	virtual ~RtdbDataWindow();
	void fillHeader(QTableWidget* fillheader);
	void initdata(QTableWidget* fillheader);




private:
	QTableWidget* tablew;
	QString m_txt;
	QString m_yctxt;
	int m_tick=0;
};

