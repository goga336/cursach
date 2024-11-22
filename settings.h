#ifndef SETTINGS_H
#define SETTINGS_H
#include "iwindow.h"
#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>

class settings : public QMainWindow, public IWindow
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = nullptr);
    ~settings();
    void initialize() override;
    void showWindow() override;

signals:
   // void userNameChanged(const QString &newName);

private slots:
    void setupUI();
    void changeSattings();
    void loadPhoto();

private:
    QLineEdit *nameInput;         // Поле ввода имени пользователя
    QLabel *photoPathLabel;       // Метка для отображения пути к выбранной фотографии
    QString selectedPhotoPath;    // Хранит путь к выбранной фотографии
};

#endif // SETTINGS_H
