#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QDebug>
#include "tcpthread.h"

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    void startServer();
    void setIpAddress(QString address);

signals:

public slots:

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QString m_ipAddress;
};

#endif // TCPSERVER_H
