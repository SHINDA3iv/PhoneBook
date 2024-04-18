#include "mainwindow.h"
#include "editdialog.h"
#include "searchdialog.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./phonebook.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть базу данных.");
        return;
    }

    query = new QSqlQuery(db);
    query->exec("CREATE TABLE IF NOT EXISTS phonebook (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, email TEXT, birthday DATA, dateAdded DATA)");
    if (query->lastError().isValid()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать таблицу: " + query->lastError().text());
        return;
    }

    QStringList headers = {"Порядковый номер", "ФИО", "E-mail", "День рождения", "Дата внесения"};

    model = new QSqlTableModel(this, db);
    model->setTable("PhoneBook");
    for (int i = 0; i < headers.size(); ++i) {
        model->setHeaderData(i, Qt::Horizontal, headers.at(i));
    }
    model->select(); // Обновляем модель данных
    ui->phoneBookTable->setModel(model);


    connect(ui->phoneBookTable->horizontalHeader(), &QHeaderView::sectionClicked, this, [=](int column){

        // Получаем текущий порядок сортировки для столбца
        Qt::SortOrder currentOrder = sortOrders.value(column, Qt::AscendingOrder);

        // Меняем порядок сортировки на противоположный
        Qt::SortOrder newOrder = (currentOrder == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;

        // Устанавливаем сортировку по выбранному столбцу
        model->setSort(column, newOrder);
        model->select();

        // Сохраняем новый порядок сортировки для столбца
        sortOrders[column] = newOrder;
    });

    // Создаем экземпляр SearchDialog
    searchDialog = new SearchDialog(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_insertButton_clicked()
{
    // Создаем и отображаем диалоговое окно для добавления данных
    EditDialog *editDialog = new EditDialog("", "", "", this); // Пустые поля

    connect(editDialog, &EditDialog::dataModified, this, [=](const QString &name, const QString &email, const QString &birthday){
        // При получении сигнала об добавление данных из диалогового окна выполняем SQL-запрос на добавление новой записи
        query->prepare("INSERT INTO phonebook (name, email, birthday, dateAdded) VALUES (?, ?, ?, ?)");
        query->addBindValue(name);
        query->addBindValue(email);
        query->addBindValue(birthday);
        query->addBindValue(QDate::currentDate().toString("yyyy.MM.dd"));

        if (query->exec()) {
            model->select(); // Обновляем модель данных
        } else {
            // В случае ошибки выводим сообщение об ошибке
            QMessageBox::critical(this, "Ошибка", "Не удалось добавить данные: " + query->lastError().text());
        }
    });
    editDialog->exec(); // Отображаем диалоговое окно
}

void MainWindow::on_deleteButton_clicked()
{
    // Получаем модель выделения
    QItemSelectionModel *selectionModel = ui->phoneBookTable->selectionModel();
    // Получаем выбранные индексы
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();

    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Выберите записи для удаления.");
        return;
    }

    // Создаем упорядоченный список уникальных строк, которые нужно удалить
    QSet<int> rowsToDelete;
    for (const QModelIndex &index : selectedIndexes) {
        rowsToDelete.insert(index.row());
    }

    // Проходим по каждой строке, которую нужно удалить, начиная с последней
    for (int row : rowsToDelete) {
        int id = ui->phoneBookTable->model()->index(row, 0).data().toInt();

        query->prepare("DELETE FROM phonebook WHERE id = ?");
        query->addBindValue(id);

        if (!query->exec()) {
            QMessageBox::critical(this, "Ошибка", "Не удалось удалить запись: " + query->lastError().text());
            return;
        }
    }

    // После удаления всех записей обновляем модель данных
    model->select();
}

void MainWindow::on_modifyButton_clicked()
{
    // Получаем модель выделения
    QItemSelectionModel *selectionModel = ui->phoneBookTable->selectionModel();
    // Получаем выбранный элемент
    QModelIndexList selection = selectionModel->selectedRows();
    // Получаем индексы выбранных элементов
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();

    int row = -1; // Объявляем переменную row здесь

    if (!selection.isEmpty()) {
        row = selection.first().row();
    }
    else if (!selectedIndexes.isEmpty()) {
        // Получаем индекс первого выбранного элемента
        QModelIndex selectedIndex = selectedIndexes.first();
        row = selectedIndex.row();
    }
    else {
        QMessageBox::warning(this, "Предупреждение", "Выберите запись для удаления.");
    }

    if (row != -1) { // Проверяем, что значение row было установлено
        int id = ui->phoneBookTable->model()->index(row, 0).data().toInt();

        // Получаем данные из выбранной записи
        QString name = ui->phoneBookTable->model()->index(row, 1).data().toString();
        QString email = ui->phoneBookTable->model()->index(row, 2).data().toString();
        QString birthday = ui->phoneBookTable->model()->index(row, 3).data().toString();

        // Создаем и отображаем диалоговое окно для редактирования
        EditDialog *editDialog = new EditDialog(name, email, birthday, this);
        connect(editDialog, &EditDialog::dataModified, this, [=](const QString &newName, const QString &newEmail, const QString &newBirthday){
            // Обновляем данные в таблице
            query->prepare("UPDATE phonebook SET name = ?, email = ?, birthday = ? WHERE id = ?");
            query->addBindValue(newName);
            query->addBindValue(newEmail);
            query->addBindValue(newBirthday);
            query->addBindValue(id);

            if (query->exec()) {
                model->select(); // Обновляем модель данных
            } else {
                QMessageBox::critical(this, "Ошибка", "Не удалось изменить запись: " + query->lastError().text());
            }
        });
        editDialog->exec();
    }
}

void MainWindow::on_searchButton_clicked()
{
    // Сохраняем исходную модель данных
    originalModel = ui->phoneBookTable->model();

    // Подключаем сигнал нажатия кнопки к слоту для запуска поиска
    connect(searchDialog, &SearchDialog::searchRequested, this, [&](const QString &searchText) {
        // Создаем прокси-модель для поиска
        QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
        proxyModel->setSourceModel(originalModel);

        // Устанавливаем фильтр для поиска
        proxyModel->setFilterKeyColumn(-1); // Поиск по всем столбцам
        proxyModel->setFilterFixedString(searchText);

        // Применяем прокси-модель к таблице
        ui->phoneBookTable->setModel(proxyModel);
    });


    // Подключаем сигнал отмены поиска к слоту для восстановления исходной модели данных
    connect(searchDialog, &SearchDialog::cancelSearch, this, [&]() {
        // Устанавливаем исходную модель данных обратно в таблицу
        ui->phoneBookTable->setModel(originalModel);
    });

    // Если searchDialog не отображается, показываем его
    if (!searchDialog->isVisible()) {
        searchDialog->show();
    } else { // Иначе скрываем его
        searchDialog->hide();
    }
}

void MainWindow::on_regenButton_clicked()
{
    // Путь к резервной копии базы данных
    QString backupFilePath = "./phonebookRegen.db";

    // Проверяем существование резервной копии базы данных
    if (!QFile::exists(backupFilePath)) {
        QMessageBox::critical(this, "Ошибка", "Резервная копия базы данных не найдена.");
        return;
    }

    // Закрываем соединение с текущей базой данных перед восстановлением
    db.close();

    // Удаляем текущую базу данных
    QFile::remove("./phonebook.db");

    // Копируем резервную копию вместо текущей базы данных
    if (!QFile::copy(backupFilePath, "./phonebook.db")) {
        QMessageBox::critical(this, "Ошибка", "Не удалось восстановить базу данных из резервной копии.");
        return;
    }

    // Переоткрываем соединение с базой данных
    if (!db.open()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть базу данных после восстановления.");
        return;
    }

    // Обновляем модель данных
    model->select();

    QMessageBox::information(this, "Успех", "База данных успешно восстановлена из резервной копии.");
}
