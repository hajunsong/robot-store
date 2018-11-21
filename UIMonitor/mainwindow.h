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
#include <QMouseEvent>
#include <QEvent>

#include "tcpclient.h"
#include "qclicklabel.h"

const int row = 6, col = 7;
const QString imagePath = "icons/";
const QString imageIcon[row*col] = {"a_1@2x","a_2@2x","a_3@2x","a_4@2x","a_5@2x","a_6@2x","a_7@2x",
                               "b_1@2x","b_2@2x","b_3@2x","b_4@2x","b_5@2x","b_6@2x","b_7@2x",
                               "c_1@2x","c_2@2x","c_3@2x","c_4@2x","c_5@2x","c_6@2x","c_7@2x",
                               "d_1@2x","d_2@2x","d_3@2x","d_4@2x","d_5@2x","d_6@2x","d_7@2x",
                               "e_1@2x","e_2@2x","e_3@2x","e_4@2x","e_5@2x","e_6@2x","e_7@2x",
                               "f_1@2x","f_2@2x","f_3@2x","f_4@2x","f_5@2x","f_6@2x","f_7@2x"};
const QString startIcon[2] = {"start_1@2x", "start_2@2x"};
const QString colorIcon[6] = {"violet", "blue", "green", "red", "orange", "yellow"};
const QString patternIcon[7] = {"gradation", "slash", "stripe", "check", "dice", "cross", "hexagon"};
const QString orderIcon[3] = {"order", "order_select", "order_base"};
enum{startPage=0, selectPage, itemPage, thankPage};

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

    void startBtnPressedSlot();
    void startBtnReleasedSlot();
    void backBtnSlot();
    void resetBtnSlot();
    void colorBtnPressedSlot();
    void colorBtnReleasedSlot();
    void patternBtnPressedSlot();
    void patternBtnReleasedSlot();
    void orderBtnPressedSlot();
    void orderBtnReleasedSlot();

private:
    Ui::MainWindow *ui;
    void init();
    TcpClient *client;
    bool connectState;
    int systemState;
    void sendMessage();
    QTimer *timer;
    int colorIndex, patternIndex;

    QStackedWidget *stackedWidget;
    QWidget *pageWidget;
    // 1 poage
    QClickLabel *startBtn;
    QPushButton *resetBtn;
    // 2 page
    QPushButton *backBtn;
    QClickLabel *colorBtn[6], *patternBtn[7], *orderBtn, *orderText;
    void combineItem();
    QVector<int> pageList;
    QLabel *itemLabel;
    // 3 page
    QLabel *itemText, *waitText;
    // 4 page
    QLabel *thankText;
};

#endif // MAINWINDOW_H
