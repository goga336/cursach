#include "weatherfetcher.h"
#include <QVBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

WeatherWindow::WeatherWindow(QWidget *parent)
    : QMainWindow(parent), networkManager(new QNetworkAccessManager(this))
{
    setupUI();
    fetchWeather();
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

        labelWeather->setText(QString("Город: %1\nТемпература: %2°C\nВлажность: %3%\nДавление: %4 гПа\nОписание: %5")
                                  .arg(cityName)
                                  .arg(temperature)
                                  .arg(humidity)
                                  .arg(pressure)
                                  .arg(description));
    }
    else
    {
        labelWeather->setText("Ошибка при загрузке погоды.");
    }
    reply->deleteLater();
}

void WeatherWindow::getParams(double &temperatur, int &humidit, int &pressur, QString &descriptio, QString &cityNam) {
    temperatur = this->temperature;
    humidit = this->humidity;
    pressur = this->pressure;
    descriptio = this->description;
    cityNam = this->cityName;

    QString message = QString("Город: %1\nТемпература: %.2f °C\nВлажность: %2d%%\nДавление: %3d hPa\nОписание: %4")
                          .arg(cityName)
                          .arg(temperature)
                          .arg(humidity)
                          .arg(pressure)
                          .arg(description);

    QMessageBox::information(nullptr, "Данные о погоде", message);
}

