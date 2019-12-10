#include "Test.h"

#include <QDebug>

Test::Test()
{

}

void Test::on_finished() {
    qDebug() << "Finished.";
}
