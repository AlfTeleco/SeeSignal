#include "fileformatterdialog.h"
#include "ui_fileformatterdialog.h"
#include "mainwindow.h"

FileFormatterDialog::FileFormatterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileFormatterDialog)
{
    ui->setupUi(this);
}

FileFormatterDialog::~FileFormatterDialog()
{
    delete ui;
}


void FileFormatterDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    QPushButton *pushed_button = dynamic_cast<QPushButton*>( button );
    if( button->text() == "Save")
    {
        m_csv_separator = ui->csv_separator_lineedit->text();
        m_max_columns = ui->max_columns_lineedit->text().toInt();
    }
}
