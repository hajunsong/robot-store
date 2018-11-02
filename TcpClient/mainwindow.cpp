#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Client");
    this->setFixedSize(400, 300);
    this->setWindowFlags(Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);

    m_connectState = false;
    client = new TcpClient(this);

    connect(ui->connectBtn, SIGNAL(clicked()), this, SLOT(connectBtnSlot()));
    connect(ui->disconnectBtn, SIGNAL(clicked()), this, SLOT(disconnectBtnSlot()));
    connect(ui->sendBtn, SIGNAL(clicked()), this, SLOT(sendRequest()));
    connect(client->m_socket, SIGNAL(connected()), this, SLOT(onConnectServer()));
    connect(client->m_socket, SIGNAL(readyRead()), this, SLOT(readMessage()));

    ui->ipAddress->setText("127.0.0.1");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete client;
}

void MainWindow::connectBtnSlot()
{
    client->setIpAddress(ui->ipAddress->text());
    emit client->connectToServer();
}

void MainWindow::disconnectBtnSlot()
{
    if (m_connectState == true){
        client->m_socket->close();
        ui->tcpMessage->append("Close ...");
        m_connectState = false;
    }
}

void MainWindow::onConnectServer()
{
    ui->tcpMessage->append("Connect complete ...");
    m_connectState = true;
}

void MainWindow::sendRequest()
{
    QString dataViewTemp;
    QString str = ui->sendMessage->text();

    if (str.isEmpty() == false){
        dataViewTemp = "Send Data : " + str;

        if (m_connectState == true){
            ui->tcpMessage->append(dataViewTemp);
            client->m_socket->write(str.toUtf8());
        }
    }

    ui->sendMessage->clear();
}

void MainWindow::readMessage()
{
    QString rxMessage;
    QByteArray baMessage = client->m_socket->readAll();

    rxMessage = "Receive Data : " + baMessage;
    ui->tcpMessage->append(rxMessage);
//    unsigned char chr;
//    QByteArray baMessageTemp;

//    baMessageTemp.append("Receive Data : ");
//    for (int index = 0; index < baMessage.length(); index++){
//        chr = baMessage.at(index);
//        baMessageTemp.append(QString::number(chr));
//        if (index != baMessage.length() - 1){
//            baMessageTemp.append(", ");
//        }
//    }

//    ui->tcpMessage->append(baMessageTemp);
}
