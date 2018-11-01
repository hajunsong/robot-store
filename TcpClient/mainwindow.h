#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tcpclient.h"

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
    void connectBtnSlot();
    void disconnectBtnSlot();
    void onConnectServer();
    void sendRequest();
    void readMessage();

private:
    Ui::MainWindow *ui;
    TcpClient *client;
    bool m_connectState;
};

#endif // MAINWINDOW_H
