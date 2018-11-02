#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{
    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_out()));

    systemState = 0;

//    timer->start();
}

void TcpServer::setIpAddress(QString address){
    m_ipAddress = address;
}

void TcpServer::startServer()
{
    if(!this->listen(QHostAddress(m_ipAddress), 9999))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening ...";
    }
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    qDebug() << QString::number(socketDescriptor) + " Connecting...";

//    TcpThread *thread = new TcpThread(socketDescriptor, this);

//    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

//    thread->start();

//    tcpThread.push_back(thread);

    socket = new QTcpSocket();

    // set the ID
    if(!socket->setSocketDescriptor(socketDescriptor))
    {
        // something's wrong, we just emit a signal
        emit error(socket->error());
        return;
    }

    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::DirectConnection);

    if (tcpSocket.length() == 3){
        timer->start();
    }
}

void TcpServer::timer_out()
{
    QString txData = QString::number(systemState);

    for(int i = 0; i < tcpSocket.length(); i++){
        tcpSocket[i]->write(txData.toUtf8());
    }
    qDebug() << "Transmit Data : " + txData;

    timer->start();
}

void TcpServer::readyRead()
{
    // get the information
    QByteArray Data = socket->readAll();

    // will write on server side window
    qDebug() << /*socketDescriptor <<*/ " Data in: " << Data;

    QString rxData = Data;
    systemState = rxData.toInt();

//    socket->write(Data);
}

void TcpServer::disconnected()
{
    qDebug() << /*socketDescriptor <<*/ " Disconnected";

    socket->deleteLater();
    exit(0);
}

