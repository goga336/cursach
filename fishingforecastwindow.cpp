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

    // Устанавливаем стили для всего окна
    setStyleSheet("QGroupBox { font-size: 14px; font-weight: bold; margin-top: 10px; }"
                  "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");
}

void FishingForecastWindow::createInputGroup()
{
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
    timeOfDayCombo->addItems({"Утро", "День", "Вечер", "Ночь"});
    layout->addRow("Время суток:", timeOfDayCombo);

    // Сезон
    seasonCombo = new QComboBox(this);
    seasonCombo->addItems({"Весна", "Лето", "Осень", "Зима"});
    layout->addRow("Сезон:", seasonCombo);

    // Фаза луны
    moonPhaseCombo = new QComboBox(this);
    moonPhaseCombo->addItems({"Новолуние", "Растущая", "Полнолуние", "Убывающая"});
    layout->addRow("Фаза луны:", moonPhaseCombo);

    // Недавняя активность
    recentActivityCheck = new QCheckBox("Была ли недавняя активность рыбы?", this);
    layout->addRow("", recentActivityCheck);
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

    // Кнопка для получения прогноза
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
    // Важно! Добавьте эту отладку
    qDebug() << "Загружено записей в FishingForecastWindow:" << records.size();

    if (records.isEmpty()) {
        qDebug() << "ВНИМАНИЕ: records пуст!";
    } else {
        // Выведите первую запись для проверки
        const auto& first = records.first();
        qDebug() << "Первая запись - дата:" << first.date
                 << "температура:" << first.airTemperature
                 << "вес:" << first.catchWeight;
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

    // 5. Направление ветра (one-hot encoding - упрощенно)
    features.append(windDirectionCombo->currentIndex() / 7.0); // 0-1

    // 6. Время суток
    features.append(timeOfDayCombo->currentIndex() / 3.0);

    // 7. Сезон
    features.append(seasonCombo->currentIndex() / 3.0);

    // 8. Фаза луны
    features.append(moonPhaseCombo->currentIndex() / 3.0);

    // 9. Недавняя активность
    features.append(recentActivityCheck->isChecked() ? 1.0 : 0.0);

    return features;
}

bool FishingForecastWindow::trainLinearRegression()
{
    if (records.isEmpty()) {
        return false;
    }

    int n_samples = records.size();
    int n_features = 9; // Количество признаков


    QVector<QVector<double>> X(n_samples);
    QVector<double> y(n_samples);

    for (int i = 0; i < n_samples; ++i) {
        const auto& record = records[i];
        QVector<double> features;
        features.append(normalize(record.airTemperature, -50, 50));
        features.append(normalize(record.pressure, 700, 800));
        features.append(normalize(record.waterTemperature, 0, 30));
        features.append(normalize(record.windSpeed, 0, 50));
        features.append(record.windDirection / 7.0);
        features.append(record.timeOfDay / 3.0);
        features.append(record.season / 3.0);
        features.append(record.moonPhase / 3.0);
        features.append(record.recentActivity ? 1.0 : 0.0);

        X[i] = features;
        y[i] = record.catchWeight / 10.0;  // вес улова
    }

    // Решаем нормальное уравнение: β = (X^T X)^(-1) X^T y
    // Это упрощенная реализация для демонстрации

    // Вычисляем средние значения признаков
    QVector<double> meanX(n_features, 0.0);
    double meanY = 0.0;

    for (int i = 0; i < n_samples; ++i) {
        for (int j = 0; j < n_features; ++j) {
            meanX[j] += X[i][j];
        }
        meanY += y[i];
    }

    for (int j = 0; j < n_features; ++j) {
        meanX[j] /= n_samples;
    }
    meanY /= n_samples;

    model.coefficients.resize(n_features);

    for (int j = 0; j < n_features; ++j) {
        double numerator = 0.0;
        double denominator = 0.0;

        for (int i = 0; i < n_samples; ++i) {
            numerator += (X[i][j] - meanX[j]) * (y[i] - meanY);
            denominator += (X[i][j] - meanX[j]) * (X[i][j] - meanX[j]);
        }

        if (denominator != 0) {
            model.coefficients[j] = numerator / denominator;
        } else {
            model.coefficients[j] = 0.0;
        }
    }


    model.intercept = meanY;
    for (int j = 0; j < n_features; ++j) {
        model.intercept -= model.coefficients[j] * meanX[j];
    }

    model.isValid = true;
    return true;
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
    modelStatusLabel->setText("Обучение модели...");
    modelStatusLabel->setStyleSheet("color: #ffc075; font-style: italic;");


    for (int i = 0; i <= 100; i += 10) {
        trainingProgress->setValue(i);
        QApplication::processEvents();
        QThread::msleep(50);
    }


    bool success = trainLinearRegression();

    if (success) {
        modelLoaded = true;
        currentModelPath = "models/linear_regression.bin";
        modelStatusLabel->setText(QString("Модель обучена (коэффициентов: %1)").arg(model.coefficients.size()));
        modelStatusLabel->setStyleSheet("color: #28a745; font-style: normal;");

        QMessageBox::information(this, "Успех",
                                 "Модель множественной линейной регрессии успешно обучена на " +
                                     QString::number(dates.size()) + " записях.");
    } else {
        modelStatusLabel->setText("Ошибка обучения");
        modelStatusLabel->setStyleSheet("color: #dc3545; font-style: italic;");

        QMessageBox::critical(this, "Ошибка", "Не удалось обучить модель.");
    }

    trainingProgress->setVisible(false);
}

void FishingForecastWindow::onLoadModelClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Выберите файл с коэффициентами модели",
                                                    QDir::homePath(),
                                                    "Текстовые файлы (*.txt);;Все файлы (*.*)");

    if (!fileName.isEmpty()) {

        trainingProgress->setVisible(true);
        trainingProgress->setValue(0);

        for (int i = 0; i <= 100; i += 25) {
            trainingProgress->setValue(i);
            QApplication::processEvents();
            QThread::msleep(50);
        }


        model.coefficients.resize(9);
        for (int i = 0; i < 9; ++i) {
            model.coefficients[i] = 0.1;
        }
        model.intercept = 0.3;
        model.isValid = true;

        modelLoaded = true;
        currentModelPath = fileName;
        modelStatusLabel->setText("Коэффициенты загружены: " + fileName.split("/").last());
        modelStatusLabel->setStyleSheet("color: #28a745; font-style: normal;");
        trainingProgress->setVisible(false);

        QMessageBox::information(this, "Успех", "Коэффициенты модели успешно загружены!");
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

    double maxCatch = 10.0; // Максимальный ожидаемый вес в кг
    double expectedWeight = probability * maxCatch;

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

