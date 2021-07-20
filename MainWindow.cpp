#include <QMainWindow>
#include <QtWidgets>
#include <QMenu>
#include <QAction>
#include <QEvent>
#include <QCloseEvent>
#include <QActionGroup>
#include <QTabWidget>
#include <QByteArray>
#include "MainWindow.h"
#include "StatusWindow.h"
#include "IECWindow.h"
#include "Database.h"
#include "../iec/IEC104Slave.h"
#include "RtdbDataWindow.h"

MainWindow::MainWindow()
{
	readSettings();
	createMenu();
	statusBar();
	//loadLanguage();

	//database = new Database;

	tabbar = new QTabWidget();
	//iecwindow = new IECWindow(database);
	statuswindow = new StatusWindow();
	m_RtdbDataWindow = new RtdbDataWindow();
	//snmpwindow = new SNMPWindow(database);

	//iec104 = new IEC104Slave();
	for (int i=0;i<10;i++)
	{
		//delete[] iec104
		iec104[i] = new IEC104Slave(i+1, 2404+i);
	}

	//tabbar->addTab(iecwindow, "104_servr");
	tabbar->addTab(m_RtdbDataWindow, "View Data");
	tabbar->addTab(statuswindow, "view Hex"); 
	
	setCentralWidget(tabbar);
	retranslateUi();
	//connect(iec104, SIGNAL(iecMessage(const QByteArray&, bool)), statuswindow, SLOT(iecMessage(const QByteArray&, bool)));
	for (int i=0;i<10;i++)
	{
		connect(iec104[i], SIGNAL(iecMessageQ(int ,const QString&, bool)), statuswindow, SLOT(iecMessageQ(int ,const QString&, bool)));
		connect(iec104[i], SIGNAL(iecMessage(int ,const QByteArray&, bool)), statuswindow, SLOT(iecMessage(int ,const QByteArray&, bool)));
		iec104[i]->start();
	}
	//connect(iec104[i], SIGNAL(iecMessage(const QByteArray&, bool)), statuswindow, SLOT(iecMessage(const QByteArray&, bool)));

}

MainWindow::~MainWindow()
{
	//delete database;
}

// The text in the menu are set in retranslateUi to be able to switch language 'on the fly'.
void MainWindow::createMenu()
{
	//Main menues

	// File menu
	fileMenu = menuBar()->addMenu("*");
	exitAct = fileMenu->addAction("*", this, &QWidget::close);
	 

	// Help menu
	helpMenu = menuBar()->addMenu("*");
	aboutAct = helpMenu->addAction("*");
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(onabout()));
	// Setting up the toolbar
//	toolbar = addToolBar("Toolbar");
}

void MainWindow::onabout()
{

	QMessageBox::information(this, "iec-104server-free", "question please mailto \r\n 3288950@qq.com");
}
void MainWindow::retranslateUi()
{
	fileMenu->setTitle(tr("File"));
	exitAct->setText(tr("Exit")); 
	helpMenu->setTitle(tr("Help"));
	aboutAct->setText(tr("About..."));
}

void MainWindow::slotLanguageChanged(QAction* action)
{ 
}

void MainWindow::loadLanguage()
{ 
}

void MainWindow::changeEvent(QEvent* event)
{ 
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	writeSettings();
	event->accept();
}

void MainWindow::writeSettings()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	settings.setValue("maingeometry", saveGeometry());
	//	settings.setValue("hgeometry", hSplitter->saveState());
	//	settings.setValue("vgeometry", vSplitter->saveState());
	settings.setValue("language", locale);
}

void MainWindow::readSettings()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	QByteArray maingeometry = settings.value("maingeometry", QByteArray()).toByteArray();
	//	QByteArray hgeometry = settings.value("hgeometry", QByteArray()).toByteArray();
	//	QByteArray vgeometry = settings.value("vgeometry", QByteArray()).toByteArray();
	locale = settings.value("language", QString()).toString();
	if (maingeometry.isEmpty())
	{
		const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
		resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
		move((availableGeometry.width() - width()) / 2,
			(availableGeometry.height() - height()) / 2);
	}
	else
	{
		restoreGeometry(maingeometry);
	}
	/* Problem with read access
	if (!hgeometry.isEmpty())
	hSplitter->restoreState(hgeometry);
	if (!vgeometry.isEmpty())
	vSplitter->restoreState(vgeometry);
	*/
	//if (locale.isEmpty())
	//{
	//	// Find the systems default language
	//	locale = QLocale::system().name();
	//	locale.truncate(locale.lastIndexOf('_'));
	//}
	//else
	//{
	//	locale = settings.value("language", QString("gb")).toString();
	//}
}
