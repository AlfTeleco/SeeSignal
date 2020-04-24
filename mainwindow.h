#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "qcustomplot.h"
#include "signaldb.h"
#include "fileparser.h"
#include "plottermanager.h"
#include "fileformatterdialog.h"


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

    /* Table methods */
    void                select_signal_at_table( int signal_id );
    void                on_actionnew_plot_triggered();
    void                on_tableWidget_itemSelectionChanged();
    void                add_signal_at_table( int signal_id );
    void                remove_signal_from_table( int signal_id );
    void                remove_signals_from_table(QList<int> signals_id);

    void                on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);

    void                on_actionnormalize_toggled(bool arg1);

    void                on_actionFile_format_triggered();

    void                on_tabWidget_tabCloseRequested(int index);

private:
    Ui::MainWindow      *ui;
    SignalDB            *m_signalDb = SignalDB::instance();
    FileParser          m_file_parser;
    PlotterManager      m_plotter_manager;
    FileFormatterDialog *m_file_formatter;

    bool                event( QEvent *event);
    bool                eventFilter(QObject *watched, QEvent *event);

    QList<int>          get_visible_plots();

    /* Table methods */
    QList< int >        get_selected_signals_at_table();

};
#endif // MAINWINDOW_H
