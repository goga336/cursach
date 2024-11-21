#include "testmainwindow.h"
#include "mainwindow.h"
#include "testdaytablewindow.h"
#include <QTest>
#include <QApplication>
#include <QSignalSpy>

TestMainwindow::TestMainwindow(QObject *parent)
    : QObject{parent}
{}

void TestMainwindow::testButton1()
{
    MainWindow mainWindow;
    QPushButton *button1 = mainWindow.findChild<QPushButton *>("button1");
    QVERIFY(button1 != nullptr);  // Проверяем, что кнопка существует
    QSignalSpy spy(button1, &QPushButton::clicked);  // Следим за сигналом нажатия кнопки
    QTest::mouseClick(button1, Qt::LeftButton);  // Имитируем клик
    QCOMPARE(spy.count(), 1);  // Проверяем, что сигнал был отправлен
}

void TestMainwindow::testButton2()
{
    MainWindow mainWindow;
    QPushButton *button2 = mainWindow.findChild<QPushButton *>("button2");
    QVERIFY(button2 != nullptr);  // Проверяем, что кнопка существует
    QSignalSpy spy(button2, &QPushButton::clicked);  // Следим за сигналом нажатия кнопки
    QTest::mouseClick(button2, Qt::LeftButton);  // Имитируем клик
    QCOMPARE(spy.count(), 1);  // Проверяем, что сигнал был отправлен
}

void TestMainwindow::testButton3()
{
    MainWindow mainWindow;
    QPushButton *button3 = mainWindow.findChild<QPushButton *>("button3");
    QVERIFY(button3 != nullptr);  // Проверяем, что кнопка существует
    QSignalSpy spy(button3, &QPushButton::clicked);  // Следим за сигналом нажатия кнопки
    QTest::mouseClick(button3, Qt::LeftButton);  // Имитируем клик
    QCOMPARE(spy.count(), 1);  // Проверяем, что сигнал был отправлен
}

void TestMainwindow::testButton4()
{
    MainWindow mainWindow;
    QPushButton *button4 = mainWindow.findChild<QPushButton *>("button4");
    QVERIFY(button4 != nullptr);  // Проверяем, что кнопка существует
    QSignalSpy spy(button4, &QPushButton::clicked);  // Следим за сигналом нажатия кнопки
    QTest::mouseClick(button4, Qt::LeftButton);  // Имитируем клик
    QCOMPARE(spy.count(), 1);  // Проверяем, что сигнал был отправлен
}

void TestMainwindow::testInitialization() {
    MainWindow mainWindow;
    mainWindow.show();

    // Check if the main window is properly initialized
    QVERIFY(mainWindow.isVisible());

    // Check if buttons exist
    QPushButton *button1 = mainWindow.findChild<QPushButton*>("button1");
    QVERIFY(button1 != nullptr);

    QPushButton *button2 = mainWindow.findChild<QPushButton*>("button2");
    QVERIFY(button2 != nullptr);

    // Check if the welcome label is displayed
    QLabel *welcomeLabel = mainWindow.findChild<QLabel*>();
    QVERIFY(welcomeLabel != nullptr);

    // Ensure the label text starts with "Добро пожаловать, "
    // Ensure UserName is being set correctly
    QString welcomeText = welcomeLabel->text();
    QVERIFY(welcomeText.startsWith("Добро пожаловать, "));
    QVERIFY(welcomeText.length() > 18);
}


// int main(int argc, char *argv[])
// {
//     QApplication app(argc, argv);  // Создаем QApplication с аргументами командной строки

//     TestMainwindow test;
//     TestDayTableWindow testd;
//     int result1 = QTest::qExec(&test, argc, argv);    // Запуск первого теста
//     int result2 = QTest::qExec(&testd, argc, argv);   // Запуск второго теста

//     // Возвращаем результат первого теста или второго (можно использовать побитовую операцию ИЛИ, если нужно учитывать оба)
//     return result1 | result2;
//    /* return QTest::qExec(&test, argc, argv); */ // Запуск тестов
// }
