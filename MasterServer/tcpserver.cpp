#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{
    systemState = 0;
}

void TcpServer::setIpAddress(QString address) {
    m_ipAddress = address;
}

void TcpServer::startServer()
{
    if (!this->listen(QHostAddress(m_ipAddress), 9999))
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

    socket = new QTcpSocket();

    // set the ID
    if (!socket->setSocketDescriptor(socketDescriptor))
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

    tcpSocket.push_back(socket);
    tcpSocketDescriptors.push_back(socketDescriptor);
    if (tcpSocket.length() == 3) {
        systemState = 0;
    }
}

void TcpServer::timer_out()
{
    QString txData = QString::number(systemState);

    for (int i = 0; i < tcpSocket.length(); i++) {
        tcpSocket[i]->write(txData.toUtf8());
    }
    qDebug() << "Transmit Data : " + txData;
}

void TcpServer::readyRead()
{
    if (tcpSocket.length() == 3) {
        QString rxData;
        for (int i = 0; i < tcpSocket.length(); i++) {
            QByteArray Data = tcpSocket[i]->readAll();
            if (Data.length() > 0) {
                rxData = Data;

                // will write on server side window
                qDebug() << "Data in: " << rxData;

                systemState = rxData.toInt();
            }
        }

        for (int i = 0; i < tcpSocket.length(); i++) {
            tcpSocket[i]->write(rxData.toUtf8());
        }

        qDebug() << "Transmit Data : " + rxData;

        for (int i = 0; i < tcpSocket.length(); i++) {
            tcpSocket[i]->flush();
        }
    }
}

void TcpServer::disconnected()
{
    int deleteIndex = -1;
    for (int i = 0; i < tcpSocket.length(); i++) {
        if (tcpSocket[i]->state() == 0) {
            qDebug() << QString::number(tcpSocketDescriptors[i]) + " Disconnected";
            tcpSocket[i]->deleteLater();
            deleteIndex = i;
        }
    }
    if (deleteIndex >= 0 && deleteIndex < 3) {
        tcpSocketDescriptors.erase(tcpSocketDescriptors.begin() + deleteIndex);
        tcpSocket.erase(tcpSocket.begin() + deleteIndex);
    }

    if (tcpSocket.length() == 0) {
        exit(0);
    }
}

