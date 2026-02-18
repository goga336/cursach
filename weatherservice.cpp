#include "weatherservice.h"

WeatherService::WeatherService(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished,
            this, &WeatherService::onFinished);
}

void WeatherService::requestWeather(const QString &city)
{
    QString url = QString("https://api.openweathermap.org/data/2.5/weather?q=%1&appid=%2&units=metric")
    .arg(city, apiKey);

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    manager->get(request);
}

void WeatherService::onFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObj = jsonDoc.object();

        // Сохраняем данные
        temperature = jsonObj["main"].toObject()["temp"].toDouble();
        pressure = jsonObj["main"].toObject()["pressure"].toDouble();
        windSpeed = jsonObj["wind"].toObject()["speed"].toDouble();
        windDeg = jsonObj["wind"].toObject()["deg"].toInt();
        description = jsonObj["weather"].toArray()[0].toObject()["description"].toString();
        cityName = jsonObj["name"].toString();
        isSuccess = true;

        emit weatherLoaded();
    }
    else
    {
        isSuccess = false;
        emit errorOccurred(reply->errorString());
    }
    reply->deleteLater();
}
