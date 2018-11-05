#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>

#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QPainter>
#include <QPicture>
#include <QTimer>

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
    void timer_out();

    void startBtnSlot();
    void backBtnSlot();
    void resetBtnSlot();
    void itemBtnSlot();

private:
    Ui::MainWindow *ui;
    TcpClient *client;
    bool connectState;
    int systemState;
    void sendMessage();
    QTimer *timer;

    QStackedWidget *stackedWidget;
    QWidget *pageWidget;
    // 1 poage
    QPushButton *startBtn, *resetBtn;
    QLabel *page1Label;
    // 2 page
    QPushButton *backBtn, *itemBtn[48];
    QVector<int> pageList;
    QLabel *itemLabel;
    // 3 page
    QLabel *itemText;
    // 4 page
    QLabel *thankText;
};

#endif // MAINWINDOW_H
