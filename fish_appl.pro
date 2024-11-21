QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    daytablewindow.cpp \
    iwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    recordwindow.cpp \
    settings.cpp \
    testdaytablewindow.cpp \
    testmainwindow.cpp \
    viewgrapchic.cpp \
    viewrecord.cpp \
    weatherfetcher.cpp \
    windowfactory.cpp

HEADERS += \
    daytablewindow.h \
    iwindow.h \
    mainwindow.h \
    recordwindow.h \
    settings.h \
    testdaytablewindow.h \
    testmainwindow.h \
    viewgrapchic.h \
    viewrecord.h \
    weatherfetcher.h \
    windowfactory.h

FORMS += \
    mainwindow.ui

# QT += core gui sql
# QT += charts
QT += core gui widgets sql charts network testlib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
