#include "tcpclient.h"

TcpClient::TcpClient(QObject *parent) : QObject(parent)
{
    m_socket = new QTcpSocket;
}

TcpClient::~TcpClient()
{

}

void TcpClient::connectToServer(){
    m_socket->connectToHost(m_ipAddress, 9999);
}

void TcpClient::setIpAddress(QString address){
    m_ipAddress = address;
}
