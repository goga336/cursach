#include "recordwindow.h"
#include <QtSql/QSqlDatabase>
#include "daytablewindow.h"
#include <QSqlError>
#include <QInputDialog>

RecordWindow::RecordWindow(QWidget *parent)
    : QMainWindow(parent)
{
    weather = new WeatherService(this);
    dbManager = new DayTableWindow(this);

    // Подключаем сигналы
    connect(weather, &WeatherService::weatherLoaded,
            this, &RecordWindow::onWeatherLoaded);
    connect(weather, &WeatherService::errorOccurred,
            this, &RecordWindow::onWeatherError);
}

RecordWindow::~RecordWindow()
{
}
void RecordWindow::showWindow()  {
    this->show();
}

void RecordWindow::initialize()
{
    setupUI();
}

void RecordWindow::setupUI()
{

    this->setWindowTitle("Добавление записи о дне");
    this->resize(800, 600);

    // Создаем центральный виджет
    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(0, 139, 139));  // Голубой цвет
    centralWidget->setPalette(palette);
    centralWidget->setAutoFillBackground(true);// Важно для применения фона

    QHBoxLayout *mainBox = new QHBoxLayout(centralWidget);
    // Настраиваем макет
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *box = new QVBoxLayout(leftPanel);
    box->setSpacing(10);

    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightBox = new QVBoxLayout(rightPanel);

    QLabel *labelWeight = new QLabel("Вес рыбы (кг):");
    labelWeight->setStyleSheet("color: white;");
    box->addWidget(labelWeight);

    fishWeight = new QDoubleSpinBox(centralWidget);
    fishWeight->setRange(0, 100);
    fishWeight->setSuffix(" кг");
    box->addWidget(fishWeight);

    QLabel *labelDate = new QLabel("Дата рыбалки:");
    labelDate->setStyleSheet("color: white;");
    box->addWidget(labelDate);

    fishDate = new QDateEdit(centralWidget);
    fishDate->setDate(QDate::currentDate());
    fishDate->setCalendarPopup(true);
    fishDate->setDisplayFormat("dd.MM.yyyy");
    box->addWidget(fishDate);

    QLabel *labelStartFishing = new QLabel("Время начала рыбалки:");
    labelStartFishing->setStyleSheet("color: white;");
    box->addWidget(labelStartFishing);

    startFishing = new QTimeEdit(centralWidget);
    startFishing->setDisplayFormat("hh:mm");
    box->addWidget(startFishing);

    QLabel *labelEndFishing = new QLabel("Время окончания рыбалки:");
    labelEndFishing->setStyleSheet("color: white;");
    box->addWidget(labelEndFishing);

    endFishing = new QTimeEdit(centralWidget);
    endFishing->setDisplayFormat("hh:mm");
    box->addWidget(endFishing);

    QLabel *tempAirl = new QLabel("Температура воздуха:");
    tempAirl->setStyleSheet("color: white;");
    box->addWidget(tempAirl);

    tempAir = new QDoubleSpinBox(centralWidget);
    tempAir->setRange(-50.0, 50.0);
    tempAir->setSuffix(" °C");
    tempAir->setValue(20.0);
    box->addWidget(tempAir);

    QLabel *tempWaterl = new QLabel("Температура воды:");
    tempWaterl->setStyleSheet("color: white;");
    box->addWidget(tempWaterl);

    tempWater = new QDoubleSpinBox(centralWidget);
    tempWater->setRange(-5.0, 50.0);
    tempWater->setSuffix(" °C");
    tempWater->setValue(20.0);
    box->addWidget(tempWater);

    QLabel *labelPressureInput = new QLabel("Атмосферное давление (мм рт.ст.):");
    labelPressureInput->setStyleSheet("color: white;");
    box->addWidget(labelPressureInput);


    pressureInput = new QDoubleSpinBox(centralWidget);
    pressureInput->setRange(700.0, 800.0);
    pressureInput->setSuffix(" мм рт.ст.");
    pressureInput->setValue(760.0);
    box->addWidget(pressureInput);

    QLabel *windSpeedl = new QLabel("Скорость воздуха:");
    windSpeedl->setStyleSheet("color: white;");
    box->addWidget(windSpeedl);

    windSpeed = new QDoubleSpinBox(centralWidget);
    windSpeed->setRange(0.0, 113.2);
    windSpeed->setSuffix(" м/с");
    windSpeed->setValue(0.0);
    box->addWidget(windSpeed);

    QLabel *timeOfDayl = new QLabel("Время дня:");
    timeOfDayl->setStyleSheet("color: white;");
    box->addWidget(timeOfDayl);

    timeOfDay = new QComboBox(centralWidget);
    timeOfDay->addItems({"День", "Ночь"});
    box->addWidget(timeOfDay);

    QLabel *windDirectionl = new QLabel("Направление ветра:");
    windDirectionl->setStyleSheet("color: white;");
    box->addWidget(windDirectionl);

    windDirection = new QComboBox(centralWidget);
    windDirection->addItems({"Северный", "Северо-восточный", "Юго-восточный",
                             "Южный", "Западный", "Северо-западный",
                             "Восточный", "Юго-западный"});
    box->addWidget(windDirection);

    QLabel *seasonl = new QLabel("Сезон:");
    seasonl->setStyleSheet("color: white;");
    box->addWidget(seasonl);

    season = new QComboBox(centralWidget);
    season->addItems({"Зима", "Весна", "Лето", "Осень"});
    box->addWidget(season);

    QLabel *moonPhasel = new QLabel("Фаза луны:");
    moonPhasel->setStyleSheet("color: white;");
    box->addWidget(moonPhasel);

    moonPhase = new QComboBox(centralWidget);
    moonPhase->addItems({"Новолуние", "Растущий серп", "Полнолуние", "Убывающий серп"});
    box->addWidget(moonPhase);

    QLabel *recentActivityl = new QLabel("Активность прессинг:");
    recentActivityl->setStyleSheet("color: white;");
    box->addWidget(recentActivityl);

    recentActivity = new QCheckBox(centralWidget);
    box->addWidget(recentActivity);

    QLabel *notel = new QLabel("Поле для заметок:");
    notel->setStyleSheet("color: white;");
    box->addWidget(notel);

    note = new QTextEdit();
    note->setPlaceholderText("Введите текст здесь...");
    note->setMinimumHeight(450);
    rightBox->addWidget(notel);
    rightBox->addWidget(note);


    send = new QPushButton("Записать", centralWidget);
    box->addWidget(send);
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

    send->setStyleSheet(buttonStyle);

    weatherBtn = new QPushButton("Загрузить погоду", this);
    weatherBtn->setStyleSheet(buttonStyle);

    connect(weatherBtn, &QPushButton::clicked, [this]() {
        QStringList cities = {"Москва", "Санкт-Петербург", "Нижний Новгород",
                              "Рязань", "Воронеж", "Тула", "Ярославль"};
        bool ok;
        QString city = QInputDialog::getItem(this, "Выберите город",
                                             "Город:", cities, 0, false, &ok);
        if (ok && !city.isEmpty()) {
            weather->requestWeather(city);
        }
    });

    box->addWidget(weatherBtn);


    mainBox->addWidget(leftPanel);
    mainBox->addWidget(rightPanel);

    centralWidget->setLayout(mainBox);
    setCentralWidget(centralWidget);
   connect(send, &QPushButton::clicked, this, &RecordWindow::recordFishingDay);
}


void RecordWindow::recordFishingDay()
{
    double weight = fishWeight->value();
    QDate date = fishDate->date();
    QTime startTime = startFishing->time();
    QTime endTime = endFishing->time();
    double airTemperature = tempAir->value();
    double waterTemperature = tempWater->value();
    double pressure = pressureInput->value();
    double windSpeedValue = windSpeed->value();
    QString windDirectionValue = windDirection->currentText();
    QString timeOfDayValue = timeOfDay->currentText();
    QString seasonValue = season->currentText();
    QString moonPhaseValue = moonPhase->currentText();
    bool recentActivityValue = recentActivity->isChecked();
    QString notesValue = note->toPlainText();

    // Проверка времени
    if (startTime >= endTime) {
        QMessageBox::warning(this, "Ошибка",
                             "Время начала рыбалки не может быть больше или равно времени окончания!");
        return;
    }

    // Сначала добавляем запись о рыбалке
    if (!dbManager->addFishingRecord(date, startTime, endTime, weight, notesValue)) {
        return;  // Ошибка уже показана в методе
    }

    // Получаем ID последней записи
    QSqlQuery query;
    query.exec("SELECT LASTVAL()");
    int fishingDayId = -1;
    if (query.next()) {
        fishingDayId = query.value(0).toInt();
    }

    if (fishingDayId == -1) {
        QMessageBox::warning(this, "Ошибка", "Не удалось получить ID записи");
        return;
    }

    // Добавляем погодные условия
    if (!dbManager->addWeatherConditions(fishingDayId, airTemperature, waterTemperature,
                                         pressure, windSpeedValue, windDirectionValue,
                                         timeOfDayValue, seasonValue, moonPhaseValue,
                                         recentActivityValue)) {
        return;  // Ошибка уже показана в методе
    }

    // Если всё успешно
    QMessageBox::information(this, "Успех", "Данные успешно записаны!");

    // Очищаем поля
    fishWeight->setValue(0);
    fishDate->setDate(QDate::currentDate());
    startFishing->setTime(QTime::fromString("00:00", "hh:mm"));
    endFishing->setTime(QTime::fromString("00:00", "hh:mm"));
    tempAir->setValue(20.0);
    tempWater->setValue(20.0);
    pressureInput->setValue(760.0);
    windSpeed->setValue(0.0);
    windDirection->setCurrentIndex(0);
    timeOfDay->setCurrentIndex(0);
    season->setCurrentIndex(0);
    moonPhase->setCurrentIndex(0);
    recentActivity->setChecked(false);
    note->clear();
}

void RecordWindow::onWeatherLoaded()
{
    // Заполняем поля
    tempAir->setValue(weather->temperature);
    tempWater->setValue(weather->temperature * 0.8);
    pressureInput->setValue(weather->pressure * 0.750062);
    windSpeed->setValue(weather->windSpeed);

    // Направление ветра
    QString windDir;
    if (weather->windDeg >= 337.5 || weather->windDeg < 22.5) windDir = "Северный";
    else if (weather->windDeg >= 22.5 && weather->windDeg < 67.5) windDir = "Северо-восточный";
    else if (weather->windDeg >= 67.5 && weather->windDeg < 112.5) windDir = "Восточный";
    else if (weather->windDeg >= 112.5 && weather->windDeg < 157.5) windDir = "Юго-восточный";
    else if (weather->windDeg >= 157.5 && weather->windDeg < 202.5) windDir = "Южный";
    else if (weather->windDeg >= 202.5 && weather->windDeg < 247.5) windDir = "Юго-западный";
    else if (weather->windDeg >= 247.5 && weather->windDeg < 292.5) windDir = "Западный";
    else windDir = "Северо-западный";

    int index = windDirection->findText(windDir);
    if (index >= 0) windDirection->setCurrentIndex(index);

    // Время дня
    QTime now = QTime::currentTime();
    timeOfDay->setCurrentText(now.hour() >= 6 && now.hour() < 18 ? "День" : "Ночь");

    // Сезон
    int month = QDate::currentDate().month();
    QString seasonStr;
    if (month == 12 || month == 1 || month == 2) seasonStr = "Зима";
    else if (month >= 3 && month <= 5) seasonStr = "Весна";
    else if (month >= 6 && month <= 8) seasonStr = "Лето";
    else seasonStr = "Осень";

    int seasonIndex = season->findText(seasonStr);
    if (seasonIndex >= 0) season->setCurrentIndex(seasonIndex);



    QMessageBox::information(this, "Успех",
                             QString("Погода для города %1 загружена!").arg(weather->cityName));
}

void RecordWindow::onWeatherError(const QString &error)
{
    QMessageBox::warning(this, "Ошибка",
                         "Не удалось загрузить погоду:\n" + error);
}
