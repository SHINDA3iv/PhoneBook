#include "editdialog.h"

EditDialog::EditDialog(const QString &name, const QString &email, const QString &birthday, QWidget *parent)
    : QDialog(parent)
{
    // Создаем вертикальное расположение для размещения виджетов
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Создаем метки и поля ввода
    QLabel *nameLabel = new QLabel("Имя:");
    nameLineEdit = new QLineEdit(name);
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(nameLineEdit);

    QLabel *emailLabel = new QLabel("Email:");
    emailLineEdit = new QLineEdit(email);
    mainLayout->addWidget(emailLabel);
    mainLayout->addWidget(emailLineEdit);

    QLabel *birthdayLabel = new QLabel("День Рождения:");
    birthdayDateEdit = new QDateEdit(QDate::fromString(birthday, "yyyy.MM.dd"));
    birthdayDateEdit->setDisplayFormat("yyyy.MM.dd");

    mainLayout->addWidget(birthdayLabel);
    mainLayout->addWidget(birthdayDateEdit);

    // Создаем кнопку "Save" и подключаем ее к слоту сохранения данных
    QPushButton *saveButton = new QPushButton("Save");
    connect(saveButton, &QPushButton::clicked, this, &EditDialog::saveData);
    mainLayout->addWidget(saveButton);

    // Устанавливаем главное расположение для диалогового окна
    setLayout(mainLayout);
}

void EditDialog::saveData()
{
    // Сохраняем измененные данные и отправляем сигнал о их изменении
    QString newName = nameLineEdit->text();
    QString newEmail = emailLineEdit->text();
    QString newBirthday = birthdayDateEdit->date().toString("yyyy.MM.dd");

    emit dataModified(newName, newEmail, newBirthday);

    // Закрываем диалоговое окно
    close();
}
