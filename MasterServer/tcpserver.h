#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QDebug>
#include <QTcpSocket>

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    void startServer();

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    int systemState;
    QTcpSocket *socket;
    QVector<QTcpSocket*> tcpSocket;
    QVector<int> tcpSocketID;
    QVector<qintptr> tcpSocketDescriptors;
    int productNumber;
};

#endif // TCPSERVER_H
