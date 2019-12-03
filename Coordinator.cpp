#include "Coordinator.h"

#include <QObject>
#include <QStringList>
#include <QDebug>


/**
 * @brief Coordinator::Coordinator
 */
Coordinator::Coordinator() {}


/**
 * @brief Coordinator::on_filesUploaded
 * @param filePaths
 */
void Coordinator::on_filesUploaded(QStringList filePaths) {
    qDebug() << "on_filesUploaded: received" << filePaths;
}


/**
 * @brief Coordinator::on_projectFileUploaded
 * @param filePath
 */
void Coordinator::on_projectFileUploaded(QStringList filePath) {
    qDebug() << "on_filesUploaded: received" << filePath;
}
