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

void TcpServer::readyRead()
{
    if (tcpSocketID.length() == 3) {
        QString rxData;
        for (int i = 0; i < tcpSocket.length(); i++) {
            QByteArray Data = tcpSocket[i]->readAll();
            rxData = Data;
            if (Data.length() > 0) {
                if (tcpSocketID[i] == 102){
                    int len = rxData.length();
                    if (len > 0){
                        //                        qDebug() << "RX length : " + QString::number(len);
                        QChar ch;
                        for(int j = 0; j < len; j++){
                            ch = rxData.at(j);
                            if (j == 14 && ch == '\u0002'){
                                systemState = 7;
                                break;
                            }
                        }
                    }
                }
                else{
                    // will write on server side window
                    qDebug() << "Data in: " << rxData;
                    systemState = rxData.toInt();
                }
            }
        }

        for (int i = 0; i < tcpSocket.length(); i++) {
            if (tcpSocketID[i] == 102){
                if (systemState / 100 == 5){
                    productNumber = systemState % 100;
                    QByteArray txData;
                    txData.append(QByteArray::fromRawData("\x02\x05", 2));
                    if (productNumber < 16){
                        txData.append(static_cast<char>(0x00));
                    }
                    QString result = QString::number(productNumber, 16);
                    txData.append(result.toUtf8());
                    txData.append(QByteArray::fromRawData("\x03\x05\x03\x05\x03\x05\x03\x05", 8));
                    txData.append(QByteArray::fromRawData("\x0D\x05",2));
                    qDebug() << "Transmit Data : " + txData;
                    tcpSocket[i]->write(txData);
                }
            }
            else {
                tcpSocket[i]->write(QString::number(systemState).toUtf8());
            }
        }

        qDebug() << "Transmit Data : " + QString::number(systemState).toUtf8();

        for (int i = 0; i < tcpSocket.length(); i++) {
            tcpSocket[i]->flush();
        }
    }
    else{
        QString rxData;
        QByteArray Data = tcpSocket.back()->readAll();
        if (Data.length() > 0) {
            rxData = Data;
            // will write on server side window
            qDebug() << "Data in: " << rxData;
            tcpSocketID.push_back(rxData.toInt());
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
            break;
        }
    }

    tcpSocketDescriptors.erase(tcpSocketDescriptors.begin() + deleteIndex);
    tcpSocket.erase(tcpSocket.begin() + deleteIndex);
    tcpSocketID.erase(tcpSocketID.begin() + deleteIndex);

    if (tcpSocket.length() == 0) {
        exit(0);
    }
}

