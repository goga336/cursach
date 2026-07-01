#include "daytablewindow.h"
#include <QDebug>
#include <QSqlError>
#include <QVector>
#include <QBuffer>
#include <QLabel>
#include <QDateTime>

DayTableWindow::DayTableWindow(QWidget *parent):QMainWindow(parent)
{
    connectToDatabase();

}

DayTableWindow::~DayTableWindow()
{
    // QSqlDatabase db = QSqlDatabase::database();
    // if (db.isOpen()) {
    //     db.close();
    //     QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    //     qDebug() << "База данных успешно закрыта.";
    // }
}



void DayTableWindow::connectToDatabase(){
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("fishing_db");
    db.setHostName("localhost");
    db.setPort(5432);
    db.setUserName("postgres");
    db.setPassword("3360146");

    if (!db.open())
    {

        QMessageBox::information(nullptr, "Успех", "ошибка!");
    }
    else
    {
        //QMessageBox::information(nullptr, "Успех", "Подключение успешно!");
    }
}


void DayTableWindow::insertIntoDatabase(const QDate &date, const QTime &start_time, const QTime &end_time, float weight){
    QSqlQuery query;
    query.prepare("insert into fishing_db (fishing_date, start_time, end_time, fisher_id, catch_weight)"
     "VALUES (:date, :start_time, :end_time, :fisher_id, :weight)");

    query.bindValue(":date", date);
    query.bindValue(":start_time", start_time);
    query.bindValue(":end_time", end_time);
    query.bindValue(":fisher_id", 1);
    query.bindValue(":weight", weight);

    // query.exec();
    if (!query.exec()) {
        QMessageBox::information(nullptr, "Ошибка подключения", "ошибка!", query.lastError().text());
    }
}

void DayTableWindow::insertSettings(QString &name){
    QSqlQuery query;
    query.prepare("UPDATE fisher SET fisher_name = :name WHERE fisher_id = 1");
    query.bindValue(":name", name);
    if (!query.exec()) {
        QMessageBox::information(nullptr, "Ошибка подключения", "ошибка!", query.lastError().text());
    } else {
        QMessageBox::information(nullptr, "Успех", "Данные изменены!");
    }
}

void DayTableWindow::insertSettingsPhoto(QImage &userPhoto){
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    userPhoto.save(&buffer, "PNG");

    QSqlQuery query;
    query.prepare("UPDATE fisher SET fisher_photo = :userPhoto WHERE fisher_id = 1");
    query.bindValue(":userPhoto", byteArray);


    if (!query.exec()) {
        QMessageBox::critical(nullptr, "Ошибка подключения",
                              "Ошибка сохранения данных:\n" + query.lastError().text());
    } else {
        QMessageBox::information(nullptr, "Успех", "Фотография успешно сохранена!");
    }
}

void DayTableWindow::getFromDatabase(QString &name){
    QSqlQuery query;
    query.exec("select fisher_name from fisher");
    // QString name ;
    while (query.next()) {

        name = query.value(0).toString();


        // QMessageBox::information(nullptr, name, name);

    }
}


void DayTableWindow::getFishingDataForGraphic(QVector<QDate> &vecDate, QVector<float> &vecWeight, const QString &year)
{
    QSqlQuery query;

    QString queryString = "SELECT fishing_date, catch_weight FROM fishing_day";
    if (!year.isEmpty() && year != "Все года") {
        queryString += " WHERE EXTRACT(YEAR FROM fishing_date) = " + year;
    }
    queryString += " ORDER BY fishing_date";

    if (query.exec(queryString)) {
        while (query.next()) {
            QDate date = query.value(0).toDate();
            float weight = query.value(1).toFloat();
            vecDate.append(date);
            vecWeight.append(weight);
        }
    } else {
        qDebug() << "Query execution error:" << query.lastError().text();
    }

    //std::sort(vecDate.begin(), vecDate.end());
}

QVector<FishingRecord> DayTableWindow::getAllDataForPrediction(){
    QVector<FishingRecord> records;
    QSqlQuery query;


    query.exec("SELECT fd.fishing_date, wc.water_temperature, wc.air_temperature, "
               "wc.wind_speed, wc.atm_pressure, wc.wind_direction, wc.time_of_day, "
               "wc.season, wc.moon_phase, wc.recent_activity, fd.catch_weight "
               "FROM weather_condition wc "
               "JOIN fishing_day fd ON wc.fishing_day_id = fd.fishing_day_id");

    if (!query.isActive()) {
        qDebug() << "Ошибка запроса:" << query.lastError().text();
        return records;
    }

    int count = 0;
    while (query.next()) {
        count++;
        FishingRecord record;

        QString dateStr = query.value("fishing_date").toString();
        record.date = QDateTime::fromString(dateStr, Qt::ISODate);
        record.waterTemperature = query.value("water_temperature").toDouble();
        record.airTemperature = query.value("air_temperature").toDouble();
        record.windSpeed = query.value("wind_speed").toDouble();
        record.pressure = query.value("atm_pressure").toDouble();

        QString windDir = query.value("wind_direction").toString();
        if (windDir == "Северный") record.windDirection = 0;
        else if (windDir == "Северо-восточный") record.windDirection = 1;
        else if (windDir == "Восточный") record.windDirection = 2;
        else if (windDir == "Юго-восточный") record.windDirection = 3;
        else if (windDir == "Южный") record.windDirection = 4;
        else if (windDir == "Юго-западный") record.windDirection = 5;
        else if (windDir == "Западный") record.windDirection = 6;
        else if (windDir == "Северо-западный") record.windDirection = 7;

        QString timeDay = query.value("time_of_day").toString();
        if (timeDay == "Утро" || timeDay == "День") {
            record.timeOfDay = 0;
        } else if (timeDay == "Вечер" || timeDay == "Ночь") {
            record.timeOfDay = 1;
        }

        QString seasonStr = query.value("season").toString();
        if (seasonStr == "Весна") record.season = 0;
        else if (seasonStr == "Лето") record.season = 1;
        else if (seasonStr == "Осень") record.season = 2;
        else if (seasonStr == "Зима") record.season = 3;

        QString moon = query.value("moon_phase").toString();
        if (moon == "Новолуние") {
            record.moonPhase = 0;
        } else if (moon == "Растущий серп") {
            record.moonPhase = 1;
        } else if (moon == "Полнолуние") {
            record.moonPhase = 2;
        } else if (moon == "Убывающий серп") {
            record.moonPhase = 3;
        }

        record.recentActivity = query.value("recent_activity").toBool();
        record.catchWeight = query.value("catch_weight").toDouble();

        records.append(record);
    }

    qDebug() << "Загружено записей:" << count;

    if (!records.isEmpty()) {
        const auto& first = records.first();
        qDebug() << "Первая запись - дата:" << first.date.toString("yyyy-MM-dd")
                 << "темп. воздуха:" << first.airTemperature
                 << "вес:" << first.catchWeight;
    }

    return records;
}

void DayTableWindow::getBestFishingDay(QDate &date, float &weight){
    QSqlQuery query;
    query.exec("SELECT fishing_date, catch_weight FROM fishing_day WHERE catch_weight = (SELECT MAX(catch_weight) FROM fishing_day)");
    while (query.next()){
        date = query.value(0).toDate();
        weight = query.value(1).toFloat();
    }

}

void DayTableWindow::getTotalFishingDays(int &totalDays) {
    QSqlQuery query;
    query.exec("SELECT COUNT(*) FROM fishing_day");

    if (query.next()) {
        totalDays = query.value(0).toInt();
    } else {
        totalDays = 0;
    }
}



void DayTableWindow::getAverageWeatherConditionsForSuccessfulFishing(float &avgTemperature, float &avgPressure, float &avgWaterTemperature, float &averageWeight) {
    averageWeight = 0.0;
    QSqlQuery avgWeightQuery("SELECT AVG(catch_weight) FROM fishing_day");
    if (avgWeightQuery.next()) {
        averageWeight = avgWeightQuery.value(0).toFloat();
    }

    QSqlQuery avgWeatherQuery;
    avgWeatherQuery.prepare("SELECT AVG(wc.air_temperature), AVG(wc.atm_pressure), AVG(wc.water_temperature) "
                            "FROM weather_condition wc "
                            "JOIN fishing_day fd ON wc.fishing_day_id = fd.fishing_day_id "
                            "WHERE fd.catch_weight > :averageWeight");
    avgWeatherQuery.bindValue(":averageWeight", averageWeight);

    if (avgWeatherQuery.exec() && avgWeatherQuery.next()) {
        avgTemperature = avgWeatherQuery.value(0).toFloat();
        avgPressure = avgWeatherQuery.value(1).toFloat();
        avgWaterTemperature = avgWeatherQuery.value(2).toFloat();
    } else {
        qDebug() << "Query execution error:" << avgWeatherQuery.lastError().text();
    }
}

void DayTableWindow::loadFisherPhoto(QLabel *fishermanImage, int userId) {
    QSqlQuery query;
    query.prepare("SELECT fisher_photo FROM fisher WHERE fisher_id = :userId");
    query.bindValue(":userId", userId);

    if (query.exec() && query.next()) {
        QByteArray byteArray = query.value(0).toByteArray();
        QPixmap pixmap;
        if (pixmap.loadFromData(byteArray)) {
            fishermanImage->setPixmap(pixmap.scaled(500, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            fishermanImage->setAlignment(Qt::AlignCenter);
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить фото из данных.");
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Фотография не найдена:\n" + query.lastError().text());
    }
}

bool DayTableWindow::addFishingRecord(const QDate &date, const QTime &startTime,
                                      const QTime &endTime, double weight,
                                      const QString &notes)
{
    QSqlQuery query;
    query.prepare("INSERT INTO fishing_day (fisher_id, catch_weight, fishing_date, start_time, end_time, notes) "
                  "VALUES (:user_id, :weight, :date, :start_time, :end_time, :notes)");
    query.bindValue(":date", date);
    query.bindValue(":start_time", startTime);
    query.bindValue(":end_time", endTime);
    query.bindValue(":user_id", 1);
    query.bindValue(":weight", weight);
    query.bindValue(":notes", notes);

    if (!query.exec()) {
        QMessageBox::warning(nullptr, "Ошибка",
                             "Не удалось записать данные о дне рыбалки: " + query.lastError().text());
        return false;
    }
    return true;
}

bool DayTableWindow::addWeatherConditions(int fishingDayId, double airTemperature,
                                          double waterTemperature, double pressure,
                                          double windSpeed, const QString &windDirection,
                                          const QString &timeOfDay, const QString &season,
                                          const QString &moonPhase, bool recentActivity)
{
    QSqlQuery query;
    query.prepare("INSERT INTO weather_condition (fishing_day_id, air_temperature, atm_pressure, water_temperature, "
                  "wind_speed, wind_direction, time_of_day, season, moon_phase, recent_activity) "
                  "VALUES (:fishingday_id, :air_temperature, :pressure, :water_temperature, "
                  ":wind_speed, :wind_direction, :time_of_day, :season, :moon_phase, :recent_activity)");

    query.bindValue(":fishingday_id", fishingDayId);
    query.bindValue(":air_temperature", airTemperature);
    query.bindValue(":pressure", pressure);
    query.bindValue(":water_temperature", waterTemperature);
    query.bindValue(":wind_speed", windSpeed);
    query.bindValue(":wind_direction", windDirection);
    query.bindValue(":time_of_day", timeOfDay);
    query.bindValue(":season", season);
    query.bindValue(":moon_phase", moonPhase);
    query.bindValue(":recent_activity", recentActivity);

    if (!query.exec()) {
        QMessageBox::warning(nullptr, "Ошибка",
                             "Не удалось записать данные о погоде: " + query.lastError().text());
        return false;
    }
    return true;
}
