#pragma once

#include "Database.h"
#include <QWidget>

class QTableView;
class QStandardItemModel;
class QAction;
class QContextMenuEvent;
class QStandardItem;

class IECWindow : public QWidget
{
	Q_OBJECT

public slots:
	void read();
	void updateTable();
	void add();
	void remove();
	void clone();
	void itemChanged(QStandardItem*);
public:
	Database * database;
	IECWindow(Database* db);

private:
	QTableView* table;
	QStandardItemModel* model;
	QVector<IECData> data;
	QAction* addAct;
	QAction* removeAct;
	QAction* cloneAct;

	void contextMenuEvent(QContextMenuEvent* event);
};
