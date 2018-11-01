#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tcpserver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void listenBtnSlot();
    void disconnectBtnSlot();
    void sendBtnSlot();

private:
    Ui::MainWindow *ui;
    TcpServer *server;
    QString m_ipAddress;
};

#endif // MAINWINDOW_H
