#include "recordwindow.h"
#include <QtSql/QSqlDatabase>
#include "daytablewindow.h"
#include <QSqlError>

RecordWindow::RecordWindow(QWidget *parent)
    : QMainWindow(parent)
{

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


    mainBox->addWidget(leftPanel);
    mainBox->addWidget(rightPanel);

    centralWidget->setLayout(mainBox);
    setCentralWidget(centralWidget);


    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("fishing_db");
    db.setHostName("localhost");
    db.setPort(5432);
    db.setUserName("postgres");
    db.setPassword("3360146");

    if (!db.open())
    {
        QMessageBox::information(nullptr, "Ошибка", "Не удалось подключиться к базе данных!");
    }
    else
    {
      //  QMessageBox::information(nullptr, "Успех", "Подключение успешно!");
    }
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

    // Проверка, что время начала меньше времени окончания
    if (startTime >= endTime) {
        QMessageBox::warning(this, "Ошибка", "Время начала рыбалки не может быть больше или равно времени окончания!");
        return; // Прерываем выполнение, если условие не выполнено
    }

    QSqlQuery query;
    query.prepare("INSERT INTO fishing_day (fisher_id, catch_weight, fishing_date, start_time, end_time, notes) VALUES (:user_id, :weight, :date, :start_time, :end_time, :notes)");
    query.bindValue(":date", date);
    query.bindValue(":start_time", startTime);
    query.bindValue(":end_time", endTime);
    query.bindValue(":user_id", 1);
    query.bindValue(":weight", weight);
    query.bindValue(":notes", notesValue);

    if (!query.exec())
    {
        QMessageBox::information(nullptr, "Ошибка", "Не удалось записать данные о дне рыбалки: " + query.lastError().text());
        return;
    }

    int fishingDayId = query.lastInsertId().toInt();

    query.prepare("INSERT INTO weather_condition (fishing_day_id, air_temperature, atm_pressure, water_temperature, "
                  "wind_speed, wind_direction, time_of_day, season, moon_phase, recent_activity) "
                  "VALUES (:fishingday_id, :air_temperature, :pressure, :water_temperature, "
                  ":wind_speed, :wind_direction, :time_of_day, :season, :moon_phase, :recent_activity)");

    query.bindValue(":fishingday_id", fishingDayId);
    query.bindValue(":air_temperature", airTemperature);
    query.bindValue(":pressure", pressure);
    query.bindValue(":water_temperature", waterTemperature);
    query.bindValue(":wind_speed", windSpeedValue);
    query.bindValue(":wind_direction", windDirectionValue);
    query.bindValue(":time_of_day", timeOfDayValue);
    query.bindValue(":season", seasonValue);
    query.bindValue(":moon_phase", moonPhaseValue);
    query.bindValue(":recent_activity", recentActivityValue);

    if (!query.exec())
    {
        QMessageBox::warning(nullptr, "Ошибка", "Не удалось записать данные о погоде: " + query.lastError().text());
    }
    else
    {
        QMessageBox::information(nullptr, "Успех", "Данные успешно записаны!");
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
}


