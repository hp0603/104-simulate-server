#include "IECDelegate.h"
#include <QWidget>
#include <QComboBox>
#include <QAbstractItemModel>

QMap<int, QString> asduType;
static bool  asduIsinit = false;

void asduInit()
{
	if (asduIsinit)
		return;
	asduType[0] = "";
	asduType[1] = "M_SP_NA_1 (1)";
	asduType[3] = "M_DP_NA_1 (3)";
	asduType[9] = "M_ME_NA_1 (9)";
	asduType[13] = "M_ME_NC_1 (13)";
	asduType[30] = "M_SP_TB_1 (30)";
	asduType[31] = "M_DP_TB_1 (31)";
	asduType[45] = "C_SC_NA_1 (45)";
	asduType[46] = "C_DC_NA_1 (46)";
	asduType[58] = "C_SC_TA_1 (58)";
	asduType[59] = "C_DC_TA_1 (59)";
	asduIsinit = true;
};

int asduInt(QString& s)
{
	asduInit();
	QMap<int, QString>::const_iterator it = asduType.begin();
	while (it != asduType.end())
	{
		if (it.value() == s)
			return it.key();
		++it;
	}
	return 0;
};

QString asduString(int n)
{
	asduInit();
	QMap<int, QString>::const_iterator it = asduType.begin();
	while (it != asduType.end())
	{
		if (it.key() == n)
			return it.value();
		++it;
	}
	return QString();
};

IECAsduDelegate::IECAsduDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
}

QWidget* IECAsduDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QComboBox* editor = new QComboBox(parent);
	editor->setFrame(false);
	//editor->setMinimumWidth(200);
	//editor->setMaximumWidth(100);

	QMap<int, QString>::const_iterator it=asduType.begin();
	while (it != asduType.end())
	{
		editor->addItem(it.value());
		++it;
	}
	return editor;
}

void IECAsduDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QString val = index.model()->data(index, Qt::EditRole).toString();
	QComboBox* combo = (QComboBox*)editor;
	combo->setCurrentText(val);
}

void IECAsduDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QComboBox* combo = (QComboBox*)editor;
	model->setData(index,combo->currentText(),Qt::EditRole);
	int row = index.row();
	int col = index.column();
}

void IECAsduDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}
