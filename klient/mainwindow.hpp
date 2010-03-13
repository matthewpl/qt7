#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtGui/QMainWindow>
#include <QAbstractSocket>

class QTextBrowser;
class QPushButton;
class QLineEdit;
class QLabel;
class QTcpSocket;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void sendMessage();
	void showConnectWindow();
	void connectToServer(QString, int, QString);
	void connectedToServer();
	void disconnectFromServer();
	void exitProgram();
	void readData();
	void displayError(QAbstractSocket::SocketError);

private:
	QMenu *menu;

	QAction *connectAction;
	QAction *disconnectAction;
	QAction *quitAction;

	QTextBrowser *chat;
	QLabel *nickLabel;
	QPushButton *sendButton;
	QLineEdit *messageLine;

	QTcpSocket *tcpSocket;

	QString nick;
	quint16 blockSize;

	void createMenu();
};

#endif // MAINWINDOW_HPP
