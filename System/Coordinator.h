#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <QObject>

#include "System/InformationCenter.h"

/**
 * @brief The Coordinator class - This class is used to model the basic workflow and to concentrate the program logic in one place
 */
class Coordinator : public QObject
{
    Q_OBJECT

private:
    InformationCenter informationCenter;

public:
    Coordinator(InformationCenter informationCenter);

public slots:
    // ################### INTERACTION WITH MAIN WINDOW ########################
    void on_filesUploaded(QStringList filePaths);
    void on_projectFileUploaded(QStringList filePaths);
    // ################### INTERACTION WITH MAIN WINDOW ########################
};

#endif // COORDINATOR_H
