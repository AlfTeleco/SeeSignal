#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    create_new_tab_plot( QString::number( ui->tabWidget->count() ) );
    initialize_file_formatter_dialog();
    ui->tool_signal_analysis->setVisible(false);
    QCustomPlot *t_plot = new QCustomPlot(ui->spectrum_plot);
    QHBoxLayout *horizontal_layout = new QHBoxLayout;
    horizontal_layout->addWidget(t_plot);
    t_plot->setObjectName("spectrum_plot");
    t_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom );
    ui->spectrum_plot->setLayout( horizontal_layout );
//    connect( &m_plotter_manager, SIGNAL( update_signal_analysis_results(float, float, float) ), this, SLOT( update_signal_anylsis_parameters(float, float, float)));
    ui->signalCalculator->setVisible(false);
    ui->op1_edit->installEventFilter( this );
    ui->op2_edit->installEventFilter( this );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize_file_formatter_dialog()
{
    m_file_formatter = new FileFormatterDialog();
}

int MainWindow::create_same_tab_plot()
{
    int new_index( -1 );
    QCustomPlot *plot = new QCustomPlot(ui->tabWidget->currentWidget());
    new_index = m_plotter_manager.add_plot( plot );
    plot->installEventFilter( this );
    ui->tabWidget->currentWidget()->installEventFilter( this );

    QTableWidget *table_widget = new QTableWidget( ui->tabWidget->currentWidget() );
    //table_widget->installEventFilter( this );
    table_widget->setColumnCount(2);
    QTableWidgetItem *signal_index_item = new QTableWidgetItem(tr("Index"));
    QTableWidgetItem *signal_name_item  = new QTableWidgetItem(tr("Name"));
    table_widget->setHorizontalHeaderItem( 0, signal_index_item);
    table_widget->setHorizontalHeaderItem( 1, signal_name_item);
    table_widget->verticalHeader()->setVisible(false);
    table_widget->horizontalHeader()->setStretchLastSection(true);
    table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_widget->setSortingEnabled(true);
    table_widget->setDragEnabled(true);

    QHBoxLayout *horizontal_layout = new QHBoxLayout;
    horizontal_layout->addWidget(plot);
    horizontal_layout->addWidget(table_widget);
    ui->tabWidget->currentWidget()->setLayout( horizontal_layout );
    horizontal_layout->setStretch(0,8);
    horizontal_layout->setStretch(1,2);

    connect( table_widget, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(tableWidget_itemDoubleClicked(QTableWidgetItem *) ) );
    //connect( plot, SIGNAL( plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));
    connect( plot, SIGNAL( mouseDoubleClick(QMouseEvent*)), this, SLOT( set_label_at_plot(QMouseEvent*)));
    connect( plot, SIGNAL( itemClick(QCPAbstractItem*, QMouseEvent*)), this, SLOT( remove_label_at_plot(QCPAbstractItem*)));
    connect( plot, SIGNAL( axisDoubleClick(QCPAxis *, QCPAxis::SelectablePart, QMouseEvent *)), this, SLOT( change_axis_label(QCPAxis*)));
    connect( plot, SIGNAL( plottableDoubleClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphDoubleClicked(QCPAbstractPlottable*,int,QMouseEvent*)));

    return new_index;
}

int MainWindow::create_new_tab_plot( const QString &tab_name )
{
    int new_index( -1 );
    if( !tab_name.isEmpty() )
    {
        QWidget *widget = new QWidget();
        ui->tabWidget->setCurrentIndex(ui->tabWidget->addTab(widget, tab_name ) );
        create_same_tab_plot();
    }
    return new_index;
}

void MainWindow::open_file(const QString &file_name)
{
    if( file_name.isEmpty() )
    {
        return;
    }

    QHash< QString, QPolygonF > t_new_signals( m_file_parser.open_csv_signal_file(file_name, m_file_formatter->get_csv_separator()) );
    QHash< QString, QPolygonF >::iterator i_new_signals = t_new_signals.begin();
    QVector< int > t_new_signals_index;
    int plot_id(0);

    while(i_new_signals != t_new_signals.end() )
    {
        t_new_signals_index.append( m_signalDb->create_signal( i_new_signals.key(), i_new_signals.value() ) );
        i_new_signals++;
    }

    if( ui->tabWidget->count() == 0)
    {
        create_new_tab_plot( file_name );
    }

    plot_id = ui->tabWidget->currentWidget()->children().first()->objectName().toInt();

    for( int l_var0 = 0; l_var0 < t_new_signals.size(); l_var0++ )
    {
        add_signal_at_table( t_new_signals_index.at( l_var0 ) );
    }

    ui->tabWidget->setTabText( ui->tabWidget->currentIndex(), file_name.split('/').last() );
    m_plotter_manager.get_plot_given_plot_index( plot_id )->replot();

}

void MainWindow::open_file(const QString &file_name, int plot_id)
{
    if( file_name.isEmpty() )
    {
        return;
    }

    QHash< QString, QPolygonF > t_new_signals( m_file_parser.open_diadem_signal_file(file_name) );
    QHash< QString, QPolygonF >::iterator i_new_signals = t_new_signals.begin();
    QVector< int > t_new_signals_index;

    while(i_new_signals != t_new_signals.end() )
    {
        t_new_signals_index.append( m_signalDb->create_signal( i_new_signals.key(), i_new_signals.value() ) );
        i_new_signals++;
    }

    for( int l_var0 = 0; l_var0 < t_new_signals.size(); l_var0++ )
    {
        m_plotter_manager.add_signal_at_plot( t_new_signals_index.at( l_var0 ), plot_id );
    }
    m_plotter_manager.get_plot_given_plot_index( plot_id )->replot();
}

void MainWindow::open_files(QStringList files_names)
{
    if( files_names.isEmpty() )
    {
        return;
    }
    for ( int l_var0 = files_names.size() - 1; l_var0 > 0; l_var0-- )
    {
        open_file( files_names.at( l_var0 ) );
    }
}

void MainWindow::update_table_widget_signal(int signal_id)
{
    if( signal_id < 0 )
    {
        return;
    }

    QTableWidget *t_visible_table = get_visible_tableWidget();

    for( int l_var0 = 0; l_var0 < t_visible_table->rowCount(); l_var0++ )
    {
        if( t_visible_table->item( l_var0, 0 )->text() == QString::number( signal_id ) )
        {
            QPen t_pen( m_signalDb->get_signal_pen( signal_id ) );
            t_visible_table->item( l_var0, 0 )->setBackgroundColor( t_pen.color() );
        }
    }
}

void MainWindow::on_actionopen_plot_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open signal"), m_open_file_path, tr("Signal Files (*.csv *.txt *.dat)"));
    m_open_file_path = fileName;
    open_file( fileName );
}

void MainWindow::on_actionreset_zoom_triggered()
{
    if( !ui->tabWidget->currentWidget()->children().isEmpty() )     // If there´s any QCustomPlot
    {                                                               // to paint at, then paint.
        int plot_id;
        plot_id = ui->tabWidget->currentWidget()->children().first()->objectName().toInt();
        QCustomPlot *plot = m_plotter_manager.get_plot_given_plot_index( plot_id );
        plot->rescaleAxes();
        plot->replot();
    }
}

void MainWindow::select_signal_at_table(int signal_id)
{
    if( signal_id < 0 )
    {
        return;
    }

    for( int l_var0 = 0; l_var0 < get_visible_tableWidget()->rowCount(); l_var0++ )
    {
        if( signal_id == get_visible_tableWidget()->item( l_var0, 0 )->text().toInt() )
        {
            get_visible_tableWidget()->item( l_var0, 0 )->setSelected( true );
            get_visible_tableWidget()->item( l_var0, 1 )->setSelected( true );
        }
    }
}

void MainWindow::graphClicked(QCPAbstractPlottable *plottable, int dataIndex)
{
    if( plottable != nullptr )
    {
        QCPGraph *graph = dynamic_cast< QCPGraph* >(plottable);

        //select_signal_at_table( graph->name().toInt() );
    }
}


void MainWindow::set_label_at_plot( QMouseEvent *event )
{
    if( get_visible_plots().isEmpty() )
        return;

    if( ui->actionset_label->isChecked()) m_plotter_manager.add_label_at_plot( get_visible_plots().first(), event );
}

void MainWindow::remove_label_at_plot(QCPAbstractItem *p_item)
{
    if( ui->actionremove_label->isChecked()) m_plotter_manager.remove_label_at_plot( get_visible_plots().first(), p_item );
}

void MainWindow::remove_signal_from_table(int signal_id)
{
    if( signal_id < 0 )
    {
        return;
    }
    for( int l_var0 = 0; l_var0 < get_visible_tableWidget()->rowCount(); l_var0++ )
    {
        if( signal_id == get_visible_tableWidget()->item( l_var0, 0 )->text().toInt() )
        {
            get_visible_tableWidget()->removeRow( l_var0 );
        }
    }

}

void MainWindow::remove_signals_from_table( QList<int> signals_id )
{
    if( signals_id.isEmpty() )
    {
        return;
    }
    QElapsedTimer timer;
    timer.start();
    int t_index;
    int t_table_index;

    START:
    for( int l_var0 = 0; l_var0 < get_visible_tableWidget()->rowCount(); l_var0++ )
    {
        t_table_index = get_visible_tableWidget()->item( l_var0, 0 )->text().toInt();

        for( int l_var1 = 0; l_var1 < signals_id.size(); l_var1++ )
        {
            if( signals_id.at(l_var1) == t_table_index )
            {
                t_index = l_var1;
                get_visible_tableWidget()->removeRow(l_var0);
                signals_id.removeAt( t_index );
                goto START;
            }
        }
    }

    qDebug() << "1 por 1 ha tardado: " << timer.nsecsElapsed() << " ns";

}

void MainWindow::update_mouse_cursors(QMouseEvent *event)
{
    if( get_visible_plots().isEmpty() )
        return;

    m_plotter_manager.update_mouse_cursors( event, get_visible_plots().first() );
}

void MainWindow::update_mouse_coords(QMouseEvent *event)
{
    if( get_visible_plots().isEmpty() )
        return;

    m_plotter_manager.update_mouse_coords( event, get_visible_plots().first() );
}

QList< int > MainWindow::get_visible_plots()
{
    QList< int > visible_plots_indexes;
    if( ui->tabWidget->count() < 1 )
    {
        return visible_plots_indexes;
    }
    QList< QObject* > possible_plots ( ui->tabWidget->currentWidget()->children() );
    QCustomPlot *plot( nullptr );
    for( int l_var0 = 0; l_var0 < possible_plots.size(); l_var0++)
    {
        plot = dynamic_cast<QCustomPlot*>( possible_plots.at( l_var0 ) );
        if( plot != nullptr )
        {
            visible_plots_indexes.append( plot->objectName().toInt() );
        }
    }
    return visible_plots_indexes;
}

void MainWindow::on_actionnew_plot_triggered()
{
    create_new_tab_plot(QString::number(ui->tabWidget->count()));
}

QList<int> MainWindow::get_selected_signals_at_table()
{
    QList< int >  signals_id;
    QList< QTableWidgetItem* > selected_items( get_visible_tableWidget()->selectedItems() );
    QTableWidgetItem *item(nullptr);

    for( int l_var0 = 0; l_var0 < selected_items.size(); l_var0++)
    {
        item = selected_items.at( l_var0 );
        if( item->column() == 0 )
        {
            signals_id.append( item->text().toInt() );
        }
    }
    return signals_id;
}

QTableWidget *MainWindow::get_visible_tableWidget()
{
    QTableWidget *ret_visible_table( nullptr );
    if( ui->tabWidget->count() < 1 )
    {
        return ret_visible_table;
    }
    QList< QObject* > possible_table ( ui->tabWidget->currentWidget()->children() );

    for( int l_var0 = 0; l_var0 < possible_table.size(); l_var0++)
    {
        ret_visible_table = dynamic_cast<QTableWidget*>( possible_table.at( l_var0 ) );
        if( ret_visible_table != nullptr )
        {
            return ret_visible_table;
        }
    }
    return ret_visible_table;
}

void MainWindow::add_signal_at_table(int signal_id)
{
    if( signal_id < 0 )
    {
        return;
    }
    QTableWidgetItem *new_item_signal_id = new QTableWidgetItem( QString::number( signal_id ));
    QTableWidgetItem *new_item_signal_name = new QTableWidgetItem(m_signalDb->get_signal_name( signal_id ));
    new_item_signal_id->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
    new_item_signal_name->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled );
    get_visible_tableWidget()->insertRow( get_visible_tableWidget()->rowCount() );
    get_visible_tableWidget()->setItem( get_visible_tableWidget()->rowCount() - 1, 0, new_item_signal_id);
    get_visible_tableWidget()->setItem( get_visible_tableWidget()->rowCount() - 1, 1, new_item_signal_name);

    QCustomPlot *plot = m_plotter_manager.get_plot_given_signal_index( signal_id );
    QCPGraph *graph(nullptr);
    QPen t_pen( m_signalDb->get_signal_pen( signal_id ) );

    get_visible_tableWidget()->item( get_visible_tableWidget()->rowCount() -1, 0 )->setBackgroundColor( t_pen.color() );

}


void MainWindow::tableWidget_itemDoubleClicked(int signal_id)
{
    int plot_id ( 0 );
    QList<int> possible_plots ( get_visible_plots() );

    if( !possible_plots.isEmpty() )
    {
        if( possible_plots.first() >= 0 && !m_plotter_manager.is_signal_at_plot( signal_id, possible_plots.first() ) )
        {
            plot_id = possible_plots.first();
            QCustomPlot *plot( m_plotter_manager.get_plot_given_plot_index( plot_id ) );
            m_plotter_manager.add_signal_at_plot( signal_id, plot_id );
            plot->rescaleAxes();
            plot->replot();
        }
    }
}

void MainWindow::set_signal_as_x_axis_values(int signal_id)
{
    int plot_id ( 0 );
    QList<int> possible_plots ( get_visible_plots() );

    if( !possible_plots.isEmpty() )
    {
        if( possible_plots.first() >= 0 && !m_plotter_manager.is_signal_at_plot( signal_id, possible_plots.first() ) )
        {
            plot_id = possible_plots.first();
            QCustomPlot *plot( m_plotter_manager.get_plot_given_plot_index( plot_id ) );

            // List of current shown signals at plot
            QList< int > t_showed_signals_at_plot = m_plotter_manager.get_signal_indexes_from_plot( plot_id );
            QVector<double> t_signal_container;

            t_signal_container = m_signalDb->get_signal_y_values( signal_id ); // X values are what assigned new values

            QListIterator<int> t_signals_iterartor ( t_showed_signals_at_plot );

            while( t_signals_iterartor.hasNext() )
            {
                int t_signal_id = t_signals_iterartor.next();
                m_signalDb->set_signal_x_values(t_signal_id, t_signal_container );
                m_plotter_manager.update_data_signal_at_plot( t_signal_id, m_signalDb->get_signal_data( t_signal_id ) );
            }
            plot->xAxis->setLabel(m_signalDb->get_signal_name( signal_id) );
            plot->rescaleAxes();
            plot->replot();
        }
    }
}

void MainWindow::on_actionFile_format_triggered()
{
    m_file_formatter->show();
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->removeTab(index);
}

void MainWindow::on_actionzoom_rect_toggled(bool arg1)
{
    m_plotter_manager.update_common_plot_properties( rect_zoom, arg1 );
}

void MainWindow::tableWidget_itemDoubleClicked(QTableWidgetItem *t_item)
{
    QFlags<Qt::MouseButton> t_mouse_button = QApplication::mouseButtons();
    QTableWidget *t_table_widget( get_visible_tableWidget() );
    if( t_table_widget == nullptr )
    {
        return;
    }
    int signal_id = 0;
    if( t_item->column() == 1 )
    {
        signal_id =  t_table_widget->item( t_item->row(), 0 )->text().toInt();
        if( t_mouse_button == Qt::LeftButton )
        {
            tableWidget_itemDoubleClicked( signal_id );
        }else if ( t_mouse_button == Qt::RightButton )
        {
            set_signal_as_x_axis_values( signal_id );
        }

    }
}

void MainWindow::on_actionnormalize_triggered()
{
    if ( !get_visible_plots().isEmpty() )
    {
        QList< int > t_indexes(m_plotter_manager.get_signal_indexes_from_plot( get_visible_plots().first() ) );
        for( int l_var0 = 0; l_var0 < t_indexes.size(); l_var0++ )
        {
            m_signalDb->set_signal_y_values( t_indexes.at(l_var0), m_plotter_manager.get_data_for_customPlot_normalized( t_indexes.at(l_var0) ).second );
            //m_plotter_manager.update_data_signal_at_plot( t_indexes.at(l_var0),  ) );
            m_plotter_manager.update_data_signal_at_plot( t_indexes.at(l_var0), m_signalDb->get_signal_data( t_indexes.at(l_var0) ) );
        }
        if( !t_indexes.isEmpty() )
        {
            m_plotter_manager.get_plot_given_signal_index( t_indexes.first() )->rescaleAxes();
            m_plotter_manager.get_plot_given_signal_index( t_indexes.first() )->replot();
        }
    }
}

void MainWindow::on_actionmouse_cursors_at_signal_toggled(bool arg1)
{
    m_plotter_manager.update_common_plot_properties( PlotProperties::mouse_cursors, arg1  );
}


void MainWindow::on_actionauto_signal_analysis_toggled(bool arg1)
{
    if( arg1 )
    {
        connect( &m_plotter_manager, SIGNAL( update_signal_analysis_results(float, QPolygonF, float) ), this, SLOT( update_signal_anylsis_parameters(float, QPolygonF, float)));
    }
    else
    {
        disconnect( &m_plotter_manager, SIGNAL( update_signal_analysis_results(float, QPolygonF, float) ), this, SLOT( update_signal_anylsis_parameters(float, QPolygonF, float)));
    }
    ui->tool_signal_analysis->setVisible( arg1 );
    ui->tool_signal_analysis->setEnabled( arg1 );
}

void MainWindow::update_signal_anylsis_parameters(float signal_average, QPolygonF spectrum, float std_deviation)
{
    ui->signal_average_label_value->setText( QString::number( signal_average ) );
    ui->signal_std_label_value->setText( QString::number( std_deviation ) );
    if( ui->spectrum_plot->findChild<QCustomPlot*>("spectrum_plot", Qt::FindChildrenRecursively)!= nullptr )
    {
        QCustomPlot *t_plot = ui->spectrum_plot->findChild<QCustomPlot*>("spectrum_plot");
        t_plot->clearGraphs();
        int t_size = spectrum.size();
        QVector<double> x(t_size), y(t_size);

        for( int l_var0 = 0; l_var0 < t_size; l_var0++ )
        {
            x[l_var0] = spectrum.at(l_var0).x();
            y[l_var0] = spectrum.at(l_var0).y();
        }

        QPair<QVector<double>,QVector<double> > t_graph;
        t_graph.first = x;
        t_graph.second = y;
        QCPGraph *new_graph = t_plot->addGraph();
        new_graph->addData( t_graph.first, t_graph.second);
        new_graph->setName( "spectrum signal" );
        t_plot->rescaleAxes();
        t_plot->replot();
    }
}

void MainWindow::on_actionsave_plot_triggered()
{
    if( get_visible_plots().isEmpty() )
    {
        return;
    }

    QString t_fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               m_open_file_path,
                               tr("Images (*.png *.pdf *.jpg *.bmp )"));

    QCustomPlot *plot = m_plotter_manager.get_plot_given_plot_index( get_visible_plots().first() );

    QString t_file_type = t_fileName.split('.').last();

    if( t_file_type == "pdf")
    {
        plot->savePdf( t_fileName, 0, 0, QCP::epAllowCosmetic ,ui->tabWidget->tabText( ui->tabWidget->currentIndex() ) );
    }
    else if( t_file_type == "jpg" )
    {
        plot->saveJpg( t_fileName );
    }
    else if( t_file_type == "bmp" )
    {
        plot->saveBmp( t_fileName );
    }
    else if( t_file_type == "png" )
    {
        plot->savePng( t_fileName );
    }
}

bool MainWindow::event(QEvent *event)
{

    if( event->type() == QEvent::DragEnter )
    {
        QDragEnterEvent *drag_event( dynamic_cast<QDragEnterEvent*>(event));
        drag_event->setAccepted(true);
    }
    else if( event->type() == QEvent::Drop )
    {
        QDropEvent *drop_event( dynamic_cast<QDropEvent*>( event ) );
        const QMimeData *t_data = drop_event->mimeData();
        if( t_data->hasUrls() )
        {
            QListIterator<QUrl> t_iterator(t_data->urls());
            while( t_iterator.hasNext() )
            {
                open_file( t_iterator.next().toLocalFile() );
            }
        }
    }
    else if ( event->type() == QEvent::KeyPress && dynamic_cast<QKeyEvent*>(event)->key() == Qt::Key_Delete )
    {
        // The mechanism has a priority over the signals selected in the table. This means
        // that the program will get the indexes out from the table widget and build
        // a table based on that. It could be different and the indexes could be got from the graph.
        QVector< int > signals_indexes( m_plotter_manager.get_selected_signal_ids_for_given_plot_id( get_visible_plots().first() ) );
        if( !signals_indexes.isEmpty() )
        {
            for( int l_var0 = 0; l_var0 < signals_indexes.size(); l_var0++ )
            {
                m_plotter_manager.remove_graph_from_plot( signals_indexes.at( l_var0 ) );
            }
            m_plotter_manager.get_plot_given_plot_index( get_visible_plots().first() )->replot();
        }

    }
    else if ( event->type() == QEvent::KeyPress && dynamic_cast<QKeyEvent*>(event)->key() == Qt::Key_F5 )
    {
        on_actionreset_zoom_triggered();
    }
    else if ( event->type() == QEvent::Resize )
    {
        if ( !get_visible_plots().isEmpty() )
        {
            m_plotter_manager.replot_all();
        }
    }

    return QWidget::event(event);

}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    switch(event->type())
    {

        case QMouseEvent::MouseMove:
        {
            QMouseEvent *t_mouse_event( dynamic_cast<QMouseEvent*>(event) );
            update_mouse_coords( t_mouse_event );
            if( ui->actionmouse_cursors_at_signal->isChecked() )
            {
                update_mouse_cursors( t_mouse_event );
            }
        }
        break;

        case QEvent::DragEnter:
        {
            if( watched == ui->op1_edit || watched == ui->op2_edit )
            {
                QDragEnterEvent *drag_event( dynamic_cast<QDragEnterEvent*>(event));
                drag_event->setAccepted(true);
                qDebug() << "Acepto la señal";
            }
        }
        break;

        case QEvent::Drop:
        {
            if( watched == ui->op1_edit || watched == ui->op2_edit )
            {
                auto t_list = get_selected_signals_at_table();
                if( !t_list.isEmpty() )
                {
                    dynamic_cast< QLineEdit* >( watched )->setText( m_signalDb->get_signal_name( t_list.first() ) );
                }
            }
        }
        break;

        case QEvent::MouseButtonDblClick:
        {
            qDebug() << "Dblclk";
            qDebug() << watched->objectName();
            QTableWidget *t_table_widget( get_visible_tableWidget() );
            if( watched == t_table_widget )
            {
                QMouseEvent *t_mouse_event = dynamic_cast<QMouseEvent*>( event );
                QPoint t_point = t_table_widget->mapFromGlobal( t_mouse_event->pos() );
                QTableWidgetItem *t_item = t_table_widget->itemAt( t_point );
                int signal_id;
                qDebug() << t_item->column();
                if( t_item->column() == 1 )
                {
                    signal_id =  t_table_widget->item( t_item->row(), 0 )->text().toInt();
                }
                if( t_mouse_event->buttons() == Qt::LeftButton )
                {
                    tableWidget_itemDoubleClicked( signal_id );
                }
                else if( t_mouse_event->buttons() == Qt::RightButton )
                {
                    set_signal_as_x_axis_values( signal_id );
                }
            }
        }
        break;
        default:
        break;
    }

    return false;
}

void MainWindow::on_action_signal_calculator_toggled(bool arg1)
{
    ui->signalCalculator->setEnabled( arg1 );
    ui->signalCalculator->setVisible( arg1 );
}

void MainWindow::on_actionone_graph_per_plot_triggered()
{

}

void MainWindow::on_show_result_operation_clicked()
{
    QPolygonF t_new_signal;

    if( ui->operation_symbol->currentText() == 'X' )
    {
        t_new_signal = m_operations_manager.perform_mutiplication( m_signalDb->get_opened_signals().key(ui->op1_edit->text()),
                                                                           m_signalDb->get_opened_signals().key(ui->op2_edit->text()));
    }
    else if( ui->operation_symbol->currentText() == '/' )
    {
        t_new_signal = m_operations_manager.perform_division( m_signalDb->get_opened_signals().key(ui->op1_edit->text()),
                                                                           m_signalDb->get_opened_signals().key(ui->op2_edit->text()));
    }
    else if( ui->operation_symbol->currentText() == '+' )
    {
        t_new_signal = m_operations_manager.perform_addition( m_signalDb->get_opened_signals().key(ui->op1_edit->text()),
                                                                           m_signalDb->get_opened_signals().key(ui->op2_edit->text()));
    }
    else if( ui->operation_symbol->currentText() == '-' )
    {
        t_new_signal = m_operations_manager.perform_subtraction( m_signalDb->get_opened_signals().key(ui->op1_edit->text()),
                                                                           m_signalDb->get_opened_signals().key(ui->op2_edit->text()));
    }

    if( t_new_signal.length() != 0 )
    {
        int t_new_index = m_signalDb->create_signal(ui->op1_edit->text()+ui->operation_symbol->currentText()+ui->op2_edit->text(), t_new_signal );
        add_signal_at_table( t_new_index );
        tableWidget_itemDoubleClicked( t_new_index );
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "SeeSig, a simple program to see signals data",    "SeeSig was programmed by Alvaro Guzman\n"
                                                                                "You can find code and conctact at\n"
                                                                                "https://github.com/AlfTeleco");
}

void MainWindow::on_actionset_label_triggered(bool checked)
{
    if( checked ) ui->actionremove_label->setChecked( false );
}

void MainWindow::on_actionremove_label_triggered(bool checked)
{
    if( checked ) ui->actionset_label->setChecked( false );
}

void MainWindow::change_axis_label(QCPAxis *p_axis)
{
    if( m_plotter_manager.set_axis_label( p_axis ) ) m_plotter_manager.get_plot_given_plot_index( get_visible_plots().first() )->replot();
}

void MainWindow::graphDoubleClicked(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *p_mouse_event)
{
    QCPGraph *t_graph = dynamic_cast< QCPGraph* >( plottable );
    m_plotter_manager.graphDoubleClicked(plottable, dataIndex, p_mouse_event);
    if( t_graph != nullptr )
    {
        update_table_widget_signal( t_graph->name().toInt() );
    }
}
