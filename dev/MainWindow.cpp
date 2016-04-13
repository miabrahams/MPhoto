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

#include <QString>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <stdio.h>

#include "MainWindow.h"
#include "ImageArea.h"
#include "Config.h"
#include "ConfigDialog.h"

MainWindow::MainWindow( QString startfile, bool fullscreen )
{
    setWindowTitle(tr("MihPhoto"));
    setWindowIcon(QIcon(g_config.install_dir+"/icon128.png"));
    resize(640, 480);
    _maximized = isMaximized();

    _image_area = new ImageArea;
    setCentralWidget(_image_area);
    _image_area->setFocusPolicy(Qt::StrongFocus);
    connect( _image_area, SIGNAL(closeOnTouch()), this, SLOT(close()) );
    connect( _image_area, SIGNAL(changeFullscreen()), this, SLOT(changeFullscreen()) );
    connect( _image_area, SIGNAL(loadFile()), this, SLOT(open()));
    connect( _image_area, SIGNAL(loadDir()), this, SLOT(openDir()));
    connect( _image_area, SIGNAL(config()), this, SLOT(config()));
    connect( _image_area, SIGNAL(hideCursor()), this, SLOT(hideCursor()));
    connect( _image_area, SIGNAL(showCursor()), this, SLOT(showCursor()));
    connect( _image_area, SIGNAL(dragFile(QString)), this, SLOT(loadDroppedFile(QString)) );

    _createActions();
    _createMenus();

    _cursor = this->cursor();
    if ( fullscreen )
    {
        menuBar()->hide();
        this->showFullScreen();
    } else {
        this->show();
        _maximized = isMaximized();
    }

    if ( !startfile.isEmpty() )
    {
        QFileInfo d(startfile);
        if ( !d.isDir() )
            g_config.started_with_file = true;
        _loadFiles( startfile );
    } else if ( g_config.start_with_last_folder ) {
        _loadFiles( g_config.last_open_dir );
    } else if ( g_config.start_with_custom_folder ) {
        _loadFiles( g_config.start_dir );
    }
}

void MainWindow::closeEvent( QCloseEvent * event )
{
    g_config.save();
    event->accept();
}

void MainWindow::keyPressEvent( QKeyEvent * event )
{
    switch ( event->key() )
    {
        // non-configurable keys
    case Qt::Key_Escape:
        if ( g_config.started_with_file )
            close();
        event->accept();
        break;
    case Qt::Key_Q:
        if ( event->modifiers() & Qt::AltModifier )
            close();
        event->accept();
        break;
    case Qt::Key_F:
        changeFullscreen();
        event->accept();
        break;
    default:
        _image_area->onKeyPress( event );
        break;
    }
}

void MainWindow::wheelEvent ( QWheelEvent * event )
{
    _image_area->onWheel( event );
}

void MainWindow::open()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), g_config.last_open_dir, tr("Image Files (*.png *.jpg *.jpeg *.bmp *.tif *.gif)"));
    if ( !filename.isNull() )
        _loadFiles( filename );
}

void MainWindow::openDir()
{
    QString dirname = QFileDialog::getExistingDirectory( this, tr("Open Directory"), g_config.last_open_dir );
    if ( !dirname.isNull() )
        _loadFiles( dirname );
}

void MainWindow::about()
{
    QString license = "<p>This program is free software: you can redistribute it and/or modify"
                      "it under the terms of the GNU General Public License as published by"
                      "the Free Software Foundation, either version 3 of the License, or"
                      "(at your option) any later version."
                      "<br /><br />"
                      "This program is distributed in the hope that it will be useful,"
                      "but WITHOUT ANY WARRANTY; without even the implied warranty of"
                      "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
                      "GNU General Public License for more details."
                      "<br /><br />"
                      "You should have received a copy of the GNU General Public License"
                      "along with this program. If not, see http://www.gnu.org/licenses/</p>";

    QString name =
        "<p><b>MihPhoto</b> - Browse photos in a folder using the touchscreen.<br />"
        "Copyright (C) 2010-2013 Mihai Paslariu<br />"
        "Copyright (C) 2015 Michael Abrahams</p>"
        "<p>See the CREDITS file in the root directory for the complete list of persons that contributed to this software.</p>";

    QString version;
    version.sprintf("<p>Version %.2lf</p>", VERSION );

    QMessageBox::about(this, tr("About MihPhoto"), name + version + license );
}

void MainWindow::changeFullscreen()
{
    if ( this->isFullScreen() )
    {
        if ( _maximized )
            this->showMaximized();
        else
            this->showNormal();
        menuBar()->show();
    } else {
        _maximized = this->isMaximized();
        this->showFullScreen();
        menuBar()->hide();
    }
}

void MainWindow::showOrHideName()
{
    g_config.show_file_name = !g_config.show_file_name;
    _image_area->update();
}

void MainWindow::config()
{
    if ( g_config.disable_settings_dialog )
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("At this time, the settings are being controlled from the touch interface.") );
        return;
    }

    bool old_show_folders = g_config.show_folders;

    ConfigDialog diag(this);
    if ( diag.exec() == QDialog::Accepted )
    {
        g_config.save();
        if ( old_show_folders != g_config.show_folders )
        {
            _image_area->reloadFiles();
            _image_area->update();
        }
    }
}

void MainWindow::hideCursor()
{
    this->setCursor( QCursor( Qt::BlankCursor ) );
}

void MainWindow::showCursor()
{
    this->setCursor( _cursor );
}

void MainWindow::loadDroppedFile( QString name )
{
    // is it a directory
    QDir img_dir(name);
    if ( img_dir.exists() )
    {
        printf("Load dragged directory %s\n", name.toUtf8().data() );
        _loadFiles( name );
        return;
    }

    // is it a file
    QFile img_file(name);
    if ( img_file.exists() )
    {
        QString lower_name = name.toLower();
        if ( lower_name.endsWith(".jpg") || lower_name.endsWith(".jpeg")
             || lower_name.endsWith(".gif") || lower_name.endsWith(".png")
             || lower_name.endsWith(".bmp") )
        {
            printf("Load dragged file %s\n", name.toUtf8().data() );
            _loadFiles( name );
        }
        return;
    }
}

void MainWindow::_createActions()
{
    _open_act = new QAction(tr("&Open ..."), this);
    _open_act->setShortcut(tr("Ctrl+O"));
    connect(_open_act, SIGNAL(triggered()), this, SLOT(open()));

    _fullscreen_act = new QAction(tr("&Fullscreen"), this);
    _fullscreen_act->setShortcut(tr("Ctrl+F"));
    connect(_fullscreen_act, SIGNAL(triggered()), this, SLOT(changeFullscreen()));

    _show_name_act = new QAction(tr("Show name"), this);
    _show_name_act->setCheckable(true);
    _show_name_act->setChecked( g_config.show_file_name );
    connect(_show_name_act, SIGNAL(triggered()), this, SLOT(showOrHideName()));

    _config_act = new QAction(tr("Settings ..."), this);
    connect(_config_act, SIGNAL(triggered()), this, SLOT(config()));

    _exit_act = new QAction(tr("E&xit"), this);
    _exit_act->setShortcut(tr("Ctrl+Q"));
    connect(_exit_act, SIGNAL(triggered()), this, SLOT(close()));

    _about_act = new QAction(tr("&About"), this);
    connect(_about_act, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::_createMenus()
{
    _file_menu = new QMenu(tr("&File"), this);
    _file_menu->addAction(_open_act);
    _file_menu->addSeparator();
    _file_menu->addAction(_exit_act);

    _options_menu = new QMenu(tr("&Options"), this);
    _options_menu->addAction(_fullscreen_act);
    _options_menu->addAction(_show_name_act);
    _options_menu->addAction(_config_act);

    _help_menu = new QMenu(tr("&Help"), this);
    _help_menu->addAction(_about_act);

    menuBar()->addMenu(_file_menu);
    menuBar()->addMenu(_options_menu);
    menuBar()->addMenu(_help_menu);
}

void MainWindow::_loadFiles( QString name )
{
    QString file_name = "";
    QString dir_name = QDir::currentPath();

    QFileInfo d(name);
    name = d.absoluteFilePath();
    if ( d.isDir() )
    {
        dir_name = name;
    } else {
        int i = name.lastIndexOf("/");
        if ( i > 0 )
        {
            dir_name = name.mid(0,i);
            file_name = name.mid(i+1);
        } else {
            file_name = name;
        }
    }

    printf("Open File %s\n", file_name.toUtf8().data());
    printf("Open Dir %s\n", dir_name.toUtf8().data());
    _loadFiles( dir_name, file_name );
}

void MainWindow::_loadFiles( QString dir, QString current_file )
{
    _image_area->setFiles( dir, current_file );
}
