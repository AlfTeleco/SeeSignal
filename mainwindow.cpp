#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    create_new_tab_plot(QString::number(ui->tabWidget->count()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::create_same_tab_plot()
{
    int new_index( -1 );
    QCustomPlot *plot = new QCustomPlot(ui->tabWidget->currentWidget());
    plot->addLayer("labels");
    new_index = m_plotter_manager.add_plot(plot);
    plot->installEventFilter( this );
    QVBoxLayout *vertical_layout = new QVBoxLayout;
    vertical_layout->addWidget(plot);
    ui->tabWidget->currentWidget()->setLayout( vertical_layout );
    plot->setObjectName( QString::number( new_index ) );
    plot->setInteractions(QCP::iSelectPlottables | QCP::iRangeDrag | QCP::iRangeZoom | QCP::iMultiSelect );
    plot->xAxis->setLabel("X");
    plot->yAxis->setLabel("Y");
    connect(plot, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));
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

    QHash< QString, QPolygonF > t_new_signals( m_file_parser.open_csv_signal_file(file_name, m_csv_separator) );
    QHash< QString, QPolygonF >::iterator i_new_signals = t_new_signals.begin();
    QVector< int > t_new_signals_index;
    int plot_id(0);

    while(i_new_signals != t_new_signals.end() )
    {
        t_new_signals_index.append( m_signalDb->create_signal( i_new_signals.key(), i_new_signals.value() ) );
        i_new_signals++;
    }

    if( ui->tabWidget->count() < 1 )
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
}

void MainWindow::on_actionopen_plot_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open signal"), "F:/Trabajo/CAF/DDP_SW_AT/PROYECTOS/BUGS/NEWCASTLE_Fogueado", tr("Signal Files (*.csv *.txt *.dat)"));
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

    for( int l_var0 = 0; l_var0 < ui->tableWidget->rowCount(); l_var0++ )
    {
        if( signal_id == ui->tableWidget->item( l_var0, 0 )->text().toInt() )
        {
            ui->tableWidget->item( l_var0, 0 )->setSelected( true );
            ui->tableWidget->item( l_var0, 1 )->setSelected( true );
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

void MainWindow::remove_signal_from_table(int signal_id)
{
    if( signal_id < 0 )
    {
        return;
    }
    for( int l_var0 = 0; l_var0 < ui->tableWidget->rowCount(); l_var0++ )
    {
        if( signal_id == ui->tableWidget->item( l_var0, 0 )->text().toInt() )
        {
            ui->tableWidget->removeRow( l_var0 );
        }
    }

    //Delete from QCustomPlot
   // QCustomPlot *plot = m_plotter_manager.get_plot_given_signal_index( signal_id );

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
    for( int l_var0 = 0; l_var0 < ui->tableWidget->rowCount(); l_var0++ )
    {
        t_table_index = ui->tableWidget->item( l_var0, 0 )->text().toInt();

        for( int l_var1 = 0; l_var1 < signals_id.size(); l_var1++ )
        {
            if( signals_id.at(l_var1) == t_table_index )
            {
                t_index = l_var1;
                ui->tableWidget->removeRow(l_var0);
                signals_id.removeAt( t_index );
                goto START;
            }
        }
    }

    qDebug() << "1 por 1 ha tardado: " << timer.nsecsElapsed() << " ns";

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
    QList< QTableWidgetItem* > selected_items( ui->tableWidget->selectedItems() );
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

void MainWindow::on_tableWidget_itemSelectionChanged()
{
    QList< int >  signals_id;
    QList< QTableWidgetItem* > selected_items( ui->tableWidget->selectedItems() );
    QTableWidgetItem *item(nullptr);

    for( int l_var0 = 0; l_var0 < selected_items.size(); l_var0++)
    {
        item = selected_items.at( l_var0 );
        if( item->column() == 0 )
        {
            signals_id.append( item->text().toInt() );
        }
    }

    m_plotter_manager.set_selected_graphs( signals_id, true );

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
    new_item_signal_name->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
    ui->tableWidget->insertRow( ui->tableWidget->rowCount() );
    ui->tableWidget->setItem( ui->tableWidget->rowCount() - 1, 0, new_item_signal_id);
    ui->tableWidget->setItem( ui->tableWidget->rowCount() - 1, 1, new_item_signal_name);

    QCustomPlot *plot = m_plotter_manager.get_plot_given_signal_index( signal_id );
    QCPGraph *graph(nullptr);
    QPen t_pen( m_signalDb->get_signal_pen( signal_id ) );

    ui->tableWidget->item( ui->tableWidget->rowCount() -1, 0 )->setBackgroundColor( t_pen.color() );

}


void MainWindow::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    int signal_id;
    if( item->column() == 1 )
    {
        signal_id =  ui->tableWidget->item( item->row(), 0 )->text().toInt();
    }
    else
    {
        signal_id = item->text().toInt();
    }

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

void MainWindow::on_actionnormalize_toggled(bool arg1)
{
    if ( arg1 )
    {
        QList< int > t_indexes(m_plotter_manager.get_signal_indexes_from_plot( get_visible_plots().first() ) );
        for( int l_var0 = 0; l_var0 < t_indexes.size(); l_var0++ )
        {
            m_plotter_manager.update_data_signal_at_plot( t_indexes.at(l_var0), m_plotter_manager.get_data_for_customPlot_normalized( t_indexes.at(l_var0) ) );
        }
        m_plotter_manager.get_plot_given_signal_index( t_indexes.first() )->replot();

    }
}

void MainWindow::on_actionFile_format_triggered()
{
    FileFormatterDialog *fileFormatter = new FileFormatterDialog();
    fileFormatter->show();
}


bool MainWindow::event(QEvent *event)
{

    if( event->type() == QEvent::DragEnter )
    {
        QDragEnterEvent *drag_event( dynamic_cast<QDragEnterEvent*>(event));
        drag_event->setAccepted(true);
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
            m_plotter_manager.get_plot_given_signal_index( signals_indexes.first() )->replot();
        }

    }
    else if( event->type() == QEvent::MouseButtonPress &&  ui->actionband_zoom->isChecked() )
    {

    }
    return QWidget::event(event);

}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if( event->type() == QEvent::Drop )
    {
        QDropEvent *drop_event( dynamic_cast<QDropEvent*>( event ) );
        qDebug() << drop_event->source()->objectName();
    }
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->removeTab(index);
}
