#include "viewgrapchic.h"
#include "daytablewindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QComboBox>

ViewGrapchic::ViewGrapchic(QWidget *parent)
    : QMainWindow{parent}, chartView(nullptr)
{
    dbManager = new DayTableWindow();
    setupUI();
}

ViewGrapchic::~ViewGrapchic()
{
    delete dbManager;
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
{
    resize(900, 900);

    // Главный виджет
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);

    QPalette palette;
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0.0, QColor(30, 80, 120));     // светло-синий сверху
    gradient.setColorAt(0.5, QColor(70, 150, 170));    // мягкий бирюзовый
    gradient.setColorAt(1.0, QColor(130, 200, 210));   // очень светлый снизу

    palette.setBrush(QPalette::Window, QBrush(gradient));
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    // Верхняя панель
    QHBoxLayout *topPanel = new QHBoxLayout();
    QLabel *modeLabel = new QLabel("Режим отображения:");
    modeLabel->setStyleSheet("color: white;");
    topPanel->addWidget(modeLabel);

    // Главный комбобокс
    mainComboBox = new QComboBox(this);
    mainComboBox->addItem("Все года");

    // Добавляем все годы из БД
    QSqlQuery yearQuery("SELECT DISTINCT EXTRACT(YEAR FROM fishing_date) AS year FROM fishing_day ORDER BY year");
    while (yearQuery.next()) {
        mainComboBox->addItem(yearQuery.value(0).toString());
    }

    mainComboBox->addItem("Наложение графиков");
    topPanel->addWidget(mainComboBox);
    topPanel->addStretch();
    mainLayout->addLayout(topPanel);

    // Панель для наложения графиков (изначально скрыта)
    overlayPanel = new QWidget(this);
    QHBoxLayout *overlayLayout = new QHBoxLayout(overlayPanel);
    overlayLayout->setContentsMargins(0, 0, 0, 0);

    overlayLayout->addWidget(new QLabel("Год 1:"));
    overlayYear1 = new QComboBox(this);
    overlayLayout->addWidget(overlayYear1);

    overlayLayout->addWidget(new QLabel("Год 2:"));
    overlayYear2 = new QComboBox(this);
    overlayLayout->addWidget(overlayYear2);

    overlayLayout->addStretch();
    mainLayout->addWidget(overlayPanel);
    overlayPanel->hide();

    // Заполняем комбобоксы для наложения годами
    loadYearsToComboBox(overlayYear1);
    loadYearsToComboBox(overlayYear2);

    // Создаем chartView
    chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    mainLayout->addWidget(chartView);

    // Подключаем сигналы
    connect(mainComboBox, &QComboBox::currentTextChanged,
            this, &ViewGrapchic::onMainComboBoxChanged);
    connect(overlayYear1, &QComboBox::currentTextChanged,
            this, &ViewGrapchic::onOverlayYearChanged);
    connect(overlayYear2, &QComboBox::currentTextChanged,
            this, &ViewGrapchic::onOverlayYearChanged);

    // Загружаем начальные данные (Все года)
    buildAllYearsGraph();
}

void ViewGrapchic::loadYearsToComboBox(QComboBox *comboBox)
{
    comboBox->clear();
    QSqlQuery yearQuery("SELECT DISTINCT EXTRACT(YEAR FROM fishing_date) AS year FROM fishing_day ORDER BY year");
    while (yearQuery.next()) {
        comboBox->addItem(yearQuery.value(0).toString());
    }
}

void ViewGrapchic::onMainComboBoxChanged(const QString &text)
{
    if (text == "Наложение графиков") {
        // Показываем панель выбора годов для наложения
        overlayPanel->show();

        // Загружаем данные для первого и второго года из комбобоксов наложения
        if (overlayYear1->count() > 0 && overlayYear2->count() > 0) {
            buildOverlayGraph(overlayYear1->currentText(), overlayYear2->currentText());
        }
    } else {
        // Скрываем панель наложения
        overlayPanel->hide();

        if (text == "Все года") {
            buildAllYearsGraph();
        } else {
            // Конкретный год
            buildSingleYearGraph(text);
        }
    }
}

void ViewGrapchic::onOverlayYearChanged()
{
    if (mainComboBox->currentText() == "Наложение графиков") {
        buildOverlayGraph(overlayYear1->currentText(), overlayYear2->currentText());
    }
}

void ViewGrapchic::buildSingleYearGraph(const QString &year)
{
    QVector<QDate> dates;
    QVector<float> weights;

    dbManager->getFishingDataForGraphic(dates, weights, year);

    if (dates.isEmpty() || weights.isEmpty()) {
        clearChart();
        return;
    }

    QChart *chart = new QChart();
    chart->setTitle(QString("График зависимости веса рыбы от даты (%1 год)").arg(year));

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setTitleText("Дата");
    axisX->setFormat("dd.MM.yyyy");
    axisX->setTickCount(8);
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Вес рыбы (кг)");
    axisY->setLabelFormat("%.1f");
    axisY->setTickCount(8);
    chart->addAxis(axisY, Qt::AlignLeft);

    QLineSeries *series = new QLineSeries();
    series->setName(year + " год");

    float maxWeight = 0;
    for (int i = 0; i < dates.size(); ++i) {
        qint64 xValue = QDateTime(dates[i].startOfDay()).toMSecsSinceEpoch();
        series->append(xValue, weights[i]);
        if (weights[i] > maxWeight) maxWeight = weights[i];
    }

    chart->addSeries(series);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    axisX->setRange(dates.first().startOfDay(), dates.last().startOfDay());
    axisY->setRange(0, maxWeight + 5);

    chartView->setChart(chart);
}

void ViewGrapchic::buildAllYearsGraph()
{
    QVector<QDate> dates;
    QVector<float> weights;

    dbManager->getFishingDataForGraphic(dates, weights, "");

    if (dates.isEmpty() || weights.isEmpty()) {
        clearChart();
        return;
    }

    QChart *chart = new QChart();
    chart->setTitle("График зависимости веса рыбы от даты (все года)");

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setTitleText("Дата");
    axisX->setFormat("dd.MM.yyyy");
    axisX->setTickCount(8);
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Вес рыбы (кг)");
    axisY->setLabelFormat("%.1f");
    axisY->setTickCount(8);
    chart->addAxis(axisY, Qt::AlignLeft);

    QLineSeries *series = new QLineSeries();
    series->setName("Все года");

    float maxWeight = 0;
    for (int i = 0; i < dates.size(); ++i) {
        qint64 xValue = QDateTime(dates[i].startOfDay()).toMSecsSinceEpoch();
        series->append(xValue, weights[i]);
        if (weights[i] > maxWeight) maxWeight = weights[i];
    }

    chart->addSeries(series);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    axisX->setRange(dates.first().startOfDay(), dates.last().startOfDay());
    axisY->setRange(0, maxWeight + 5);

    chartView->setChart(chart);
}

void ViewGrapchic::buildOverlayGraph(const QString &year1, const QString &year2)
{
    if (year1.isEmpty() || year2.isEmpty()) {
        clearChart();
        return;
    }

    // Загружаем данные для первого года
    QVector<QDate> dates1;
    QVector<float> weights1;
    dbManager->getFishingDataForGraphic(dates1, weights1, year1);

    // Загружаем данные для второго года
    QVector<QDate> dates2;
    QVector<float> weights2;
    dbManager->getFishingDataForGraphic(dates2, weights2, year2);

    if ((dates1.isEmpty() || weights1.isEmpty()) &&
        (dates2.isEmpty() || weights2.isEmpty())) {
        clearChart();
        return;
    }

    QChart *chart = new QChart();
    chart->setTitle(QString("Наложение графиков: %1 и %2 годы").arg(year1, year2));

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setTitleText("Дата");
    axisX->setFormat("dd.MM.yyyy");
    axisX->setTickCount(8);
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Вес рыбы (кг)");
    axisY->setLabelFormat("%.1f");
    axisY->setTickCount(8);
    chart->addAxis(axisY, Qt::AlignLeft);

    float maxWeight = 0;
    QDate minDate, maxDate;

    // Первая серия (синий)
    if (!dates1.isEmpty() && !weights1.isEmpty()) {
        QLineSeries *series1 = new QLineSeries();
        series1->setName(year1 + " год");

        for (int i = 0; i < dates1.size(); ++i) {
            qint64 xValue = QDateTime(dates1[i].startOfDay()).toMSecsSinceEpoch();
            series1->append(xValue, weights1[i]);

            if (weights1[i] > maxWeight) maxWeight = weights1[i];
            if (!minDate.isValid() || dates1[i] < minDate) minDate = dates1[i];
            if (!maxDate.isValid() || dates1[i] > maxDate) maxDate = dates1[i];
        }

        chart->addSeries(series1);
        series1->attachAxis(axisX);
        series1->attachAxis(axisY);
    }

    // Вторая серия (красный)
    if (!dates2.isEmpty() && !weights2.isEmpty()) {
        QLineSeries *series2 = new QLineSeries();
        series2->setName(year2 + " год");

        QPen pen(Qt::red);
        pen.setWidth(2);
        series2->setPen(pen);

        for (int i = 0; i < dates2.size(); ++i) {
            qint64 xValue = QDateTime(dates2[i].startOfDay()).toMSecsSinceEpoch();
            series2->append(xValue, weights2[i]);

            if (weights2[i] > maxWeight) maxWeight = weights2[i];
            if (!minDate.isValid() || dates2[i] < minDate) minDate = dates2[i];
            if (!maxDate.isValid() || dates2[i] > maxDate) maxDate = dates2[i];
        }

        chart->addSeries(series2);
        series2->attachAxis(axisX);
        series2->attachAxis(axisY);
    }

    // Устанавливаем диапазоны
    if (minDate.isValid() && maxDate.isValid()) {
        axisX->setRange(minDate.startOfDay(), maxDate.startOfDay());
    }

    if (maxWeight > 0) {
        axisY->setRange(0, maxWeight + 5);
    }

    chartView->setChart(chart);
}

void ViewGrapchic::clearChart()
{
    QChart *emptyChart = new QChart();
    emptyChart->setTitle("Нет данных для отображения");
    chartView->setChart(emptyChart);
}
