#pragma once
#include <QObject>
//#include <QSqlDatabase>
#include <QString>
#include <QVector>
 
struct IECData
{
	int id;
	int address;
	int type;
	int snmpid;
	QString description;
};

class Database : public QObject
{
	Q_OBJECT

private:
	//QSqlDatabase db;
signals:
	void databaseMessage(const QString& msg);

public:
	Database(QObject *parent=0);
	~Database();
	bool create();
	int add(IECData& data, bool isOpen=false);
	bool add(QVector<IECData>& data);
	bool read(QVector<IECData>& data);
	void remove(IECData& data);
};
