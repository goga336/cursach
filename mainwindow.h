#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "iwindow.h"

#include <QScrollArea>
#include <QMainWindow>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include "daytablewindow.h"

// QT_BEGIN_NAMESPACE
// namespace Ui {
// class MainWindow;
// }
// QT_END_NAMESPACE

class MainWindow : public QMainWindow, public IWindow
{
    Q_OBJECT

public:
    void initialize() override {
        // Реализация метода initialize
    }

    void showWindow() override {
        this->show();
    }
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void createScrollArea();
    void hadlerButton1();
    void hadlerButton2();
    void hadlerButton3();
    void hadlerButton4();
    void hadlerButton5();
    void hadlerButton6();
    void updateWeather();
    void displayWeatherData(const QString &weatherInfo);
    QLabel *labelwelcome;
private:


    QString QVectorToString(const QVector<float> &vec);
    DayTableWindow *daytable;
    void setupUI();
    QLabel* updateFishingStatistics();
    QLabel *recentday;
    QLabel *counter;
    QLabel *avgWeightLabel;
    QLabel *temperatureLabel;
    QLabel *pressureLabel;
    QLabel *waterTemperatureLabel;


public slots:



};
#endif // MAINWINDOW_H
