#include "connectwindow.hpp"

#include <QtGui>

ConnectWindow::ConnectWindow(QWidget *parent, Qt::WindowFlags f) :
	 QWidget(parent, f)
{
	hostEdit = new QLineEdit(this);

	portEdit = new QLineEdit(this);
	portEdit->setValidator(new QIntValidator(1, 65535, this));

	nickEdit = new QLineEdit(this);

	hostLabel = new QLabel(tr("&Serwer:"), this);
	hostLabel->setBuddy(hostEdit);

	portLabel = new QLabel(tr("Po&rt:"), this);
	portLabel->setBuddy(portEdit);

	nickLabel = new QLabel(tr("&Nick:"), this);
	nickLabel->setBuddy(nickEdit);

	connectButton = new QPushButton(tr("&Połącz"), this);
	connect(connectButton, SIGNAL(clicked()), this, SLOT(connectToHost()));

	QGridLayout *mainLayout = new QGridLayout(this);
	mainLayout->addWidget(hostLabel, 0, 0, 1, 1);
	mainLayout->addWidget(hostEdit, 0, 1, 1, 2);
	mainLayout->addWidget(portLabel, 1, 0, 1, 1);
	mainLayout->addWidget(portEdit, 1, 1, 1, 2);
	mainLayout->addWidget(nickLabel, 2, 0, 1, 1);
	mainLayout->addWidget(nickEdit, 2, 1, 1, 2);
	mainLayout->addWidget(connectButton, 3, 2, 1, 1);
}

void ConnectWindow::connectToHost()
{
	emit connectToHost(hostEdit->text(), portEdit->text().toInt(), nickEdit->text());
	close();
}
