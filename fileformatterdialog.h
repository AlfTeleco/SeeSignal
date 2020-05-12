#ifndef FILEFORMATTERDIALOG_H
#define FILEFORMATTERDIALOG_H

#include <QDialog>
#include <QPushButton>

namespace Ui {
class FileFormatterDialog;
}

class FileFormatterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileFormatterDialog(QWidget *parent = nullptr);
    ~FileFormatterDialog();

    inline QString          get_csv_separator()                 const { return m_csv_separator; }
    inline int              get_max_columns()                   const { return m_max_columns; }
    inline int              get_time_values_are_x_values()      const { return m_time_values_are_x_values; }

private slots:

    void                    on_buttonBox_clicked(QAbstractButton *button);

    void                    on_time_values_x_values_stateChanged(int arg1);

private:
    Ui::FileFormatterDialog *ui;
    QString             m_csv_separator = ";";
    int                 m_max_columns= 9999;
    int                 m_time_values_are_x_values = 1;
};

#endif // FILEFORMATTERDIALOG_H
