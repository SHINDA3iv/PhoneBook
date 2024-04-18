#include "searchdialog.h"

SearchDialog::SearchDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Поиск");

    // Создаем вертикальный макет для размещения виджетов
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Создаем строку поиска и добавляем её на форму
    searchLineEdit = new QLineEdit(this);
    layout->addWidget(searchLineEdit);

    // Создаем горизонтальный макет для кнопок
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    // Создаем кнопки и добавляем их на форму
    searchButton = new QPushButton("Поиск", this);
    layout->addWidget(searchButton);

    cancelButton = new QPushButton("Отмена", this);
    buttonLayout->addWidget(cancelButton);

    // Добавляем макет с кнопками на форму
    layout->addLayout(buttonLayout);

    // Соединяем сигнал нажатия на кнопку "Поиск" с обработчиком
    connect(searchButton, &QPushButton::clicked, this, [this]() {
        emit searchRequested(searchLineEdit->text());
    });

    // Соединяем сигнал нажатия на кнопку "Отмена" с обработчиком
    connect(cancelButton, &QPushButton::clicked, this, [this]() {
        searchLineEdit->clear();
        emit cancelSearch();
    });

    // Устанавливаем макет на диалоговое окно
    setLayout(layout);
}

QString SearchDialog::searchText() const
{
    // Получение текста из строки поиска
    return searchLineEdit->text();
}

void SearchDialog::closeEvent(QCloseEvent *event)
{
    // Очищаем текстовое поле поиска и отправляем сигнал об отмене поиска перед закрытием окна
    searchLineEdit->clear();
    emit cancelSearch();

    // Вызываем метод базового класса для обработки события закрытия окна
    QDialog::closeEvent(event);
}
