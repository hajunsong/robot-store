#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>

#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

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
    void onConnectServer();
    void readMessage();

    void startBtnSlot();
    void listBtnSlot();
    void cartBtnSlot();
    void orderBtnSlot();
    void exitBtnSlot();
    void backBtnSlot();

private:
    Ui::MainWindow *ui;
    TcpClient *client;
    bool connectState;
    QVector<QString> PID;
    int systemState;
    void sendMessage();
    void sendMessageOrderList();

    QStackedWidget *stackedWidget;
    QWidget *pageWidget;
    // 1 poage
    QPushButton *startBtn;
    QLabel *page1Label;
    // 2 page
    QPushButton *listBtn, *cartBtn, *orderBtn, *exitBtn, *backBtn, *yesBtn, *noBtn;
    QLabel *page2Label;
    QVector<int> pageList;
};

#endif // MAINWINDOW_H
