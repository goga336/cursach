#ifndef VIEWRECORD_H
#define VIEWRECORD_H

#include "iwindow.h"
#include <QMainWindow>
#include <QSqlQuery>
#include <QTableWidget>
#include <QVBoxLayout>

class ViewRecord : public QMainWindow, public IWindow
{
    Q_OBJECT
public:
    explicit ViewRecord(QWidget *parent = nullptr);
    ~ViewRecord();
    void initialize() override {
        // Реализация метода initialize
    }

    void showWindow() override {
        this->show();
    }

    void displayRecords(); // Метод для отображения данных
    void setupUI();
    void saveChanges();

private:
    QTableWidget *tableWidget; // Виджет для таблицы
};

#endif // VIEWRECORD_H
