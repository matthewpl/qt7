#include "widget.hpp"

#include <QtNetwork>
#include <QtGui>

Widget::Widget(QWidget *parent)
	: QWidget(parent)
{
	tcpServer = new QTcpServer(this);
	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(addClient()));

	clientsList = new QList<QTcpSocket*>();
	blockSizeList = new QList<quint16>();

	portLineEdit = new QLineEdit(this);
	portLineEdit->setValidator(new QIntValidator(1, 65535, this));

	portLabel = new QLabel(tr("&Port:"), this);
	portLabel->setBuddy(portLineEdit);

	statusLabel = new QLabel(tr("Serwer wyłączony"), this);

	startServerButton = new QPushButton(tr("&Uruchom serwer"), this);
	connect(startServerButton, SIGNAL(clicked()), this, SLOT(serverStart()));

	stopServerButton = new QPushButton(tr("&Zatrzymaj serwer"), this);
	stopServerButton->setEnabled(false);
	connect(stopServerButton, SIGNAL(clicked()), this, SLOT(serverStop()));

	quitServerButton = new QPushButton(tr("&Wyjście"), this);
	connect(quitServerButton, SIGNAL(clicked()), this, SLOT(serverQuit()));

	QGridLayout *mainLayout = new QGridLayout(this);
	mainLayout->addWidget(portLabel, 0, 0, 1, 1);
	mainLayout->addWidget(portLineEdit, 0, 1, 1, 1);
	mainLayout->addWidget(statusLabel, 1, 0, 1, 2);
	mainLayout->addWidget(startServerButton, 2, 0, 1, 2);
	mainLayout->addWidget(stopServerButton, 3, 0, 1, 2);
	mainLayout->addWidget(quitServerButton, 4, 0, 1, 2);
}

Widget::~Widget()
{
	delete clientsList;
	delete blockSizeList;
}

void Widget::serverStart()
{
	if (!tcpServer->listen(QHostAddress::Any, portLineEdit->text().toInt()))
	{
		QMessageBox::critical(this, tr("Błąd!"), tr("Serwer nie może zostać poprawnie uruchomionym"));
		return;
	}
	else
	{
		QString ipAddress;
		QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

		for (int i = 0; i < ipAddressesList.size(); ++i)
		{
			if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address())
			{
				ipAddress = ipAddressesList.at(i).toString();
				break;
			}
		}

		if (ipAddress.isEmpty())
		{
			ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
		}

		statusLabel->setText(tr("Serwer słucha na IP: %1\nna porcie: %2").arg(ipAddress).arg(tcpServer->serverPort()));
		startServerButton->setEnabled(false);
		stopServerButton->setEnabled(true);
	}
}

void Widget::serverStop()
{
	tcpServer->close();
	statusLabel->setText(tr("Serwer wyłączony"));
	startServerButton->setEnabled(true);
	stopServerButton->setEnabled(false);
}

void Widget::serverQuit()
{
	tcpServer->close();
	close();
}

void Widget::addClient()
{
	QTcpSocket *client = tcpServer->nextPendingConnection();
	clientsList->push_back(client);
	blockSizeList->push_back(0);

	connect(client, SIGNAL(disconnected()), this, SLOT(removeClient()));
	connect(client, SIGNAL(disconnected()), client, SLOT(deleteLater()));
	connect(client, SIGNAL(readyRead()), this, SLOT(sendForward()));
}

void Widget::removeClient()
{
	QTcpSocket *client = (QTcpSocket*) sender();

	int index = clientsList->indexOf(client);
	clientsList->removeAt(index);
	blockSizeList->removeAt(index);
}

void Widget::sendForward()
{
	QTcpSocket *client = (QTcpSocket*) sender();
	int index = clientsList->indexOf(client);

	QDataStream in(client);
	in.setVersion(QDataStream::Qt_4_6);

	if (blockSizeList->at(index) == 0)
	{
		if (client->bytesAvailable() < (int)sizeof(quint16))
		{
			return;
		}

		quint16 sizeOfBlock;
		in >> sizeOfBlock;
		(*blockSizeList)[index] = sizeOfBlock;
	}

	if (client->bytesAvailable() < blockSizeList->at(index))
	{
		return;
	}

	QString nick;
	in >> nick;

	QString message;
	in >> message;

	QByteArray data;
	QDataStream out(&data, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_6);

	out << blockSizeList->at(index);
	out << nick;
	out << message;

	for (int i = 0; i < clientsList->size(); ++i)
	{
		if (i != index)
		{
			clientsList->at(i)->write(data);
		}
	}

	(*blockSizeList)[index] = (quint16)0;
}
