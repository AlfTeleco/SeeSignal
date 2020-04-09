#ifndef SIGNALITEM_H
#define SIGNALITEM_H

#include <QObject>
#include <QPolygonF>
#include <QString>
#include <QPen>

class SignalItem : public QObject
{
    Q_OBJECT
public:
    explicit SignalItem(QObject *parent = nullptr, int signal_index=0, const QString &signal_name="", const QPolygonF &signal_data=QPolygonF() );
    explicit SignalItem(QObject *parent = nullptr, int signal_index=0, const QString &signal_name="", const QPolygonF &signal_data=QPolygonF() , const QPen &signal_pen=QPen());
    SignalItem( const SignalItem &signal );

    ~SignalItem();
    inline void        set_signal_name( QString name ){ m_signal_name = name; }
    inline void        set_signal_index( int index ){ m_signal_index = index; }
    inline void        set_signal_pen( QPen pen ){ m_signal_pen = pen; }
    inline void        set_signal_data( QPolygonF data ){ m_signal_data = data; }

    inline QString     get_signal_name() const { return m_signal_name; }
    inline int         get_signal_index() const { return m_signal_index; }
    inline QPen        get_signal_pen()   const{ return m_signal_pen; }
    inline QPolygonF   get_signal_data()  const  { return m_signal_data; }



public slots:

private:

    int         m_signal_index;
    QString     m_signal_name;
    QPen        m_signal_pen;
    QPolygonF   m_signal_data;

signals:

};

#endif // SIGNALITEM_H
