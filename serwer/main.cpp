#include <QtGui/QApplication>
#include <QTextCodec>
#include "widget.hpp"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTextCodec::setCodecForTr(QTextCodec::codecForName ("UTF-8"));

	Widget w;
	w.show();

	return a.exec();
}
