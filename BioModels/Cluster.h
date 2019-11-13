#ifndef CLUSTER_H
#define CLUSTER_H

#include <QStringList>

struct Cluster
{
    const QStringList m_expressedFeatures;

    Cluster(QStringList expressedFeatures);
};

#endif // CLUSTER_H
