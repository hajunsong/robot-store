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

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    TcpClient *client;
    bool connectState;
    bool guestState;
    int systemState;
    void moving();
    void sendMessage();
    QTimer *timer;
};

#endif // MAINWINDOW_H
