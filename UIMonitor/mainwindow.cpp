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
    quint64 pages = 4;
    pageWidget = new QWidget[pages];
    for (quint64 i = 0; i < pages; i++) {
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

    resetBtn = new QPushButton(&pageWidget[0]);
    int resetBtnWidth = 100, resetBtnHeight = 70;
    resetBtn->setGeometry(pageWidget[0].width() - resetBtnWidth, pageWidget[0].height() - resetBtnHeight - startBtnHeight - 20, resetBtnWidth, resetBtnHeight);
    resetBtn->setText("Reset");
    resetBtn->setFont(btnFont);
    resetBtn->setObjectName(QStringLiteral("resetBtn"));
    resetBtn->hide();

    // 2 page
    backBtn = new QPushButton(&pageWidget[1]);
    backBtn->setText("Quit");
    backBtn->setFont(btnFont);
    backBtn->setObjectName(QStringLiteral("backBtn"));
    int backBtnWidth = 100, backBtnHeight = 50;
    backBtn->setGeometry(pageWidget[0].width() - backBtnWidth, pageWidget[0].height() - backBtnHeight, backBtnWidth, backBtnHeight);

    int pageWidth = pageWidget[1].width() - backBtnWidth, pageHeight = pageWidget[1].height();
    btnFont.setPointSize(15);
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 8; j++) {
            itemBtn[i * 8 + j] = new QPushButton(&pageWidget[1]);
            itemBtn[i * 8 + j]->setGeometry(static_cast<int>(pageWidth*(0.05 + j*0.12)), static_cast<int>(pageHeight*(0.05 + i*0.15)), static_cast<int>(pageWidth / 12), static_cast<int>(pageHeight / 8));
            QString itemText = "Item " + QString::number(i*8+j + 1);
            itemBtn[i * 8 + j]->setFont(btnFont);
            itemBtn[i * 8 + j]->setObjectName(itemText);
            connect(itemBtn[i * 8 + j], SIGNAL(clicked()), this, SLOT(itemBtnSlot()));
            if (i == 0 && j == 0){
                QImage *image = new QImage();
                QPixmap *buffer = new QPixmap();
                if(image->load("../tobot.jpg"))
                {
                    *buffer = QPixmap::fromImage(*image);
                    *buffer = buffer->scaled(image->width(),image->height());
                }
                else
                {
                    qDebug() << "Image load Error";
                }
                itemBtn[i * 8 + j]->setAutoFillBackground(true);
                QPalette palette = itemBtn[i * 8 + j]->palette();
                palette.setColor(QPalette::Button,QColor(82,110,166));
                itemBtn[i * 8 + j]->setPalette(palette);
                itemBtn[i * 8 + j]->setIcon(QIcon("../tobot.jpg"));
                itemBtn[i * 8 + j]->setIconSize(QSize(itemBtn[i * 8 + j]->width(),itemBtn[i * 8 + j]->height()));
            }
            else{
                itemBtn[i * 8 + j]->setText(itemText);
            }
        }
    }

    itemLabel = new QLabel(&pageWidget[1]);
    itemLabel->setText("Please Wait....");
    itemLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
    itemLabel->setGeometry(0, 0, static_cast<int>(pageWidth), static_cast<int>(pageHeight));
    QFont font;
    font.setPointSize(50);
    font.setBold(true);
    itemLabel->setFont(font);
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::transparent);
    itemLabel->setAutoFillBackground(true);
    itemLabel->setPalette(palette);
    itemLabel->hide();

    // 3 page
    itemText = new QLabel(&pageWidget[2]);
    QFont page3LabelFont;
    page3LabelFont.setPointSize(50);
    page3LabelFont.setBold(true);
    itemText->setFont(page3LabelFont);
    itemText->setGeometry(static_cast<int>(pageWidth*0.2), static_cast<int>(pageHeight*0.2), static_cast<int>(pageWidth*0.6), static_cast<int>(pageHeight*0.6));
    itemText->setAlignment(Qt::AlignmentFlag::AlignCenter);

    // 4 page
    thankText = new QLabel(&pageWidget[3]);
    QFont page4LabelFont;
    page4LabelFont.setPointSize(50);
    page4LabelFont.setBold(true);
    thankText->setFont(page4LabelFont);
    thankText->setText("Thank you !!\n!^^!");
    thankText->setGeometry(static_cast<int>(pageWidth*0.2), static_cast<int>(pageHeight*0.2), static_cast<int>(pageWidth*0.6), static_cast<int>(pageHeight*0.6));
    thankText->setAlignment(Qt::AlignmentFlag::AlignCenter);

    connect(startBtn, SIGNAL(clicked()), this, SLOT(startBtnSlot()));
    connect(backBtn, SIGNAL(clicked()), this, SLOT(backBtnSlot()));
    connect(resetBtn, SIGNAL(clicked()), this, SLOT(resetBtnSlot()));

    ui->ipAddress->setText("127.0.0.1");

    connectState = false;
    client = new TcpClient(this);

    connect(ui->connectBtn, SIGNAL(clicked()), this, SLOT(connectBtnSlot()));
    connect(client->socket, SIGNAL(connected()), this, SLOT(onConnectServer()));
    connect(client->socket, SIGNAL(readyRead()), this, SLOT(readMessage()));

    timer = new QTimer(this);
    timer->setInterval(3000);
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_out()));

    systemState = 0;
}

MainWindow::~MainWindow() {
    delete startBtn;
    delete[] pageWidget;
    delete stackedWidget;
}

void MainWindow::connectBtnSlot()
{
    if (connectState) {
        client->socket->close();
        ui->tcpMessage->append("Close ...");
        connectState = false;
        ui->connectBtn->setText("Connect");
    }
    else {
        client->setIpAddress(ui->ipAddress->text());
        emit client->connectToServer();
    }
}

void MainWindow::onConnectServer()
{
    ui->tcpMessage->append("Connect complete ...");
    connectState = true;
    client->socket->write(QString::number(systemState).toUtf8());
    ui->connectBtn->setText("Disconnect");

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

    if (systemState == 3) {
        stackedWidget->setVisible(true);
        systemState = 4;
        itemLabel->hide();
    }
    else if(systemState == 7){
        stackedWidget->setCurrentIndex(3);
        timer->start();
    }
}

void MainWindow::timer_out(){
    stackedWidget->setCurrentIndex(1);
    timer->stop();
}

void MainWindow::startBtnSlot()
{
    stackedWidget->setCurrentIndex(1);
    systemState = 1;
    pageList.push_back(stackedWidget->currentIndex());

    sendMessage();
}

void MainWindow::resetBtnSlot()
{
    ui->centralWidget->setHidden(false);
    stackedWidget->setHidden(true);
}

void MainWindow::itemBtnSlot()
{
    if (systemState == 4 || systemState == 7){
        systemState = 5;
        QString objName = sender()->objectName();
        for(int i = 0; i < 48; i++){
            if (objName.compare(itemBtn[i]->objectName()) == 0){
                systemState = systemState*100 + (i +1);
                sendMessage();
            }
        }
        stackedWidget->setCurrentIndex(2);
        itemText->setText(objName);
        QImage *image = new QImage();
        QPixmap *buffer = new QPixmap();
        if(image->load("../tobot.jpg"))
        {
            *buffer = QPixmap::fromImage(*image);
            *buffer = buffer->scaled(image->width(),image->height());
        }
        else
        {
            qDebug() << "Image load Error";
        }
        itemText->setPixmap(*buffer);
    }
    else if (systemState == 1){
        itemLabel->setVisible(true);
    }
}

void MainWindow::backBtnSlot()
{
    stackedWidget->setCurrentIndex(0);
    systemState = 0;
    sendMessage();
}

void MainWindow::sendMessage()
{
    QString txData = QString::number(systemState);
    client->socket->write(txData.toUtf8());
    QString txMessage = "Transmit Data : " + txData;
    ui->tcpMessage->append(txMessage);
}
