#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class EditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditDialog(const QString &name, const QString &email, const QString &birthday, QWidget *parent = nullptr);

signals:
    void dataModified(const QString &name, const QString &email, const QString &birthday);

private slots:
    void saveData();

private:
    QLineEdit *nameLineEdit;
    QLineEdit *emailLineEdit;
    QDateEdit *birthdayDateEdit;
};

#endif // EDITDIALOG_H
