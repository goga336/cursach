#include "weatherfetcher.h"
#include <QVBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

WeatherWindow::WeatherWindow(QWidget *parent)
    : QMainWindow(parent), networkManager(new QNetworkAccessManager(this))
{

}

void WeatherWindow::initialize()
{
    setupUI();
    fetchWeather();
}

void WeatherWindow::showWindow()
{
    this->show();
}

void WeatherWindow::setupUI()
{
    this->setWindowTitle("Текущая погода");
    labelWeather = new QLabel("Загрузка данных о погоде...", this);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(labelWeather);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

void WeatherWindow::fetchWeather()
{
    QString apiKey = "fd1dc3c51b504a0a030e072fa0559be7";
    QString city = "Moscow";
    QString url = QString("https://api.openweathermap.org/data/2.5/weather?q=%1&appid=%2&units=metric").arg(city, apiKey);

    QNetworkRequest request;
    request.setUrl(QUrl(url));

    connect(networkManager, &QNetworkAccessManager::finished, this, &WeatherWindow::handleWeatherResponse);
    networkManager->get(request);
}


// void WeatherWindow::handleWeatherResponse(QNetworkReply *reply)
// {
//     if (reply->error() == QNetworkReply::NoError)
//     {
//         QByteArray response = reply->readAll();
//         QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
//         QJsonObject jsonObj = jsonDoc.object();

//         temperature = jsonObj["main"].toObject()["temp"].toDouble();
//         humidity = jsonObj["main"].toObject()["humidity"].toInt();
//         pressure = jsonObj["main"].toObject()["pressure"].toInt();
//         description = jsonObj["weather"].toArray()[0].toObject()["description"].toString();
//         cityName = jsonObj["name"].toString();

//         labelWeather->setText(QString("Город: %1\nТемпература: %2°C\nВлажность: %3%\nДавление: %4 гПа\nОписание: %5")
//                                   .arg(cityName)
//                                   .arg(temperature)
//                                   .arg(humidity)
//                                   .arg(pressure)
//                                   .arg(description));
//     }
//     else
//     {
//         labelWeather->setText("Ошибка при загрузке погоды.");
//     }
//     reply->deleteLater();
// }

void WeatherWindow::handleWeatherResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObj = jsonDoc.object();

        temperature = jsonObj["main"].toObject()["temp"].toDouble();
        humidity = jsonObj["main"].toObject()["humidity"].toInt();
        pressure = jsonObj["main"].toObject()["pressure"].toInt();
        description = jsonObj["weather"].toArray()[0].toObject()["description"].toString();
        cityName = jsonObj["name"].toString();

        double feelsLike = jsonObj["main"].toObject()["feels_like"].toDouble();
        double tempMin = jsonObj["main"].toObject()["temp_min"].toDouble();
        double tempMax = jsonObj["main"].toObject()["temp_max"].toDouble();
        //int seaLevel = jsonObj["main"].toObject()["sea_level"].toInt(); // давление на уровне моря
        //int grndLevel = jsonObj["main"].toObject()["grnd_level"].toInt(); // давление на уровне земли


        double windSpeed = jsonObj["wind"].toObject()["speed"].toDouble();
        int windDeg = jsonObj["wind"].toObject()["deg"].toInt();
        ///double windGust = jsonObj["wind"].toObject()["gust"].toDouble();

        int clouds = jsonObj["clouds"].toObject()["all"].toInt();

        int visibility = jsonObj["visibility"].toInt();

        qint64 sunrise = jsonObj["sys"].toObject()["sunrise"].toInt();
        qint64 sunset = jsonObj["sys"].toObject()["sunset"].toInt();
        QString country = jsonObj["sys"].toObject()["country"].toString();

        QString mainWeather = jsonObj["weather"].toArray()[0].toObject()["main"].toString();
        QString iconCode = jsonObj["weather"].toArray()[0].toObject()["icon"].toString();


        labelWeather->setText(QString("Город: %1, %2\n"
                                      "Температура: %3°C (ощущается как %4°C)\n"
                                      "Минимум: %5°C, Максимум: %6°C\n"
                                      "Влажность: %7%\n"
                                      "Давление: %8 гПа\n"
                                      "Ветер: %9 м/с, направление: %10°\n"
                                      "Облачность: %11%\n"
                                      "Видимость: %12 м\n"
                                      "Описание: %13\n"
                                      "Восход: %14, Закат: %15")
                                  .arg(cityName)
                                  .arg(country)
                                  .arg(temperature)
                                  .arg(feelsLike)
                                  .arg(tempMin)
                                  .arg(tempMax)
                                  .arg(humidity)
                                  .arg(pressure)
                                  .arg(windSpeed)
                                  .arg(windDeg)
                                  .arg(clouds)
                                  .arg(visibility)
                                  .arg(description)
                                  .arg(QDateTime::fromSecsSinceEpoch(sunrise).toString("hh:mm"))
                                  .arg(QDateTime::fromSecsSinceEpoch(sunset).toString("hh:mm")));

    }
    else
    {
        labelWeather->setText("Ошибка при загрузке погоды.");
    }
    reply->deleteLater();
}
