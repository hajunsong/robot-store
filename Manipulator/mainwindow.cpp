#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(500, 400);
    this->setWindowTitle("Manipulator");

    ui->ipAddress->setText("127.0.0.1");

    connectState = false;
    client = new TcpClient(this);

    connect(ui->connectBtn, SIGNAL(clicked()), this, SLOT(connectBtnSlot()));
    connect(client->socket, SIGNAL(connected()), this, SLOT(onConnectServer()));
    connect(client->socket, SIGNAL(readyRead()), this, SLOT(readMessage()));

    systemState = 0;

    timer = new QTimer(this);
    timer->setInterval(10000);
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_out()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete client;
}

void MainWindow::connectBtnSlot()
{
    if (connectState){
        client->socket->close();
        ui->tcpMessage->append("Close ...");
        connectState = false;
        ui->connectBtn->setText("Connect");
    }
    else{
        client->setIpAddress(ui->ipAddress->text());
        emit client->connectToServer();
    }
}

void MainWindow::onConnectServer()
{
    ui->tcpMessage->append("Connect complete ...");
    connectState = true;
    client->socket->write(QString::number(systemState).toUtf8());
    ui->connectBtn->setText("Disconnect");
}

void MainWindow::timer_out(){
    systemState = 7;
    sendMessage();
    timer->stop();
}

void MainWindow::readMessage()
{
    QString rxMessage;
    QString rxData = client->socket->readAll();

    rxMessage = "Receive Data : " + rxData;
    ui->tcpMessage->append(rxMessage);

    systemState = rxData.toInt();
    if (systemState/100 == 5){
        systemState = 6;
        timer->start();
    }
}

void MainWindow::sendMessage()
{
    QString txData = QString::number(systemState);
    client->socket->write(txData.toUtf8());
    QString txMessage = "Transmit Data : " + txData;
    ui->tcpMessage->append(txMessage);
}
