#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1280, 720);
    this->setWindowTitle("UI Monitor");

    stackedWidget = new QStackedWidget(this);
    stackedWidget->hide();
    stackedWidget->setGeometry(this->rect());
    quint64 pages = 6;
    pageWidget = new QWidget[pages];
    for(quint64 i = 0; i < pages; i++){
        pageWidget[i].setGeometry(stackedWidget->rect());
        stackedWidget->addWidget(&pageWidget[i]);
    }

    // 1 page
    page1Label = new QLabel(&pageWidget[0]);
    page1Label->setGeometry(pageWidget[0].rect());
    page1Label->setText("Welcome to Robot Store\n!^^!");
    QFont page1LabelFont;
    page1LabelFont.setPointSize(50);
    page1LabelFont.setBold(true);
    page1Label->setFont(page1LabelFont);
    page1Label->setAlignment(Qt::AlignmentFlag::AlignCenter);

    startBtn = new QPushButton(&pageWidget[0]);
    int startBtnWidth = 100, startBtnHeight = 70;
    startBtn->setGeometry(pageWidget[0].width() - startBtnWidth, pageWidget[0].height() - startBtnHeight, startBtnWidth, startBtnHeight);
    startBtn->setText("Start\nShopping");
    QFont btnFont;
    btnFont.setPointSize(15);
    btnFont.setBold(true);
    startBtn->setFont(btnFont);
    startBtn->setObjectName(QStringLiteral("startBtn"));

    // 2 page
    backBtn = new QPushButton(&pageWidget[1]);
    backBtn->setText("Back");
    backBtn->setFont(btnFont);
    backBtn->setObjectName(QStringLiteral("backBtn"));
    int backBtnWidth = 100, backBtnHeight = 50;
    backBtn->setGeometry(pageWidget[0].width() - backBtnWidth, pageWidget[0].height() - backBtnHeight, backBtnWidth, backBtnHeight);
    int pageWidth = pageWidget[1].width() - backBtnWidth, pageHeight = pageWidget[1].height();
    listBtn = new QPushButton(&pageWidget[1]);
    listBtn->setGeometry(static_cast<int>(pageWidth*0.05), static_cast<int>(pageHeight*0.05), static_cast<int>(pageWidth*0.4), static_cast<int>(pageHeight*0.4));
    listBtn->setText("Products\nList");
    btnFont.setPointSize(50);
    listBtn->setFont(btnFont);
    listBtn->setObjectName(QStringLiteral("listBtn"));
    cartBtn = new QPushButton(&pageWidget[1]);
    cartBtn->setGeometry(static_cast<int>(pageWidth*0.55), static_cast<int>(pageHeight*0.05), static_cast<int>(pageWidth*0.4), static_cast<int>(pageHeight*0.4));
    cartBtn->setText("Shopping\nCart");
    cartBtn->setFont(btnFont);
    cartBtn->setObjectName(QStringLiteral("cartBtn"));
    orderBtn = new QPushButton(&pageWidget[1]);
    orderBtn->setGeometry(static_cast<int>(pageWidth*0.05), static_cast<int>(pageHeight*0.55), static_cast<int>(pageWidth*0.4), static_cast<int>(pageHeight*0.4));
    orderBtn->setText("Products\nOrder");
    orderBtn->setFont(btnFont);
    orderBtn->setObjectName(QStringLiteral("orderBtn"));
    exitBtn = new QPushButton(&pageWidget[1]);
    exitBtn->setGeometry(static_cast<int>(pageWidth*0.55), static_cast<int>(pageHeight*0.55), static_cast<int>(pageWidth*0.4), static_cast<int>(pageHeight*0.4));
    exitBtn->setText("Exit\nStore");
    exitBtn->setFont(btnFont);
    exitBtn->setObjectName(QStringLiteral("exitBtn"));

    connect(startBtn, SIGNAL(clicked()), this, SLOT(startBtnSlot()));
    connect(listBtn, SIGNAL(clicked()), this, SLOT(listBtnSlot()));
    connect(cartBtn, SIGNAL(clicked()), this, SLOT(cartBtnSlot()));
    connect(orderBtn, SIGNAL(clicked()), this, SLOT(orderBtnSlot()));
    connect(exitBtn, SIGNAL(clicked()), this, SLOT(exitBtnSlot()));
    connect(backBtn, SIGNAL(clicked()), this, SLOT(backBtnSlot()));

    pageList.push_back(stackedWidget->currentIndex());

    ui->ipAddress->setText("127.0.0.1");

    connectState = false;
    client = new TcpClient(this);

    connect(ui->connectBtn, SIGNAL(clicked()), this, SLOT(connectBtnSlot()));
    connect(client->socket, SIGNAL(connected()), this, SLOT(onConnectServer()));
    connect(client->socket, SIGNAL(readyRead()), this, SLOT(readMessage()));

    systemState = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete startBtn;
    delete[] pageWidget;
    delete stackedWidget;
}

void MainWindow::connectBtnSlot()
{
    if (connectState){
        client->socket->close();
        ui->tcpMessage->append("Close ...");
        connectState = false;
    }
    else{
        client->setIpAddress(ui->ipAddress->text());
        emit client->connectToServer();
    }
}

void MainWindow::onConnectServer()
{
    ui->tcpMessage->append("Connect complete ...");
    connectState = true;
    client->socket->write(QString::number(systemState).toUtf8());

    ui->centralWidget->hide();
    stackedWidget->setHidden(false);
}

void MainWindow::readMessage()
{
    QString rxMessage;
    QString rxData = client->socket->readAll();
    systemState = rxData.toInt();

    rxMessage = "Receive Data : " + rxData;
    ui->tcpMessage->append(rxMessage);

    if (systemState == 3){
        stackedWidget->setVisible(true);
        backBtn->setVisible(true);
        systemState = 4;
    }
}

void MainWindow::startBtnSlot()
{
    stackedWidget->setCurrentIndex(1);
    systemState = 1;
    backBtn->setVisible(false);

    sendMessage();
}

void MainWindow::listBtnSlot()
{

}

void MainWindow::cartBtnSlot()
{

}

void MainWindow::orderBtnSlot()
{
    systemState = 5;
    sendMessage();
    sendMessageOrderList();
}

void MainWindow::exitBtnSlot()
{
    pageList.clear();
    systemState = 0;
    sendMessage();
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::backBtnSlot()
{
    if (systemState == 4){
        stackedWidget->setCurrentIndex(pageList.back());
        pageList.pop_back();
    }
}

void MainWindow::sendMessage()
{
    QString txData = QString::number(systemState);
    client->socket->write(txData.toUtf8());
    QString txMessage = "Transmit Data : " + txData;
    ui->tcpMessage->append(txMessage);
}

void MainWindow::sendMessageOrderList()
{
    QString txData = "";
    if (!PID.empty()) {
        for(int i = 0; i < PID.length(); i++){
            txData += "," + PID[i];
        }
        client->socket->write(txData.toUtf8());
        QString txMessage = "Transmit Data : " + txData;
        ui->tcpMessage->append(txMessage);
    }
}

