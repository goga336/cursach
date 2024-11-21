#include "mainwindow.h"
#include "daytablewindow.h"
#include "iwindow.h"
#include "windowfactory.h"



#include <QVector>
#include <QDate>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), daytable(new DayTableWindow) {
    setupUI();
}
// MainWindow::MainWindow(QWidget *parent)
//     : QMainWindow(parent)
//     , ui(new Ui::MainWindow)
// {
//     ui->setupUi(this);
//     createScrollArea();
//     resize(500, 300);
//     DayTableWindow ff;


// }

MainWindow::~MainWindow()
{
    delete daytable;
}

void MainWindow::setupUI() {
    resize(500, 300);
    this->setWindowTitle("Главная страница");
    createScrollArea();
}

void MainWindow::createScrollArea(){
    QPalette palette;
    QPixmap background("/home/goga/Desktop/study/cursav/photo/phone.jpg");
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

    QDate bestday;
    float bestweight;
    daytable->getBestFishingDay(bestday, bestweight);
    recentday = new QLabel("Ваша лучшая рыбалка " + bestday.toString("dd.MM.yyyy") + ": " + QString::number(bestweight) + " кг", this);
    int count;
    daytable->getTotalFishingDays(count);
    counter = new QLabel("Общее количество дней, проведенных на рыбалке: " + QString::number(count));

    float temperatures;
    float pressures;
    float waterTemperatures;
    float avg;
    daytable->getAverageWeatherConditionsForSuccessfulFishing (temperatures, pressures, waterTemperatures, avg);
    QLabel *avgWeightLabel = new QLabel("Средний вес улова за день: " + QString::number(avg, 'f', 2) + " кг");
    QLabel *temperatureLabel = new QLabel("Среднее значение тем   пературы воздуха для дней с уловом выше среднего: " + QString::number(temperatures, 'f', 1) + " °C");
    QLabel *pressureLabel = new QLabel("Среднее значение атмосферного давления для дней с уловом выше среднего: " + QString::number(pressures, 'f', 1) + " мм рт. ст.");
    QLabel *waterTemperatureLabel = new QLabel("Среднее значение температуры воды для дней с уловом выше среднего: " + QString::number(waterTemperatures, 'f', 1) + " °C");
    avgWeightLabel->setStyleSheet("font-size: 16px; color: white; font-weight: normal; padding: 5px;");
    temperatureLabel->setStyleSheet("font-size: 16px; color: white; font-weight: normal; padding: 5px;");
    pressureLabel->setStyleSheet("font-size: 16px; color: white; font-weight: normal; padding: 5px;");
    waterTemperatureLabel->setStyleSheet("font-size: 16px; color: white; font-weight: normal; padding: 5px;");
    labelwelcome->setStyleSheet("font-size: 16px; color: white; font-weight: normal; padding: 5px;");
    counter->setStyleSheet("font-size: 16px; color: white; font-weight: normal; padding: 5px;");
    recentday->setStyleSheet("font-size: 16px; color: white; font-weight: normal; padding: 5px;");



    container->setLayout(layout);
    scrollarea->setWidget(container);
    lauoutmain->addWidget(scrollarea);
    lauoutmain->addWidget(fishermanImage);
    lauoutmain->addWidget(recentday);
    //lauoutmain->addWidget(weatherLabel);
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
        "background-image: url(/home/goga/Desktop/study/cursav/photo/phone.jpg);"
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
    window->showWindow();
}

void MainWindow::hadlerButton2(){
    IWindow *window = WindowFactory::createWindow("ViewGraphWindow", this);
    window->showWindow();
}

void MainWindow::hadlerButton3(){
    IWindow *window = WindowFactory::createWindow("ViewRecordWindow", this);
    window->showWindow();

}

void MainWindow::hadlerButton4() {
    IWindow *window = WindowFactory::createWindow("SettingsWindow", this);
    window->showWindow();
}

void MainWindow::hadlerButton5(){
    IWindow *window = WindowFactory::createWindow("ViewWeatherWindow", this);
    window->showWindow();
}

void MainWindow::hadlerButton6(){
    IWindow *window = WindowFactory::createWindow("ViewWeatherWindow", this);
    window->showWindow();
}

QString MainWindow::QVectorToString(const QVector<float> &vec) {
    QStringList list;
    for (float value : vec) {
        list << QString::number(value);
    }
    return list.join(", ");
}

QLabel* MainWindow::updateFishingStatistics() {
    QDate bestday;
    float bestweight;
    daytable->getBestFishingDay(bestday, bestweight);

    QLabel *label = new QLabel("Ваша лучшая рыбалка " + bestday.toString("dd.MM.yyyy") + ": " + QString::number(bestweight) + " кг", this);
    return label;
}
