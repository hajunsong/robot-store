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

    QTcpSocket *socket;
    void setIpAddress(QString address);
    void setPort(quint16 num);

signals:
public slots:
    void connectToServer();

private:
    QString ipAddress;
    quint16 portNum;
};

#endif // TCPCLIENT_H
