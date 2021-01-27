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

    if( plot == nullptr  )
    {
        return;
    }
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
    connect( new_plot, SIGNAL( plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));

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
    QVector<double> x,y;

    x = m_signal_db->get_signal_x_values( signal_id );
    y = m_signal_db->get_signal_y_values( signal_id );

    int t_size = y.size();

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

double PlotterManager::get_maximum_from_vector( const QVector<double> &values)
{
    int ret(0);
    if( values.isEmpty() )
    {
        return ret;
    }

    for( int l_var0 = 0; l_var0 < values.size(); l_var0++ )
    {
        if( values.at( l_var0 ) >= values.at( ret ) )
        {
            ret = l_var0;
        }
    }
    return  values.at( ret );
}

bool PlotterManager::update_data_signal_at_plot(int signal_id, const QPolygonF &new_signal_data )
{
    QCPGraph *t_graph( nullptr );

    QPair< QVector< double >, QVector< double > > data;

    for( int l_var0 = 0; l_var0 < new_signal_data.size(); l_var0++ )
    {
        data.first.append( new_signal_data.at(l_var0).x() );
        data.second.append( new_signal_data.at(l_var0).y() );
    }

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
                plot->replot();
            break;
            default:
                break;
        }
    }
}

void PlotterManager::replot_all()
{
    QHash< int, QCustomPlot* >::iterator t_has_iterator =  m_plot_widget_2_plot_id.begin();
    while( t_has_iterator != m_plot_widget_2_plot_id.end() )
    {
        t_has_iterator.value()->replot();
        t_has_iterator++;
        update_signal_names( t_has_iterator.key() );
    }
}

void PlotterManager::initialize_plot(QCustomPlot *plot)
{
    plot->setOpenGl( true );
    plot->setInteractions(QCP::iSelectPlottables | QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectItems | QCP::iMultiSelect );
    initialize_layers(plot);
    initialize_mouse_cursors(plot);
}

void PlotterManager::initialize_layers(QCustomPlot *plot)
{
    if( plot == nullptr )
    {
        return;
    }

    for( int l_var0 = 0; l_var0 < m_default_plot_layers.size(); l_var0++ )
    {
        plot->addLayer( m_default_plot_layers.at( l_var0 ) );
    }

    plot->layer("MouseCursors")->setVisible(false);

}

void PlotterManager::initialize_mouse_cursors(QCustomPlot *plot)
{
    // add mouse cursors
    QCPItemStraightLine *x_line = new QCPItemStraightLine(plot);
    QCPItemStraightLine *y_line = new QCPItemStraightLine(plot);
    QCPItemLine *x_line_cross = new QCPItemLine(plot);
    QCPItemLine *y_line_cross = new QCPItemLine(plot);
    QPen t_pen( x_line_cross->pen() );
    t_pen.setWidth( t_pen.width()*2 );

    x_line->setLayer("MouseCursors");
    x_line->setObjectName("line_x");
    x_line->setSelectable(false);
    x_line->position("point1")->setCoords( QPointF( 0.0, 0.0 ) );
    x_line->position("point2")->setCoords( QPointF( 0.0, 0.0 ) );

    y_line->setLayer("MouseCursors");
    y_line->setObjectName("line_y");
    y_line->setSelectable(false);
    y_line->position("point1")->setCoords( QPointF( 0.0, 0.0 ) );
    y_line->position("point2")->setCoords( QPointF( 0.0, 0.0 ) );

    x_line_cross->setLayer("MouseCursors");
    x_line_cross->setObjectName("line_x_cross");
    x_line_cross->setSelectable(false);
    x_line_cross->position("start")->setCoords( QPointF( 0.0, 0.0 ) );
    x_line_cross->position("end")->setCoords( QPointF( 0.0, 0.0 ) );
    x_line_cross->setPen( t_pen );

    y_line_cross->setLayer("MouseCursors");
    y_line_cross->setObjectName("line_y_cross");
    y_line_cross->setSelectable(false);
    y_line_cross->position("start")->setCoords( QPointF( 0.0, 0.0 ) );
    y_line_cross->position("end")->setCoords( QPointF( 0.0, 0.0 ) );
    y_line_cross->setPen( t_pen );

    /*Add mouse coordinates*/
    QCPItemText *mouse_coordinates = new QCPItemText( plot );
    mouse_coordinates->setLayer("MouseCursors");
    mouse_coordinates->setObjectName("mouse_cursor_label");
    mouse_coordinates->position->setType( QCPItemPosition::ptPlotCoords );
    mouse_coordinates->setPositionAlignment( Qt::AlignLeft | Qt::AlignBottom );
    mouse_coordinates->setColor( Qt::black );
    mouse_coordinates->setText( "( 0.0 , 0.0 )" );
    mouse_coordinates->setFont( QFont( plot->font().family(), 10) );
    mouse_coordinates->setSelectable( false );

    QFontMetrics *fontMetrics = new QFontMetrics( mouse_coordinates->font() );
    int pixels_length = fontMetrics->width( mouse_coordinates->text() );
    QPointF label_position = plot->axisRect()->bottomRight();
    label_position.setX( label_position.x() - pixels_length );
    label_position.setY( label_position.y() - fontMetrics->height() );
    mouse_coordinates->position->setCoords(label_position ); // move 10 pixels to the top from bracket center anchor


}

void PlotterManager::initialize_mouse_coords(QCustomPlot *plot)
{
    /*Add mouse coordinates*/
    QCPItemText *mouse_coordinates = new QCPItemText( plot );
    mouse_coordinates->setLayer("MouseCoordinates");
    mouse_coordinates->setObjectName("mouse_coordinates");
    mouse_coordinates->position->setType( QCPItemPosition::ptViewportRatio );
    mouse_coordinates->setPositionAlignment( Qt::AlignLeft | Qt::AlignBottom );
    mouse_coordinates->setColor( Qt::black );
    mouse_coordinates->setText( "[ 0.0 , 0.0 ]" );
    mouse_coordinates->setFont( QFont( plot->font().family(), 10) );
    mouse_coordinates->setSelectable( false );

    QFontMetrics *fontMetrics = new QFontMetrics( mouse_coordinates->font() );
    int pixels_length = fontMetrics->width( mouse_coordinates->text() );
    QPointF label_position = plot->axisRect()->bottomRight();
    label_position.setX( label_position.x() - pixels_length );
    label_position.setY( label_position.y() - fontMetrics->height() );
    mouse_coordinates->position->setPixelPosition( label_position ); // move 10 pixels to the top from bracket center anchor
}

void PlotterManager::initialize_axis(QCustomPlot *plot)
{
    plot->xAxis->setLabel("X");
    plot->yAxis->setLabel("Y");
    plot->xAxis->setRange(-50.0, 50.0 );
    plot->yAxis->setRange(-50.0, 50.0 );
}

void PlotterManager::graphClicked(QCPAbstractPlottable *plottable, int dataIndex)
{
    QCPGraph *t_possible_graph = dynamic_cast<QCPGraph*>( plottable );

    if( t_possible_graph == nullptr )
    {
        return;
    }

    cast_signal_analysis_results( m_plot_id_2_signal_id.key( t_possible_graph->name().toInt() ), t_possible_graph->name().toInt() );

}

bool PlotterManager::add_label_at_plot( const int &p_plot_id, QMouseEvent *p_event )
{

    QCustomPlot *plot = m_plot_widget_2_plot_id.value( p_plot_id );

    plot->layer("Labels")->setVisible(true);

    if( plot->layer( "MouseCoordinates" )->children().isEmpty() )
    {
        initialize_mouse_coords( plot );
    }

    QPointF t_label_position( plot->xAxis->pixelToCoord( p_event->pos().x() ), plot->yAxis->pixelToCoord( p_event->pos().y() ) );

    QCPItemText *mouse_coordinates = dynamic_cast < QCPItemText* > ( plot->layer( "MouseCoordinates" )->children().at( 0 ) );

    /*Add label*/
    QCPItemText *t_new_label = new QCPItemText( plot );
    QString t_label_name ( mouse_coordinates->text() );
    t_new_label->setLayer("Labels");
    t_new_label->position->setType( QCPItemPosition::ptPlotCoords );
    t_new_label->setPositionAlignment( Qt::AlignLeft | Qt::AlignBottom );
    t_new_label->setColor( Qt::red );
    t_new_label->setSelectable( false );
    t_new_label->setText( t_label_name );
    t_new_label->setFont( QFont( plot->font().family(), 10) );
    t_new_label->setObjectName( t_label_name );
    t_new_label->position->setCoords( t_label_position ); // move 10 pixels to the top from bracket center anchor

    plot->replot();

    return ( true );
}

bool PlotterManager::remove_label_at_plot( const int &p_plot_id, QCPAbstractItem *p_item )
{

    QCustomPlot *plot = m_plot_widget_2_plot_id.value( p_plot_id );

    plot->layer("Labels")->setVisible(true);

    if( plot->layer( "Labels" )->children().isEmpty() )
    {
        initialize_mouse_coords( plot );
    }

    QCPItemText *t_may_be_selected_label = 0;
    for( int l_var0 = 0; l_var0 < plot->layer( "Labels" )->children().size(); l_var0++ )
    {
        t_may_be_selected_label = dynamic_cast < QCPItemText* > ( plot->layer( "Labels" )->children().at( l_var0 ) );
        if( t_may_be_selected_label ==  dynamic_cast < QCPItemText* >( p_item ) )
        {
            plot->removeItem( t_may_be_selected_label );
        }
    }

    plot->replot();

    return ( true );
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
    name_coordinates->position->setType( QCPItemPosition::ptViewportRatio );
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

void PlotterManager::update_mouse_cursors(QMouseEvent *event, int plot_id)
{
    QCustomPlot *plot = m_plot_widget_2_plot_id.value( plot_id );
    if( !plot->axisRect()->rect().contains( event->pos().x(), event->pos().y() ) )
    {
        plot->layer("MouseCursors")->setVisible(false);
        plot->replot();
        return;
    }else if( !plot->layer("MouseCursors")->visible() )
    {
         plot->layer("MouseCursors")->setVisible(true);
    }


    /*Get every graph on the plot*/
    QList< QCPGraph* > t_graphs_at_plot;
    for( int l_var0 = 0; l_var0 < plot->graphCount(); l_var0++ )
    {
        t_graphs_at_plot.append( plot->graph(l_var0) );
    }

    if( t_graphs_at_plot.isEmpty() ) return;

    /*Locate the mouse position inbetween graph values*/
    double t_x_mouse_position = plot->xAxis->pixelToCoord( event->pos().x() );
    int t_x_value_at_graph = 0;

    for( int l_var0 = 0; l_var0 < t_graphs_at_plot.at(0)->dataCount(); l_var0++ )
    {
        if( t_graphs_at_plot.at(0)->data()->at( l_var0 )->key < t_x_mouse_position )
        {
            t_x_value_at_graph = l_var0;
        }
    }

    //Find the "y" value inmediatly smaller than the mouse cursor
    double t_mouse_y_position = plot->yAxis->pixelToCoord( event->pos().y() );
    int t_nearest_graph_index = 0;
    double t_distance_to_graph = abs(t_mouse_y_position - t_graphs_at_plot.first()->data()->at(t_x_value_at_graph)->value );

    for( int l_var0 = 0; l_var0 < t_graphs_at_plot.size(); l_var0++ )
    {
        if( abs(t_mouse_y_position - t_graphs_at_plot.at(l_var0)->data()->at(t_x_value_at_graph)->value ) < t_distance_to_graph )
        {
            t_nearest_graph_index = l_var0;
            t_distance_to_graph = abs(t_mouse_y_position - t_graphs_at_plot.at(l_var0)->data()->at(t_x_value_at_graph)->value );
        }

    }

    QPointF mousePosition( plot->xAxis->pixelToCoord( event->pos().x() ), plot->yAxis->pixelToCoord( event->pos().y() ) );
    mousePosition.setY( t_graphs_at_plot.at(t_nearest_graph_index)->data()->at(t_x_value_at_graph)->value );
    QCPItemStraightLine *t_lineX = dynamic_cast < QCPItemStraightLine* > ( plot->layer( "MouseCursors" )->children().at( 0 ) );
    QCPItemStraightLine *t_lineY = dynamic_cast < QCPItemStraightLine* > ( plot->layer( "MouseCursors" )->children().at( 1 ) );
    QCPItemLine *t_lineX_cross = dynamic_cast < QCPItemLine* > ( plot->layer( "MouseCursors" )->children().at( 2 ) );
    QCPItemLine *t_lineY_cross = dynamic_cast < QCPItemLine* > ( plot->layer( "MouseCursors" )->children().at( 3 ) );

    QPointF xlineStart( plot->xAxis->range().lower, mousePosition.y() );
    QPointF xlineEnd( plot->xAxis->range().upper, mousePosition.y() );
    QPointF ylineStart( mousePosition.x(), plot->yAxis->range().lower );
    QPointF ylineEnd( mousePosition.x(),  plot->yAxis->range().upper );

    QPointF xlineCrossStart( mousePosition.x() - plot->xAxis->range().size()/50, mousePosition.y() );
    QPointF xlineCrossEnd( mousePosition.x() + plot->xAxis->range().size()/50, mousePosition.y() );
    QPointF ylineCrossStart( mousePosition.x(), mousePosition.y() - plot->yAxis->range().size()/50 );
    QPointF ylineCrossEnd( mousePosition.x(),  mousePosition.y() + plot->yAxis->range().size()/50  );

    t_lineX->position("point1")->setCoords( xlineStart );
    t_lineX->position("point2")->setCoords( xlineEnd );
    t_lineY->position("point1")->setCoords( ylineStart );
    t_lineY->position("point2")->setCoords( ylineEnd );

    t_lineX_cross->position("start")->setCoords( xlineCrossStart );
    t_lineX_cross->position("end")->setCoords( xlineCrossEnd );
    t_lineY_cross->position("start")->setCoords( ylineCrossStart );
    t_lineY_cross->position("end")->setCoords( ylineCrossEnd );

    QCPItemText *mouse_coordinates = dynamic_cast < QCPItemText* > ( plot->layer( "MouseCursors" )->children().at( 4 ) );
    mouse_coordinates->setText( "( " + QString::number( mousePosition.x() ) + " , " +  QString::number( mousePosition.y() ) + " )");
    QPointF label_position;
    label_position.setX( mousePosition.x() );
    label_position.setY( mousePosition.y() );
    mouse_coordinates->position->setCoords(label_position ); // move 10 pixels to the top from bracket center anchor

}

void PlotterManager::update_mouse_coords(QMouseEvent *event, int plot_id)
{
    QCustomPlot *plot = m_plot_widget_2_plot_id.value( plot_id );

    plot->layer("MouseCoordinates")->setVisible(true);
    QPointF mousePosition( plot->xAxis->pixelToCoord( event->pos().x() ), plot->yAxis->pixelToCoord( event->pos().y() ) );
    if( plot->layer( "MouseCoordinates" )->children().isEmpty() )
    {
        initialize_mouse_coords( plot );
    }

    QCPItemText *mouse_coordinates = dynamic_cast < QCPItemText* > ( plot->layer( "MouseCoordinates" )->children().at( 0 ) );
    mouse_coordinates->setText( "( " + QString::number( mousePosition.x() ) + " , " +  QString::number( mousePosition.y() ) + " )");
    QFontMetrics *fontMetrics = new QFontMetrics( mouse_coordinates->font() );
    int pixels_length = fontMetrics->width( mouse_coordinates->text() );
    QPointF label_position = plot->axisRect()->bottomRight();
    label_position.setX( label_position.x() - pixels_length );
    label_position.setY( label_position.y() - fontMetrics->height() );
    mouse_coordinates->position->setPixelPosition( label_position ); // move 10 pixels to the top from bracket center anchor
    plot->replot();
}

void PlotterManager::cast_signal_analysis_results(int plot_id, int signal_id)
{
    QCustomPlot *plot = m_plot_widget_2_plot_id.value( plot_id );
    if( plot == nullptr )
    {
        return;
    }
    float t_average = m_signal_db->get_signal( signal_id )->get_signal_average();
    float t_std_deviation = m_signal_db->get_signal( signal_id )->get_signal_std_deviation();
    float t_first_harmonic = m_signal_db->get_signal( signal_id )->get_signal_first_harmonic();

    emit update_signal_analysis_results( t_average, t_first_harmonic, t_std_deviation );

}

bool PlotterManager::set_axis_label(QCPAxis *p_axis)
{
    bool ok;
    QString text = QInputDialog::getText( new QWidget, tr("Set Axis label"),
                                        tr("Axis name"), QLineEdit::Normal,
                                        "Time", &ok);
    if (ok && !text.isEmpty())
    {
       p_axis->setLabel( text );
    }

    return (ok);
}

void PlotterManager::graphDoubleClicked(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *p_mouse_event)
{
    QCPGraph *t_graph = dynamic_cast< QCPGraph* >(plottable);
    QPen t_pen( QColorDialog::getColor("New colour for signal"));
    m_signal_db->set_signal_pen( t_graph->name().toInt(), t_pen );

}
