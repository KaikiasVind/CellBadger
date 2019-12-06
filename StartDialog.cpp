#include "StartDialog.h"
#include "ui_StartDialog.h"

#include <QStringList>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QPushButton>
#include <QLabel>

StartDialog::StartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartDialog)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
}

StartDialog::~StartDialog()
{
    delete this;
}

QPushButton * StartDialog::createPushButton() {
    QPushButton * button = new QPushButton();
    button->setText("+");
    return button;
}

/**
 * @brief StartDialog::openFileDialog - Opens a file dialog specific to files with given type to files with given type
 * @param validMimeTypeExtensions - StringList that contains the valid file types that the dialog shows
 * @return - List of user-selected files
 */
QStringList StartDialog::openFileDialog(QStringList validMimeTypeExtensions) {
    QFileDialog fileDialog(this);
    fileDialog.setDirectory(QDir::home());
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setMimeTypeFilters(validMimeTypeExtensions);

    QStringList fileNames;
    if (fileDialog.exec())
        fileNames = fileDialog.selectedFiles();

    return fileNames;
}

// ++++++++++++++++++++++++++++++++ SLOTS ++++++++++++++++++++++++++++++++

// STACKED WIDGET PAGE ONE
__attribute__((noreturn)) void StartDialog::on_buttonMenuBarExit_clicked() {
    exit(0);
}

__attribute__((noreturn)) void StartDialog::on_buttonExit_clicked() {
    exit(0);
}

void StartDialog::on_buttonLoadProject_clicked() {
    QStringList csvMimeTypes = { "text/plain" };
    QStringList fileNames = this->openFileDialog(csvMimeTypes);

    if (fileNames.empty())
        return;

    qDebug() << "Sent project file name.";
    emit projectFileUploaded(fileNames);
}

void StartDialog::on_buttonNewProject_clicked() {
    qDebug() << "New Project.";
    ui->stackedWidget->setCurrentIndex(1);
}

// STACKED WIDGET PAGE TWO
void StartDialog::on_buttonMenuBarBack_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void StartDialog::on_checkBoxUseDefault_stateChanged(int state) {
    if (state == Qt::CheckState::Checked)
        ui->buttonLoadCustom->setDisabled(true);
    else
        ui->buttonLoadCustom->setDisabled(false);
}

__attribute__((noreturn)) void StartDialog::on_buttonMenuBarExit_2_clicked() {
    exit(0);
}

// ++++++++++++++++++++++++++++++++ SLOTS ++++++++++++++++++++++++++++++++

void StartDialog::on_buttonLoadCustom_clicked()
{
    QStringList csvMimeTypes = { "text/csv" };
    QStringList fileNames = this->openFileDialog(csvMimeTypes);

    if (fileNames.empty())
        return;

    qDebug() << "Uploaded" << fileNames[0];
}
