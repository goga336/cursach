#include "mainwindow.h"
#include "windowfactory.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), daytable(new DayTableWindow)
{

}

MainWindow::~MainWindow()
{
    delete daytable;
}


void MainWindow::showWindow()
{
    this->show();
}

void MainWindow::initialize()
{
    setupUI();
    updateStatisticsUI();
}

void MainWindow::setupUI() {
    resize(500, 300);
    this->setWindowTitle("Главная страница");
    createScrollArea();
}

void MainWindow::createScrollArea(){
    QPalette palette;
    QPixmap background("/home/goga/Desktop/study/8_semestr/diplom/qr-code.png");
    palette.setBrush(QPalette::Window, background.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    this->setPalette(palette);

    QVBoxLayout *lauoutmain = new QVBoxLayout;

    QString UserName;
    daytable->getFromDatabase(UserName);
    labelwelcome = new QLabel("Добро пожаловать, " + UserName, this);
    lauoutmain->addWidget(labelwelcome);
    labelwelcome->setObjectName("welcomeLabel");


    QLabel *fishermanImage = new QLabel(this);
    daytable->loadFisherPhoto(fishermanImage, 1); // ID пользователя, например 1
    fishermanImage->setAlignment(Qt::AlignCenter);


    QWidget *container = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    QScrollArea *scrollarea = new QScrollArea(this);

    scrollarea->setStyleSheet("background: transparent;");
    scrollarea->viewport()->setStyleSheet("background: transparent;");
    scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollarea->setFixedHeight(150);

    QPushButton *buttonRecordDay = new QPushButton(this);
    QPushButton *button2 = new QPushButton(this);
    QPushButton *button3 = new QPushButton(this);
    QPushButton *button4 = new QPushButton(this);
    QPushButton *button5 = new QPushButton(this);
    QPushButton *button6 = new QPushButton(this);

    buttonRecordDay->setText("Записать день");
    button2->setText("Построить график");
    button3->setText("Посмотреть все записи");
    button4->setText("Настройки");
    button5->setText("Погода");
    button6->setText("Статистика");

    buttonRecordDay->setObjectName("button1");
    button2->setObjectName("button2");
    button3->setObjectName("button3");
    button4->setObjectName("button4");

    layout->addWidget(buttonRecordDay);
    layout->addWidget(button2);
    layout->addWidget(button3);
    layout->addWidget(button4);
    layout->addWidget(button5);
    layout->addWidget(button6);

    layout->setSpacing(50);
    buttonRecordDay->setFixedSize(200, 120);
    button2->setFixedSize(200, 120);
    button3->setFixedSize(200, 120);
    button4->setFixedSize(200, 120);
    button5->setFixedSize(200, 120);
    button6->setFixedSize(200, 120);

    QString buttonStyle = "QPushButton {"
                          "font-size: 14px;"
                          "font-weight: bold;"
                          "color: white;"
                          "background-color: #007BFF;"
                          "border-radius: 20px;"
                          "padding: 10px;"
                          "}"
                          "QPushButton:hover {"
                          "background-color: #0056b3;"
                          "}";

    buttonRecordDay->setStyleSheet(buttonStyle);
    button2->setStyleSheet(buttonStyle);
    button3->setStyleSheet(buttonStyle);
    button4->setStyleSheet(buttonStyle);
    button5->setStyleSheet(buttonStyle);
    button6->setStyleSheet(buttonStyle);

    recentday = new QLabel(this);
    counter = new QLabel(this);
    avgWeightLabel = new QLabel(this);
    temperatureLabel = new QLabel(this);
    pressureLabel = new QLabel(this);
    waterTemperatureLabel = new QLabel(this);

    container->setLayout(layout);
    scrollarea->setWidget(container);
    lauoutmain->addWidget(scrollarea);
    lauoutmain->addWidget(fishermanImage);
    lauoutmain->addWidget(recentday);
    lauoutmain->addWidget(counter);
    lauoutmain->addWidget(temperatureLabel);
    lauoutmain->addWidget(pressureLabel );
    lauoutmain->addWidget(waterTemperatureLabel);
    lauoutmain->addWidget(avgWeightLabel);


    QWidget *mainwidget = new QWidget(this);
    mainwidget->setLayout(lauoutmain);

    setCentralWidget(mainwidget);

    mainwidget->setStyleSheet(
        "QWidget {"
        "background-image: url(/home/goga/Desktop/study/8_semestr/diplom/qr-code.png);"
        "background-position: center;"
        "background-repeat: no-repeat;"
        "background-size: cover;"
        "}"
        );

    connect(buttonRecordDay, &QPushButton::clicked, this, &MainWindow::hadlerButton1);
    connect(button2, &QPushButton::clicked, this, &MainWindow::hadlerButton2);
    connect(button3, &QPushButton::clicked, this, &MainWindow::hadlerButton3);
    connect(button4, &QPushButton::clicked, this, &MainWindow::hadlerButton4);
    connect(button5, &QPushButton::clicked, this, &MainWindow::hadlerButton5);
    connect(button6, &QPushButton::clicked, this, &MainWindow::hadlerButton6);
}

void MainWindow::hadlerButton1(){
    IWindow *window = WindowFactory::createWindow("RecordWindow", this);
    window->initialize();
    window->showWindow();
}

void MainWindow::hadlerButton2(){
    IWindow *window = WindowFactory::createWindow("ViewGraphWindow", this);
    window->initialize();
    window->showWindow();
}

void MainWindow::hadlerButton3(){
    IWindow *window = WindowFactory::createWindow("ViewRecordWindow", this);
    window->initialize();
    window->showWindow();

}

void MainWindow::hadlerButton4() {
    IWindow *window = WindowFactory::createWindow("SettingsWindow", this);
    window->initialize();
    window->showWindow();
}

void MainWindow::hadlerButton5(){
    IWindow *window = WindowFactory::createWindow("ViewWeatherWindow", this);
    window->initialize();
    window->showWindow();
}

void MainWindow::hadlerButton6(){
    IWindow *window = WindowFactory::createWindow("ViewWeatherWindow", this);
    window->initialize();
    window->showWindow();
}

QString MainWindow::QVectorToString(const QVector<float> &vec) {
    QStringList list;
    for (float value : vec) {
        list << QString::number(value);
    }
    return list.join(", ");
}


void MainWindow::updateStatisticsUI() {
    QDate bestday;
    float bestweight;
    daytable->getBestFishingDay(bestday, bestweight);
    recentday->setText("Ваша лучшая рыбалка " + bestday.toString("dd.MM.yyyy") + ": " + QString::number(bestweight) + " кг");

    int count;
    daytable->getTotalFishingDays(count);
    counter->setText("Общее количество дней, проведенных на рыбалке: " + QString::number(count));

    float temperatures, pressures, waterTemperatures, avg;
    daytable->getAverageWeatherConditionsForSuccessfulFishing(temperatures, pressures, waterTemperatures, avg);

    avgWeightLabel->setText("Средний вес улова за день: " + QString::number(avg, 'f', 2) + " кг");
    temperatureLabel->setText("Среднее значение температуры воздуха для дней с уловом выше среднего: " + QString::number(temperatures, 'f', 1) + " °C");
    pressureLabel->setText("Среднее значение атмосферного давления для дней с уловом выше среднего: " + QString::number(pressures, 'f', 1) + " мм рт. ст.");
    waterTemperatureLabel->setText("Среднее значение температуры воды для дней с уловом выше среднего: " + QString::number(waterTemperatures, 'f', 1) + " °C");

    QString labelStyle = "font-size: 16px; color: white; font-weight: normal; padding: 5px;";
    recentday->setStyleSheet(labelStyle);
    counter->setStyleSheet(labelStyle);
    avgWeightLabel->setStyleSheet(labelStyle);
    temperatureLabel->setStyleSheet(labelStyle);
    pressureLabel->setStyleSheet(labelStyle);
    waterTemperatureLabel->setStyleSheet(labelStyle);
}

