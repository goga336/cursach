#include "viewrecord.h"
#include <QHeaderView>
#include <QDebug>
#include <QDate>
#include <QSqlError>
#include <QPushButton>
#include <QMessageBox>

ViewRecord::ViewRecord(QWidget *parent)
    : QMainWindow(parent)
{

}

ViewRecord::~ViewRecord()
{
    delete tableWidget;
}

void ViewRecord::showWindow()
{
    this->show();
}

void ViewRecord::initialize()
{
    setupUI();
    displayRecords();
}

void ViewRecord::setupUI()
{
    resize(1200, 800);
    this->setWindowTitle("Таблица записей о рыбалке");

    tableWidget = new QTableWidget(this);

    tableWidget->setColumnCount(15);

    tableWidget->setColumnWidth(0, 50);   // ID записи
    tableWidget->setColumnWidth(1, 100);  // Дата
    tableWidget->setColumnWidth(2, 120);  // Время начала
    tableWidget->setColumnWidth(3, 120);  // Время окончания
    tableWidget->setColumnWidth(4, 100);  // Вес рыбы
    tableWidget->setColumnWidth(5, 120);  // Температура воздуха
    tableWidget->setColumnWidth(6, 120);  // Температура воды
    tableWidget->setColumnWidth(7, 120);  // Давление
    tableWidget->setColumnWidth(8, 120);  // Скорость ветра
    tableWidget->setColumnWidth(9, 150);  // Направление ветра
    tableWidget->setColumnWidth(10, 120); // Время дня
    tableWidget->setColumnWidth(11, 100); // Сезон
    tableWidget->setColumnWidth(12, 150); // Фаза луны
    tableWidget->setColumnWidth(13, 120); // Активность
    tableWidget->setColumnWidth(14, 200); // Заметки

    QStringList headers = {
        "ID записи",
        "Дата",
        "Время начала",
        "Время окончания",
        "Вес рыбы (кг)",
        "Темп. воздуха (°C)",
        "Темп. воды (°C)",
        "Давление (мм рт.ст.)",
        "Скорость ветра (м/с)",
        "Направление ветра",
        "Время дня",
        "Сезон",
        "Фаза луны",
        "Активность",
        "Заметки"
    };

    tableWidget->setHorizontalHeaderLabels(headers);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);


    tableWidget->setAlternatingRowColors(true);

    QPushButton *saveButton = new QPushButton("Внести изменения", this);
    QString saveButtonStyle = "QPushButton {"
                              "font-size: 14px;"
                              "font-weight: bold;"
                              "color: white;"
                              "background-color: #007BFF;"
                              "border-radius: 5px;"
                              "padding: 10px;"
                              "}"
                              "QPushButton:hover {"
                              "background-color: #0056b3;"
                              "}";
    saveButton->setStyleSheet(saveButtonStyle);
    connect(saveButton, &QPushButton::clicked, this, &ViewRecord::saveChanges);


    QPushButton *deleteButton = new QPushButton("Удалить запись", this);
    QString deleteButtonStyle = "QPushButton {"
                                "font-size: 14px;"
                                "font-weight: bold;"
                                "color: white;"
                                "background-color: #dc3545;"
                                "border-radius: 5px;"
                                "padding: 10px;"
                                "}"
                                "QPushButton:hover {"
                                "background-color: #c82333;"
                                "}";
    deleteButton->setStyleSheet(deleteButtonStyle);
    connect(deleteButton, &QPushButton::clicked, this, &ViewRecord::deleteRecord);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(tableWidget);
    mainLayout->addLayout(buttonLayout);

    QWidget *container = new QWidget();
    container->setLayout(mainLayout);
    setCentralWidget(container);
}

void ViewRecord::displayRecords()
{
    QSqlQuery query;

    QString sqlQuery =
        "SELECT "
        "   fd.fishing_day_id, "           // ID записи
        "   fd.fishing_date, "             // Дата рыбалки
        "   fd.start_time, "              // Время начала
        "   fd.end_time, "               // Время окончания
        "   fd.catch_weight, "           // Вес рыбы
        "   fd.notes, "                 // Заметки
        "   wc.air_temperature, "       // Температура воздуха
        "   wc.water_temperature, "     // Температура воды
        "   wc.atm_pressure, "          // Атмосферное давление
        "   wc.wind_speed, "           // Скорость ветра
        "   wc.wind_direction, "       // Направление ветра
        "   wc.time_of_day, "          // Время дня
        "   wc.season, "              // Сезон
        "   wc.moon_phase, "          // Фаза луны
        "   wc.recent_activity "      // Активность прессинг
        "FROM fishing_day fd "
        "LEFT JOIN weather_condition wc ON fd.fishing_day_id = wc.fishing_day_id "
        "ORDER BY fd.fishing_date DESC, fd.start_time DESC";

    if (!query.exec(sqlQuery)) {
        qWarning() << "Не удалось выполнить запрос к базе данных:" << query.lastError().text();
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить данные: " + query.lastError().text());
        return;
    }

    int row = 0;
    tableWidget->setRowCount(0);

    while (query.next()) {
        tableWidget->insertRow(row);

        // ID записи
        QTableWidgetItem *idItem = new QTableWidgetItem(query.value(0).toString());
        idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
        idItem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(row, 0, idItem);

        // Дата
        QTableWidgetItem *dateItem = new QTableWidgetItem(query.value(1).toDate().toString("dd.MM.yyyy"));
        dateItem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(row, 1, dateItem);

        // Время начала
        QTableWidgetItem *startTimeItem = new QTableWidgetItem(query.value(2).toTime().toString("hh:mm"));
        startTimeItem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(row, 2, startTimeItem);

        // Время окончания
        QTableWidgetItem *endTimeItem = new QTableWidgetItem(query.value(3).toTime().toString("hh:mm"));
        endTimeItem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(row, 3, endTimeItem);

        // Вес рыбы (с 2 знаками после запятой)
        QTableWidgetItem *weightItem = new QTableWidgetItem(QString::number(query.value(4).toDouble(), 'f', 2));
        weightItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableWidget->setItem(row, 4, weightItem);

        // Температура воздуха (с 1 знаком после запятой)
        QTableWidgetItem *airTempItem = new QTableWidgetItem(QString::number(query.value(6).toDouble(), 'f', 1));
        airTempItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableWidget->setItem(row, 5, airTempItem);

        // Температура воды (с 1 знаком после запятой)
        QTableWidgetItem *waterTempItem = new QTableWidgetItem(QString::number(query.value(7).toDouble(), 'f', 1));
        waterTempItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableWidget->setItem(row, 6, waterTempItem);

        // Давление (целое число)
        QTableWidgetItem *pressureItem = new QTableWidgetItem(QString::number(query.value(8).toDouble(), 'f', 0));
        pressureItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableWidget->setItem(row, 7, pressureItem);

        // Скорость ветра (с 1 знаком после запятой)
        QTableWidgetItem *windSpeedItem = new QTableWidgetItem(QString::number(query.value(9).toDouble(), 'f', 1));
        windSpeedItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableWidget->setItem(row, 8, windSpeedItem);

        // Направление ветра
        QTableWidgetItem *windDirItem = new QTableWidgetItem(query.value(10).toString());
        windDirItem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(row, 9, windDirItem);

        // Время дня
        QTableWidgetItem *timeOfDayItem = new QTableWidgetItem(query.value(11).toString());
        timeOfDayItem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(row, 10, timeOfDayItem);

        // Сезон
        QTableWidgetItem *seasonItem = new QTableWidgetItem(query.value(12).toString());
        seasonItem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(row, 11, seasonItem);

        // Фаза луны
        QTableWidgetItem *moonPhaseItem = new QTableWidgetItem(query.value(13).toString());
        moonPhaseItem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(row, 12, moonPhaseItem);

        // Активность прессинг
        QString activityStr = query.value(14).toBool() ? "Да" : "Нет";
        QTableWidgetItem *activityItem = new QTableWidgetItem(activityStr);
        activityItem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(row, 13, activityItem);

        // Заметки
        QTableWidgetItem *notesItem = new QTableWidgetItem(query.value(5).toString());
        tableWidget->setItem(row, 14, notesItem);

        row++;
    }

    qDebug() << "Загружено записей:" << row;
}

void ViewRecord::saveChanges()
{
    bool tempCorrect;
    bool pressureCorrect;
    bool weightCorrect;
    bool waterTempCorrect;
    bool windSpeedCorrect;

    QSqlDatabase::database().transaction();

    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        // Получаем ID записи
        int fishingDayId = tableWidget->item(row, 0)->text().toInt();

        // Проверка даты
        QDate date = QDate::fromString(tableWidget->item(row, 1)->text(), "dd.MM.yyyy");
        if (!date.isValid() || date > QDate::currentDate()) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Неверная дата в строке %1 (должна быть <= текущей даты)").arg(row + 1));
            QSqlDatabase::database().rollback();
            return;
        }

        // Проверка времени
        QTime startTime = QTime::fromString(tableWidget->item(row, 2)->text(), "hh:mm");
        QTime endTime = QTime::fromString(tableWidget->item(row, 3)->text(), "hh:mm");

        if (!startTime.isValid()) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Неверный формат времени начала в строке %1").arg(row + 1));
            QSqlDatabase::database().rollback();
            return;
        }

        if (!endTime.isValid()) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Неверный формат времени окончания в строке %1").arg(row + 1));
            QSqlDatabase::database().rollback();
            return;
        }

        // Проверка что время начала меньше времени окончания
        if (startTime >= endTime) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Время начала должно быть меньше времени окончания в строке %1").arg(row + 1));
            QSqlDatabase::database().rollback();
            return;
        }

        // Вес рыбы
        float weightDay = tableWidget->item(row, 4)->text().toFloat(&weightCorrect);
        if (!weightCorrect || weightDay < 0) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Неверный формат веса (должен быть >= 0) в строке %1").arg(row + 1));
            QSqlDatabase::database().rollback();
            return;
        }

        // Температура воздуха (от -50 до 50)
        float airTemperature = tableWidget->item(row, 5)->text().toFloat(&tempCorrect);
        if (!tempCorrect || airTemperature < -50.0 || airTemperature > 50.0) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Неверная температура воздуха (от -50°C до +50°C) в строке %1").arg(row + 1));
            QSqlDatabase::database().rollback();
            return;
        }

        // Температура воды
        float waterTemperature = tableWidget->item(row, 6)->text().toFloat(&waterTempCorrect);
        if (!waterTempCorrect) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Неверный формат температуры воды в строке %1").arg(row + 1));
            QSqlDatabase::database().rollback();
            return;
        }

        // Давление
        float pressure = tableWidget->item(row, 7)->text().toFloat(&pressureCorrect);
        if (!pressureCorrect || pressure < 700 || pressure > 800) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Неверное давление (от 700 до 800 мм рт.ст.) в строке %1").arg(row + 1));
            QSqlDatabase::database().rollback();
            return;
        }

        // Скорость ветра
        float windSpeed = tableWidget->item(row, 8)->text().toFloat(&windSpeedCorrect);
        if (!windSpeedCorrect || windSpeed < 0) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Неверная скорость ветра (должна быть >= 0) в строке %1").arg(row + 1));
            QSqlDatabase::database().rollback();
            return;
        }


        QString windDirection = tableWidget->item(row, 9)->text();
        QString timeOfDay = tableWidget->item(row, 10)->text();
        QString season = tableWidget->item(row, 11)->text();
        QString moonPhase = tableWidget->item(row, 12)->text();
        bool recentActivity = (tableWidget->item(row, 13)->text() == "Да");
        QString notes = tableWidget->item(row, 14)->text();

        QSqlQuery fishingQuery;
        fishingQuery.prepare(
            "UPDATE fishing_day SET "
            "fishing_date = :date, "
            "start_time = :start_time, "
            "end_time = :end_time, "
            "catch_weight = :weight, "
            "notes = :notes "
            "WHERE fishing_day_id = :id"
            );
        fishingQuery.bindValue(":date", date);
        fishingQuery.bindValue(":start_time", startTime);
        fishingQuery.bindValue(":end_time", endTime);
        fishingQuery.bindValue(":weight", weightDay);
        fishingQuery.bindValue(":notes", notes);
        fishingQuery.bindValue(":id", fishingDayId);

        if (!fishingQuery.exec()) {
            qWarning() << "Не удалось обновить данные в fishing_day:" << fishingQuery.lastError().text();
            QSqlDatabase::database().rollback();
            QMessageBox::warning(this, "Ошибка",
                                 "Не удалось обновить данные: " + fishingQuery.lastError().text());
            return;
        }

        QSqlQuery weatherQuery;
        weatherQuery.prepare(
            "UPDATE weather_condition SET "
            "air_temperature = :air_temperature, "
            "water_temperature = :water_temperature, "
            "atm_pressure = :pressure, "
            "wind_speed = :wind_speed, "
            "wind_direction = :wind_direction, "
            "time_of_day = :time_of_day, "
            "season = :season, "
            "moon_phase = :moon_phase, "
            "recent_activity = :recent_activity "
            "WHERE fishing_day_id = :fishingday_id"
            );
        weatherQuery.bindValue(":air_temperature", airTemperature);
        weatherQuery.bindValue(":water_temperature", waterTemperature);
        weatherQuery.bindValue(":pressure", pressure);
        weatherQuery.bindValue(":wind_speed", windSpeed);
        weatherQuery.bindValue(":wind_direction", windDirection);
        weatherQuery.bindValue(":time_of_day", timeOfDay);
        weatherQuery.bindValue(":season", season);
        weatherQuery.bindValue(":moon_phase", moonPhase);
        weatherQuery.bindValue(":recent_activity", recentActivity);
        weatherQuery.bindValue(":fishingday_id", fishingDayId);

        if (!weatherQuery.exec()) {
            qWarning() << "Не удалось обновить данные в weather_condition:" << weatherQuery.lastError().text();
            QSqlDatabase::database().rollback();
            QMessageBox::warning(this, "Ошибка",
                                 "Не удалось обновить данные о погоде: " + weatherQuery.lastError().text());
            return;
        }
    }


    QSqlDatabase::database().commit();
    QMessageBox::information(this, "Успех", "Изменения успешно сохранены!");
    qDebug() << "Изменения успешно сохранены!";
}


void ViewRecord::deleteRecord()
{
    int currentRow = tableWidget->currentRow();

    if (currentRow < 0) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, выберите запись для удаления!");
        return;
    }

    int fishingDayId = tableWidget->item(currentRow, 0)->text().toInt();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение удаления",
                                  QString("Вы уверены, что хотите удалить запись с ID %1?").arg(fishingDayId),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QSqlDatabase::database().transaction();

        QSqlQuery query;

        query.prepare("DELETE FROM weather_condition WHERE fishing_day_id = :id");
        query.bindValue(":id", fishingDayId);

        if (!query.exec()) {
            qWarning() << "Не удалось удалить данные из weather_condition:" << query.lastError().text();
            QSqlDatabase::database().rollback();
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить данные о погоде!");
            return;
        }

        query.prepare("DELETE FROM fishing_day WHERE fishing_day_id = :id");
        query.bindValue(":id", fishingDayId);

        if (!query.exec()) {
            qWarning() << "Не удалось удалить данные из fishing_day:" << query.lastError().text();
            QSqlDatabase::database().rollback();
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить запись о рыбалке!");
            return;
        }

        QSqlDatabase::database().commit();

        tableWidget->removeRow(currentRow);

        QMessageBox::information(this, "Успех", "Запись успешно удалена!");
        qDebug() << "Запись с ID" << fishingDayId << "успешно удалена!";
    }
}
