#include "settings.h"
#include "daytablewindow.h"

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QDebug>

settings::settings(QWidget *parent)
    : QMainWindow{parent}
{

}

settings::~settings()
{

}

void settings::showWindow()
{
    this->show();
}

void settings::initialize()
{
    setupUI();
}

void settings::setupUI(){
    this->resize(200, 150);
    QPalette palette;
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0.0, QColor(30, 90, 120));     // светло-синий сверху
    gradient.setColorAt(0.5, QColor(70, 150, 170));    // мягкий бирюзовый
    gradient.setColorAt(1.0, QColor(130, 200, 210));   // очень светлый снизу

    palette.setBrush(QPalette::Window, QBrush(gradient));
    this->setPalette(palette);
    this->setAutoFillBackground(true);

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

    if (!selectedPhotoPath.isEmpty()) {
        QImage userPhoto(selectedPhotoPath);
        if (!userPhoto.isNull()) {
            dayTableWindow.insertSettingsPhoto(userPhoto);
        } else {
            qDebug() << "Ошибка: не удалось загрузить фотографию.";
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить выбранное фото.");
        }
    } else {
        qDebug() << "Фотография не выбрана.";
    }
}
