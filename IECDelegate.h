#pragma once

#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QMap>
#include <QString>

class QObject;
class QWidget;
class QAbstractItemModel;

extern int asduInt(QString& s);
extern QString asduString(int n);

class IECAsduDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	IECAsduDelegate(QObject *parent=0);
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
