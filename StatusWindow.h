#pragma once

#include <QWidget>
#include <QString>
#include <QByteArray>

class QTextEdit;

class StatusWindow : public QWidget
{
	Q_OBJECT

public slots:
	void iecMessage(int i, const QByteArray&, bool);
	void iecMessageQ(int i, const QString&, bool);

public:
	StatusWindow(QWidget* parent=0);
	virtual ~StatusWindow();
private:
	QTextEdit* iecLog;
};

