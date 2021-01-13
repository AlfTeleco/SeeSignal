#ifndef OPERATIONSMANAGER_H
#define OPERATIONSMANAGER_H

#include <QObject>
#include <QPolygonF>
#include "signaldb.h"

class OperationsManager : public QObject
{
    Q_OBJECT
public:
    explicit OperationsManager(QObject *parent = nullptr);

    QPolygonF perform_addition( int p_first_signal, int p_second_signal );
    QPolygonF perform_subtraction( int p_first_signal, int p_second_signal );
    QPolygonF perform_division( int p_first_signal, int p_second_signal );
    QPolygonF perform_mutiplication( int p_first_signal, int p_second_signal );

private:

    SignalDB                            *m_signal_db = SignalDB::instance();

signals:



};

#endif // OPERATIONSMANAGER_H
