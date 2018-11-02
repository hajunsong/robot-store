#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{
    index = 0;
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

    TcpThread *thread = new TcpThread(socketDescriptor, this);

    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();

    socketID[index++] = socketDescriptor;
    tcpThread.push_back(thread);

//    // connect signal/slot
//    // once a thread is not needed, it will be beleted later
//    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

//    thread->start();

//    tcpThread.push_back(thread);
}
