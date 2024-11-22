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
    void initialize() override;
    void showWindow() override;

private:
    void displayRecords(); // Метод для отображения данных
    void setupUI();
    void saveChanges();
    QTableWidget *tableWidget; // Виджет для таблицы
};

#endif // VIEWRECORD_H
