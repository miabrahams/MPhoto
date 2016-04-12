/*******************************************************************************

MihPhoto - Photo viewer for multi-touch devices
Copyright (C) 2010 Mihai Paslariu

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

#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QList>
#include <QMainWindow>

class ImageArea;

/**
 *  Main entry point for the program.
 */


class MainWindow : public QMainWindow
{
Q_OBJECT

private:

	ImageArea * _image_area;
	bool _maximized;

	QMenu * _file_menu;
	QMenu * _options_menu;
	QMenu * _help_menu;

	QAction * _open_act;
	QAction * _fullscreen_act;
	QAction * _show_name_act;
	QAction * _config_act;
	QAction * _exit_act;
	QAction * _about_act;

	QCursor _cursor;

public:

	MainWindow( QString startfile, bool fullscreen );

protected:

	void closeEvent( QCloseEvent * event );
	void keyPressEvent( QKeyEvent * event );
	void wheelEvent( QWheelEvent * event );

private slots:

	void open();
	void openDir();
	void about();
	void changeFullscreen();
	void showOrHideName();
    void config(); // Show popup config dialog from menu
	void hideCursor();
	void showCursor();
	void loadDroppedFile( QString name );

private:

	void _createActions();
	void _createMenus();
	void _loadFiles( QString name );
	void _loadFiles( QString dir, QString current_file );
};

#endif
