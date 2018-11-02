#include "tcpclient.h"

TcpClient::TcpClient(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket;
}

TcpClient::~TcpClient()
{

}

void TcpClient::connectToServer(){
    socket->connectToHost(ipAddress, 9999);
}

void TcpClient::setIpAddress(QString address){
    ipAddress = address;
}
