#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Server");
    this->setFixedSize(400, 300);
    this->setWindowFlags(Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);

    connect(ui->listenBtn, SIGNAL(clicked()), this, SLOT(listenBtnSlot()));
    connect(ui->disconnectBtn, SIGNAL(clicked()), this, SLOT(disconnectBtnSlot()));
    connect(ui->sendBtn, SIGNAL(clicked()), this, SLOT(sendBtnSlot()));

    ui->ipAddress->setText("127.0.0.1");

    server = new TcpServer;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete server;
}

void MainWindow::listenBtnSlot()
{
    server->setIpAddress(ui->ipAddress->text());
    server->startServer();
}

void MainWindow::disconnectBtnSlot()
{

}

void MainWindow::sendBtnSlot()
{

}
