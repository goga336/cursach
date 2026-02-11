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
    void displayRecords();
    void setupUI();
    void saveChanges();
    void deleteRecord();
    QTableWidget *tableWidget;
};

#endif // VIEWRECORD_H
