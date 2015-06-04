/*******************************************************************************

MPhoto - Photo viewer for multi-touch devices
Copyright (C) 2010 Mihai Paslariu
Copyright (C) 2015 Michael Abrahams

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QVector>

namespace Ui {
    class ConfigDialog;
}

struct ComboSlideSpeedValue
{
	QString name;
	int value;
};

struct UISizeValue
{
	QString name;
	int value;
};


class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = 0);
    ~ConfigDialog();

private:
    Ui::ConfigDialog *ui;
    QVector<ComboSlideSpeedValue> _slide_speed_value;
    QVector<UISizeValue> _ui_size_value;

private slots:
    void on_b_defaults_clicked();
    void on_cb_show_name_toggled(bool checked);
    void accept();
    void on_b_text_clicked();
    void on_b_background_clicked();
    void on_radioButton_toggled(bool checked);
    void on_radioButton_2_toggled(bool checked);
    void on_radioButton_3_toggled(bool checked);
    void on_b_browse_clicked();
    
private:

	void _setValues( void );
	void _addSlideSpeedComboItem( QString name, int value );
	void _addUISizeComboItem( QString name, int value );
    
};

#endif // CONFIGDIALOG_H
