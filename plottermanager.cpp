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

        update_signal_names( plot_id );
//        QCPItemText *label;
//        for( int l_var0 = 0; l_var0 < plot->itemCount(); l_var0++ )
//        {
//            if( plot->item( l_var0 )->layer()->name() == "SignalNames" )
//            {
//                label = dynamic_cast< QCPItemText*>( plot->item( l_var0 ) );
//                if( label->text() == m_signal_db->get_signal_name( signal_id ) )
//                {
//                    plot->removeItem( label );

//                }
//            }
//        }
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

void PlotterManager::update_signal_names( int plot_id )
{

    QCustomPlot *plot = get_plot_given_plot_index( plot_id );

    // First delete all names
    QList<QCPItemText*> name_coordinates_list;
    for( int l_var0 = 0; l_var0 < plot->itemCount(); l_var0++ )
    {
        if( plot->item( l_var0 )->layer()->name() == "SignalNames" )
        {
            name_coordinates_list.append( dynamic_cast< QCPItemText*>( plot->item( l_var0 ) ) );
        }
    }

    QListIterator< QCPItemText* > t_list_iterator( name_coordinates_list );
    while( t_list_iterator.hasNext() )
    {
        plot->removeItem( t_list_iterator.next() );
    }

    // get info to print signals: names and colours
    QListIterator<int> t_signal_indexes( get_signal_indexes_from_plot(plot_id) );
    QList< const SignalItem* > t_signals;
    while( t_signal_indexes.hasNext() )
    {
        t_signals.append( m_signal_db->get_signal( t_signal_indexes.next() ) );
    }

    // Print them on the upperright corner of the plot
    QCPItemText *name_coordinates ( nullptr );
    QFontMetrics *fontMetrics = nullptr;
    int pixels_length = 0;
    QPointF label_position;
    QListIterator< const SignalItem*> t_signal_iterator( t_signals );
    const SignalItem *t_signal;
    while( t_signal_iterator.hasNext() )
    {
        t_signal = t_signal_iterator.next();
        name_coordinates = new QCPItemText( plot );
        name_coordinates->setLayer("SignalNames");
        name_coordinates->position->setType( QCPItemPosition::ptAbsolute );
        name_coordinates->setPositionAlignment( Qt::AlignLeft | Qt::AlignBottom );
        name_coordinates->setText( t_signal->get_signal_name() );
        name_coordinates->setFont( QFont( plot->font().family(), 10) );
        name_coordinates->setSelectable( false );
        name_coordinates->setColor( t_signal->get_signal_pen().color() );
        fontMetrics = new QFontMetrics( name_coordinates->font() );
        pixels_length = fontMetrics->width( t_signal->get_signal_name() );
        label_position = plot->axisRect()->topRight();
        label_position.setX( label_position.x() - pixels_length );
        label_position.setY( label_position.y() + fontMetrics->height() * ( t_signals.indexOf( t_signal ) + 1 ) );
        name_coordinates->position->setPixelPosition( label_position ); // move 10 pixels to the top from bracket center anchor
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
    new_plot->setObjectName( QString::number( new_index ) );
    initialize_plot( new_plot );
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

void PlotterManager::update_common_plot_properties(const PlotProperties &plot_property, bool enabled)
{
    QCustomPlot *plot( nullptr );
    for( int l_var0 = 0; l_var0 < m_plot_indexes.size(); l_var0++ )
    {
        plot = get_plot_given_plot_index( m_plot_indexes.at( l_var0 ) );
        switch ( plot_property )
        {
            case rect_zoom:
                enabled ? plot->setSelectionRectMode(QCP::srmZoom) : plot->setSelectionRectMode(QCP::srmNone);
            break;
            case normalized:
                break;
            case mouse_cursors:
                plot->layer("MouseCursors")->setVisible(enabled);
            break;
            default:
                break;
        }
    }
}

void PlotterManager::initialize_plot(QCustomPlot *plot)
{
    plot->setInteractions(QCP::iSelectPlottables | QCP::iRangeDrag | QCP::iRangeZoom | QCP::iMultiSelect );
    plot->xAxis->setLabel("X");
    plot->yAxis->setLabel("Y");
    plot->xAxis->setRange(-50.0, 50.0 );
    plot->yAxis->setRange(-50.0, 50.0 );

    for( int l_var0 = 0; l_var0 < m_default_plot_layers.size(); l_var0++ )
    {
        plot->addLayer(m_default_plot_layers.at(l_var0));
    }
    // add mouse cursors
    QCPItemStraightLine *x_line = new QCPItemStraightLine(plot);
    QCPItemStraightLine *y_line = new QCPItemStraightLine(plot);
    x_line->setLayer("MouseCursors");
    x_line->setObjectName("line_x");
    x_line->setSelectable(false);

    y_line->setLayer("MouseCursors");
    y_line->setObjectName("line_y");
    y_line->setSelectable(false);

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

    /*Add name and coordinates*/
    QCPItemText *name_coordinates = new QCPItemText( plot );
    name_coordinates->setLayer("SignalNames");
    name_coordinates->position->setType( QCPItemPosition::ptAbsolute );
    name_coordinates->setPositionAlignment( Qt::AlignLeft | Qt::AlignBottom );
    name_coordinates->setColor( signal->get_signal_pen().color() );
    name_coordinates->setText( signal->get_signal_name() );
    name_coordinates->setFont( QFont( plot->font().family(), 10) );
    name_coordinates->setSelectable( false );

    QFontMetrics *fontMetrics = new QFontMetrics( name_coordinates->font() );
    int pixels_length = fontMetrics->width( signal->get_signal_name() );
    QPointF label_position( plot->axisRect()->topRight() );
    label_position.setX( label_position.x() - pixels_length );
    label_position.setY( label_position.y() + fontMetrics->height() * get_signal_indexes_from_plot( plot_id ).count() );
    name_coordinates->position->setPixelPosition( label_position ); // move 10 pixels to the top from bracket center anchor
    return true;
}

bool PlotterManager::remove_signal_from_plot(int signal_id, int plot_id)
{
    QCustomPlot *plot = get_plot_given_plot_index( plot_id );
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
        QCPItemText *label;
        for( int l_var0 = 0; l_var0 < plot->itemCount(); l_var0++ )
        {
            label = dynamic_cast< QCPItemText*>( plot->item( l_var0 ) );
            if( label->text() == m_signal_db->get_signal_name( signal_id ) )
            {
                plot->removeItem( label );
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
