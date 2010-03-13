#include "mainwindow.hpp"

#include <QtGui>
#include <QtNetwork>
#include "connectwindow.hpp"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	createMenu();

	tcpSocket = new QTcpSocket(this);
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
	connect(tcpSocket, SIGNAL(connected()), this, SLOT(connectedToServer()));
	connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

	chat = new QTextBrowser(this);
	chat->setReadOnly(true);
	chat->setFocusPolicy(Qt::NoFocus);

	nickLabel = new QLabel("Nick:", this);

	messageLine = new QLineEdit(this);
	connect(messageLine, SIGNAL(returnPressed()), this, SLOT(sendMessage()));

	sendButton = new QPushButton(tr("&Wyślij"), this);
	sendButton->setDefault(true);
	sendButton->setAutoDefault(true);
	connect(sendButton, SIGNAL(clicked()), this, SLOT(sendMessage()));

	QWidget *mainWidget = new QWidget(this);
	QGridLayout *mainLayout = new QGridLayout(mainWidget);
	mainLayout->addWidget(chat, 0, 0, 2, 3);
	mainLayout->addWidget(nickLabel, 2, 0, 1, 1);
	mainLayout->addWidget(messageLine, 2, 1, 1, 1);
	mainLayout->addWidget(sendButton, 2, 2, 1, 1);

	setCentralWidget(mainWidget);
}

MainWindow::~MainWindow()
{

}

void MainWindow::createMenu()
{
	menu = menuBar()->addMenu(tr("&Menu"));

	connectAction = new QAction(tr("&Połącz"), this);
	connect(connectAction, SIGNAL(triggered()), this, SLOT(showConnectWindow()));
	menu->addAction(connectAction);

	disconnectAction = new QAction(tr("&Rozłącz"), this);
	connect(disconnectAction, SIGNAL(triggered()), this, SLOT(disconnectFromServer()));
	menu->addAction(disconnectAction);

	menu->addSeparator();

	quitAction = new QAction(tr("Wyj&dź"), this);
	connect(quitAction, SIGNAL(triggered()), this, SLOT(exitProgram()));
	menu->addAction(quitAction);
}

void MainWindow::showConnectWindow()
{
	ConnectWindow *connectWindow = new ConnectWindow(this, Qt::Window);
	connect(connectWindow, SIGNAL(connectToHost(QString, int, QString)), this, SLOT(connectToServer(QString, int, QString)));
	connectWindow->show();
}

void MainWindow::sendMessage()
{
	chat->append(nick + ": " + messageLine->text());

	QByteArray data;
	QDataStream out(&data, QIODevice::WriteOnly);

	out.setVersion(QDataStream::Qt_4_6);
	out << (quint16) 0;

	out << nick;

	out << messageLine->text();

	out.device()->seek(0);

	out << (quint16) (data.size() - sizeof(quint16));

	tcpSocket->write(data);

	messageLine->clear();
}

void MainWindow::readData()
{
	QDataStream in(tcpSocket);
	in.setVersion(QDataStream::Qt_4_6);

	if (blockSize == 0)
	{
		if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
		{
			return;
		}

		in >> blockSize;
	}

	if (tcpSocket->bytesAvailable() < blockSize)
	{
		return;
	}

	QString nick;
	in >> nick;

	QString message;
	in >> message;

	chat->append(nick + ": " + message);

	blockSize = 0;
}

void MainWindow::connectedToServer()
{
	chat->clear();
	chat->append(tr("Połączyłeś się z serwerem %1 port %2").arg(tcpSocket->peerAddress().toString()).arg(tcpSocket->peerPort()));
	nickLabel->setText(nick + ':');
}

void MainWindow::connectToServer(QString ip, int port, QString nick)
{
	tcpSocket->abort();
	blockSize = 0;

	tcpSocket->connectToHost(ip, port);
	this->nick = nick;
}

void MainWindow::disconnectFromServer()
{
	tcpSocket->close();
	this->nick.clear();
}

void MainWindow::exitProgram()
{
	disconnectFromServer();
	qApp->quit();
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
	switch (socketError)
	{
	case QAbstractSocket::RemoteHostClosedError:
		break;

	case QAbstractSocket::HostNotFoundError:
		QMessageBox::information(this, tr("Fortune Client"),
										 tr("The host was not found. Please check the "
											 "host name and port settings."));
		break;

	case QAbstractSocket::ConnectionRefusedError:
		QMessageBox::information(this, tr("Fortune Client"),
										 tr("The connection was refused by the peer. "
											 "Make sure the fortune server is running, "
											 "and check that the host name and port "
											 "settings are correct."));
		break;

	default:
		QMessageBox::information(this, tr("Fortune Client"),
										 tr("The following error occurred: %1.")
										 .arg(tcpSocket->errorString()));
	}
}
