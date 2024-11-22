#include "recordwindow.h"
#include <QtSql/QSqlDatabase>
#include "daytablewindow.h"
#include <QSqlError>

RecordWindow::RecordWindow(QWidget *parent)
    : QMainWindow(parent)
{

}

RecordWindow::~RecordWindow()
{
}
void RecordWindow::showWindow()  {
    this->show();
}

void RecordWindow::initialize()
{
    setupUI();
}

void RecordWindow::setupUI()
{

    this->setWindowTitle("Добавление записи о дне");
    this->resize(400, 300);

    // Создаем центральный виджет
    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(0, 139, 139));  // Голубой цвет
    centralWidget->setPalette(palette);
    centralWidget->setAutoFillBackground(true);// Важно для применения фона

    // Настраиваем макет
    QVBoxLayout *box = new QVBoxLayout(centralWidget);
    box->setSpacing(10);

    QLabel *labelWeight = new QLabel("Вес рыбы (кг):");
    labelWeight->setStyleSheet("color: white;");
    box->addWidget(labelWeight);

    fishWeight = new QDoubleSpinBox(centralWidget);
    fishWeight->setRange(0, 100);
    fishWeight->setSuffix(" кг");
    box->addWidget(fishWeight);

    QLabel *labelDate = new QLabel("Дата рыбалки:");
    labelDate->setStyleSheet("color: white;");
    box->addWidget(labelDate);

    fishDate = new QDateEdit(centralWidget);
    fishDate->setDate(QDate::currentDate());
    fishDate->setCalendarPopup(true);
    fishDate->setDisplayFormat("dd.MM.yyyy");
    box->addWidget(fishDate);

    QLabel *labelStartFishing = new QLabel("Время начала рыбалки:");
    labelStartFishing->setStyleSheet("color: white;");
    box->addWidget(labelStartFishing);

    startFishing = new QTimeEdit(centralWidget);
    startFishing->setDisplayFormat("hh:mm");
    box->addWidget(startFishing);

    QLabel *labelEndFishing = new QLabel("Время окончания рыбалки:");
    labelEndFishing->setStyleSheet("color: white;");
    box->addWidget(labelEndFishing);

    endFishing = new QTimeEdit(centralWidget);
    endFishing->setDisplayFormat("hh:mm");
    box->addWidget(endFishing);

    QLabel *tempWaterl = new QLabel("Температура воды:");
    tempWaterl->setStyleSheet("color: white;");
    box->addWidget(tempWaterl);

    tempWater = new QDoubleSpinBox(centralWidget);
    tempWater->setRange(-50.0, 50.0);
    tempWater->setSuffix(" °C");
    tempWater->setValue(20.0);
    box->addWidget(tempWater);

    QLabel *labelPressureInput = new QLabel("Атмосферное давление (мм рт.ст.):");
    labelPressureInput->setStyleSheet("color: white;");
    box->addWidget(labelPressureInput);


    pressureInput = new QDoubleSpinBox(centralWidget);
    pressureInput->setRange(700.0, 800.0);
    pressureInput->setSuffix(" мм рт.ст.");
    pressureInput->setValue(760.0);
    box->addWidget(pressureInput);

    send = new QPushButton("Записать", centralWidget);
    box->addWidget(send);
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

    send->setStyleSheet(buttonStyle);


    centralWidget->setLayout(box);
    setCentralWidget(centralWidget);


    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("fishingdays");
    db.setHostName("localhost");
    db.setPort(5432);
    db.setUserName("postgres");
    db.setPassword("3360146");

    if (!db.open())
    {
        QMessageBox::information(nullptr, "Ошибка", "Не удалось подключиться к базе данных!");
    }
    else
    {
      //  QMessageBox::information(nullptr, "Успех", "Подключение успешно!");
    }
   connect(send, &QPushButton::clicked, this, &RecordWindow::recordFishingDay);
}

void RecordWindow::recordFishingDay()
{
    double weight = fishWeight->value();
    QDate date = fishDate->date();
    QTime startTime = startFishing->time();
    QTime endTime = endFishing->time();
    double waterTemperature = tempWater->value();
    double pressure = pressureInput->value();

    // Проверка, что время начала меньше времени окончания
    if (startTime >= endTime) {
        QMessageBox::warning(this, "Ошибка", "Время начала рыбалки не может быть больше или равно времени окончания!");
        return; // Прерываем выполнение, если условие не выполнено
    }

    QSqlQuery query;
    query.prepare("INSERT INTO fishingday (fisher_id, weight, date, start_time, end_time) VALUES (:user_id, :weight, :date, :start_time, :end_time)");
    query.bindValue(":date", date);
    query.bindValue(":start_time", startTime);
    query.bindValue(":end_time", endTime);
    query.bindValue(":user_id", 1);
    query.bindValue(":weight", weight);

    if (!query.exec())
    {
        QMessageBox::information(nullptr, "Ошибка", "Не удалось записать данные о дне рыбалки: " + query.lastError().text());
        return;
    }

    int fishingDayId = query.lastInsertId().toInt();

    query.prepare("INSERT INTO WheatherCondition (temperature, pressure, water_temperature, fishingday_id) VALUES (:temperature, :pressure, :water_temperature, :fishingday_id)");
    query.bindValue(":temperature", waterTemperature);
    query.bindValue(":pressure", pressure);
    query.bindValue(":water_temperature", waterTemperature);
    query.bindValue(":fishingday_id", fishingDayId);

    if (!query.exec())
    {
        QMessageBox::warning(nullptr, "Ошибка", "Не удалось записать данные о погоде: " + query.lastError().text());
    }
    else
    {
        QMessageBox::information(nullptr, "Успех", "Данные успешно записаны!");
        fishWeight->setValue(0); // Устанавливаем 0 для веса рыбы
        fishDate->setDate(QDate::currentDate()); // Устанавливаем текущую дату
        startFishing->setTime(QTime::fromString("00:00", "hh:mm")); // Устанавливаем время по умолчанию для начала
        endFishing->setTime(QTime::fromString("00:00", "hh:mm")); // Устанавливаем время по умолчанию для окончания
        tempWater->setValue(20.0); // Устанавливаем стандартное значение для температуры воды
        pressureInput->setValue(760.0);
    }
}


