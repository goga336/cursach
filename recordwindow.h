#ifndef RECORDWINDOW_H
#define RECORDWINDOW_H

#include "iwindow.h"
#include <QMainWindow>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDoubleValidator>
#include <QTimeEdit>
#include <QMessageBox>
#include <QComboBox>
#include <QCheckBox>
#include <QTextEdit>

class RecordWindow : public QMainWindow, public IWindow
{
    Q_OBJECT

public:
    explicit RecordWindow(QWidget *parent = nullptr);
    ~RecordWindow();
    void recordFishingDay();
    void initialize() override;
    void showWindow() override;

private:
    QDoubleSpinBox *fishWeight;
    QLabel *labelWeight;
    QDateEdit *fishDate;
    QDoubleSpinBox *tempWater;
    QDoubleSpinBox *pressureInput;
    QPushButton *send;
    QTimeEdit *startFishing;
    QTimeEdit *endFishing;
    QDoubleSpinBox *tempAir;
    QDoubleSpinBox *windSpeed;
    QComboBox *timeOfDay;
    QComboBox *windDirection;
    QComboBox *season;
    QComboBox *moonPhase;
    QCheckBox *recentActivity;
    QTextEdit *note;
    void setupUI();



};
#endif // RECORDWINDOW_H
