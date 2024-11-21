#ifndef WEATHERWINDOW_H
#define WEATHERWINDOW_H

#include "iwindow.h"

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QLabel>
#include <QSignalMapper>

class WeatherWindow : public QMainWindow, public IWindow
{
    Q_OBJECT

signals:
    void weatherDataReady();

public:
    explicit WeatherWindow(QWidget *parent = nullptr);
    void getParams(double &temperatur, int &humidit, int &pressur, QString &descriptio, QString &cityNam);
    void recordFishingDay();
    void initialize() override {
        // Реализация метода initialize
    }

    void showWindow() override {
        this->show();
    }

private slots:
    void handleWeatherResponse(QNetworkReply *reply);



private:
    QNetworkAccessManager *networkManager;
    QLabel *labelWeather;

    void setupUI();
    void fetchWeather();

    double temperature;
    int humidity;
    int pressure;
    QString description;
    QString cityName;
};

#endif // WEATHERWINDOW_H
