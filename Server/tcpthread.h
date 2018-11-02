#ifndef TCPTHREAD_H
#define TCPTHREAD_H

#include <QThread>
#include <QTcpSocket>

class TcpThread : public QThread
{
    Q_OBJECT
public:
    explicit TcpThread(qintptr ID, QObject *parent = nullptr);

    void run();
    QTcpSocket *socket;

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();

private:
    qintptr socketDescriptor;
};

#endif // TCPTHREAD_H
