#ifndef TSNEDIALOG_H
#define TSNEDIALOG_H

#include <QWidget>

#include "ExportDialog.h"

namespace Ui {
class TSNEDialog;
}

class TSNEDialog : public ExportDialog
{
    Q_OBJECT

public:
    explicit TSNEDialog(QWidget *parent = nullptr);
    ~TSNEDialog();

private:
    Ui::TSNEDialog *ui;
};

#endif // TSNEDIALOG_H
