#include "plottermanager.h"

PlotterManager::PlotterManager(QObject *parent)
    : QObject(parent)
{

}

QVector<int> PlotterManager::get_selected_signal_ids_for_given_plot_id(int plot_id)
{
    QVector<int> selected_indexes_at_plot;
    QCustomPlot *plot = get_plot_given_plot_index( plot_id );
    for( int l_var0 = 0; l_var0 < plot->graphCount(); l_var0++ )
    {
        if(  plot->graph(l_var0)->selected() )
        {
            selected_indexes_at_plot.append( plot->graph(l_var0)->name().toInt() );
        }
    }

    return selected_indexes_at_plot;
}

bool PlotterManager::set_signal_at_specific_plot(int signal_id, int plot_id)
{
    if( !m_plot_id_2_signal_id.contains(plot_id, signal_id ) )
    {
        QCustomPlot *plot( m_plot_widget_2_plot_id.value( plot_id ) );
        m_plot_id_2_signal_id.insert( plot_id, signal_id );
        m_plot_widget_2_plot_id.insert( signal_id, plot );
        connect( m_signal_db, SIGNAL( signal_removed(int) ), this, SLOT( remove_graph_from_plot(int) ) );
        return true;
    }
    return false;
}

bool PlotterManager::is_signal_at_plot(int signal_id, int plot_id)
{
    return m_plot_id_2_signal_id.contains( plot_id, signal_id );
}

bool PlotterManager::remove_graph_from_plot( int signal_id )
{
    bool ret = false;
    int plot_id( m_plot_id_2_signal_id.key( signal_id ) );
    QCustomPlot *plot( m_plot_widget_2_plot_id.value( plot_id ) );
    if(  plot != nullptr )
    {
        QCPGraph *graph;
        for( int l_var0 = 0; l_var0 < plot->graphCount(); l_var0++ )
        {
            graph = plot->graph(l_var0);
            if( graph->name().toInt() == signal_id )
            {
                plot->removeGraph( graph );
                m_plot_id_2_signal_id.remove( plot_id, signal_id );
                ret = true;
            }
        }
    }
    return ret;
}

bool PlotterManager::remove_signal_from_dbs(int signal_id)
{
    while( m_plot_id_2_signal_id.find(signal_id) != m_plot_id_2_signal_id.end() )
    {
        m_plot_id_2_signal_id.remove(signal_id);
    }
}

int PlotterManager::new_plot_index()
{
    m_plot_indexes.append( m_plot_indexes.size() );
    return m_plot_indexes.last();
}

int PlotterManager::add_plot(QCustomPlot *new_plot)
{
    int new_index( new_plot_index() );

    m_plot_widget_2_plot_id.insert( new_index, new_plot );

    return new_index;
}


QList<int> PlotterManager::get_signal_indexes_from_plot(int plot_id)
{
    QList<int> indexes_at_plot;
    QCustomPlot *plot = get_plot_given_plot_index( plot_id );
    for( int l_var0 = 0; l_var0 < plot->graphCount(); l_var0++ )
    {
        indexes_at_plot.append( plot->graph(l_var0)->name().toInt() );
    }

    return indexes_at_plot;
}

QPair<QVector<double>, QVector<double> > PlotterManager::get_data_for_customPlot_given_signal_id(int signal_id)
{
    QPolygonF signal = m_signal_db->get_signal_data( signal_id );
    int t_size = signal.size();
    QVector<double> x(t_size), y(t_size);



    for( int l_var0 = 0; l_var0 < t_size; l_var0++ )
    {
        x[l_var0] = signal.at(l_var0).x();
        y[l_var0] = signal.at(l_var0).y();
    }

    QPair<QVector<double>,QVector<double> > ret;
    ret.first = x;
    ret.second = y;
    return ret;
}

QPair<QVector<double>, QVector<double> > PlotterManager::get_data_for_customPlot_normalized(int signal_id)
{
    QPolygonF signal = m_signal_db->get_signal_data( signal_id );
    int t_size = signal.size();
    QVector<double> x(t_size), y(t_size);



    for( int l_var0 = 0; l_var0 < t_size; l_var0++ )
    {
        x[l_var0] = signal.at(l_var0).x();
        y[l_var0] = signal.at(l_var0).y();
    }

    double max = get_maximum_from_vector( y );


    for( int l_var0 = 0; l_var0 < t_size; l_var0++ )
    {
        y[l_var0] /= max;
    }

    QPair<QVector<double>,QVector<double> > ret;
    ret.first = x;
    ret.second = y;
    return ret;
}

int PlotterManager::get_maximum_from_vector(QVector<double> values)
{
    int ret(0);
    if( values.isEmpty() )
    {
        return ret;
    }

    for( int l_var0 = 0; l_var0 < values.size(); l_var0++ )
    {
        if( values.at( l_var0 ) > values.at( ret ) )
        {
            ret = l_var0;
        }
    }
    return  ret;
}

bool PlotterManager::update_data_signal_at_plot(int signal_id, QPair<QVector<double>, QVector<double> > data)
{
    QCPGraph *t_graph( nullptr );

    auto plot = get_plot_given_signal_index( signal_id );
    for( int l_var0 = 0; l_var0 < plot->graphCount(); l_var0++ )
    {
        if( plot->graph(l_var0)->name().toInt() == signal_id )
        {
            t_graph = plot->graph(l_var0);
            break;
        }
    }
    if( t_graph != nullptr )
    {
        t_graph->setData( data.first, data.second );
        return true;
    }
    return false;
}

bool PlotterManager::add_signal_at_plot( int signal_id, int plot_id )
{
    QPair< QVector<double>, QVector<double> > t_graph;
    SignalItem *signal = m_signal_db->get_signal( signal_id );
    set_signal_at_specific_plot( signal_id, plot_id);
    t_graph = get_data_for_customPlot_given_signal_id( signal_id );
    QCustomPlot *plot = get_plot_given_plot_index( plot_id );
    QCPGraph *new_graph = plot->addGraph();
    new_graph->addData( t_graph.first, t_graph.second);
    new_graph->setName( QString::number( signal_id ) );
    new_graph->setPen( signal->get_signal_pen() );
    return true;
}

bool PlotterManager::remove_signal_from_plot(int signal_id, int plot_id)
{
    QCustomPlot *plot( get_plot_given_plot_index( plot_id ) );
    bool ret = false;
    if( plot != nullptr && is_signal_at_plot( signal_id, plot_id ) )
    {
        QCPGraph *graph;
        for( int l_var0 = 0; l_var0 < plot->graphCount(); l_var0++ )
        {
            graph = plot->graph(l_var0);
            if( graph->name().toInt() == signal_id )
            {
                plot->removeGraph( graph );
                ret = true;
            }
        }
    }

    return ret;
}

bool PlotterManager::remove_selected_signals_at_plot(int plot_id)
{
    bool ret = false;
    QCustomPlot *plot = get_plot_given_plot_index( plot_id );
    QCPGraph *graph( nullptr );

    for( int l_var0 = 0; l_var0 < plot->graphCount(); l_var0++ )
    {
        graph = plot->graph( l_var0 );
        if( graph->selected() )
        {
            plot->removeGraph(graph);
            ret = true;
        }
    }
    plot->replot();
    return ret;
}

QCustomPlot *PlotterManager::get_plot_given_plot_index(int plot_id)
{
    return m_plot_widget_2_plot_id.value( plot_id );
}

QCustomPlot *PlotterManager::get_plot_given_signal_index(int signal_id)
{
    return m_plot_widget_2_plot_id.value( m_plot_id_2_signal_id.key( signal_id ) );
}

void PlotterManager::set_selected_graphs(QList<int> signals_id, bool selected)
{

}
