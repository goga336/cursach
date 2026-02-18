#ifndef DAYTABLEWINDOW_H
#define DAYTABLEWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QVariant>
#include <QTime>
#include <QDate>

struct FishingRecord {
    QDateTime date;
    double waterTemperature;
    double airTemperature;
    double windSpeed;
    double pressure;
    int windDirection;
    int timeOfDay;
    int season;
    int moonPhase;
    bool recentActivity;
    double catchWeight;
};

class DayTableWindow: public QMainWindow
{
public:
    DayTableWindow(QWidget *parent = nullptr);
    ~DayTableWindow();
    void connectToDatabase();
    void insertIntoDatabase(const QDate &date, const QTime &start_time, const QTime &end_time, float weight);
    void getFromDatabase(QString &name);
    void getFromDatabasetoGraphic(QVector<QDate> &vecDate, QVector<float> &vecWeight);
    void getBestFishingDay(QDate &date, float &weight);
    void getTotalFishingDays(int &totalDays);
    void insertSettings(QString &name);
    void getAverageWeatherConditionsForSuccessfulFishing(float &avgTemperature, float &avgPressure, float &avgWaterTemperature, float &averageWeight);
    void insertSettingsPhoto(QImage &userPhoto);
    void loadFisherPhoto(QLabel *fishermanImage, int userId);
    QVector<FishingRecord> getAllDataForPrediction();
};

#endif // DAYTABLEWINDOW_H
