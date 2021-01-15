#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "qcustomplot.h"
#include "signaldb.h"
#include "fileparser.h"
#include "plottermanager.h"
#include "fileformatterdialog.h"
#include "operationsmanager.h"




QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int                 create_same_tab_plot();
    int                 create_new_tab_plot(const QString &tab_name);
    void                set_csv_separator( const QString &separator );

public slots:

    void                open_file( const QString &file_name );
    void                open_file( const QString &file_name, int plot_id );
    void                open_files( QStringList files_names );

private slots:
    void                on_actionopen_plot_triggered();
    void                on_actionreset_zoom_triggered();

    /* Plot methods */
    void                graphClicked(QCPAbstractPlottable *plottable, int dataIndex);
    void                set_label_at_plot( QMouseEvent *event );
    void                remove_label_at_plot( QCPAbstractItem *p_item );

    /* Table methods */
    void                select_signal_at_table( int signal_id );
    void                on_actionnew_plot_triggered();
    void                add_signal_at_table( int signal_id );
    void                remove_signal_from_table( int signal_id );
    void                remove_signals_from_table(QList<int> signals_id);
    void                update_mouse_cursors( QMouseEvent *event );
    void                update_mouse_coords( QMouseEvent *event );

    void                on_actionFile_format_triggered();

    void                on_tabWidget_tabCloseRequested(int index);

    void                on_actionzoom_rect_toggled(bool arg1);

    void                tableWidget_itemDoubleClicked(QTableWidgetItem *item);

    void                tableWidget_itemDoubleClicked(int signal_id);

    void                set_signal_as_x_axis_values( int signal_id );

    void                on_actionnormalize_triggered();

    void                on_actionmouse_cursors_at_signal_toggled(bool arg1);

    void                on_actionauto_signal_analysis_toggled(bool arg1);

    void                update_signal_anylsis_parameters( float signal_average, float first_harmonic, float std_deviation );

    void                on_actionsave_plot_triggered();

    void 				on_actionone_graph_per_plot_triggered();

    void 				on_actionAbout_triggered();

    void                on_action_signal_calculator_toggled(bool arg1);

    void                on_show_result_operation_clicked();

    void                on_actionset_label_triggered(bool checked);

    void                on_actionremove_label_triggered(bool checked);

private:
    Ui::MainWindow      *ui;
    SignalDB            *m_signalDb = SignalDB::instance();
    FileParser          m_file_parser;
    PlotterManager      m_plotter_manager;
    FileFormatterDialog *m_file_formatter;
    OperationsManager   m_operations_manager;

    QString             m_open_file_path;

    bool                event( QEvent *event);
    bool                eventFilter(QObject *watched, QEvent *event);


    QList<int>          get_visible_plots();

    /* Table methods */
    QList< int >        get_selected_signals_at_table();
    QTableWidget        *get_visible_tableWidget();

    void                initialize_file_formatter_dialog();

};
#endif // MAINWINDOW_H
