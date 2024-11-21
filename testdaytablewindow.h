#ifndef TESTDAYTABLEWINDOW_H
#define TESTDAYTABLEWINDOW_H
#include "daytablewindow.h"
#include <QObject>
#include <QtTest/QtTest>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QTime>
#include <QImage>
#include <QBuffer>
#include <QDebug>

class TestDayTableWindow : public QObject
{
    Q_OBJECT
public:
    explicit TestDayTableWindow(QObject *parent = nullptr);
private slots:
    void initTestCase();  // Подготовка тестовой базы данных
    void cleanupTestCase(); // Удаление базы данных после тестов

    void testInsertIntoDatabase();
    void testInsertSettings();
//     void testInsertSettingsPhoto();
    void testGetFromDatabase();
//     void testGetFromDatabasetoGraphic();
//    void testGetBestFishingDay();
//     void testGetTotalFishingDays();
//     void testGetAverageWeatherConditionsForSuccessfulFishing();
private:
    DayTableWindow *window;
    QSqlDatabase testDb;
};

#endif // TESTDAYTABLEWINDOW_H
