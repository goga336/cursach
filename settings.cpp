#include "settings.h"
#include "daytablewindow.h"
#include "mainwindow.h"

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QDebug>

settings::settings(QWidget *parent)
    : QMainWindow{parent}
{
    setupUI();
}

settings::~settings(){

}



void settings::setupUI(){
    this->resize(200, 150);
    QPalette palette;
    QPixmap background("/home/goga/Desktop/study/cursav/photo/phone.jpg");
    palette.setBrush(QPalette::Window, background.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));


    this->setWindowTitle("Настройки");
    QWidget *mainwidg = new QWidget(this);
    QVBoxLayout *box = new QVBoxLayout(mainwidg);

    QLabel *label = new QLabel("Имя пользователя:", mainwidg);
    box->addWidget(label);
    label->setStyleSheet("font-size: 16px; color: white; font-weight: normal; padding: 5px;");

    nameInput = new QLineEdit(mainwidg);
    box->addWidget(nameInput);

    QPushButton *photoButton = new QPushButton("Загрузить фотографию", mainwidg);
    box->addWidget(photoButton);

    photoPathLabel = new QLabel("Фотография не выбрана", mainwidg);
    box->addWidget(photoPathLabel);
    photoPathLabel->setStyleSheet("font-size: 16px; color: white; font-weight: normal; padding: 5px;");

    QPushButton *saveButton = new QPushButton("Сохранить изменения", mainwidg);
    box->addWidget(saveButton);
    QString buttonStyle = "QPushButton {"
                          "font-size: 14px;"
                          "font-weight: bold;"
                          "color: white;"
                          "background-color: #007BFF;"
                          "border-radius: 20px;"
                          "padding: 10px;"
                          "}"
                          "QPushButton:hover {"
                          "background-color: #0056b3;"
                          "}";
    saveButton->setStyleSheet(buttonStyle);

    mainwidg->setLayout(box);
    setCentralWidget(mainwidg);

    mainwidg->setPalette(palette);
    mainwidg->setAutoFillBackground(true);

    connect(photoButton, &QPushButton::clicked, this, &settings::loadPhoto);
    connect(saveButton, &QPushButton::clicked, this, &settings::changeSattings);
}

void settings::loadPhoto() {
    QString photoPath = QFileDialog::getOpenFileName(this, "Выберите фотографию", QString(), "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
    if (!photoPath.isEmpty()) {
        photoPathLabel->setText("Выбрано фото: " + photoPath);
        selectedPhotoPath = photoPath;
    } else {
        photoPathLabel->setText("Фотография не выбрана");
    }
}

void settings::changeSattings(){
    QString name = nameInput->text();
    DayTableWindow dayTableWindow;
    dayTableWindow.insertSettings(name);
    emit userNameChanged(name);

    // Вывод пути к выбранной фотографии для теста
    if (!selectedPhotoPath.isEmpty()) {
        QImage userPhoto(selectedPhotoPath); // Загружаем фото по выбранному пути
        if (!userPhoto.isNull()) {          // Проверяем, корректно ли загружено фото
            dayTableWindow.insertSettingsPhoto(userPhoto); // Используем ID пользователя
            qDebug() << "Выбранная фотография сохранена в БД:" << selectedPhotoPath;
        } else {
            qDebug() << "Ошибка: не удалось загрузить фотографию.";
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить выбранное фото.");
        }
    } else {
        qDebug() << "Фотография не выбрана.";
    }
}
