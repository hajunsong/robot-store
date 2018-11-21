#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1280, 720);
    this->setWindowTitle("UI Monitor");

    ui->ipAddress->setText("127.0.0.1");

    client = new TcpClient(this);

    connect(ui->connectBtn, SIGNAL(clicked()), this, SLOT(connectBtnSlot()));
    connect(client->socket, SIGNAL(connected()), this, SLOT(onConnectServer()));
    connect(client->socket, SIGNAL(readyRead()), this, SLOT(readMessage()));

    timer = new QTimer(this);
    timer->setInterval(3000);
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_out()));

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
    startBtn = new QClickLabel(&pageWidget[startPage]);
//    int startBtnWidth = 160, startBtnHeight = 160;
    startBtn->setGeometry(pageWidget[startPage].rect());
    startBtn->setObjectName(QStringLiteral("startBtn"));
    QImage *startImage = new QImage();
    QPixmap *startImageBuffer = new QPixmap();
    if (startImage->load(imagePath + startIcon[0])){
        *startImageBuffer = QPixmap::fromImage(*startImage);
        *startImageBuffer = startImageBuffer->scaled(static_cast<int>(startImage->width()*3), static_cast<int>(startImage->height()*3));
    }
    startBtn->setPixmap(*startImageBuffer);
    startBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);

    resetBtn = new QPushButton(&pageWidget[startPage]);
    int resetBtnWidth = 100, resetBtnHeight = 70;
    resetBtn->setGeometry(pageWidget[startPage].width() - resetBtnWidth, pageWidget[0].height() - resetBtnHeight - resetBtnWidth - 20, resetBtnWidth, resetBtnHeight);
    resetBtn->setText("Reset");
    QFont btnFont;
    btnFont.setPointSize(15);
    btnFont.setBold(true);
    resetBtn->setFont(btnFont);
    resetBtn->setObjectName(QStringLiteral("resetBtn"));
    resetBtn->hide();

    // 2 page
    backBtn = new QPushButton(&pageWidget[selectPage]);
    backBtn->setText("Quit");
    backBtn->setFont(btnFont);
    backBtn->setObjectName(QStringLiteral("backBtn"));
    int backBtnWidth = 100, backBtnHeight = 50;
    backBtn->setGeometry(pageWidget[selectPage].width() - backBtnWidth, pageWidget[selectPage].height() - backBtnHeight, backBtnWidth, backBtnHeight);

    int pageWidth = pageWidget[selectPage].width() - backBtnWidth, pageHeight = pageWidget[selectPage].height();
    QImage *colorImage = new QImage();
    QPixmap *colorImageBuffer = new QPixmap();
    QString text;

    for(int i = 0; i < 6; i++){
        colorBtn[i] = new QClickLabel(&pageWidget[selectPage]);
        colorBtn[i]->setGeometry(static_cast<int>(pageWidth*(0.055 + i*0.15)), static_cast<int>(pageHeight/3*0.3),
                           static_cast<int>(pageWidth / 6), static_cast<int>(pageHeight / 3));

        text = "Color " + QString::number(i + 1);
        colorBtn[i]->setObjectName(text);

        if (colorImage->load(imagePath + colorIcon[i])){
            *colorImageBuffer = QPixmap::fromImage(*colorImage);
            *colorImageBuffer = colorImageBuffer->scaled(static_cast<int>(colorImage->width()*0.7), static_cast<int>(colorImage->height()*0.7));
        }
        colorBtn[i]->setPixmap(*colorImageBuffer);
        colorBtn[i]->setAlignment(Qt::AlignmentFlag::AlignCenter);

        connect(colorBtn[i], SIGNAL(mousePressed()), this, SLOT(colorBtnPressedSlot()));
        connect(colorBtn[i], SIGNAL(mouseReleased()), this, SLOT(colorBtnReleasedSlot()));
    }

    for(int i = 0; i < 7; i++){
        patternBtn[i] = new QClickLabel(&pageWidget[selectPage]);
        patternBtn[i]->setGeometry(static_cast<int>(pageWidth*(0.05 + i*0.13)), static_cast<int>(pageHeight/3*1.2),
                           static_cast<int>(pageWidth / 7), static_cast<int>(pageHeight / 3));

        text = "Pattern " + QString::number(i + 1);
        patternBtn[i]->setObjectName(text);

        if (colorImage->load(imagePath + patternIcon[i])){
            *colorImageBuffer = QPixmap::fromImage(*colorImage);
            *colorImageBuffer = colorImageBuffer->scaled(static_cast<int>(colorImage->width()*0.7), static_cast<int>(colorImage->height()*0.7));
        }
        patternBtn[i]->setPixmap(*colorImageBuffer);
        patternBtn[i]->setAlignment(Qt::AlignmentFlag::AlignCenter);

        connect(patternBtn[i], SIGNAL(mousePressed()), this, SLOT(patternBtnPressedSlot()));
        connect(patternBtn[i], SIGNAL(mouseReleased()), this, SLOT(patternBtnReleasedSlot()));
    }

    QWidget *layout = new QWidget(&pageWidget[selectPage]);
    layout->setGeometry(0, pageHeight/5*4, pageWidth, pageHeight/5);
    orderBtn = new QClickLabel(layout);
    int layoutWidth = layout->width();
    int layoutHeight = layout->height();
    orderBtn->setGeometry(layoutWidth/3, 0, layoutWidth/3, layoutHeight/5*4);
    QFont font;
    font.setPointSize(25);
    font.setBold(true);
    orderBtn->setFont(font);
    orderBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);
    orderBtn->setText("");

    QImage *image = new QImage();
    QPixmap *buffer = new QPixmap();
    if (image->load(imagePath + orderIcon[2])){
        *buffer = QPixmap::fromImage(*image);
        *buffer = buffer->scaled(static_cast<int>(orderBtn->width()), static_cast<int>(orderBtn->height()*0.8));
    }
    orderBtn->setPixmap(*buffer);
    orderBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);

    orderText = new QClickLabel(layout);
    orderText->setGeometry(layoutWidth/3, 0, layoutWidth/3, layoutHeight/5*4);
    orderText->setFont(font);
    orderText->setText("Choose among these\nfavorite color & pattern");
    orderText->setAlignment(Qt::AlignmentFlag::AlignCenter);

    connect(orderText, SIGNAL(mousePressed()), this, SLOT(orderBtnPressedSlot()));
    connect(orderText, SIGNAL(mouseReleased()), this, SLOT(orderBtnReleasedSlot()));

    itemLabel = new QLabel(&pageWidget[selectPage]);
    itemLabel->setText("The clerk robot is coming...\nPlease Wait....");
    itemLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
    itemLabel->setGeometry(0, 0, static_cast<int>(pageWidth), static_cast<int>(pageHeight));
    font.setPointSize(50);
    font.setBold(true);
    itemLabel->setFont(font);
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::transparent);
    itemLabel->setAutoFillBackground(true);
    itemLabel->setPalette(palette);
    itemLabel->hide();

    // 3 page
    itemText = new QLabel(&pageWidget[itemPage]);
    QFont page3LabelFont;
    page3LabelFont.setPointSize(50);
    page3LabelFont.setBold(true);
    itemText->setFont(page3LabelFont);
    itemText->setGeometry(static_cast<int>(pageWidth*0.2), static_cast<int>(pageHeight*0.2), static_cast<int>(pageWidth*0.6), static_cast<int>(pageHeight*0.6));
    itemText->setAlignment(Qt::AlignmentFlag::AlignCenter);

    waitText = new QLabel(&pageWidget[itemPage]);
    page3LabelFont.setPointSize(30);
    waitText->setFont(page3LabelFont);
    waitText->setGeometry(static_cast<int>(pageWidth*0.2), static_cast<int>(pageHeight*0.8), static_cast<int>(pageWidth*0.6), static_cast<int>(pageHeight*0.2));
    waitText->setAlignment(Qt::AlignmentFlag::AlignCenter);
    waitText->setText("Robot is taking goods out.\nPlease wait...");

    // 4 page
    thankText = new QLabel(&pageWidget[thankPage]);
    QFont page4LabelFont;
    page4LabelFont.setPointSize(50);
    page4LabelFont.setBold(true);
    thankText->setFont(page4LabelFont);
    thankText->setText("Thank you !!\n!^^!");
    thankText->setGeometry(static_cast<int>(pageWidth*0.2), static_cast<int>(pageHeight*0.2), static_cast<int>(pageWidth*0.6), static_cast<int>(pageHeight*0.6));
    thankText->setAlignment(Qt::AlignmentFlag::AlignCenter);

    connect(startBtn, SIGNAL(mousePressed()), this, SLOT(startBtnPressedSlot()));
    connect(startBtn, SIGNAL(mouseReleased()), this, SLOT(startBtnReleasedSlot()));
    connect(backBtn, SIGNAL(clicked()), this, SLOT(backBtnSlot()));
    connect(resetBtn, SIGNAL(clicked()), this, SLOT(resetBtnSlot()));

    connectState = false;

    systemState = 0;
    colorIndex = -1;
    patternIndex = -1;
}

void MainWindow::init(){
    stackedWidget->setCurrentIndex(startPage);
    systemState = 0;
    colorIndex = -1;
    patternIndex = -1;

    QImage *colorImage = new QImage();
    QPixmap *colorImageBuffer = new QPixmap();
    QString text;

    for(int i = 0; i < 6; i++){
        if (colorImage->load(imagePath + colorIcon[i])){
            *colorImageBuffer = QPixmap::fromImage(*colorImage);
            *colorImageBuffer = colorImageBuffer->scaled(static_cast<int>(colorImage->width()*0.7), static_cast<int>(colorImage->height()*0.7));
        }
        colorBtn[i]->setPixmap(*colorImageBuffer);
        colorBtn[i]->setAlignment(Qt::AlignmentFlag::AlignCenter);
    }

    for(int i = 0; i < 7; i++){
        if (colorImage->load(imagePath + patternIcon[i])){
            *colorImageBuffer = QPixmap::fromImage(*colorImage);
            *colorImageBuffer = colorImageBuffer->scaled(static_cast<int>(colorImage->width()*0.7), static_cast<int>(colorImage->height()*0.7));
        }
        patternBtn[i]->setPixmap(*colorImageBuffer);
        patternBtn[i]->setAlignment(Qt::AlignmentFlag::AlignCenter);
    }

    QImage *image = new QImage();
    QPixmap *buffer = new QPixmap();
    if (image->load(imagePath + orderIcon[2])){
        *buffer = QPixmap::fromImage(*image);
        *buffer = buffer->scaled(static_cast<int>(orderBtn->width()), static_cast<int>(orderBtn->height()*0.8));
    }
    orderBtn->setPixmap(*buffer);
    orderBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);

    orderText->setText("Choose among these\nfavorite color & pattern");
    orderText->setAlignment(Qt::AlignmentFlag::AlignCenter);
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
    client->socket->write(QString::number(100).toUtf8());
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
        stackedWidget->setCurrentIndex(thankPage);
        timer->start();
    }
}

void MainWindow::timer_out(){
    timer->stop();
    init();
    systemState = 7;
    stackedWidget->setCurrentIndex(selectPage);
}

void MainWindow::startBtnPressedSlot(){
    QImage *startImage = new QImage();
    QPixmap *startImageBuffer = new QPixmap();
    if (startImage->load(imagePath + startIcon[1])){
        *startImageBuffer = QPixmap::fromImage(*startImage);
        *startImageBuffer = startImageBuffer->scaled(static_cast<int>(startImage->width()*3), static_cast<int>(startImage->height()*3));
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
    if (startImage->load(imagePath + startIcon[0])){
        *startImageBuffer = QPixmap::fromImage(*startImage);
        *startImageBuffer = startImageBuffer->scaled(static_cast<int>(startImage->width()*3), static_cast<int>(startImage->height()*3));
    }
    else{
        qDebug() << "Image load Error";
    }
    startBtn->setPixmap(*startImageBuffer);
    startBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);

    stackedWidget->setCurrentIndex(selectPage);
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
            append_text = "_select";
            colorIndex = index;
        }
        if (image->load(imagePath + colorIcon[i] + append_text)){
            *buffer = QPixmap::fromImage(*image);
            *buffer = buffer->scaled(static_cast<int>(image->width()*0.7), static_cast<int>(image->height()*0.7));
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
            append_text = "_select";
            patternIndex = index;
        }
        if (image->load(imagePath + patternIcon[i] + append_text)){
            *buffer = QPixmap::fromImage(*image);
            *buffer = buffer->scaled(static_cast<int>(image->width()*0.7), static_cast<int>(image->height()*0.7));
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
        QImage *image = new QImage();
        QPixmap *buffer = new QPixmap();
        if (image->load(imagePath + orderIcon[0])){
            *buffer = QPixmap::fromImage(*image);
            *buffer = buffer->scaled(static_cast<int>(orderBtn->width()), static_cast<int>(orderBtn->height()*0.8));
        }
        orderBtn->setPixmap(*buffer);
        orderBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);
        orderText->setText("Order goods");
    }
}

void MainWindow::orderBtnPressedSlot(){
    if (colorIndex >= 0 && patternIndex >= 0){
        QImage *image = new QImage();
        QPixmap *buffer = new QPixmap();
        if (image->load(imagePath + orderIcon[1])){
            *buffer = QPixmap::fromImage(*image);
            *buffer = buffer->scaled(static_cast<int>(orderBtn->width()), static_cast<int>(orderBtn->height()*0.8));
        }
        orderBtn->setPixmap(*buffer);
        orderBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);
    }
}

void MainWindow::orderBtnReleasedSlot(){
    if (colorIndex >= 0 && patternIndex >= 0){
        QImage *image = new QImage();
        QPixmap *buffer = new QPixmap();
        if (image->load(imagePath + orderIcon[0])){
            *buffer = QPixmap::fromImage(*image);
            *buffer = buffer->scaled(static_cast<int>(orderBtn->width()), static_cast<int>(orderBtn->height()*0.8));
        }
        orderBtn->setPixmap(*buffer);
        orderBtn->setAlignment(Qt::AlignmentFlag::AlignCenter);
    }

    if (systemState == 4 || systemState == 7){
        systemState = 5;
        systemState = systemState*100 + (colorIndex*col + patternIndex + 1);
        sendMessage();
        stackedWidget->setCurrentIndex(itemPage);
//        itemText->setText(objName);
        int itemIndex = colorIndex*col + patternIndex;
//        qDebug() << itemIndex;
        QImage *image = new QImage();
        QPixmap *buffer = new QPixmap();
        if (image->load(imagePath + imageIcon[itemIndex])){
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

void MainWindow::backBtnSlot()
{
    init();
    sendMessage();
}

void MainWindow::sendMessage()
{
    QString txData = QString::number(systemState);
    client->socket->write(txData.toUtf8());
    QString txMessage = "Transmit Data : " + txData;
    ui->tcpMessage->append(txMessage);
}
