#include "fishingforecastwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QCheckBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QDebug>
#include <QApplication>
#include <QThread>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProcess>
#include <QInputDialog>


double normalize(double value, double min, double max) {
    if (max - min == 0) return 0.5;
    return (value - min) / (max - min);
}

FishingForecastWindow::FishingForecastWindow(QWidget *parent)
    : QMainWindow(parent)
    , daytable(new DayTableWindow)
    , modelLoaded(false)
{
    model.isValid = false;
    weather = new WeatherService(this);

    connect(weather, &WeatherService::weatherLoaded,
            this, &FishingForecastWindow::onWeatherLoaded);
    connect(weather, &WeatherService::errorOccurred,
            this, &FishingForecastWindow::onWeatherError);
}

FishingForecastWindow::~FishingForecastWindow()
{
    delete daytable;
}

void FishingForecastWindow::showWindow()
{
    this->show();
}

void FishingForecastWindow::initialize()
{
    setupUI();
    loadWeatherDataForPrediction();
}

void FishingForecastWindow::setupUI()
{
    resize(800, 600);
    setWindowTitle("Прогноз клева");

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);


    createInputGroup();
    createModelGroup();
    createResultGroup();

    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(modelGroup);
    mainLayout->addWidget(resultGroup);


    setCentralWidget(centralWidget);

    setStyleSheet("QGroupBox { font-size: 14px; font-weight: bold; margin-top: 10px;  }"
                  "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");
}

void FishingForecastWindow::createInputGroup()
{
    QPalette palette;
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0.0, QColor(30, 90, 120));     // светло-синий сверху
    gradient.setColorAt(0.5, QColor(70, 150, 170));    // мягкий бирюзовый
    gradient.setColorAt(1.0, QColor(130, 200, 210));   // очень светлый снизу

    palette.setBrush(QPalette::Window, QBrush(gradient));
    this->setPalette(palette);
    this->setAutoFillBackground(true);
    inputGroup = new QGroupBox("Введите текущие погодные условия", this);
    QFormLayout *layout = new QFormLayout(inputGroup);

    // Дата и время
    dateTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), this);
    dateTimeEdit->setCalendarPopup(true);
    layout->addRow("Дата и время:", dateTimeEdit);

    // Температура воздуха
    airTempSpinBox = new QDoubleSpinBox(this);
    airTempSpinBox->setRange(-50, 50);
    airTempSpinBox->setSuffix(" °C");
    airTempSpinBox->setValue(15.0);
    layout->addRow("Температура воздуха:", airTempSpinBox);

    // Атмосферное давление
    pressureSpinBox = new QDoubleSpinBox(this);
    pressureSpinBox->setRange(700, 800);
    pressureSpinBox->setSuffix(" мм рт. ст.");
    pressureSpinBox->setValue(750.0);
    layout->addRow("Давление:", pressureSpinBox);

    // Температура воды
    waterTempSpinBox = new QDoubleSpinBox(this);
    waterTempSpinBox->setRange(0, 30);
    waterTempSpinBox->setSuffix(" °C");
    waterTempSpinBox->setValue(12.0);
    layout->addRow("Температура воды:", waterTempSpinBox);

    // Скорость ветра
    windSpeedSpinBox = new QDoubleSpinBox(this);
    windSpeedSpinBox->setRange(0, 50);
    windSpeedSpinBox->setSuffix(" м/с");
    windSpeedSpinBox->setValue(5.0);
    layout->addRow("Скорость ветра:", windSpeedSpinBox);

    // Направление ветра
    windDirectionCombo = new QComboBox(this);
    windDirectionCombo->addItems({"Северный", "Северо-восточный", "Восточный",
                                  "Юго-восточный", "Южный", "Юго-западный",
                                  "Западный", "Северо-западный"});
    layout->addRow("Направление ветра:", windDirectionCombo);

    // Время суток
    timeOfDayCombo = new QComboBox(this);
    timeOfDayCombo->addItems({"День", "Ночь"});
    layout->addRow("Время суток:", timeOfDayCombo);

    // Сезон
    seasonCombo = new QComboBox(this);
    seasonCombo->addItems({"Зима", "Весна", "Лето", "Осень"});
    layout->addRow("Сезон:", seasonCombo);

    // Фаза луны
    moonPhaseCombo = new QComboBox(this);
    moonPhaseCombo->addItems({"Новолуние", "Растущий серп", "Полнолуние", "Убывающий серп"});
    layout->addRow("Фаза луны:", moonPhaseCombo);

    // Недавняя активность
    recentActivityCheck = new QCheckBox("Была ли недавняя активность рыбы?", this);
    layout->addRow("", recentActivityCheck);

    weatherButton = new QPushButton ("Загрузить погоду");
    weatherButton->setStyleSheet("QPushButton {"
                                 "font-size: 14px;"
                                 "font-weight: bold;"
                                 "color: white;"
                                 "background-color: #007BFF;"
                                 "border-radius: 5px;"
                                 "padding: 8px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "background-color: #0056b3;"
                                 "}");
    layout->addRow("", weatherButton);
    connect(weatherButton, &QPushButton::clicked, [this]() {
        QStringList cities = {"Москва", "Санкт-Петербург", "Нижний Новгород",
                              "Рязань", "Воронеж", "Тула", "Ярославль"};
        bool ok;
        QString city = QInputDialog::getItem(this, "Выберите город",
                                             "Город:", cities, 0, false, &ok);
        if (ok && !city.isEmpty()) {
            weather->requestWeather(city);
        }
    });
}

void FishingForecastWindow::createModelGroup()
{
    modelGroup = new QGroupBox("Управление моделью", this);
    QVBoxLayout *mainLayout = new QVBoxLayout(modelGroup);

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    trainButton = new QPushButton("Обучить модель (линейная регрессия)", this);
    trainButton->setStyleSheet("QPushButton { background-color: #28a745; color: white; padding: 8px; border-radius: 5px; }"
                               "QPushButton:hover { background-color: #218838; }");
    connect(trainButton, &QPushButton::clicked, this, &FishingForecastWindow::onTrainModelClicked);

    loadModelButton = new QPushButton("Загрузить коэффициенты", this);
    loadModelButton->setStyleSheet("QPushButton { background-color: #17a2b8; color: white; padding: 8px; border-radius: 5px; }"
                                   "QPushButton:hover { background-color: #138496; }");
    connect(loadModelButton, &QPushButton::clicked, this, &FishingForecastWindow::onLoadModelClicked);

    buttonLayout->addWidget(trainButton);
    buttonLayout->addWidget(loadModelButton);
    mainLayout->addLayout(buttonLayout);


    QLabel *modelInfoLabel = new QLabel("Используется множественная линейная регрессия", this);
    modelInfoLabel->setStyleSheet("color: #666; font-style: italic;");
    mainLayout->addWidget(modelInfoLabel);


    modelStatusLabel = new QLabel("Модель не обучена", this);
    modelStatusLabel->setStyleSheet("color: #dc3545; font-style: italic;");
    mainLayout->addWidget(modelStatusLabel);

    trainingProgress = new QProgressBar(this);
    trainingProgress->setRange(0, 100);
    trainingProgress->setValue(0);
    trainingProgress->setVisible(false);
    mainLayout->addWidget(trainingProgress);
}

void FishingForecastWindow::createResultGroup()
{
    resultGroup = new QGroupBox("Результат прогноза", this);
    QVBoxLayout *layout = new QVBoxLayout(resultGroup);

    predictButton = new QPushButton("Получить прогноз", this);
    predictButton->setStyleSheet("QPushButton { background-color: #007bff; color: white; padding: 10px; border-radius: 5px; font-size: 16px; }"
                                 "QPushButton:hover { background-color: #0056b3; }");
    connect(predictButton, &QPushButton::clicked, this, &FishingForecastWindow::onMakePredictionClicked);
    layout->addWidget(predictButton);

    // Вероятность клева
    probabilityLabel = new QLabel("Вероятность клева: —", this);
    probabilityLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #28a745;");
    layout->addWidget(probabilityLabel);

    // Ожидаемый вес
    expectedWeightLabel = new QLabel("Ожидаемый вес улова: —", this);
    expectedWeightLabel->setStyleSheet("font-size: 14px;");
    layout->addWidget(expectedWeightLabel);

    // Рекомендации
    recommendationLabel = new QLabel("Рекомендации: —", this);
    recommendationLabel->setWordWrap(true);
    recommendationLabel->setStyleSheet("font-size: 14px; font-style: italic;");
    layout->addWidget(recommendationLabel);
}

void FishingForecastWindow::loadWeatherDataForPrediction()
{
    records = daytable->getAllDataForPrediction();
    qDebug() << "Загружено записей в FishingForecastWindow:" << records.size();

    if (records.isEmpty()) {
        qDebug() << "ВНИМАНИЕ: records пуст!";
    }
}



void FishingForecastWindow::exportToCSV()
{
    QFile file("fishing_data.csv");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "air_temperature,pressure,water_temperature,wind_speed,"
        << "wind_direction,time_of_day,season,moon_phase,recent_activity,catch_weight\n";

    for (const auto& r : records) {
        // НОРМАЛИЗУЕМ значения

        // time_of_day: 0=День, 1=Ночь -> 0.0 или 1.0
        double timeOfDayNorm = r.timeOfDay;  // уже 0 или 1

        // season: 0=Зима, 1=Весна, 2=Лето, 3=Осень -> 0.0, 0.33, 0.66, 1.0
        double seasonNorm = r.season / 3.0;

        // moon_phase: 0=Новолуние, 1=Растущий серп, 2=Полнолуние, 3=Убывающий серп -> 0.0, 0.33, 0.66, 1.0
        double moonPhaseNorm = r.moonPhase / 3.0;

        // wind_direction: 0-7 -> 0.0-1.0
        double windDirNorm = r.windDirection / 7.0;

        out << r.airTemperature << ","
            << r.pressure << ","
            << r.waterTemperature << ","
            << r.windSpeed << ","
            << windDirNorm << ","
            << timeOfDayNorm << ","      // 0.0 или 1.0
            << seasonNorm << ","         // 0.0, 0.33, 0.66, 1.0
            << moonPhaseNorm << ","      // 0.0, 0.33, 0.66, 1.0
            << (r.recentActivity ? 1 : 0) << ","
            << r.catchWeight << "\n";
    }
    file.close();

}

bool FishingForecastWindow::loadPythonModel(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    // Загружаем коэффициенты
    QJsonArray coefArray = obj["coefficients"].toArray();
    model.coefficients.resize(coefArray.size());
    for (int i = 0; i < coefArray.size(); ++i)
        model.coefficients[i] = coefArray[i].toDouble();

    model.intercept = obj["intercept"].toDouble();
    model.maxWeight = obj["max_weight"].toDouble();
    model.isValid = true;

    //загружаем метрики
    QJsonObject metrics = obj["metrics"].toObject();
    modelMetrics.r2 = metrics["r2"].toDouble();
    modelMetrics.train_r2 = metrics["train_r2"].toDouble();
    modelMetrics.test_r2 = metrics["test_r2"].toDouble();
    modelMetrics.gap = metrics["gap"].toDouble();
    modelMetrics.cv_mean = metrics["cv_mean"].toDouble();
    modelMetrics.cv_std = metrics["cv_std"].toDouble();

    return true;
}

void FishingForecastWindow::trainModelPy() {
    exportToCSV();

    QProcess process;
    process.start("python", QStringList() << "/home/goga/Desktop/study/8_semestr/diplom/fish_app/cursach/trainmodel.py");
    process.waitForFinished(-1); // ждём завершения

    if (process.exitCode() != 0) {
        QMessageBox::critical(this, "Ошибка",
                              "Python-скрипт завершился с ошибкой:\n" +
                                  process.readAllStandardError());
        return;
    }
    QString modelPath = "/home/goga/Desktop/study/8_semestr/diplom/fish_app/cursach/build/Desktop-Debug/model_coefficients.json";
    if (loadPythonModel(modelPath)) {
        modelLoaded = true;
        modelStatusLabel->setText("Модель обучена через Python");
        modelStatusLabel->setStyleSheet("color: #28a745; font-style: normal;");
        QMessageBox::information(this, "Успех", "Модель обучена через Python!");
    }
}

QVector<double> FishingForecastWindow::prepareFeaturesFromInput()

{
    QVector<double> features;

    // 1. Температура воздуха (нормализованная)
    features.append(normalize(airTempSpinBox->value(), -50, 50));

    // 2. Давление (нормализованное)
    features.append(normalize(pressureSpinBox->value(), 700, 800));

    // 3. Температура воды (нормализованная)
    features.append(normalize(waterTempSpinBox->value(), 0, 30));

    // 4. Скорость ветра (нормализованная)
    features.append(normalize(windSpeedSpinBox->value(), 0, 50));

    // 5. Направление ветра (0-1) - 8 направлений
    features.append(windDirectionCombo->currentIndex() / 7.0);

    // 6. Время суток (0-1) - ТЕПЕРЬ 2 значения!
    features.append(timeOfDayCombo->currentIndex() / 1.0);  // 0 или 1

    // 7. Сезон (0-1) - 4 сезона, но нормализуем в 0.0-1.0
    features.append(seasonCombo->currentIndex() / 3.0);  // 0,1,2,3 -> 0.0,0.33,0.66,1.0

    // 8. Фаза луны (0-1) - 4 фазы
    features.append(moonPhaseCombo->currentIndex() / 3.0);  // 0,1,2,3 -> 0.0,0.33,0.66,1.0

    // 9. Недавняя активность
    features.append(recentActivityCheck->isChecked() ? 1.0 : 0.0);

    return features;
}


double FishingForecastWindow::predictWithModel(const QVector<double> &features)
{
    if (!model.isValid || features.size() != model.coefficients.size()) {
        return 0.5;
    }

    double prediction = model.intercept;
    for (int i = 0; i < features.size(); ++i) {
        prediction += model.coefficients[i] * features[i];
    }

    return qBound(0.0, prediction, 1.0);
}


void FishingForecastWindow::onTrainModelClicked()
{
    if (records.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение",
                             "Нет данных для обучения модели. Сначала добавьте записи о рыбалке.");
        return;
    }

    trainingProgress->setVisible(true);
    trainingProgress->setValue(0);
    modelStatusLabel->setText("Обучение модели через Python...");
    modelStatusLabel->setStyleSheet("color: #ffc075; font-style: italic;");

    trainModelPy();

    if (modelLoaded && model.isValid) {
        QString metricsText;

        // Проверка данных
        int n_samples = records.size();
        int n_features = 9;
        int min_needed = n_features * 10;

        metricsText += "═══════════════════════════════════════\n";
        metricsText += "📊 ДАННЫЕ:\n";
        metricsText += QString("   Записей: %1 (мин. нужно: %2)\n").arg(n_samples).arg(min_needed);

        if (n_samples >= min_needed) {
            metricsText += "   ✅ Данных достаточно\n";
        } else {
            metricsText += QString("   ⚠ Нужно еще %1 записей\n").arg(min_needed - n_samples);
        }

        // Метрики
        metricsText += "\n📈 КАЧЕСТВО МОДЕЛИ:\n";
        metricsText += QString("   R² (общая): %1\n").arg(modelMetrics.r2, 0, 'f', 4);
        metricsText += QString("   Train R²: %1\n").arg(modelMetrics.train_r2, 0, 'f', 4);
        metricsText += QString("   Test R²: %1\n").arg(modelMetrics.test_r2, 0, 'f', 4);
        metricsText += QString("   Разрыв: %1\n").arg(modelMetrics.gap, 0, 'f', 4);
        metricsText += QString("   CV (5-fold): %1 ± %2\n").arg(modelMetrics.cv_mean, 0, 'f', 4).arg(modelMetrics.cv_std, 0, 'f', 4);

        // Диагностика
        metricsText += "\n⚠️ ДИАГНОСТИКА:\n";
        if (modelMetrics.gap > 0.2) {
            metricsText += "   🔴 ВЫСОКИЙ РИСК ПЕРЕОБУЧЕНИЯ!\n";
        } else if (modelMetrics.gap > 0.1) {
            metricsText += "   🟡 ЕСТЬ РИСК ПЕРЕОБУЧЕНИЯ\n";
        } else {
            metricsText += "   ✅ ПЕРЕОБУЧЕНИЯ НЕТ\n";
        }

        if (modelMetrics.cv_std > 0.1) {
            metricsText += "   🔴 Модель нестабильна (CV std > 0.1)\n";
        } else {
            metricsText += "   ✅ Модель стабильна\n";
        }

        metricsText += "═══════════════════════════════════════";

        QMessageBox::information(this, "Результаты обучения", metricsText);
    }

    trainingProgress->setVisible(false);
}



void FishingForecastWindow::onLoadModelClicked()
{
    QString appPath = QCoreApplication::applicationDirPath();
    QString modelsPath = appPath + "/model_coefficients.json";
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Выберите файл с коэффициентами модели",
                                                    modelsPath,
                                                    "JSON файлы (*.json);;Все файлы (*.*)");

    if (!fileName.isEmpty()) {
        trainingProgress->setVisible(true);
        trainingProgress->setValue(0);

        if (loadPythonModel(fileName)) {
            for (int i = 0; i <= 100; i += 25) {
                trainingProgress->setValue(i);
                QApplication::processEvents();
                QThread::msleep(50);
            }

            modelLoaded = true;
            currentModelPath = fileName;
            modelStatusLabel->setText("Коэффициенты загружены: " + fileName.split("/").last());
            modelStatusLabel->setStyleSheet("color: #28a745; font-style: normal;");

            QMessageBox::information(this, "Успех",
                                     QString("Коэффициенты модели успешно загружены!\n"
                                             "Max weight: %1 кг").arg(model.maxWeight));
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить коэффициенты из файла!");
        }

        trainingProgress->setVisible(false);
    }
}
void FishingForecastWindow::onMakePredictionClicked()
{
    if (!modelLoaded || !model.isValid) {
        QMessageBox::warning(this, "Предупреждение",
                             "Сначала обучите или загрузите модель!");
        return;
    }

    QVector<double> features = prepareFeaturesFromInput();
    double probability = predictWithModel(features);

    double expectedWeight = probability * model.maxWeight;

    updatePredictionResult(probability, expectedWeight);
}

void FishingForecastWindow::updatePredictionResult(double probability, double expectedWeight)
{

    int percent = static_cast<int>(probability * 100);
    probabilityLabel->setText("Вероятность клева: " + QString::number(percent) + "%");

    if (percent >= 70) {
        probabilityLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #28a745;");
    } else if (percent >= 40) {
        probabilityLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #ffc075;");
    } else {
        probabilityLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #dc3545;");
    }

    expectedWeightLabel->setText("Ожидаемый вес улова: " + QString::number(expectedWeight, 'f', 2) + " кг");

    QString recommendation;
    if (percent >= 70) {
        recommendation = "Отличные условия для рыбалки! Высокая вероятность хорошего улова. "
                         "Рекомендуется использовать различные приманки.";
    } else if (percent >= 40) {
        recommendation = "Умеренная вероятность клева. Стоит попробовать порыбачить, "
                         "но не рассчитывайте на рекордный улов.";
    } else {
        recommendation = "Низкая вероятность клева. Возможно, стоит выбрать другое время "
                         "или место для рыбалки.";
    }

    if (airTempSpinBox->value() < 5) {
        recommendation += " Обратите внимание: холодно, рыба может быть малоактивной.";
    } else if (airTempSpinBox->value() > 30) {
        recommendation += " Очень жарко, лучше рыбачить ранним утром или вечером.";
    }

    if (windSpeedSpinBox->value() > 10) {
        recommendation += " Сильный ветер может затруднить рыбалку.";
    }

    recommendationLabel->setText("Рекомендации: " + recommendation);
}


void FishingForecastWindow::onWeatherLoaded()
{
    airTempSpinBox->setValue(weather->temperature);
    waterTempSpinBox->setValue(weather->temperature * 0.8);
    pressureSpinBox->setValue(weather->pressure * 0.750062);
    windSpeedSpinBox->setValue(weather->windSpeed);

    QString windDir;
    if (weather->windDeg >= 337.5 || weather->windDeg < 22.5) windDir = "Северный";
    else if (weather->windDeg >= 22.5 && weather->windDeg < 67.5) windDir = "Северо-восточный";
    else if (weather->windDeg >= 67.5 && weather->windDeg < 112.5) windDir = "Восточный";
    else if (weather->windDeg >= 112.5 && weather->windDeg < 157.5) windDir = "Юго-восточный";
    else if (weather->windDeg >= 157.5 && weather->windDeg < 202.5) windDir = "Южный";
    else if (weather->windDeg >= 202.5 && weather->windDeg < 247.5) windDir = "Юго-западный";
    else if (weather->windDeg >= 247.5 && weather->windDeg < 292.5) windDir = "Западный";
    else windDir = "Северо-западный";

    int index = windDirectionCombo->findText(windDir);
    if (index >= 0) windDirectionCombo->setCurrentIndex(index);

    // ВРЕМЯ СУТОК - теперь только День/Ночь
    QTime now = QTime::currentTime();
    bool isDay = (now.hour() >= 6 && now.hour() < 18);
    timeOfDayCombo->setCurrentText(isDay ? "День" : "Ночь");

    // СЕЗОН - порядок: Зима, Весна, Лето, Осень
    int month = QDate::currentDate().month();
    QString seasonStr;
    if (month == 12 || month == 1 || month == 2) seasonStr = "Зима";
    else if (month >= 3 && month <= 5) seasonStr = "Весна";
    else if (month >= 6 && month <= 8) seasonStr = "Лето";
    else seasonStr = "Осень";

    int seasonIndex = seasonCombo->findText(seasonStr);
    if (seasonIndex >= 0) seasonCombo->setCurrentIndex(seasonIndex);


    QMessageBox::information(this, "Успех",
                             QString("Погода для города %1 загружена!").arg(weather->cityName));
}

void FishingForecastWindow::onWeatherError(const QString &error)
{
    QMessageBox::warning(this, "Ошибка",
                         "Не удалось загрузить погоду:\n" + error);
}

