#pragma once

#include <QTranslator>
#include <QMainWindow>

class QMenu;
class QAction;
class QEvent;
class QCloseEvent;
class QActionGroup;
class QTabWidget;
class StatusWindow;
//class IECWindow;
class SNMPWindow;
class IEC104Slave;
//class Database;
class RtdbDataWindow;

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	QMenu * fileMenu;
	QMenu* settingsMenu;
	QMenu* helpMenu;
	QMenu* langMenu;
	QToolBar* toolbar;
	QAction* exitAct;
	QAction* engAct;
	QAction* norAct;
	QAction* aboutAct;
	QTranslator translator;
	QActionGroup * langGroup;
	QString locale;
	QTabWidget* tabbar;
	StatusWindow* statuswindow;
	//IECWindow* iecwindow;
	SNMPWindow* snmpwindow;
	RtdbDataWindow* m_RtdbDataWindow;
	//IEC104Slave* iec104;
	IEC104Slave* iec104[10];
	void closeEvent(QCloseEvent* event) override;
	void createMenu();
	void loadLanguage();
	void retranslateUi();
	void readSettings();
	void writeSettings();

protected:
	void changeEvent(QEvent* event);

protected slots:
	void slotLanguageChanged(QAction* action);
	void onabout();
public:
	//Database * database;

	MainWindow();
	virtual ~MainWindow();
};