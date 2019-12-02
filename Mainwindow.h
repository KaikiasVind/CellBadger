#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QVector>
#include <QPair>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void plotHeatMap(QVector<QVector<QPair<QString, double>>> tissueCorrelations);
    void plotHeatMap();

signals:
    QStringList filesUploaded(QStringList filePaths);

private slots:
    void on_buttonExit_clicked();

    void on_buttonMaximize_clicked();

    void on_buttonMinimize_clicked();

    void on_buttonUploadData_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
