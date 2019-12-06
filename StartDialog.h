#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QPushButton>

namespace Ui {
class StartDialog;
}

class StartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartDialog(QWidget *parent = nullptr);
    ~StartDialog();

signals:
    void projectFileUploaded(QStringList fileNames);

private slots:
    // STACKED WIDGET PAGE ONE
    __attribute__((noreturn)) void on_buttonMenuBarExit_clicked();

    __attribute__((noreturn)) void on_buttonExit_clicked();

    void on_buttonLoadProject_clicked();

    void on_buttonNewProject_clicked();

    // STACKED WIDGET PAGE TWO
    void on_buttonMenuBarBack_clicked();

    void on_checkBoxUseDefault_stateChanged(int arg1);

     __attribute__((noreturn)) void on_buttonMenuBarExit_2_clicked();

     void on_buttonLoadCustom_clicked();

private:
    Ui::StartDialog *ui;

    QPushButton * createPushButton();
    QStringList openFileDialog(QStringList validMimetypeExtensions);
};

#endif // STARTDIALOG_H
