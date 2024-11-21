#include "daytablewindow.h"
#include <QDebug>
#include <QSqlError>
#include <QVector>
#include <QBuffer>
#include <QLabel>

DayTableWindow::DayTableWindow(QWidget *parent):QMainWindow(parent)
{
    connectToDatabase();

}

DayTableWindow::~DayTableWindow(){}

// void DayTableWindow::connectToDatabase() {
//     if (QSqlDatabase::contains("testConnection")) {
//         QSqlDatabase db = QSqlDatabase::database("testConnection");
//         if (!db.isOpen()) {
//             db.open();
//         }
//     } else {
//         qDebug() << "Ошибка: соединение 'testConnection' не существует.";
//     }
// }



void DayTableWindow::connectToDatabase(){
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("fishingdays");
    db.setHostName("localhost"); // только имя хоста
    db.setPort(5432); // указание порта
    db.setUserName("postgres");
    db.setPassword("3360146");

    if (!db.open())
    {

        QMessageBox::information(nullptr, "Успех", "ошибка!");
    }
    else
    {
      //  QMessageBox::information(nullptr, "Успех", "Подключение успешно!");
    }
}


void DayTableWindow::insertIntoDatabase(const QDate &date, const QTime &start_time, const QTime &end_time, float weight){
    QSqlQuery query;
    query.prepare("insert into fishingday (date, start_time, end_time, fisher_id, weight)"
     "VALUES (:date, :start_time, :end_time, :fisher_id, :weight)");

    query.bindValue(":date", date);
    query.bindValue(":start_time", start_time);
    query.bindValue(":end_time", end_time);
    query.bindValue(":fisher_id", 1);
    query.bindValue(":weight", weight);

    // query.exec();
    if (!query.exec()) {
        QMessageBox::information(nullptr, "Ошибка подключения", "ошибка!", query.lastError().text());
    } else {
       // QMessageBox::information(nullptr, "Успех", "Подключение успешно!");
    }
}

void DayTableWindow::insertSettings(QString &name){
    QSqlQuery query;
    query.prepare("UPDATE fisher SET user_name = :name WHERE id = 1");
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
    userPhoto.save(&buffer, "PNG"); // Сохраняем изображение в формате PNG

    // Подготовка SQL-запроса
    QSqlQuery query;
    query.prepare("UPDATE fisher SET photo = :userPhoto WHERE id = 1");
    query.bindValue(":userPhoto", byteArray); // Привязываем данные как bytea     // Привязываем ID пользователя

    // Выполнение запроса
    if (!query.exec()) {
        QMessageBox::critical(nullptr, "Ошибка подключения",
                              "Ошибка сохранения данных:\n" + query.lastError().text());
    } else {
        QMessageBox::information(nullptr, "Успех", "Фотография успешно сохранена!");
    }
}

void DayTableWindow::getFromDatabase(QString &name){
    QSqlQuery query;
    query.exec("select user_name from fisher");
    // QString name ;
    while (query.next()) {

        name = query.value(0).toString();


        // QMessageBox::information(nullptr, name, name);

    }
}

void DayTableWindow::getFromDatabasetoGraphic(QVector<QDate> &vecDate, QVector<float> &vecWeight){
    QSqlQuery query;
    query.exec("select date, weight from fishingday");
    float weight;
    QDate date;
    while (query.next()){
        date = query.value(0).toDate();
        weight = query.value(1).toFloat();
        vecDate.append(date);
        vecWeight.append(weight);
    }
}

void DayTableWindow::getBestFishingDay(QDate &date, float &weight){
    QSqlQuery query;
    query.exec("SELECT date, weight FROM FishingDay WHERE weight = (SELECT MAX(weight) FROM FishingDay)");
    while (query.next()){
        date = query.value(0).toDate();
        weight = query.value(1).toFloat();
    }

}

void DayTableWindow::getTotalFishingDays(int &totalDays) {
    QSqlQuery query;
    query.exec("SELECT COUNT(*) FROM FishingDay");

    if (query.next()) {
        totalDays = query.value(0).toInt();
    } else {
        totalDays = 0;
    }
}



void DayTableWindow::getAverageWeatherConditionsForSuccessfulFishing(float &avgTemperature, float &avgPressure, float &avgWaterTemperature, float &averageWeight) {
    averageWeight = 0.0;
    QSqlQuery avgWeightQuery("SELECT AVG(weight) FROM FishingDay");
    if (avgWeightQuery.next()) {
        averageWeight = avgWeightQuery.value(0).toFloat();
    }

    QSqlQuery avgWeatherQuery;
    avgWeatherQuery.prepare("SELECT AVG(wc.temperature), AVG(wc.pressure), AVG(wc.water_temperature) "
                            "FROM WheatherCondition wc "
                            "JOIN FishingDay fd ON wc.fishingday_id = fd.id "
                            "WHERE fd.weight > :averageWeight");
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
    // SQL-запрос для получения фотографии
    QSqlQuery query;
    query.prepare("SELECT photo FROM fisher WHERE id = :userId");
    query.bindValue(":userId", userId);

    if (query.exec() && query.next()) {
        QByteArray byteArray = query.value(0).toByteArray(); // Извлечение данных из BLOB/bytea
        QPixmap pixmap;
        if (pixmap.loadFromData(byteArray)) { // Загрузка QPixmap из данных
            fishermanImage->setPixmap(pixmap.scaled(500, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            fishermanImage->setAlignment(Qt::AlignCenter);
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить фото из данных.");
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Фотография не найдена:\n" + query.lastError().text());
    }
}
