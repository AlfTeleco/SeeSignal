#ifndef PLOTTERMANAGER_H
#define PLOTTERMANAGER_H

#include <QObject>
#include <QMultiHash>
#include <math.h>
#include "signaldb.h"
#include "qcustomplot.h"

/* Layers at plots:
 * MouseCursors
 * MouseCoordinates
 * SignalLabels
 * SignalNames
 *

*/

enum PlotProperties {   rect_zoom     = 0       // selection rect mode
                        ,normalized             // normalization on plot
                        ,mouse_cursors          // normalization on plot
                    };

class PlotterManager : public QObject
{
    Q_OBJECT
public:
    explicit        PlotterManager(QObject *parent = nullptr );

    int             get_plot_id_for_given_signal_id( int signal_id );
    QVector<int>    get_signal_ids_for_given_plot_id( int plot_id );
    QVector<int>    get_plots_id_for_given_signal_id( int signal_id );
    QVector<int>    get_selected_signal_ids_for_given_plot_id( int plot_id );

    bool            set_signal_at_specific_plot( int signal_id, int plot_id );
    bool            is_signal_at_plot( int signal_id, int plot_id );
    int             new_plot_index();
    int             add_plot(QCustomPlot *new_plot);

    /* Plot methods */
    QList<int>      get_signal_indexes_from_plot( int plot_id );
    bool            add_signal_at_plot( int signal_id, int plot_id );
    bool            remove_signal_from_plot( int signal_id, int plot_id );
    bool            remove_selected_signals_at_plot( int plot_id );
    QCustomPlot     *get_plot_given_plot_index( int plot_id );
    QCustomPlot     *get_plot_given_signal_index( int signal_id );
    void            set_selected_graphs( QList< int > signals_id, bool selected );
    QPair<QVector<double>, QVector<double> > get_data_for_customPlot_given_signal_id( int signal_id );
    QPair<QVector<double>, QVector<double> > get_data_for_customPlot_normalized(int signal_id );
    double          get_maximum_from_vector(const QVector<double> &values );
    bool            update_data_signal_at_plot( int signal_id, QPair<QVector<double>, QVector<double> > data);
    bool            update_data_signal_at_plot(int signal_id, const QPolygonF &new_signal_data);
    void            update_common_plot_properties( const PlotProperties &plot_property, bool enabled );
    void            replot_all();

private:

    /* For each signal will be an element at the multihash. As each signal can be drawn at several plots simultaneusly there can be several elements with the same signal_id.
     * Also, as there will be several signals drawn at the same plot, there will be several elements with the same plot_id and different signal_id.
     */
    //QHash< int, QWidget > m_plot_indexes;
    QMultiHash< int, QCustomPlot* >     m_plot_widget_2_plot_id;
    QMultiHash< int, int>               m_plot_id_2_signal_id;
    SignalDB                            *m_signal_db = SignalDB::instance();
    QList< int >                        m_plot_indexes;

    QStringList                         m_default_plot_layers =
                                        { "MouseCursors",
                                          "MouseCoordinates",
                                          "SignalLabels",
                                          "SignalNames",
                                          "Labels"
                                        };

    void                                initialize_plot( QCustomPlot *plot );
    void                                initialize_layers( QCustomPlot *plot );
    void                                initialize_mouse_cursors( QCustomPlot *plot );
    void                                initialize_mouse_coords( QCustomPlot *plot );
    void                                initialize_axis( QCustomPlot *plot );

private slots:

    /* Plot methods */
    void                                graphClicked(QCPAbstractPlottable *plottable, int dataIndex);    

public slots:

    bool                                add_label_at_plot(const int &p_plot_id, QMouseEvent *p_event );
    bool                                remove_label_at_plot(const int &p_plot_id , QCPAbstractItem *p_item);
    bool                                remove_graph_from_plot( int signal_id );
    bool                                remove_signal_from_dbs( int signal_id );
    void                                update_signal_names( int plot_id );
    void                                update_mouse_cursors(QMouseEvent *event, int plot_id);
    void                                update_mouse_coords(QMouseEvent *event, int plot_id);
    void                                cast_signal_analysis_results( int plot_id, int signal_id );

    bool                                set_axis_label( QCPAxis *p_axis );
    void                                graphDoubleClicked(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *p_mouse_event);


signals:

    void                                update_signal_analysis_results( float signal_average, float first_harmonic, float std_deviation );

};

#endif // PLOTTERMANAGER_H
