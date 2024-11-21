#include <QtTest/QtTest>
// #include "daytablewindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QTime>
#include <QImage>
#include <QBuffer>
#include <QDebug>

#include "testdaytablewindow.h"
#include <QTest>
#include <QApplication>
#include <QSignalSpy>

TestDayTableWindow::TestDayTableWindow(QObject *parent)
    : QObject{parent}
{}


void TestDayTableWindow::initTestCase() {
    if (!QSqlDatabase::contains("mainConnection")) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", "mainConnection");
        db.setDatabaseName("fishingdays");
        db.setHostName("localhost");
        db.setPort(5432);
        db.setUserName("postgres");
        db.setPassword("3360146");

        if (!db.open()) {
            QMessageBox::information(nullptr, "Ошибка", "Не удалось подключиться к базе данных: " + db.lastError().text());
        }
    }
}


void TestDayTableWindow::cleanupTestCase() {

}


void TestDayTableWindow::testInsertIntoDatabase() {
    QDate date = QDate::currentDate();
    QTime start_time = QTime::currentTime();
    QTime end_time = start_time.addSecs(3600); // 1 час
    float weight = 5.5;

    window->connectToDatabase();
    window->insertIntoDatabase(date, start_time, end_time, weight);

    QSqlDatabase db = QSqlDatabase::database("mainConnection");
    QSqlQuery query(db);
    query.exec("SELECT COUNT(*) FROM fishingday WHERE weight = 5.5");
    query.next();
    QCOMPARE(query.value(0).toInt(), 1);

    // Удаление записи после проверки
    query.prepare("DELETE FROM fishingday WHERE weight = :weight");
    query.bindValue(":weight", weight);
    if (!query.exec()) {
        qDebug() << "Ошибка удаления записи:" << query.lastError().text();
    }
}

void TestDayTableWindow::testInsertSettings() {
    // Подключение к базе данных
    window->connectToDatabase();

    // Получаем текущее имя пользователя из базы данных
    QSqlDatabase db = QSqlDatabase::database("mainConnection");
    QSqlQuery query(db);
    query.exec("SELECT user_name FROM fisher WHERE id = 1");
    query.next();
    QString currentName = query.value(0).toString();

    // Проверяем, что имя не пустое
    QVERIFY(!currentName.isEmpty());

    // Изменим имя пользователя
    QString newName = "UpdatedUser";
    window->insertSettings(newName);

    // Проверим, что имя обновилось
    query.exec("SELECT user_name FROM fisher WHERE id = 1");
    query.next();
    QCOMPARE(query.value(0).toString(), newName);

    // Восстанавливаем имя в исходное состояние
    window->insertSettings(currentName);
}

void TestDayTableWindow::testGetFromDatabase() {
    // Подключаемся к базе данных
    window->connectToDatabase();

    // Инициализируем переменную для имени
    QString retrievedName;

    // Получаем имя из базы данных
    window->getFromDatabase(retrievedName);

    // Проверяем, что имя не пустое (мы ожидаем, что оно будет)
    QVERIFY(!retrievedName.isEmpty());

    // Вставляем тестовое имя, если его еще нет
    QString testName = "TestUser";
    QSqlQuery query;
    query.exec("SELECT user_name FROM fisher WHERE id = 1");
    query.next();
    if (query.value(0).toString() != testName) {
        // Если имя не совпадает, вставляем его
        window->insertSettings(testName);
    }

    // Еще раз получаем имя и проверяем, что оно совпадает
    window->getFromDatabase(retrievedName);
    QCOMPARE(retrievedName, testName);
}



// void TestDayTableWindow::testInsertSettingsPhoto() {
//     QImage photo(100, 100, QImage::Format_RGB32);
//     photo.fill(Qt::red);

//     window->insertSettingsPhoto(photo);

//     QSqlQuery query(testDb);
//     query.exec("SELECT photo FROM fisher WHERE id = 1");
//     query.next();
//     QByteArray photoData = query.value(0).toByteArray();

//     QImage loadedPhoto;
//     loadedPhoto.loadFromData(photoData, "PNG");
//     QCOMPARE(loadedPhoto.size(), QSize(100, 100));
// }

// void TestDayTableWindow::testGetFromDatabase() {
//     QString name;
//     window->getFromDatabase(name);
//     QCOMPARE(name, "TestUser");
// }

// void TestDayTableWindow::testGetFromDatabasetoGraphic() {
//     QVector<QDate> dates;
//     QVector<float> weights;

//     window->getFromDatabasetoGraphic(dates, weights);

//     QCOMPARE(dates.size(), 2);
//     QCOMPARE(weights.size(), 2);
//     QCOMPARE(weights[0], 3.5f);
// }

// void TestDayTableWindow::testGetBestFishingDay() {
//     QDate bestDate;
//     float bestWeight;

//     window->getBestFishingDay(bestDate, bestWeight);

//     QCOMPARE(bestDate, QDate(2024, 11, 18));
//     QCOMPARE(bestWeight, 5.0f);
// }

// void TestDayTableWindow::testGetTotalFishingDays() {
//     int totalDays;
//     window->getTotalFishingDays(totalDays);
//     QCOMPARE(totalDays, 2);
// }

// void TestDayTableWindow::testGetAverageWeatherConditionsForSuccessfulFishing() {
//     float avgTemperature, avgPressure, avgWaterTemperature, averageWeight;
//     window->getAverageWeatherConditionsForSuccessfulFishing(avgTemperature, avgPressure, avgWaterTemperature, averageWeight);

//     QCOMPARE(avgTemperature, 15.0f);
//     QCOMPARE(avgPressure, 1015.0f);
// }

// void TestDayTableWindow::testLoadFisherPhoto() {
//     QLabel label;
//     window->loadFisherPhoto(&label, 1);
//     QVERIFY(!label.pixmap()->isNull());
// }

// // Точка входа для тестов
// QTEST_MAIN(TestDayTableWindow)
// #include "test_daytablewindow.moc"
