#ifndef VIEWGRAPCHIC_H
#define VIEWGRAPCHIC_H

#include <QMainWindow>
#include <QVector>
#include <QDate>
#include <QString>
#include <QChartView>
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
    void initialize() override;
    void showWindow() override;
    void setupUI();


private slots:
    void onYearChanged(const QString &year);

private:
    void getFromDatabasetoGraphic(QVector<QDate>& vecDate, QVector<float>& vecWeight, const QString &year);
    void buildgraph(const QVector<QDate>& vecDate, const QVector<float>& vecWeight);

    QComboBox *comboBoxYear;                // Комбо-бокс для выбора года
    QChartView *chartView = nullptr;  // Виджет для отображения графика
};

#endif // VIEWGRAPCHIC_H
