#ifndef WINDOWFACTORY_H
#define WINDOWFACTORY_H
#include "mainwindow.h"
#include "viewrecord.h"
#include "recordwindow.h"
#include "viewgrapchic.h"
#include "weatherfetcher.h"
#include "settings.h"
#include "iwindow.h"
#include "fishingforecastwindow.h"

class WindowFactory {
public:
    static IWindow* createWindow(const QString &type, QWidget *parent = nullptr) {
        if (type == "RecordWindow") {
            return new RecordWindow(parent);
        } else if (type == "SettingsWindow") {
            return new settings(parent);
        } else if (type == "ViewGraphWindow") {
            return new ViewGrapchic(parent);
        } else if (type == "ViewRecordWindow") {
            return new ViewRecord(parent);
        } else if (type == "ViewWeatherWindow") {
            return new WeatherWindow(parent);
        }else if (type == "MainWindow") {
            return new MainWindow(parent);
        }else if (type == "FishingForecastWindow") {
            return new FishingForecastWindow(parent);
        }
        return nullptr;
    }
};


#endif
