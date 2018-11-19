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
    client->socket->write(QString::number(102).toUtf8());
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

    qDebug() << rxData.length();
    if (rxData.length() < 2){
        systemState = rxData.toInt();
    }
    else{
        timer->start();
    }
}

void MainWindow::sendMessage()
{
//    QString txData = QString::number(systemState);

    QByteArray txData;
    txData.append(QByteArray::fromRawData("\x03\x06", 2));
    txData.append(QByteArray::fromRawData("\x03\x05\x03\x05\x03\x05\x03\x05\x03\x05\x03\x05", 12));
    txData.append(QByteArray::fromRawData("\x02\x05", 2));
    txData.append(QByteArray::fromRawData("\x03\x05\x03\x05\x03\x05", 6));
    txData.append(QByteArray::fromRawData("\x0D\x05",2));
    qDebug() << "Transmit Data : " + txData;

    client->socket->write(txData);
    QString txMessage = "Transmit Data : " + txData;
    ui->tcpMessage->append(txMessage);
}
