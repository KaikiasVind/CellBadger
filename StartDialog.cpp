#include "StartDialog.h"
#include "ui_StartDialog.h"

#include <QStringList>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <QObject>
#include <QAction>
#include <QMouseEvent>

#include "Utils/Helper.h"

using Helper::chopFileName;
using Helper::openFileDialog;


/**
 * @brief StartDialog::StartDialog
 * @param parent
 */
StartDialog::StartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->stackedWidget->setCurrentIndex(0);
    ui->buttonRun->setDisabled(true);
}


/**
 * @brief StartDialog::~StartDialog
 */
StartDialog::~StartDialog() { /*REMEMBER: What should be done here?*/ }


/**
 * @brief StartDialog::createPushButton
 * @return
 */
QPushButton * StartDialog::createPushButton() {
    QPushButton * button = new QPushButton();
    button->setText("+");
    return button;
}



/**
 * @brief StartDialog::addDatasetToLayout
 * @param name
 */
void StartDialog::addDatasetToLayout(QString filePath) {
    QString fileName = chopFileName(filePath);
    QListWidgetItem * item = new QListWidgetItem(fileName);
    ui->listDatasets->addItem(item);
}


/**
 * @brief StartDialog::enableRunButtonIfReady - If every requirement is set, enable the run button
 */
void StartDialog::enableRunButtonIfReady() {
    bool isUploadedDataset = !uploadedDatasets.isEmpty();
    bool isUseDefaultSelected = ui->checkBoxUseDefault->isChecked();
    bool isCustomMarkerFileUploaded = !uploadedMarkerFile.isNull();

    if (isUploadedDataset && (isUseDefaultSelected || isCustomMarkerFileUploaded)) {
        ui->buttonRun->setEnabled(true);
    }
}

/**
 * @brief StartDialog::disableUseDefaultButton - Disables the default button (in case no valid default file was found)
 */
void StartDialog::disableUseDefaultButton() {
    this->ui->checkBoxUseDefault->setDisabled(true);
}

// ++++++++++++++++++++++++++++++++ SLOTS ++++++++++++++++++++++++++++++++
// STACKED WIDGET PAGE ONE
/**
 * @brief StartDialog::on_buttonMenuBarExit_clicked
 */
__attribute__((noreturn)) void StartDialog::on_buttonMenuBarExit_clicked() {
    exit(0);
}


/**
 * @brief StartDialog::on_buttonExit_clicked
 */
__attribute__((noreturn)) void StartDialog::on_buttonExit_clicked() {
    exit(0);
}


/**
 * @brief StartDialog::on_buttonLoadProject_clicked
 */
void StartDialog::on_buttonLoadProject_clicked() {
    QStringList csvMimeTypes = { "text/plain" };
    QStringList fileNames = openFileDialog(this, csvMimeTypes, false);

    if (fileNames.empty())
        return;

    qDebug() << "Sent project file name.";
    emit projectFileUploaded(fileNames);
}


/**
 * @brief StartDialog::on_buttonNewProject_clicked
 */
void StartDialog::on_buttonNewProject_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}


// STACKED WIDGET PAGE TWO
/**
 * @brief StartDialog::on_buttonMenuBarBack_clicked
 */
void StartDialog::on_buttonMenuBarBack_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}


/**
 * @brief StartDialog::on_checkBoxUseDefault_stateChanged
 * @param state
 */
void StartDialog::on_checkBoxUseDefault_stateChanged(int state) {
    if (state == Qt::CheckState::Checked) {
        ui->buttonLoadCustom->setDisabled(true);

        // REMEMBER: This should be done differently
        uploadedMarkerFile = "nAn";
        enableRunButtonIfReady();
    } else {
        ui->buttonLoadCustom->setDisabled(false);
        ui->buttonRun->setDisabled(true);
    }
}


/**
 * @brief StartDialog::on_buttonMenuBarExit_2_clicked
 */
__attribute__((noreturn)) void StartDialog::on_buttonMenuBarExit_2_clicked() {
    exit(0);
}


/**
 * @brief StartDialog::on_buttonLoadCustom_clicked
 */
void StartDialog::on_buttonLoadCustom_clicked() {
    QStringList csvMimeTypes = { "text/csv" };
    QStringList fileNames = openFileDialog(this, csvMimeTypes, false);

    if (fileNames.empty())
        return;

    uploadedMarkerFile = fileNames.first();
    enableRunButtonIfReady();
    qDebug() << "Uploaded" << fileNames[0];
}


/**
 * @brief StartDialog::on_buttonAddDataset_clicked
 */
void StartDialog::on_buttonAddDataset_clicked() {
    QStringList csvMimeTypes = { "text/csv" };
    QStringList filePaths = openFileDialog(this, csvMimeTypes, true);

    if (filePaths.empty())
        return;

    for (int i = 0; i < filePaths.length(); i++) {
        QString filePath = filePaths[i];
        QString fileName = chopFileName(filePath);

        // If file has already been uploaded, skip it
        if (uploadedDatasets.contains(fileName)) {
            continue;
        }

        uploadedDatasets.insert(fileName, filePath);
        addDatasetToLayout(fileName);
        qDebug() << "Uploaded" << fileName;
    }
    enableRunButtonIfReady();
}


/**
 * @brief StartDialog::on_buttonRemoveDataset_clicked
 */
void StartDialog::on_buttonRemoveDataset_clicked() {
    qDebug() << uploadedDatasets;
    for (QListWidgetItem * item : ui->listDatasets->selectedItems()) {
        uploadedDatasets.remove(item->text());
        delete ui->listDatasets->takeItem(ui->listDatasets->row(item));
    }
    qDebug() << uploadedDatasets;
}


/**
 * @brief StartDialog::on_buttonRun_clicked
 */
void StartDialog::on_buttonRun_clicked() {
    emit runNewProject(uploadedMarkerFile, uploadedDatasets.values());
    this->close();
    //REMEMBER: How to delete this the right way?
//    this->deleteLater(); ?!
//    this->~StartDialog();
}

// ++++++++++++++++++++++++++++++++ SLOTS ++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++ MOUSE ++++++++++++++++++++++++++++++++
void StartDialog::mousePressEvent(QMouseEvent * mousePressEvent) {
this->mouseClickXCoordinate = mousePressEvent->x();
this->mouseClickYCoordinate = mousePressEvent->y();
}

void StartDialog::mouseMoveEvent(QMouseEvent * mouseMoveEvent) {
this->move(mouseMoveEvent->globalX() - this->mouseClickXCoordinate, mouseMoveEvent->globalY() - this->mouseClickYCoordinate);
}
