#ifndef IWINDOW_H
#define IWINDOW_H

class IWindow
{
public:
    virtual ~IWindow() = default;

    // Метод для инициализации окна
    virtual void initialize() = 0;

    // Метод для отображения окна
    virtual void showWindow() = 0;

    // Метод для передачи данных между окнами (если необходимо)
   // virtual void updateData(const QString &data) = 0;
};

#endif // IWINDOW_H
