#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tcpclient.h"
#include <QTimer>

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
    void onConnectServer();
    void readMessage();
    void timer_out();

private:
    Ui::MainWindow *ui;
    TcpClient *client;
    bool connectState;
    int systemState;
    void sendMessage();
    QTimer *timer;
};

#endif // MAINWINDOW_H
