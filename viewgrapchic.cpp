#include "viewgrapchic.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QComboBox>
#include <QVBoxLayout>
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

ViewGrapchic::ViewGrapchic(QWidget *parent)
    : QMainWindow{parent}
{
    setupUI();
}
ViewGrapchic::~ViewGrapchic()
{

}

void ViewGrapchic::showWindow()
{
    this->show();
}

void ViewGrapchic::initialize()
{
    setupUI();
}

void ViewGrapchic::setupUI()
{    comboBoxYear = new QComboBox(this);
    comboBoxYear->addItem("Все года");  // Опция для отображения всех данных
    resize(900, 900);
    QSqlQuery yearQuery("SELECT DISTINCT EXTRACT(YEAR FROM date) AS year FROM fishingday ORDER BY year");
    while (yearQuery.next()) {
        comboBoxYear->addItem(yearQuery.value(0).toString());
    }

    connect(comboBoxYear, &QComboBox::currentTextChanged, this, &ViewGrapchic::onYearChanged);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(comboBoxYear);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Изначально показываем все данные
    QVector<QDate> dates;
    QVector<float> weights;
    getFromDatabasetoGraphic(dates, weights, "");
    buildgraph(dates, weights);}

void ViewGrapchic::onYearChanged(const QString &year) {
    QVector<QDate> dates;
    QVector<float> weights;
    getFromDatabasetoGraphic(dates, weights, year);
    buildgraph(dates, weights);
}

void ViewGrapchic::getFromDatabasetoGraphic(QVector<QDate>& vecDate, QVector<float>& vecWeight, const QString &year) {
    QSqlQuery query;

    QString queryString = "SELECT date, weight FROM fishingday";
    if (!year.isEmpty() && year != "Все года") {
        queryString += " WHERE EXTRACT(YEAR FROM date) = " + year;
    }

    if (query.exec(queryString)) {
        while (query.next()) {
            QDate date = query.value(0).toDate();
            float weight = query.value(1).toFloat();
            vecDate.append(date);
            vecWeight.append(weight);
        }
    } else {
        qDebug() << "Query execution error:" << query.lastError().text();
    }

    std::sort(vecDate.begin(), vecDate.end());
}

void ViewGrapchic::buildgraph(const QVector<QDate>& vecDate, const QVector<float>& vecWeight) {
    QtCharts::QLineSeries *series = new QtCharts::QLineSeries();

    for (int i = 0; i < vecDate.size(); ++i) {
        qint64 xValue = QDateTime(vecDate[i].startOfDay()).toMSecsSinceEpoch();
        series->append(xValue, vecWeight[i]);
    }

    QtCharts::QChart *chart = new QtCharts::QChart();


    chart->addSeries(series);
    chart->setTitle("График зависимости веса рыбы от даты");

    QtCharts::QDateTimeAxis *axisX = new QtCharts::QDateTimeAxis();
    axisX->setTitleText("Дата");
    axisX->setFormat("dd.MM.yyyy");
    axisX->setTickCount(5);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis();
    axisY->setTitleText("Вес рыбы (кг)");
    axisY->setLabelFormat("%.1f");
    axisY->setTickCount(6);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    if (!vecDate.isEmpty()) {
        axisX->setRange(vecDate.first().startOfDay(), vecDate.last().startOfDay());
    }
    axisY->setRange(0, *std::max_element(vecWeight.begin(), vecWeight.end()) + 5);

    // Удалить старый график перед добавлением нового
    if (chartView) {
        centralWidget()->layout()->removeWidget(chartView);
        delete chartView;
    }

    chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    centralWidget()->layout()->addWidget(chartView);
}
