#include <QCoreApplication>
#include "tcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TcpServer *server = new TcpServer();
    server->setIpAddress("127.0.0.1");
    server->startServer();

    return a.exec();
}
