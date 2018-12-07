#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(500, 400);
    this->setWindowTitle("Mobile Robot");

    ui->ipAddress->setText("127.0.0.1");

    connectState = false;
    client = new TcpClient(this);

    connect(ui->connectBtn, SIGNAL(clicked()), this, SLOT(connectBtnSlot()));
    connect(client->socket, SIGNAL(connected()), this, SLOT(onConnectServer()));
    connect(client->socket, SIGNAL(readyRead()), this, SLOT(readMessage()));

    guestState = false;
    systemState = 0;

    timer = new QTimer(this);
    timer->setInterval(3000);
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_out()));
}

void MainWindow::timer_out(){
    sendMessage();
    timer->stop();
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

//    QByteArray txData;
//    txData.append(QByteArray::fromRawData("\x02\x05",2));
//    txData.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 10));
//    txData.append(QByteArray::fromRawData("\x0D\x05",2));
//    qDebug() << "Transmit Data : " + txData;
//    client->socket->write(txData);

    ui->connectBtn->setText("Disconnect");
}

void MainWindow::readMessage()
{
    QString rxMessage;
    QString rxData = client->socket->readAll();

    rxMessage = "Receive Data : " + rxData;
    ui->tcpMessage->append(rxMessage);

    int len = rxData.length();
    qDebug() << "RX length : " + QString::number(len);
    qDebug() << rxData;
    QChar ch;
    for(int j = 0; j < len; j++){
        ch = rxData.at(j);
//        qDebug() << ch;
    }

    systemState = rxData.toInt();
    if (systemState == 1){
        ui->guestCbox->setChecked(true);
        systemState = 2;
        moving();
        timer->start();
    }
    else if(systemState == 0){
        // move autonomous
        ui->guestCbox->setChecked(false);
    }
}

void MainWindow::moving(){
    for(int i = 0, j = 0; i < 10000000; i++){
        j++;
    }
    systemState = 3;
}

void MainWindow::sendMessage()
{
//    QString txData = QString::number(systemState);
//    client->socket->write("txData.toUtf8()");
//    QString txMessage = "Transmit Data : " + txData;
//    ui->tcpMessage->append(txMessage);
    int productNumber = systemState;
    QByteArray txData;
    txData.append(QByteArray::fromRawData("\x02\x05", 2));
    if (productNumber < 16){
        txData.append(static_cast<char>(0x00));
    }
    QString result = QString::number(productNumber, 10);
    txData.append(result.toUtf8());
    txData.append(QByteArray::fromRawData("\x03\x05\x03\x05\x03\x05\x03\x05", 8));
    txData.append(QByteArray::fromRawData("\x0D\x05",2));
    qDebug() << "Transmit Data : " + txData;
    client->socket->write(txData);
}

void MainWindow::on_pushButton_clicked()
{
    QByteArray txData;

    txData.append(QByteArray::fromRawData("\x02\x05",2));
    txData.append(QByteArray::fromRawData("\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00", 10));
    txData.append(QByteArray::fromRawData("\x0D\x05",2));
    qDebug() << "Transmit Data : " + txData;

    client->socket->write(txData);
}

void MainWindow::on_pushButton_2_clicked()
{
    QByteArray txData;

    txData.append(QByteArray::fromRawData("\x02\x05",2));
    txData.append(QByteArray::fromRawData("\x02\x01\x00\x00\x00\x00\x00\x00\x00\x00", 10));
    txData.append(QByteArray::fromRawData("\x0D\x05",2));
    qDebug() << "Transmit Data : " + txData;

    client->socket->write(txData);
}

void MainWindow::on_pushButton_3_clicked()
{
    QByteArray txData;

    txData.append(QByteArray::fromRawData("\x02\x05",2));
    txData.append(QByteArray::fromRawData("\x03\x01\x00\x00\x00\x00\x00\x00\x00\x00", 10));
    txData.append(QByteArray::fromRawData("\x0D\x05",2));
    qDebug() << "Transmit Data : " + txData;

    client->socket->write(txData);
}

void MainWindow::on_pushButton_4_clicked()
{
    QByteArray txData;

    txData.append(QByteArray::fromRawData("\x02\x05",2));
    txData.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 10));
    txData.append(QByteArray::fromRawData("\x0D\x05",2));
    qDebug() << "Transmit Data : " + txData;

    client->socket->write(txData);
}
