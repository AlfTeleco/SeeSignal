#ifndef SIGNALDB_H
#define SIGNALDB_H

#include <QObject>
#include <QPolygonF>
#include <QHash>
#include <QDebug>

#include "qcustomplot.h"
#include "signalitem.h"
#include "qtcsv/stringdata.h"
#include "qtcsv/reader.h"

class SignalDB : public QObject
{
    Q_OBJECT
public:
    explicit SignalDB(QObject *parent = nullptr);

    static SignalDB* instance();

    /*Returns a list containning the signals_id of that archive
     *
     */
    int                     create_signal(const QPolygonF &new_signal_data);
    int                     create_signal(const QString &new_signal_name, const QPolygonF &new_signal_data );
    SignalItem             *get_signal( int signal_id ) const;
    bool                    update_signal( int signal_id, const QPolygonF &new_data );
    bool                    close_signal( int signal_id );
    QString                 get_signal_name( int signal_id ) const;
    QPolygonF               get_signal_data( int signal_id ) const;
    QPen                    get_signal_pen( int signal_id ) const;
    QHash< int, QString >   get_opened_signals() const;
    QColor                  create_new_colour();

    inline void             set_signal_pen( int signal_id, const QPen &pen ){ m_signals_db.value( signal_id )->set_signal_pen( pen ); }


private:

    static SignalDB             *m_signal_db_instance;
    QHash< int, SignalItem* >    m_signals_db;
    QStringList                  m_colours_list;


signals:
    void                    signal_removed( int signal_id );

};

#endif // SIGNALDB_H
