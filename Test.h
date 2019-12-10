#ifndef TEST_H
#define TEST_H

#include <QObject>

class Test : public QObject
{
    Q_OBJECT

public:
    Test();

public slots:
    void on_finished();
};

#endif // TEST_H
