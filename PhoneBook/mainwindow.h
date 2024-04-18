#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QMessageBox>
#include <QDebug>
#include <QStyledItemDelegate>
#include "searchdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_insertButton_clicked();
    void on_deleteButton_clicked();
    void on_modifyButton_clicked();
    void on_searchButton_clicked();
    void on_regenButton_clicked();

private:
    SearchDialog *searchDialog;
    QAbstractItemModel *originalModel = nullptr;
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlQuery *query;
    QSqlTableModel *model;
    QMap<int, Qt::SortOrder> sortOrders;
};
#endif // MAINWINDOW_H
