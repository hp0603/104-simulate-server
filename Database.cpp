#include "Database.h"
#include <QCoreApplication>
//#include <QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QVariant>
#include <QDebug>

const char* DBVERSION = "1.0";
const char* DBTYPE = "IEC104DB";


Database::Database(QObject *parent)
	: QObject(parent)
{
	if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
	{
		qWarning("No SQLITE driver available.");
		return;
	}
	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation,"/IEC104Server", QStandardPaths::LocateDirectory);
	if (path.isEmpty())
	{
		QStringList l = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
		if (l.size())
		{
			path = l.front();
			path += "/IEC104Server";
			QDir().mkdir(path);
		}

	}
		
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(path+ "/IEC104Server.data");
	path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "/IEC104Server/IEC104Server.data", QStandardPaths::LocateFile);
	if (path.isEmpty())
		create();
}

Database::~Database()
{
}

bool Database::create()
{
	if (!db.open())
		return false;

	QSqlQuery query(db);
	query.exec("CREATE TABLE info ( type TEXT, version TEXT, PRIMARY KEY;");
	query.prepare("INSERT INTO info (type, version) VALUES ( :type, :version);");
	query.bindValue(":type", DBTYPE);
	query.bindValue(":version", DBVERSION);
	query.exec();

	query.exec("CREATE TABLE iec ( "
		"id INTEGER,"
		"address INTEGER,"
		"type INTEGER,"
		"snmpid INTEGER,"
		"description TEXT,"
		"PRIMARY KEY(id)); ");

	query.exec("CREATE TABLE stations ( "
		"id INTEGER,"
		"station TEXT,"
		"addresses BLOB,"
		"description TEXT,"
		"PRIMARY KEY(id));");

	query.exec("CREATE TABLE snmp ( "
		"id INTEGER,"
		"station TEXT,"
		"object TEXT,"
		"trap TEXT,"
		"iecid INTEGER,"
		"poll INTEGER,"
		"description TEXT,"
		"PRIMARY KEY(id));");

	db.close();
	return true;
}

int Database::add(IECData& data, bool isOpen)
{
	if (!isOpen && !db.open())
		return 0;
	QSqlQuery query(db);
	if (data.id != 0)
	{
		query.prepare("UPDATE iec SET "
			"address = :address,"
			"type = :type,"
			"snmpid = :snmpid,"
			"description = :description "
			"WHERE id = :id;");
		query.bindValue(":id", data.id);
	}
	else
	{
		query.prepare("INSERT INTO iec (address, type, snmpid, description) VALUES ( :address, :type, :snmpid, :description);");
	}
	query.bindValue(":address", data.address);
	query.bindValue(":type", data.type);
	query.bindValue(":snmpid", data.snmpid);
	query.bindValue(":description", data.description);
	query.exec();
	data.id= query.lastInsertId().toInt();
	if (!isOpen)
		db.close();
	return data.id;
}

bool Database::add(QVector<IECData>& data)
{
	if (!db.open())
		return false;
	QVector<IECData>::iterator it = data.begin();
	while (it != data.end())
	{
		add(*it, true);
		++it;
	}
	db.close();
	return true;
}

void Database::remove(IECData& data)
{
	if (!db.open())
		return;
	QSqlQuery query(db);
	query.prepare("DELETE FROM iec WHERE id = :id;");
	query.bindValue(":id", data.id);
	query.exec();
	db.close();
}

bool Database::read(QVector<IECData>& data)
{
	data.clear();
	if (!db.open())
		return false;
	data.clear();
	IECData iec;
	QSqlQuery query(db);
	query.exec("SELECT * FROM iec;");
	while (query.next())
	{
		iec.id = query.value("id").toInt();
		iec.address = query.value("address").toInt();
		iec.type = query.value("type").toInt();
		iec.snmpid = query.value("snmpid").toInt();
		iec.description = query.value("description").toString();
		data.push_back(iec);
	}
	db.close();
	return true;
}
