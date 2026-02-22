#ifndef FISHINGFORECASTWINDOW_H
#define FISHINGFORECASTWINDOW_H

#include "iwindow.h"
#include "daytablewindow.h"
#include "weatherservice.h"
#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>
#include <QTextEdit>
#include <QProgressBar>
#include <QCheckBox>
#include <QVector>
#include <QRandomGenerator>


struct LinearRegressionModel {
    QVector<double> coefficients;
    double intercept;
    bool isValid;
    double maxWeight;

    LinearRegressionModel() : intercept(0.0), isValid(false) {}
};

class FishingForecastWindow : public QMainWindow, public IWindow
{
    Q_OBJECT

public:
    explicit FishingForecastWindow(QWidget *parent = nullptr);
    ~FishingForecastWindow();

    void showWindow() override;
    void initialize() override;

private slots:
    void onTrainModelClicked();
    void onLoadModelClicked();
    void onMakePredictionClicked();

private:
    void setupUI();
    void createInputGroup();
    void createModelGroup();
    void createResultGroup();
    void loadWeatherDataForPrediction();
    void updatePredictionResult(double probability, double expectedWeight);
    void exportToCSV();
    void trainModelPy();
    bool loadPythonModel(const QString& filename);


    bool trainLinearRegression();
    double predictWithModel(const QVector<double> &features);
    QVector<double> prepareFeaturesFromInput();

    void onWeatherLoaded();
    void onWeatherError(const QString &error);

    DayTableWindow *daytable;
    QGroupBox *inputGroup;
    QDateTimeEdit *dateTimeEdit;
    QDoubleSpinBox *airTempSpinBox;
    QDoubleSpinBox *pressureSpinBox;
    QDoubleSpinBox *waterTempSpinBox;
    QDoubleSpinBox *windSpeedSpinBox;
    QComboBox *windDirectionCombo;
    QComboBox *seasonCombo;
    QComboBox *moonPhaseCombo;
    QComboBox *timeOfDayCombo;
    QCheckBox *recentActivityCheck;

    QGroupBox *modelGroup;
    QPushButton *trainButton;
    QPushButton *loadModelButton;
    QLabel *modelStatusLabel;
    QProgressBar *trainingProgress;

    QGroupBox *resultGroup;
    QLabel *probabilityLabel;
    QLabel *expectedWeightLabel;
    QLabel *recommendationLabel;
    QPushButton *predictButton;
    QPushButton *backButton;
    QPushButton *weatherButton;


    QVector<QDate> dates;
    QVector<float> catches;

    LinearRegressionModel model;

    bool modelLoaded;
    QString currentModelPath;

    QVector<FishingRecord> records;

    WeatherService *weather;
};

#endif // FISHINGFORECASTWINDOW_H
