#ifndef WEATHER_H
#define WEATHER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class WeatherService : public QObject
{
    Q_OBJECT
public:
    explicit WeatherService(QObject *parent = nullptr);

    // Простой метод запросить погоду для города
    void requestWeather(const QString &city);

    // Данные о погоде (публичные для простоты)
    double temperature = 0;
    double pressure = 0;
    double windSpeed = 0;
    int windDeg = 0;
    QString description;
    QString cityName;
    bool isSuccess = false;

signals:
    // Сигнал, когда погода загружена
    void weatherLoaded();
    void errorOccurred(const QString &error);

private slots:
    void onFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
    QString apiKey = "fd1dc3c51b504a0a030e072fa0559be7";
};

#endif // WEATHER_H
