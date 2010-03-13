#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <QtGui>
#include <QList>

class QTcpServer;
class QTcpSocket;

class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget *parent = 0);
	~Widget();

private slots:
	void serverStart();
	void serverStop();
	void serverQuit();
	void addClient();
	void removeClient();
	void sendForward();

private:
	QTcpServer *tcpServer;

	QLabel *portLabel;
	QLabel *statusLabel;
	QLineEdit *portLineEdit;
	QPushButton *startServerButton;
	QPushButton *stopServerButton;
	QPushButton *quitServerButton;

	QList<QTcpSocket*> *clientsList;
	QList<quint16> *blockSizeList;
};

#endif // WIDGET_HPP
