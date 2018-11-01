#include "mainwindow.h"
#include "ui_tcpserver.h"
#include "tcpthread.h"

TcpServer::TcpServer(QWidget *parent) : QMainWindow(parent), ui(new Ui::TcpServer)
{
    ui->setupUi(this);

    connect(ui->listenBtn, SIGNAL(clicked()), this, SLOT(clicked_btnListen()));

    ui->ipAddress->setText("127.0.0.1");
}

TcpServer::~TcpServer()
{
    delete ui;
    delete server;
}

void TcpServer::clicked_btnListen(){
    if (!QString::compare(ui->listenBtn->text(), strConnect)){
        server = new Server(ui, this);
        server->start();
        ui->listenBtn->setText(strDisconnect);
    }
    else if (!QString::compare(ui->listenBtn->text(), strDisconnect)){
        ui->listenBtn->setText(strConnect);
    }
}

void Server::start()
{
    if(!this->listen(QHostAddress(ui->ipAddress->text()), 9999))
    {
        ui->tcpMessage->append("Could not start server");
    }
    else
    {
        ui->tcpMessage->append("Listening ...");
    }
}

Server::Server(Ui::TcpServer *mainUi, QObject *parent) : QTcpServer(parent)
{
    ui = mainUi;
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    ui->tcpMessage->append(QString::number(socketDescriptor) + " Connecting...");

    TcpThread *thread = new TcpThread(socketDescriptor, this);

    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}
