#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = nullptr);
    QString searchText() const;

signals:
    void searchRequested(const QString &text);
    void cancelSearch();

protected:
    // Переопределяем метод обработки событий закрытия окна
    void closeEvent(QCloseEvent *event) override;

private:
    QLineEdit *searchLineEdit;
    QPushButton *searchButton;
    QPushButton *cancelButton;
};

#endif // SEARCHDIALOG_H
