#include "signaldb.h"

SignalDB* SignalDB::m_signal_db_instance = 0;

SignalDB::SignalDB(QObject *parent) : QObject(parent)
{

}

SignalDB *SignalDB::instance()
{
    if (m_signal_db_instance == 0)
    {
        m_signal_db_instance = new SignalDB();
    }
    return m_signal_db_instance;
}

int SignalDB::create_signal(const QPolygonF &new_signal_data)
{
    int t_new_index = -1;

    return t_new_index;
}

int SignalDB::create_signal(const QString &new_signal_name, const QPolygonF &new_signal_data )
{
    int t_new_index = -1;

    if( new_signal_name != nullptr && new_signal_data.size() > 0 )
    {
        t_new_index = m_signals_db.size();
        while( m_signals_db.contains( t_new_index ) )
        {
            t_new_index++;
        }
        QPen t_pen( create_new_colour() );
        t_pen.setWidth( 2 );
        SignalItem *signal = new SignalItem( this, t_new_index, new_signal_name, new_signal_data, t_pen );
        m_signals_db.insert( t_new_index, signal );
    }

    return t_new_index;
}

QColor SignalDB::create_new_colour()
{
    int i = m_colours_list.size();
    QColor new_colour( qSin(i*0.3)*100+100, qSin(i*0.6+0.7)*100+100, qSin(i*0.4+0.6)*100+100 );
    QString colour_name( new_colour.name() );
    while( m_colours_list.contains( colour_name ) )
    {
        i++;
        new_colour = QColor( qSin(i*0.3)*100+100, qSin(i*0.6+0.7)*100+100, qSin(i*0.4+0.6)*100+100 );
        colour_name = new_colour.name();
    }
    m_colours_list.append( colour_name );
    return new_colour;

}

SignalItem *SignalDB::get_signal(int signal_id) const
{
    return  m_signals_db.value( signal_id );
}

bool SignalDB::close_signal(int signal_id)
{
    m_colours_list.removeOne( get_signal_pen(signal_id).color().name() );
    m_signals_db.remove( signal_id );
    emit signal_removed( signal_id );
    return true;

}

QString SignalDB::get_signal_name(int signal_id) const
{
    return m_signals_db.value( signal_id )->get_signal_name();
}

QPolygonF SignalDB::get_signal_data(int signal_id) const
{
    return m_signals_db.value( signal_id )->get_signal_data();
}

QPen SignalDB::get_signal_pen(int signal_id) const
{
    return m_signals_db.value( signal_id )->get_signal_pen();
}

QHash<int, QString> SignalDB::get_opened_signals() const
{
    QHash<int, QString> ret_value;

    return ret_value;
}
