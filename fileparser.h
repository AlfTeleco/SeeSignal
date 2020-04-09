#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <QObject>
#include <QPolygonF>
#include <QHash>
#include <QDebug>

#include "qtcsv/stringdata.h"
#include "qtcsv/reader.h"

class FileParser : public QObject
{
    Q_OBJECT
public:
    explicit FileParser(QObject *parent = nullptr);

    QHash<QString, QPolygonF> open_signal_file(const QString &path);
    QHash<QString, QPolygonF> open_diadem_signal_file(const QString &path);
    QHash<QString, QPolygonF> open_csv_signal_file(const QString &path, const QString &separator );

private:



signals:

};

#endif // FILEPARSER_H
