#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QMainWindow>
#include <QTcpServer>

namespace Ui {
class TcpServer;
}

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(Ui::TcpServer *ui, QObject *parent = nullptr);
    void start();
    void end();

signals:

public slots:

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    Ui::TcpServer *ui;
};


class TcpServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit TcpServer(QWidget *parent = nullptr);
    ~TcpServer();

private:
    Ui::TcpServer *ui;
    Server *server;
    const QString strConnect = "Listen";
    const QString strDisconnect = "Disconnect";

private slots:
    void clicked_btnListen();
};

#endif // TCPSERVER_H
