#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QVector>
#include <QPair>
#include <QString>

#include "StartDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

//    void plotHeatMap(QVector<QVector<QPair<QString, double>>> tissueCorrelations);
//    void plotHeatMap();

public slots:
    void on_newProjectStarted(QStringList datasetFilePaths);
    void on_clusterFileParsed();

signals:
    void filesUploaded(QStringList filePaths);
    void projectFileUploaded(QStringList filePath);

private slots:
    __attribute__((noreturn)) void on_buttonExit_clicked();

    void on_buttonMaximize_clicked();

    void on_buttonMinimize_clicked();

private:
    Ui::MainWindow *ui;
    QVector<QThread> workingThreads;
};
#endif // MAINWINDOW_H
