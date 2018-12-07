#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1900, 1000);
    this->setWindowTitle("UI Monitor");

    ui->plcIP->setText("127.0.0.1");
    ui->pcIP->setText("127.0.0.1");
    ui->plcPort->setText("9999");
    ui->pcPort->setText("8888");

    plcClient = new TcpClient(this);
    pcClient = new TcpClient(this);

    connect(ui->plcConnectBtn, SIGNAL(clicked()), this, SLOT(plcConnectBtnSlot()));
    connect(plcClient->socket, SIGNAL(connected()), this, SLOT(onConnectPLCServer()));
    connect(plcClient->socket, SIGNAL(readyRead()), this, SLOT(readPLCMessage()));

    connect(ui->pcConnectBtn, SIGNAL(clicked()), this, SLOT(pcConnectBtnSlot()));
    connect(pcClient->socket, SIGNAL(connected()), this, SLOT(onConnectPCServer()));
    connect(pcClient->socket, SIGNAL(readyRead()), this, SLOT(readPCMessage()));

    connect(ui->uiStartBtn, SIGNAL(clicked()), this, SLOT(uiStartBtnSlot()));
    ui->uiStartBtn->setDisabled(true);

    timer = new QTimer(this);
    timer->setInterval(3000);
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_out()));

    stackedWidget = new SlidingStackedWidget(this);
    stackedWidget->hide();
    stackedWidget->setGeometry(this->rect());
    quint64 pages = 4;
    pageWidget = new QWidget[pages];
    for (quint64 i = 0; i < pages; i++) {
        pageWidget[i].setGeometry(stackedWidget->rect());
        stackedWidget->addWidget(&pageWidget[i]);
    }
    stackedWidget->setVerticalMode(true);
    stackedWidget->setSpeed(500);

    // 1 page
    startBtn = new QClickLabel(&pageWidget[startPage]);
    startBtn->setGeometry(pageWidget[startPage].rect());
    startBtn->setObjectName(QStringLiteral("startBtn"));
    QImage *startImage = new QImage();
    QPixmap *startImageBuffer = new QPixmap();
    double scale = 0.3;
    if (startImage->load(imageHeader + startIcon[0])){
        *startImageBuffer = QPixmap::fromImage(*startImage);
        *startImageBuffer = startImageBuffer->scaled(static_cast<int>(startImage->width()*scale), static_cast<int>(startImage->height()*scale));
    }
    startBtn->setPixmap(*startImageBuffer);
    startBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);

    resetBtn = new QPushButton(&pageWidget[startPage]);
    int resetBtnWidth = 100, resetBtnHeight = 70;
    resetBtn->setGeometry(pageWidget[startPage].width() - resetBtnWidth, pageWidget[0].height() - resetBtnHeight - resetBtnWidth - 20, resetBtnWidth, resetBtnHeight);
    resetBtn->setObjectName(QStringLiteral("resetBtn"));
    resetBtn->hide();

    // 2 Page
    int pageWidth = pageWidget[selectPage].width(), pageHeight = pageWidget[selectPage].height();
    QImage *colorImage = new QImage();
    QPixmap *colorImageBuffer = new QPixmap();
    QString text;

    for(int i = 0; i < 6; i++){
        colorBtn[i] = new QClickLabel(&pageWidget[selectPage]);
        colorBtn[i]->setGeometry(static_cast<int>(pageWidth*(0.04 + i*0.15)), static_cast<int>(pageHeight/3*0.3),
                           static_cast<int>(pageWidth / 6), static_cast<int>(pageHeight / 3));

        text = "Color " + QString::number(i + 1);
        colorBtn[i]->setObjectName(text);

        if (colorImage->load(imageHeader + colorIcon[i])){
            *colorImageBuffer = QPixmap::fromImage(*colorImage);
            *colorImageBuffer = colorImageBuffer->scaled(static_cast<int>(colorImage->width()*0.35), static_cast<int>(colorImage->height()*0.35));
        }
        colorBtn[i]->setPixmap(*colorImageBuffer);
        colorBtn[i]->setAlignment(Qt::AlignmentFlag::AlignCenter);

        connect(colorBtn[i], SIGNAL(mousePressed()), this, SLOT(colorBtnPressedSlot()));
        connect(colorBtn[i], SIGNAL(mouseReleased()), this, SLOT(colorBtnReleasedSlot()));
    }

    for(int i = 0; i < 7; i++){
        patternBtn[i] = new QClickLabel(&pageWidget[selectPage]);
        patternBtn[i]->setGeometry(static_cast<int>(pageWidth*(0.035 + i*0.13)), static_cast<int>(pageHeight/3*1.2),
                           static_cast<int>(pageWidth / 7), static_cast<int>(pageHeight / 3));

        text = "Pattern " + QString::number(i + 1);
        patternBtn[i]->setObjectName(text);

        if (colorImage->load(imageHeader + patternIcon[i])){
            *colorImageBuffer = QPixmap::fromImage(*colorImage);
            *colorImageBuffer = colorImageBuffer->scaled(static_cast<int>(colorImage->width()*0.35), static_cast<int>(colorImage->height()*0.35));
        }
        patternBtn[i]->setPixmap(*colorImageBuffer);
        patternBtn[i]->setAlignment(Qt::AlignmentFlag::AlignCenter);

        connect(patternBtn[i], SIGNAL(mousePressed()), this, SLOT(patternBtnPressedSlot()));
        connect(patternBtn[i], SIGNAL(mouseReleased()), this, SLOT(patternBtnReleasedSlot()));
    }

    layout = new QWidget(&pageWidget[selectPage]);
    layout->setGeometry(0, pageHeight/5*4, pageWidth, pageHeight/5);
    orderBtn = new QClickLabel(layout);
    layoutWidth = layout->width();
    layoutHeight = layout->height();
    orderBtn->setGeometry(layoutWidth/6, 0, layoutWidth/3*2, layoutHeight/5*4);
    orderBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);

    QImage *image = new QImage();
    QPixmap *buffer = new QPixmap();
    if (image->load(imageHeader + orderIcon[0])){
        *buffer = QPixmap::fromImage(*image);
        *buffer = buffer->scaled(static_cast<int>(image->width()*0.2), static_cast<int>(image->height()*0.2));
    }
    orderBtn->setPixmap(*buffer);
    orderBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);

    connect(orderBtn, SIGNAL(mousePressed()), this, SLOT(orderBtnPressedSlot()));
    connect(orderBtn, SIGNAL(mouseReleased()), this, SLOT(orderBtnReleasedSlot()));

    pageWidth = pageWidget[selectPage].width();
    pageHeight = pageWidget[selectPage].height();
    itemLabel = new QLabel(&pageWidget[selectPage]);
    itemLabel->setGeometry(0, 0, static_cast<int>(pageWidth), static_cast<int>(pageHeight));
    if (image->load(imageHeader + clerkIcon)){
        *buffer = QPixmap::fromImage(*image);
        *buffer = buffer->scaled(static_cast<int>(image->width()*0.2), static_cast<int>(image->height()*0.2));
    }
    itemLabel->setPixmap(*buffer);
    itemLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

    itemLabel->hide();

    // 3 page
    itemText = new QLabel(&pageWidget[itemPage]);
    itemText->setGeometry(static_cast<int>(pageWidth*0.2), static_cast<int>(pageHeight*0.2), static_cast<int>(pageWidth*0.6), static_cast<int>(pageHeight*0.6));
    itemText->setAlignment(Qt::AlignmentFlag::AlignCenter);

    waitText = new QLabel(&pageWidget[itemPage]);
    waitText->setGeometry(static_cast<int>(pageWidth*0.2), static_cast<int>(pageHeight*0.8), static_cast<int>(pageWidth*0.6), static_cast<int>(pageHeight*0.2));
    if (image->load(imageHeader + waitIcon)){
        *buffer = QPixmap::fromImage(*image);
        *buffer = buffer->scaled(static_cast<int>(image->width()*0.12), static_cast<int>(image->height()*0.12));
    }
    waitText->setPixmap(*buffer);
    waitText->setAlignment(Qt::AlignmentFlag::AlignCenter);

    // 4 page
    thankText = new QLabel(&pageWidget[thankPage]);
    thankText->setGeometry(0, 0, pageWidth, pageHeight);

    if (image->load(imageHeader + thankIcon)){
        *buffer = QPixmap::fromImage(*image);
        *buffer = buffer->scaled(static_cast<int>(image->width()*0.7), static_cast<int>(image->height()*0.7));
    }
    thankText->setPixmap(*buffer);
    thankText->setAlignment(Qt::AlignmentFlag::AlignCenter);

    connect(startBtn, SIGNAL(mousePressed()), this, SLOT(startBtnPressedSlot()));
    connect(startBtn, SIGNAL(mouseReleased()), this, SLOT(startBtnReleasedSlot()));
    connect(resetBtn, SIGNAL(clicked()), this, SLOT(resetBtnSlot()));

    connectStatePLC = false;
    connectStatePC = false;

    systemState = 0;
    colorIndex = -1;
    patternIndex = -1;
}

void MainWindow::init(){
//    stackedWidget->setCurrentIndex(startPage);
//    stackedWidget->slideInIdx(startPage);
    systemState = 0;
    colorIndex = -1;
    patternIndex = -1;

    QImage *colorImage = new QImage();
    QPixmap *colorImageBuffer = new QPixmap();
    QString text;

    for(int i = 0; i < 6; i++){
        if (colorImage->load(imageHeader + colorIcon[i])){
            *colorImageBuffer = QPixmap::fromImage(*colorImage);
            *colorImageBuffer = colorImageBuffer->scaled(static_cast<int>(colorImage->width()*0.35), static_cast<int>(colorImage->height()*0.35));
        }
        colorBtn[i]->setPixmap(*colorImageBuffer);
        colorBtn[i]->setAlignment(Qt::AlignmentFlag::AlignCenter);
    }

    for(int i = 0; i < 7; i++){
        if (colorImage->load(imageHeader + patternIcon[i])){
            *colorImageBuffer = QPixmap::fromImage(*colorImage);
            *colorImageBuffer = colorImageBuffer->scaled(static_cast<int>(colorImage->width()*0.35), static_cast<int>(colorImage->height()*0.35));
        }
        patternBtn[i]->setPixmap(*colorImageBuffer);
        patternBtn[i]->setAlignment(Qt::AlignmentFlag::AlignCenter);
    }

    orderBtn->setGeometry(layoutWidth/6, 0, layoutWidth/3*2, layoutHeight/5*4);
    QImage *image = new QImage();
    QPixmap *buffer = new QPixmap();
    if (image->load(imageHeader + orderIcon[0])){
        *buffer = QPixmap::fromImage(*image);
        *buffer = buffer->scaled(static_cast<int>(image->width()*0.2), static_cast<int>(image->height()*0.2));
    }
    orderBtn->setPixmap(*buffer);
    orderBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);
}

MainWindow::~MainWindow() {
    delete startBtn;
    delete[] pageWidget;
    delete stackedWidget;
}

void MainWindow::plcConnectBtnSlot()
{
    if (connectStatePLC) {
        plcClient->socket->close();
        ui->tcpMessage->append("PLC Server Close ...");
        connectStatePLC = false;
        ui->plcConnectBtn->setText("Connect");
        ui->uiStartBtn->setDisabled(true);
    }
    else {
        plcClient->setIpAddress(ui->plcIP->text());
        plcClient->setPort(ui->plcPort->text().toUShort());
        emit plcClient->connectToServer();
    }
}

void MainWindow::pcConnectBtnSlot()
{
    if (connectStatePC) {
        pcClient->socket->close();
        ui->tcpMessage->append("PC Server Close ...");
        connectStatePC = false;
        ui->pcConnectBtn->setText("Connect");
        ui->uiStartBtn->setDisabled(true);
    }
    else {
        pcClient->setIpAddress(ui->pcIP->text());
        pcClient->setPort(ui->pcPort->text().toUShort());
        emit pcClient->connectToServer();
    }
}

void MainWindow::onConnectPLCServer()
{
    ui->tcpMessage->append("PLC Server Connect complete ...");
    connectStatePLC = true;
    ui->plcConnectBtn->setText("Disconnect");

    if(connectStatePLC & connectStatePC)
        ui->uiStartBtn->setEnabled(true);
}

void MainWindow::onConnectPCServer()
{
    ui->tcpMessage->append("PC Server Connect complete ...");
    connectStatePC = true;
    ui->pcConnectBtn->setText("Disconnect");

    if(connectStatePLC & connectStatePC)
        ui->uiStartBtn->setEnabled(true);
}

void MainWindow::readPLCMessage()
{
    QString rxMessage;
    QString rxData = plcClient->socket->readAll();
    systemState = rxData.toInt();

    rxMessage = "Receive Data(From PLC) : " + rxData;
    ui->tcpMessage->append(rxMessage);

    if (systemState == 3) {
        stackedWidget->setVisible(true);
        systemState = 4;
        itemLabel->hide();
    }
    else if(systemState == 7){
//        stackedWidget->setCurrentIndex(thankPage);
        stackedWidget->slideInIdx(thankPage, stackedWidget->BOTTOM2TOP);
        timer->start();
    }
}

void MainWindow::readPCMessage()
{
    QString rxMessage;
    QString rxData = pcClient->socket->readAll();
    systemState = rxData.toInt();

    rxMessage = "Receive Data(From PC) : " + rxData;
    ui->tcpMessage->append(rxMessage);

    if (systemState == 3) {
        stackedWidget->setVisible(true);
        systemState = 4;
        itemLabel->hide();
    }
    else if(systemState == 7){
//        stackedWidget->setCurrentIndex(thankPage);
        stackedWidget->slideInIdx(thankPage, stackedWidget->BOTTOM2TOP);
        timer->start();
    }
}

void MainWindow::uiStartBtnSlot(){
    ui->centralWidget->hide();
    stackedWidget->setHidden(false);
}

void MainWindow::timer_out(){
    timer->stop();
    init();
    systemState = 0;
//    stackedWidget->setCurrentIndex(selectPage);
//    stackedWidget->slideInIdx(selectPage, stackedWidget->TOP2BOTTOM);

    sendMessage();
    stackedWidget->slideInIdx(startPage, stackedWidget->TOP2BOTTOM);
}

void MainWindow::startBtnPressedSlot(){
    QImage *startImage = new QImage();
    QPixmap *startImageBuffer = new QPixmap();
    if (startImage->load(imageHeader + startIcon[1])){
        *startImageBuffer = QPixmap::fromImage(*startImage);
        *startImageBuffer = startImageBuffer->scaled(static_cast<int>(startImage->width()*0.3), static_cast<int>(startImage->height()*0.3));
    }
    else{
        qDebug() << "Image load Error";
    }
    startBtn->setPixmap(*startImageBuffer);
    startBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);
}

void MainWindow::startBtnReleasedSlot(){
    QImage *startImage = new QImage();
    QPixmap *startImageBuffer = new QPixmap();
    if (startImage->load(imageHeader + startIcon[0])){
        *startImageBuffer = QPixmap::fromImage(*startImage);
        *startImageBuffer = startImageBuffer->scaled(static_cast<int>(startImage->width()*0.3), static_cast<int>(startImage->height()*0.3));
    }
    else{
        qDebug() << "Image load Error";
    }
    startBtn->setPixmap(*startImageBuffer);
    startBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);

//    stackedWidget->setCurrentIndex(selectPage);
    stackedWidget->slideInIdx(selectPage, stackedWidget->BOTTOM2TOP);
    systemState = 1;
    pageList.push_back(stackedWidget->currentIndex());

    sendMessage();

}

void MainWindow::resetBtnSlot()
{
    ui->centralWidget->setHidden(false);
    stackedWidget->setHidden(true);
    colorIndex = -1;
    patternIndex = -1;
}

void MainWindow::colorBtnPressedSlot(){
//    qDebug() << sender()->objectName();
    int index = sender()->objectName().split(" ")[1].toInt() - 1;
    QImage *image = new QImage();
    QPixmap *buffer = new QPixmap();
    for(int i = 0; i < 6; i++){
        QString append_text = "";
        if (i == index){
            colorIndex = index;

            if (image->load(imageHeader + "m" + QString::number(i+1))){
                *buffer = QPixmap::fromImage(*image);
                *buffer = buffer->scaled(static_cast<int>(image->width()*0.35), static_cast<int>(image->height()*0.35));
            }
        }
        else{
            if (image->load(imageHeader + colorIcon[i])){
                *buffer = QPixmap::fromImage(*image);
                *buffer = buffer->scaled(static_cast<int>(image->width()*0.35), static_cast<int>(image->height()*0.35));
            }
        }

        colorBtn[i]->setPixmap(*buffer);
        colorBtn[i]->setAlignment(Qt::AlignmentFlag::AlignCenter);
    }
}

void MainWindow::colorBtnReleasedSlot(){
    combineItem();
}

void MainWindow::patternBtnPressedSlot(){
//    qDebug() << sender()->objectName();
    int index = sender()->objectName().split(" ")[1].toInt() - 1;
    QImage *image = new QImage();
    QPixmap *buffer = new QPixmap();
    for(int i = 0; i < 7; i++){
        QString append_text = "";
        if (i == index){
            patternIndex = index;

            if (image->load(imageHeader + "gb" + QString::number(i+1))){
                *buffer = QPixmap::fromImage(*image);
                *buffer = buffer->scaled(static_cast<int>(image->width()*0.35), static_cast<int>(image->height()*0.35));
            }
        }
        else{
            if (image->load(imageHeader + patternIcon[i])){
                *buffer = QPixmap::fromImage(*image);
                *buffer = buffer->scaled(static_cast<int>(image->width()*0.35), static_cast<int>(image->height()*0.35));
            }
        }
        patternBtn[i]->setPixmap(*buffer);
        patternBtn[i]->setAlignment(Qt::AlignmentFlag::AlignCenter);
    }
}

void MainWindow::patternBtnReleasedSlot(){
    combineItem();
}

void MainWindow::combineItem(){
    if (colorIndex >= 0 && patternIndex >= 0){
        orderBtn->setGeometry(layoutWidth/3, 0, layoutWidth/3, layoutHeight/5*4);
        QImage *image = new QImage();
        QPixmap *buffer = new QPixmap();
        if (image->load(imageHeader + orderIcon[1])){
            *buffer = QPixmap::fromImage(*image);
            *buffer = buffer->scaled(static_cast<int>(image->width()*0.2), static_cast<int>(image->height()*0.2));
        }
        orderBtn->setPixmap(*buffer);
        orderBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);
    }
}

void MainWindow::orderBtnPressedSlot(){
    if (colorIndex >= 0 && patternIndex >= 0){
        QImage *image = new QImage();
        QPixmap *buffer = new QPixmap();
        if (image->load(imageHeader + orderIcon[2])){
            *buffer = QPixmap::fromImage(*image);
            *buffer = buffer->scaled(static_cast<int>(image->width()*0.2), static_cast<int>(image->height()*0.2));
        }
        orderBtn->setPixmap(*buffer);
        orderBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);
    }
}

void MainWindow::orderBtnReleasedSlot(){
    if (colorIndex >= 0 && patternIndex >= 0){
        QImage *image = new QImage();
        QPixmap *buffer = new QPixmap();
        if (image->load(imageHeader + orderIcon[1])){
            *buffer = QPixmap::fromImage(*image);
            *buffer = buffer->scaled(static_cast<int>(image->width()*0.2), static_cast<int>(image->height()*0.2));
        }
        orderBtn->setPixmap(*buffer);
        orderBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);
    }

    if (systemState == 4 || systemState == 7){
        systemState = 5;
        systemState = systemState*100 + (colorIndex*col + patternIndex + 1);
        sendMessage();
//        stackedWidget->setCurrentIndex(itemPage);
        stackedWidget->slideInIdx(itemPage, stackedWidget->BOTTOM2TOP);
//        itemText->setText(objName);
        int itemIndex = colorIndex*col + patternIndex;
//        qDebug() << itemIndex;
        QImage *image = new QImage();
        QPixmap *buffer = new QPixmap();
        if (image->load(imageHeader + imageIcon[itemIndex])){
            *buffer = QPixmap::fromImage(*image);
            *buffer = buffer->scaled(static_cast<int>(image->width()*4), static_cast<int>(image->height()*4));
        }
        else{
            qDebug() << "Image load Error";
        }
        itemText->setPixmap(*buffer);
        itemText->setAlignment(Qt::AlignmentFlag::AlignCenter);
    }
    else if (systemState == 1){
        itemLabel->setVisible(true);
    }
}

void MainWindow::sendMessage()
{
    QString txData = QString::number(systemState);
    pcClient->socket->write(txData.toUtf8());
    QString txMessage = "Transmit Data : " + txData;
    ui->tcpMessage->append(txMessage);
}
