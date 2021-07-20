#include "IECWindow.h"
#include "../IECDelegate.h"
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QBoxLayout>
#include <QStringList>
#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QComboBox>
#include <QDebug>
#include <QStandardItem>
#include <QHeaderView>

IECWindow::IECWindow(Database* db)
	:QWidget()
{
	database = db;
	QVBoxLayout* vbox = new QVBoxLayout;
	model = new QStandardItemModel(0,5);
	QStringList header;
	header << "Id" << "Address" << "ASDU"  ;
	model->setHorizontalHeaderLabels(header);
	table = new QTableView;
	table->setModel(model);
	table->setColumnWidth(2, 200);
//	table->hideColumn(0);
	
	IECAsduDelegate* asduDelegate = new IECAsduDelegate;
	table->setItemDelegateForColumn(2, asduDelegate);

	read();

	vbox->addWidget(table);
	setLayout(vbox);

	addAct = new QAction(tr("Add"), this);
	removeAct = new QAction(tr("Delete"), this);
	cloneAct = new QAction(tr("Clone"), this);

	connect(addAct, &QAction::triggered, this, &IECWindow::add);
	connect(removeAct, &QAction::triggered, this, &IECWindow::remove);
	connect(cloneAct, &QAction::triggered, this, &IECWindow::clone);
	connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)));
}

void IECWindow::read()
{
	database->read(data);
	updateTable();
}

void IECWindow::updateTable()
{
	QStandardItem* item;
	int col, row;
	if (!data.size())
		return;

	model->setRowCount(data.size());
	QVector<IECData>::iterator it = data.begin();
	row = 0;
	while (it != data.end())
	{
		col = 0;
		item = new QStandardItem;
		item->setData(it->id, Qt::EditRole);
		model->setItem(row, col++, item);
		item = new QStandardItem;
		item->setData(it->address,Qt::EditRole);
		model->setItem(row, col++, item);
		item = new QStandardItem;
		item->setData(asduString(it->type), Qt::EditRole);
		model->setItem(row, col++, item);

		item = new QStandardItem;
		item->setData(it->snmpid, Qt::EditRole);  //editrole
		model->setItem(row, col++, item);
		item = new QStandardItem;
		item->setData(it->description, Qt::EditRole);
		model->setItem(row++, col, item);
		++it;
	}
/*
	QTableWidgetItem* item;
	QComboBox* citem;
	QStringList id;
	int row, col;
	table->clear();
	item = new QTableWidgetItem("Address");
	table->setHorizontalHeaderItem(0, item);
	item = new QTableWidgetItem("Type");
	table->setHorizontalHeaderItem(1, item);
	item = new QTableWidgetItem("SNMP id");
	table->setHorizontalHeaderItem(2, item);
	item = new QTableWidgetItem("Description");
	table->setHorizontalHeaderItem(3, item);
	table->setRowCount(data.size());
	QVector<IECData>::iterator it = data.begin();

	table->setVerticalHeaderLabels(id);
	table->scrollToBottom();
	*/
}

void IECWindow::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu* menu = new QMenu(this);
	menu->addAction(addAct);
	menu->addAction(removeAct);
	menu->addAction(cloneAct);
	bool disabled = false;
	if (table->currentIndex().row() < 0)
		disabled = true;
	removeAct->setDisabled(disabled);
	cloneAct->setDisabled(disabled);
	menu->exec(event->globalPos());
}

void IECWindow::add()
{
	disconnect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)));
	IECData iec;
	iec.id = 0;
	iec.address = 0;
	iec.snmpid = 0;
	iec.type = 0;
	iec.id = database->add(iec);
	
	data.push_back(iec);

	int rows = model->rowCount();
	model->setRowCount(rows + 1);

	QStandardItem* item;
	int col = 0;
	item = new QStandardItem;
	item->setData(iec.id, Qt::EditRole);
	model->setItem(rows, col++, item);
	item = new QStandardItem;
	item->setData(iec.address, Qt::EditRole);
	model->setItem(rows, col++, item);
	item = new QStandardItem;
	item->setData(asduString(iec.type), Qt::EditRole);
	model->setItem(rows, col++, item);
	item = new QStandardItem;
	item->setData(iec.snmpid, Qt::EditRole);
	model->setItem(rows, col++, item);
	item = new QStandardItem;
	item->setData(iec.description, Qt::EditRole);
	model->setItem(rows, col, item);

	connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)));
}

void IECWindow::remove()
{
	int row = table->currentIndex().row();
	if (row < 0)
		return;
	int id = model->item(row, 0)->data(Qt::EditRole).toInt();
	
	for (int it = 0; it < data.size(); it++)
	{
		if (data[it].id == id)
		{
			data.remove(it);
			continue;
		}
	}
	IECData iec;
	iec.id = id;
	model->removeRow(row);
	database->remove(iec);
}

void IECWindow::clone()
{

}

void IECWindow::itemChanged(QStandardItem* item)
{
	qDebug() << "itemChanged ( " << item->row() << "," << item->column() << ")";
	IECData iec;
	iec.id = model->item(item->row(), 0)->data(Qt::EditRole).toInt();
	iec.address = model->item(item->row(), 1)->data(Qt::EditRole).toInt();
	iec.type = asduInt(model->item(item->row(), 2)->data(Qt::EditRole).toString());
	iec.snmpid = model->item(item->row(), 3)->data(Qt::EditRole).toInt();
	iec.description = model->item(item->row(), 4)->data(Qt::EditRole).toString();
	database->add(iec);
	for (int it = 0; it < data.size(); it++)
	{
		if (data[it].id == iec.id)
		{
			data.replace(it,iec);
			continue;
		}
	}
}

