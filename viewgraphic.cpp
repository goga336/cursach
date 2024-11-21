// #include "viewgraphic.h"
// #include <QtCharts/QChart>
// #include <QSqlError>
// #include <QMessageBox>
// #include <QDate>

// ViewGraphic::ViewGraphic(QWidget *parent)
//     : QMainWindow(parent),
//     chartView(new QChartView(this)),
//     series(new QLineSeries())
// {
//     setCentralWidget(chartView);
//     plotWeightByYear();
//     resize(800, 600);
// }

// ViewGraphic::~ViewGraphic()
// {
//     delete chartView;
//     delete series;
// }

// void ViewGraphic::connectToDatabase()
// {
//     QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
//     db.setHostName("localhost");
//     db.setDatabaseName("fishingdays");
//     db.setUserName("postgres");
//     db.setPassword("password");
//     if (!db.open()) {
//         QMessageBox::critical(this, "Database Error", db.lastError().text());
//     }
// }

