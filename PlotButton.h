#ifndef PLOTBUTTON_H
#define PLOTBUTTON_H

#include <QObject>
#include <QPushButton>

class PlotButton : public QPushButton
{
    Q_OBJECT
public:
    PlotButton(const QString & text, QWidget * parent, const int buttonIndex);
    virtual ~PlotButton();

signals:
    void clickedWithIndex(const int buttonIndex = -1);

private:
    int buttonIndex;

private slots:
    void on_clicked(bool checked = false);

};

#endif // PLOTBUTTON_H
