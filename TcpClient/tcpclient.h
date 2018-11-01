#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent=nullptr);
    ~TcpClient();

    QTcpSocket *m_socket;
    void setIpAddress(QString address);

signals:
public slots:
    void connectToServer();

private:
    QString m_ipAddress;
};

#endif // TCPCLIENT_H
