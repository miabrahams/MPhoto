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

#include <QFileDialog>
#include <QColorDialog>
#include <stdio.h>

#include "Config.h"

#include "ConfigDialog.h"
#include "ui_ConfigDialog.h"

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
	ui->setupUi(this);

	_addSlideSpeedComboItem( tr("No transition"), 10000 );
	_addSlideSpeedComboItem( tr("Slow"), 30 );
	_addSlideSpeedComboItem( tr("Normal"), 50 );
	_addSlideSpeedComboItem( tr("Fast"), 100 );
	_addSlideSpeedComboItem( tr("Very fast"), 200 );

	_addUISizeComboItem( tr("Auto"), 0 );
	_addUISizeComboItem( tr("Normal"), 48 );
	_addUISizeComboItem( tr("Big"), 64 );
	_addUISizeComboItem( tr("Huge"), 128 );
		
	// "slide speed" and "ui size" combo
	QComboBox * combo = ui->combo_slide_speed;
	for ( int i = 0; i < _slide_speed_value.size(); i++ )
	{
		ComboSlideSpeedValue v = _slide_speed_value.at(i);
		combo->addItem( v.name );
	}
	combo = ui->combo_ui_size;
	for ( int i = 0; i < _ui_size_value.size(); i++ )
	{
		UISizeValue v = _ui_size_value.at(i);
		combo->addItem( v.name );
	}
	
	// file name combo
	combo = ui->combo_file_name_position;
	combo->addItem( tr("top-left") );
	combo->addItem( tr("top-center") );
	combo->addItem( tr("top-right") );
	combo->addItem( tr("bottom-left") );
	combo->addItem( tr("bottom-center") );
	combo->addItem( tr("bottom-right") );

	_setValues();
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::on_b_browse_clicked()
{
	QString dirname = QFileDialog::getExistingDirectory( this, tr("Open Directory"), g_config.last_open_dir );
	if ( !dirname.isNull() )
		ui->le_start_path->setText( dirname );
}

void ConfigDialog::on_radioButton_3_toggled(bool checked)
{
	ui->b_browse->setEnabled( checked );
	ui->le_start_path->setEnabled( checked );
}

void ConfigDialog::on_radioButton_2_toggled(bool checked)
{
	ui->b_browse->setEnabled( !checked );
	ui->le_start_path->setEnabled( !checked );
}

void ConfigDialog::on_radioButton_toggled(bool checked)
{
	ui->b_browse->setEnabled( !checked );
	ui->le_start_path->setEnabled( !checked );
}

void ConfigDialog::on_b_background_clicked()
{
	QColor c = QColorDialog::getColor( g_config.background_color, this );
	if ( c.isValid() )
	{
		QWidget * w = ui->lb_color_preview;
		QPalette pal = w->palette();
		pal.setColor(w->backgroundRole(), c );
		w->setPalette(pal);
	}
}

void ConfigDialog::on_b_text_clicked()
{
	QColor c = QColorDialog::getColor( g_config.text_color, this );
	if ( c.isValid() )
	{
		QWidget * w = ui->lb_color_preview;
		QPalette pal = w->palette();
		pal.setColor(w->foregroundRole(), c );
		w->setPalette(pal);
	}
}

void ConfigDialog::on_cb_show_name_toggled(bool checked)
{
	QComboBox * combo_fn = ui->combo_file_name_position;
	combo_fn->setEnabled( checked );
}

void ConfigDialog::on_b_defaults_clicked()
{
	g_config.loadDefaults();
	_setValues();   
}

void ConfigDialog::accept()
{
	// gather parameters and apply
	if ( ui->radioButton_2->isChecked() ) {
		g_config.start_with_custom_folder = false;
		g_config.start_with_last_folder = true;
	} else if ( ui->radioButton_3->isChecked() ) {
		g_config.start_with_custom_folder = true;
		g_config.start_with_last_folder = false;
		g_config.start_dir = ui->le_start_path->text();
	} else {
		g_config.start_with_custom_folder = false;
		g_config.start_with_last_folder = false;
	}

	g_config.show_file_name = ui->cb_show_name->isChecked();
	g_config.file_name_size = (float)ui->spin_file_name_size->value();
	g_config.always_show_scroll = ui->cb_show_scroll->isChecked();
	g_config.show_folders = ui->cb_showfolders->isChecked();
	g_config.show_extra_buttons = ui->cb_extra_buttons->isChecked();
	g_config.smooth_images = ui->cb_smooth_images->isChecked();
	g_config.disable_animations = ui->cb_disable_animations->isChecked();	
	g_config.allow_rotation = ui->cb_allow_rotation->isChecked();
	
	g_config.thumbnails_crop = ui->cb_thumbnail_crop->isChecked();
	g_config.thumbnails_square = ui->cb_thumbnail_square->isChecked();
	g_config.thumbnails_space = ui->cb_thumbnail_spacing->isChecked();
	g_config.thumbnails_text_under_image = ui->cb_thumbnail_text_under_image->isChecked();
	g_config.thumbnails_show_name = ui->cb_thumbnails_show_name->isChecked();

	QWidget * w = ui->lb_color_preview;
	QPalette pal = w->palette();
	g_config.background_color = pal.color( w->backgroundRole() );
	g_config.text_color = pal.color( w->foregroundRole() );

	QComboBox * combo = ui->combo_slide_speed;
	int combo_index = combo->currentIndex();
	ComboSlideSpeedValue v = _slide_speed_value.at(combo_index);
	g_config.image_scroll_speed = v.value;

	combo = ui->combo_ui_size;
	combo_index = combo->currentIndex();
	UISizeValue vui = _ui_size_value.at(combo_index);
	g_config.choose_ui_size = vui.value;

	QComboBox * combo_fn = ui->combo_file_name_position;
	g_config.file_name_position = combo_fn->currentIndex();

	QDialog::accept();
}

void ConfigDialog::_setValues( void )
{
	ui->le_start_path->setText( g_config.start_dir );
	if ( g_config.start_with_last_folder )
		ui->radioButton_2->setChecked( true );
	else if ( g_config.start_with_custom_folder )
		ui->radioButton_3->setChecked( true );
	else
		ui->radioButton->setChecked( true );
		
	ui->cb_show_name->setChecked( g_config.show_file_name );
	ui->spin_file_name_size->setValue( g_config.file_name_size );
	ui->cb_show_scroll->setChecked( g_config.always_show_scroll );
	ui->cb_showfolders->setChecked( g_config.show_folders );
	ui->cb_extra_buttons->setChecked( g_config.show_extra_buttons );
	ui->cb_smooth_images->setChecked( g_config.smooth_images );	
	ui->cb_allow_rotation->setChecked( g_config.allow_rotation );
	ui->cb_disable_animations->setChecked( g_config.disable_animations );
	
	ui->cb_thumbnail_crop->setChecked( g_config.thumbnails_crop );
	ui->cb_thumbnail_square->setChecked( g_config.thumbnails_square );
	ui->cb_thumbnail_spacing->setChecked( g_config.thumbnails_space );
	ui->cb_thumbnail_text_under_image->setChecked( g_config.thumbnails_text_under_image );
	ui->cb_thumbnails_show_name->setChecked( g_config.thumbnails_show_name );

	QWidget * w = ui->lb_color_preview;
	QPalette pal = w->palette();
	pal.setColor(w->backgroundRole(), g_config.background_color );
	pal.setColor(w->foregroundRole(), g_config.text_color );
	w->setAutoFillBackground(true);
	w->setPalette(pal);
	
	// slide speed combo
	QComboBox * combo = ui->combo_slide_speed;
	int current_index = 0;
	for ( int i = 0; i < _slide_speed_value.size(); i++ )
	{
		ComboSlideSpeedValue v = _slide_speed_value.at(i);
		if ( v.value == g_config.image_scroll_speed )
		{
			current_index = i;
			break;
		}
	}
	combo->setCurrentIndex( current_index );

	// ui size combo
	combo = ui->combo_ui_size;
	current_index = 0;
	for ( int i = 0; i < _ui_size_value.size(); i++ )
	{
		UISizeValue v = _ui_size_value.at(i);
		if ( v.value == g_config.choose_ui_size )
		{
			current_index = i;
			break;
		}
	}
	combo->setCurrentIndex( current_index );
	
	combo = ui->combo_file_name_position;
	current_index = g_config.file_name_position;
	if ( current_index < 0 ) current_index = 0;
	if ( current_index >= 6 ) current_index = 0;
	combo->setCurrentIndex( current_index );
	combo->setEnabled( g_config.show_file_name );
}

void ConfigDialog::_addSlideSpeedComboItem( QString name, int value )
{
	ComboSlideSpeedValue v;
	v.name = name;
	v.value = value;
	_slide_speed_value.append( v );
}

void ConfigDialog::_addUISizeComboItem( QString name, int value )
{
	UISizeValue v;
	v.name = name;
	v.value = value;
	_ui_size_value.append( v );
}
