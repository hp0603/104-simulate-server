#include "MainWindow.h"
#include <QApplication>
#include "iostream"
using namespace std;
int main(int argc, char *argv[])
{
	struct IECValue
	{ 
		union
		{
			float fVal;
			int	nVal;
		};
	};
	IECValue siec;
	siec.nVal = 1;
	QVector<IECValue>  sm;
	sm.push_back(siec);
	QVector<IECValue>::iterator it = sm.begin();
	int aa;
	for (it = sm.begin(); it != sm.end(); it++)
	{
		aa = it->nVal;
		it->nVal = 3;
	}
	for (it = sm.begin(); it != sm.end(); it++)
	{
		aa = it->nVal;
	}

	//QVector<int>  stored;
	//stored.push_back(1);
	//QVector<int>::iterator it = stored.begin();
	//int aa;
	//for (it;it!=stored.end();it++)
	//{
	//	aa=  *it;
	//	*it = 222;
	//}
	//for (it = stored.begin(); it != stored.end();it++)
	//{
	//	aa = *it; 
	//}

	QApplication app(argc, argv);

	QCoreApplication::setOrganizationName("3288950@qq.com");
	QCoreApplication::setApplicationName("IEC104Server");
	QCoreApplication::setApplicationVersion("1.234");

	MainWindow w;
	w.show();
	return app.exec();
}
