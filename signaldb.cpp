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
        t_pen.setWidth( 1 );
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

bool SignalDB::update_signal(int signal_id, const QPolygonF &new_data)
{
    SignalItem *signal( m_signals_db.value( signal_id ) );
    if( signal != nullptr )
    {
        signal->set_signal_data( new_data );
        return true;
    }
    return false;
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

QVector<double> SignalDB::get_signal_x_values(int signal_id) const
{
    QVector< double > t_return_values;

    // is there any signal_id at DB
    if( m_signals_db.contains( signal_id ) )
    {
        QPolygonF t_signal_data = get_signal_data( signal_id );
        for( int l_var0 = 0; l_var0 < t_signal_data.size(); l_var0++ )
        {
            t_return_values.append( t_signal_data.at(l_var0).x() );
        }
    }

    return t_return_values;
}

bool SignalDB::set_signal_x_values(int signal_id, const QVector<double> &data )
{
    bool ret = false;

    // is there any signal_id at DB
    if( m_signals_db.contains( signal_id ) )
    {
        QPolygonF t_current_data = get_signal_data( signal_id );
        if( t_current_data.size() >= data.size() )
        {
            for( int l_var0 = 0; l_var0 < data.size(); l_var0++ )
            {
                t_current_data[l_var0].setX(data.at( l_var0 ) );
            }
        }
        m_signals_db.value( signal_id )->set_signal_data( t_current_data );
    }

    return ret;
}

QVector<double> SignalDB::get_signal_y_values(int signal_id) const
{
    QVector< double > t_return_values;
    // is there any signal_id at DB
    if( m_signals_db.contains( signal_id ) )
    {
        QPolygonF t_signal_data = get_signal_data( signal_id );
        for( int l_var0 = 0; l_var0 < t_signal_data.size(); l_var0++ )
        {
            t_return_values.append( t_signal_data.at(l_var0).y() );
        }
    }

    return t_return_values;
}

bool SignalDB::set_signal_y_values(int signal_id, const QVector<double> &data )
{
    bool ret = false;

    // is there any signal_id at DB
    if( m_signals_db.contains( signal_id ) )
    {
        QPolygonF t_current_data = get_signal_data( signal_id );
        if( t_current_data.size() >= data.size() )
        {
            for( int l_var0 = 0; l_var0 < data.size(); l_var0++ )
            {
                t_current_data[l_var0].setY(data.at( l_var0 ) );
            }
        }
        m_signals_db.value( signal_id )->set_signal_data( t_current_data );
    }

    return ret;
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
