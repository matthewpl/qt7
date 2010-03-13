#ifndef CONNECTWINDOW_HPP
#define CONNECTWINDOW_HPP

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;

class ConnectWindow : public QWidget
{
	Q_OBJECT
public:
	explicit ConnectWindow(QWidget *parent = 0, Qt::WindowFlags f = 0);

signals:
	void connectToHost(QString, int, QString);

private slots:
	void connectToHost();

private:
	QLabel *hostLabel;
	QLabel *portLabel;
	QLabel *nickLabel;

	QLineEdit *hostEdit;
	QLineEdit *portEdit;
	QLineEdit *nickEdit;

	QPushButton *connectButton;

};

#endif // CONNECTWINDOW_HPP
