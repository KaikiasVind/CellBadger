#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QPushButton>
#include <QHBoxLayout>
#include <QHash>
#include <QMouseEvent>

namespace Ui {
class StartDialog;
}

class StartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartDialog(QWidget *parent = nullptr);
    ~StartDialog();

    void disableUseDefaultButton();

signals:
    void projectFileUploaded(QStringList fileNames);
    void runNewProject(QString markerFilePath, QStringList datasetFileNames);

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

     void on_buttonAddDataset_clicked();

     void on_buttonRemoveDataset_clicked();

     void on_buttonRun_clicked();

private:
    Ui::StartDialog *ui;
    //REMEMBER: Isn't a hashmap an overkill here?
    QHash<QString, QString> uploadedDatasets;
    QString uploadedMarkerFile;

    QPushButton * createPushButton();
    void addDatasetToLayout(QString filePath);
    void removeDataSetFromLayout(QString name);

    void enableRunButtonIfReady();

    // Mouse interaction - Necessary for frameless windows
    void mousePressEvent(QMouseEvent * mousePressEvent);
    void mouseMoveEvent(QMouseEvent * mouseMoveEvent);
    int mouseClickXCoordinate;
    int mouseClickYCoordinate;
};

#endif // STARTDIALOG_H
