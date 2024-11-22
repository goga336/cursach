#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "iwindow.h"
#include "daytablewindow.h"

#include <QScrollArea>
#include <QMainWindow>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QVector>
#include <QDate>
#include <QTimer>



class MainWindow : public QMainWindow, public IWindow
{
    Q_OBJECT

public:
    void initialize() override;
    void showWindow() override;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateWeather();
    void displayWeatherData(const QString &weatherInfo);
    QLabel *labelwelcome;
private:
    QString QVectorToString(const QVector<float> &vec);
    DayTableWindow *daytable;
    void setupUI();
    void createScrollArea();
    void updateStatisticsUI();
    QLabel *recentday;
    QLabel *counter;
    QLabel *avgWeightLabel;
    QLabel *temperatureLabel;
    QLabel *pressureLabel;
    QLabel *waterTemperatureLabel;



public slots:
    void hadlerButton1();
    void hadlerButton2();
    void hadlerButton3();
    void hadlerButton4();
    void hadlerButton5();
    void hadlerButton6();


};
#endif // MAINWINDOW_H
