#ifndef TESTMAINWINDOW_H
#define TESTMAINWINDOW_H

#include <QObject>

class TestMainwindow : public QObject
{
    Q_OBJECT
public:
    explicit TestMainwindow(QObject *parent = nullptr);

private slots:
    void testButton1();
    void testButton2();
    void testButton3();
    void testButton4();
    void testInitialization();
};

#endif // TESTMAINWINDOW_H
