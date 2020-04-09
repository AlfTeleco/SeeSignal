#include "fileparser.h"

FileParser::FileParser(QObject *parent) : QObject(parent)
{

}


QHash<QString, QPolygonF> FileParser::open_signal_file(const QString &path)
{

    /*Create a QpolygonF for each signal and fulfill it*/
    /*Add the QPolygonF to the member m_signals_db creating at the same time a new signal_id*/
    QHash<QString, QPolygonF> t_signals;


    return t_signals;

}

QHash<QString, QPolygonF> FileParser::open_diadem_signal_file(const QString &path)
{
    QVector<int> t_indexes;
    QList<QStringList> read_data = QtCSV::Reader::readToList(path, ";");
    QStringList t_titles = read_data.first();
    QStringList *t_row_values;
    read_data.removeFirst();
    int column_size = read_data.first().size();
    int row_size = read_data.size();
    QHash< QString, QPolygonF > t_new_signals;
    t_new_signals.reserve( read_data.size() + 1);
    QVector< QPolygonF > t_data(column_size);

    if( row_size > 0 && column_size > 0 )
    {
        for ( int l_var0 = 0; l_var0 < row_size; l_var0++ )
        {
            t_row_values = &read_data[ l_var0 ];
            for( int l_var1 = 0; l_var1 < column_size; l_var1++ )
            {
                t_data[l_var1] << QPointF( l_var0 , t_row_values->at( l_var1 ).toFloat() );
            }
        }
    }

    for( int l_var0 = 0; l_var0 < t_titles.size(); l_var0++ )
    {
        t_new_signals.insert( t_titles.at(l_var0), t_data.at(l_var0) );
    }

    return t_new_signals;
}

QHash<QString, QPolygonF> FileParser::open_csv_signal_file(const QString &path, const QString &separator)
{
    QVector<int> t_indexes;
    QList<QStringList> read_data = QtCSV::Reader::readToList(path, separator);
    QStringList t_titles = read_data.first();
    QStringList *t_row_values;
    read_data.removeFirst();
    int column_size = read_data.first().size();
    int row_size = read_data.size();
    QHash< QString, QPolygonF > t_new_signals;
    t_new_signals.reserve( read_data.size() + 1);
    QVector< QPolygonF > t_data(column_size);

    if( row_size > 0 && column_size > 0 )
    {
        for ( int l_var0 = 0; l_var0 < row_size; l_var0++ )
        {
            t_row_values = &read_data[ l_var0 ];
            for( int l_var1 = 0; l_var1 < column_size; l_var1++ )
            {
                t_data[l_var1] << QPointF( l_var0 , t_row_values->at( l_var1 ).toFloat() );
            }
        }
    }

    for( int l_var0 = 0; l_var0 < t_titles.size(); l_var0++ )
    {
        t_new_signals.insert( t_titles.at(l_var0), t_data.at(l_var0) );
    }

    return t_new_signals;
}
