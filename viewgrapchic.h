// #ifndef VIEWGRAPCHIC_H
// #define VIEWGRAPCHIC_H

// #include <QtCharts/QChartView>
// #include <QtCharts/QLineSeries>
// #include <QMainWindow>
// #include <QVector>
// #include <QDate>
// #include <QSqlQuery>

// class ViewGrapchic : public QMainWindow
// {
//     Q_OBJECT
// public:
//     explicit ViewGrapchic(QWidget *parent = nullptr);

//     void getFromDatabasetoGraphic(QVector<QDate>& vecDate, QVector<float>& vecWeight, const QString &year);
//     void buildgraph(const QVector<QDate>& vecDate, const QVector<float>& vecWeight);

// signals:

// private slots:
//     void onYearChanged(const QString &year);
// };

// #endif // VIEWGRAPCHIC_H
#ifndef VIEWGRAPCHIC_H
#define VIEWGRAPCHIC_H

#include <QMainWindow>
#include <QVector>
#include <QDate>
#include <QString>
#include "iwindow.h"

namespace QtCharts {
class QChartView;
}

class QComboBox;

class ViewGrapchic : public QMainWindow, public IWindow
{
    Q_OBJECT

public:
    explicit ViewGrapchic(QWidget *parent = nullptr);
    ~ViewGrapchic();
    void initialize() override {
        // Реализация метода initialize
    }

    void showWindow() override {
        this->show();
    }
    void setupUI();


private slots:
    void onYearChanged(const QString &year);

private:
    void getFromDatabasetoGraphic(QVector<QDate>& vecDate, QVector<float>& vecWeight, const QString &year);
    void buildgraph(const QVector<QDate>& vecDate, const QVector<float>& vecWeight);

    QComboBox *comboBoxYear;                // Комбо-бокс для выбора года
    QtCharts::QChartView *chartView = nullptr;  // Виджет для отображения графика
};

#endif // VIEWGRAPCHIC_H
