#ifndef OPERATIONSMANAGER_H
#define OPERATIONSMANAGER_H

#include <QObject>

class OperationsManager : public QObject
{
    Q_OBJECT
public:
    explicit OperationsManager(QObject *parent = nullptr);

signals:

};

#endif // OPERATIONSMANAGER_H
