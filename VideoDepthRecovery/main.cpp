#include "videodepthrecovery.h"
#include "MainWindowControler.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	VideoDepthRecovery w;
	MainWindowControler::GetInstance()->SetMainWindow(&w);

	w.show();
	return a.exec();
}
