#ifndef VIEWGRAPHIC_H
#define VIEWGRAPHIC_H

#include <QMainWindow>
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QSqlDatabase>
#include <QSqlQuery>

QT_CHARTS_USE_NAMESPACE

    class ViewGraphic : public QMainWindow
{
    Q_OBJECT
public:
    explicit ViewGraphic(QWidget *parent = nullptr);
    ~ViewGraphic();
    void connectToDatabase();
    void plotWeightByYear();

private:
    QChartView *chartView;
    QLineSeries *series;
};

#endif // VIEWGRAPHIC_H
