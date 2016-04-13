/*******************************************************************************

MPhoto - Photo viewer for multi-touch devices
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

#ifndef __IMAGE_AREA_H__
#define __IMAGE_AREA_H__

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include <QTouchEvent>

#include "ScreenViewer.h"
#include "ScreenDirectory.h"

class ImageArea : public QWidget
{
    Q_OBJECT

private:

    bool _dir_view;
    ScreenViewer _image_viewer;
    ScreenDirectory _dir_viewer;
    ScreenBase * _front_viewer;
    QList<ScreenBase*> _old_viewers;
    QTimer _timer;
    int _hide_cursor_timer;

    QTimer _enlarge_timer;
    QTimer _reduce_timer;
    QTimer _fit_timer;
    double _enlarge_parameter;
    double _start_zoom;
    double _start_angle;
    QPoint _start_pos;
    QPoint _thumb_pos;
    double _thumb_zoom;

public:

    ImageArea( QWidget *parent = 0 );
    ~ImageArea( void );

public:

    void setFiles( QString dir_name, QString current );
    void reloadFiles( void );
    void updateSize( int w, int h );

public slots:

    void passCloseOnTouch( void );
    void passChangeFullScreen( void );
    void passLoadFile( void );
    void passLoadDir( void );
    void passConfig( void );

protected:

    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    bool event(QEvent *event);
    void dropEvent( QDropEvent * de );
    void dragMoveEvent( QDragMoveEvent * de );
    void dragEnterEvent( QDragEnterEvent * event );

public:

    void onKeyPress( QKeyEvent * event );
    void onWheel( QWheelEvent * event );

protected slots:

    void onTimer( void );
    void onStartTimer( void );
    void onChangeMode( void );
    void indexChanged( int );
    void enlargeImage( void );
    void reduceImage( void );
    void fitImage( void );
    void onFitImage( void );

signals:

    void closeOnTouch( void );
    void changeFullscreen( void );
    void loadFile( void );
    void loadDir( void );
    void config( void );
    void hideCursor( void );
    void showCursor( void );
    void dragFile( QString name );

private:

    void _connectSignals( ScreenBase * w );
    void _cleanOldViewers( void );

public:

    inline ScreenBase * getCurrentViewer( void )
    {
        if ( _front_viewer )
            return _front_viewer;
        return _dir_view ? (ScreenBase*) &_dir_viewer : (ScreenBase*) &_image_viewer;
    }

};

#endif
