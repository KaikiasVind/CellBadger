#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QWidget>

namespace Ui {
class TabWidget;
}

class TabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TabWidget(QWidget *parent = nullptr);
    ~TabWidget();

private:
    Ui::TabWidget *ui;
};

#endif // TABWIDGET_H
