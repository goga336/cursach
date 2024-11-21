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
    setupUI();
    displayRecords();

}

ViewRecord::~ViewRecord()
{
    delete tableWidget;
}

void ViewRecord::setupUI()
{
    resize(1200, 600);
    this->setWindowTitle("Таблица записей");
    tableWidget = new QTableWidget(this);


    tableWidget->setColumnCount(8);
    tableWidget->setColumnWidth(3, 200);
    tableWidget->setColumnWidth(2, 200);
    tableWidget->setColumnWidth(6, 200);
    QStringList headers = {"ID", "Дата", "Время начала", "Время окончания", "Температура", "Давление", "Температура воды", "Вес"};
    tableWidget->setHorizontalHeaderLabels(headers);
    tableWidget->horizontalHeader()->setStretchLastSection(true);

    tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);


    QPushButton *saveButton = new QPushButton("Внести изменения", this);
    QString buttonStyle = "QPushButton {"
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

    saveButton->setStyleSheet(buttonStyle);
    connect(saveButton, &QPushButton::clicked, this, &ViewRecord::saveChanges);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(tableWidget);
    layout->addWidget(saveButton);

    QWidget *container = new QWidget();
    container->setLayout(layout);
    setCentralWidget(container);
}

void ViewRecord::displayRecords()
{
    QSqlQuery query;
    if (!query.exec("SELECT FishingDay.id, FishingDay.date, FishingDay.start_time, FishingDay.end_time, "
                    "WheatherCondition.temperature, WheatherCondition.pressure, WheatherCondition.water_temperature, FishingDay.weight "
                    "FROM FishingDay "
                    "JOIN WheatherCondition ON FishingDay.id = WheatherCondition.fishingday_id")) {
        qWarning() << "Не удалось выполнить запрос к базе данных:" << query.lastError().text();
        return;
    }

    int row = 0;
    tableWidget->setRowCount(query.size());

    while (query.next()) {
        QTableWidgetItem *idItem = new QTableWidgetItem(query.value(0).toString());
        idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);

        tableWidget->setItem(row, 0, idItem);
        //tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toDate().toString("dd.MM.yyyy")));
        tableWidget->setItem(row, 2, new QTableWidgetItem(query.value(2).toTime().toString("hh:mm")));
        tableWidget->setItem(row, 3, new QTableWidgetItem(query.value(3).toTime().toString("hh:mm")));
        tableWidget->setItem(row, 4, new QTableWidgetItem(query.value(4).toString()));
        tableWidget->setItem(row, 5, new QTableWidgetItem(query.value(5).toString()));
        tableWidget->setItem(row, 6, new QTableWidgetItem(query.value(6).toString()));
        tableWidget->setItem(row, 7, new QTableWidgetItem(query.value(7).toString()));
        idItem->setTextAlignment(Qt::AlignCenter);
        row++;
    }
}

void ViewRecord::saveChanges()
{
    bool tempCorrect;
    bool pressureCorrect;
    bool weightCorrect;
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        int id = tableWidget->item(row, 0)->text().toInt();
        QDate date = QDate::fromString(tableWidget->item(row, 1)->text(), "dd.MM.yyyy");
        QTime startTime = QTime::fromString(tableWidget->item(row, 2)->text(), "hh:mm");
        QTime endTime = QTime::fromString(tableWidget->item(row, 3)->text(), "hh:mm");
        float temperature = tableWidget->item(row, 4)->text().toFloat(&tempCorrect);
        float pressure = tableWidget->item(row, 5)->text().toFloat(&pressureCorrect);
        float waterTemperature = tableWidget->item(row, 6)->text().toFloat();
        float weightDay = tableWidget->item(row, 7)->text().toFloat(&weightCorrect);


        if (!startTime.isValid()) {
            QMessageBox::warning(this, "Ошибка", "Неверный формат времени начала в строке " + QString::number(row + 1));
            return;
        }

        if (!endTime.isValid()) {
            QMessageBox::warning(this, "Ошибка", "Неверный формат времени окончания в строке " + QString::number(row + 1));
            return;
        }

        if (!tempCorrect || temperature < -50.0 || temperature > 50.0) {
            QMessageBox::warning(this, "Ошибка", "Неверный формат температуры (от -50°C до +50°C) в строке " + QString::number(row + 1));
            return;
        }

        if (!weightCorrect) {
            QMessageBox::warning(this, "Ошибка", "Неверный формат веса в строке " + QString::number(row + 1));
            return;
        }

        if (!pressureCorrect) {
            QMessageBox::warning(this, "Ошибка", "Неверный формат давления (от 700 мм рт. ст. до 800 мм рт. ст.) в строке " + QString::number(row + 1));
            return;
        }

        QSqlQuery query;
        query.prepare("UPDATE FishingDay SET date = :date, start_time = :start_time, end_time = :end_time, weight = :weight WHERE id = :id");
        query.bindValue(":date", date);
        query.bindValue(":start_time", startTime);
        query.bindValue(":end_time", endTime);
        query.bindValue(":weight", weightDay);
        query.bindValue(":id", id);

        if (!query.exec()) {
            qWarning() << "Не удалось обновить данные в FishingDay:" << query.lastError().text();
        }

        QSqlQuery weatherQuery;
        weatherQuery.prepare("UPDATE WheatherCondition SET temperature = :temperature, pressure = :pressure, water_temperature = :waterTemperature "
                             "WHERE fishingday_id = :fishingday_id");
        weatherQuery.bindValue(":temperature", temperature);
        weatherQuery.bindValue(":pressure", pressure);
        weatherQuery.bindValue(":waterTemperature", waterTemperature);
        weatherQuery.bindValue(":fishingday_id", id);

        if (!weatherQuery.exec()) {
            qWarning() << "Не удалось обновить данные в WheatherCondition:" << weatherQuery.lastError().text();
        }
    }
    qDebug() << "Изменения успешно сохранены!";
}
