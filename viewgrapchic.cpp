#include "viewgrapchic.h"
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QComboBox>
#include <QVBoxLayout>
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QLineSeries>

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
    QSqlQuery yearQuery("SELECT DISTINCT EXTRACT(YEAR FROM fishing_date) AS year FROM fishing_day ORDER BY year");
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

    QString queryString = "SELECT fishing_date, catch_weight FROM fishing_day";
    if (!year.isEmpty() && year != "Все года") {
        queryString += " WHERE EXTRACT(YEAR FROM fishing_date) = " + year;
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
    QLineSeries *series = new QLineSeries();

    if (vecDate.isEmpty() || vecWeight.isEmpty()) {
        qDebug() << "Нет данных для построения графика";

        QChart *emptyChart = new QChart();
        emptyChart->setTitle("Нет данных для отображения");

        chartView = new QChartView(emptyChart);
        chartView->setRenderHint(QPainter::Antialiasing);
        centralWidget()->layout()->addWidget(chartView);
        return;
    }

    for (int i = 0; i < vecDate.size(); ++i) {
        qint64 xValue = QDateTime(vecDate[i].startOfDay()).toMSecsSinceEpoch();
        series->append(xValue, vecWeight[i]);
    }

    QChart *chart = new QChart();


    chart->addSeries(series);
    chart->setTitle("График зависимости веса рыбы от даты");

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setTitleText("Дата");
    axisX->setFormat("dd.MM.yyyy");
    axisX->setTickCount(5);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
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

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    centralWidget()->layout()->addWidget(chartView);
}
