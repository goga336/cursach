#ifndef VIEWGRAPCHIC_H
#define VIEWGRAPCHIC_H

#include <QMainWindow>
#include <QChartView>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>
#include "iwindow.h"

class QComboBox;
class QLabel;
class QWidget;
class DayTableWindow;

class ViewGrapchic : public QMainWindow, public IWindow
{
    Q_OBJECT
public:
    explicit ViewGrapchic(QWidget *parent = nullptr);
    ~ViewGrapchic();

    void showWindow();
    void initialize();

private slots:
    void onMainComboBoxChanged(const QString &text);
    void onOverlayYearChanged();

private:
    void setupUI();
    void loadYearsToComboBox(QComboBox *comboBox);
    void buildSingleYearGraph(const QString &year);
    void buildAllYearsGraph();
    void buildOverlayGraph(const QString &year1, const QString &year2);
    void clearChart();

    DayTableWindow *dbManager;
    QChartView *chartView;


    QComboBox *mainComboBox;
    QComboBox *overlayYear1;
    QComboBox *overlayYear2;
    QWidget *overlayPanel;
};

#endif // VIEWGRAPCHIC_H
