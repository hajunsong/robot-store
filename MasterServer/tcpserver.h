#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTimer>
#include <QDebug>
#include <QTcpSocket>

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    void startServer();
    void setIpAddress(QString address);

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void timer_out();
    void readyRead();
    void disconnected();

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QString m_ipAddress;
    int systemState;
    void sendMessage();
    QTimer *timer;
    QTcpSocket *socket;
    QVector<QTcpSocket*> tcpSocket;

};

#endif // TCPSERVER_H
