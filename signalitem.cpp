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

float SignalItem::get_signal_average() const
{
    float ret = 0.0;

    for( int l_var0 = 0; l_var0 < m_signal_data.size(); l_var0++ )
    {
        ret += m_signal_data.at( l_var0 ).y();
    }


    return  ( ret / m_signal_data.size() );
}

float SignalItem::get_signal_first_harmonic() const
{
    float ret = 0.0;

    return ret;
}

float SignalItem::get_signal_std_deviation() const
{
    float ret = 0.0;   
    float sum = 0.0, mean, standardDeviation = 0.0;

    for(int l_var0 = 0; l_var0 < m_signal_data.size(); ++l_var0)
    {
        sum += m_signal_data.at( l_var0 ).y();
    }

    mean = sum/m_signal_data.size();

    for(int l_var0 = 0; l_var0 < m_signal_data.size(); ++l_var0)
    {
        standardDeviation += pow(m_signal_data.at( l_var0 ).y()- mean, 2);
    }
    ret = sqrt(standardDeviation / m_signal_data.size());

    return ret;
}
