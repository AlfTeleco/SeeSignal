#include "signalitem.h"

SignalItem::SignalItem(QObject *parent, int signal_index, const QString &signal_name, const QPolygonF &signal_data) :
    QObject(parent),
    m_signal_index(signal_index),
    m_signal_name(signal_name),
    m_signal_data(signal_data)
{

}

SignalItem::SignalItem(QObject *parent, int signal_index, const QString &signal_name, const QPolygonF &signal_data, const QPen &signal_pen ) :
    QObject(parent),
    m_signal_index(signal_index),
    m_signal_name(signal_name),
    m_signal_data(signal_data),
    m_signal_pen(signal_pen)
{

}

SignalItem::SignalItem(const SignalItem &signal)
{

}

SignalItem::~SignalItem()
{

}
