#include "operationsmanager.h"

OperationsManager::OperationsManager(QObject *parent) : QObject(parent)
{

}

QPolygonF OperationsManager::perform_addition(int p_first_signal, int p_second_signal)
{
    QPolygonF t_ret;
    QPolygonF t_first_operand = m_signal_db->get_signal_data(p_first_signal);
    QPolygonF t_second_operand = m_signal_db->get_signal_data(p_second_signal);

    if( !m_signal_db->get_opened_signals().contains( p_first_signal) || !m_signal_db->get_opened_signals().contains( p_second_signal ) )
    {
        qDebug() << "There are no signals with those indexes";
        return t_ret;
    }

    // First check length of both vectors

    if( t_first_operand.length() == t_second_operand.length() )
    {
        for( int l_var0 = 0; l_var0 < t_first_operand.length(); l_var0++ )
        {
            t_ret.append( QPointF(t_first_operand.at(l_var0).x(), t_first_operand.at(l_var0).y() + t_second_operand.at(l_var0).y() ) );
        }
    }
    else
    {

    }


    return (t_ret);
}

QPolygonF OperationsManager::perform_subtraction(int p_first_signal, int p_second_signal)
{
    QPolygonF t_ret;
    QPolygonF t_first_operand = m_signal_db->get_signal_data(p_first_signal);
    QPolygonF t_second_operand = m_signal_db->get_signal_data(p_second_signal);

    if( !m_signal_db->get_opened_signals().contains( p_first_signal) || !m_signal_db->get_opened_signals().contains( p_second_signal ) )
    {
        qDebug() << "There are no signals with those indexes";
        return t_ret;
    }

    // First check length of both vectors

    if( t_first_operand.length() == t_second_operand.length() )
    {
        for( int l_var0 = 0; l_var0 < t_first_operand.length(); l_var0++ )
        {
            t_ret.append( QPointF(t_first_operand.at(l_var0).x(), t_first_operand.at(l_var0).y() - t_second_operand.at(l_var0).y() ) );
        }
    }
    else
    {

    }


    return (t_ret);
}

QPolygonF OperationsManager::perform_division(int p_first_signal, int p_second_signal)
{
    QPolygonF t_ret;
    QPolygonF t_first_operand = m_signal_db->get_signal_data(p_first_signal);
    QPolygonF t_second_operand = m_signal_db->get_signal_data(p_second_signal);

    if( !m_signal_db->get_opened_signals().contains( p_first_signal) || !m_signal_db->get_opened_signals().contains( p_second_signal ) )
    {
        qDebug() << "There are no signals with those indexes";
        return t_ret;
    }

    // First check length of both vectors

    if( t_first_operand.length() == t_second_operand.length() )
    {
        for( int l_var0 = 0; l_var0 < t_first_operand.length(); l_var0++ )
        {
            t_ret.append( QPointF(t_first_operand.at(l_var0).x(), t_first_operand.at(l_var0).y() / t_second_operand.at(l_var0).y() ) );
        }
    }
    else
    {

    }


    return (t_ret);
}

QPolygonF OperationsManager::perform_mutiplication(int p_first_signal, int p_second_signal)
{
    QPolygonF t_ret;
    QPolygonF t_first_operand = m_signal_db->get_signal_data(p_first_signal);
    QPolygonF t_second_operand = m_signal_db->get_signal_data(p_second_signal);

    if( !m_signal_db->get_opened_signals().contains( p_first_signal) || !m_signal_db->get_opened_signals().contains( p_second_signal ) )
    {
        qDebug() << "There are no signals with those indexes";
        return t_ret;
    }

    // First check length of both vectors

    if( t_first_operand.length() == t_second_operand.length() )
    {
        for( int l_var0 = 0; l_var0 < t_first_operand.length(); l_var0++ )
        {
            t_ret.append( QPointF(t_first_operand.at(l_var0).x(), t_first_operand.at(l_var0).y() * t_second_operand.at(l_var0).y() ) );
        }
    }
    else
    {

    }


    return (t_ret);
}

QPolygonF OperationsManager::perform_fft(int p_signal)
{
    QPolygonF t_ret;
    /*******************************Do the magic******************************************/
    JFFT t_fft;
    QVector<JFFT::cpx_type> t_x;
    QPolygonF t_signal( m_signal_db->get_signal_data(p_signal));
    double t_size = log2(t_signal.size());
    t_size = floor(t_size);
    t_size = exp2( t_size );
    t_x.resize(t_size);

    for( int l_var0 = 0; l_var0 < t_x.size(); l_var0++ )
    {
        t_x[l_var0]=t_signal.at(l_var0).y();
    }

    t_fft.fft(t_x);

    for( int l_var0 = 0; l_var0 < t_x.size(); l_var0++ )
    {
        QPointF t_pointf( t_signal.at(l_var0).x(), t_x[l_var0].real() );
        t_ret.append( t_pointf );
    }
    /******************************Magic trick is done************************************/


    return( t_ret );
}
