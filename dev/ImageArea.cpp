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

#include <QtGui>
#include <stdio.h>

#include "Config.h"
#include "ImageArea.h"
#include "ScreenSettings.h"

/*******************************************************************************
 * CONSTRUCTOR / DESTRUCTOR
 *******************************************************************************/

ImageArea::ImageArea(QWidget *parent)
    : QWidget(parent)
    , _front_viewer(nullptr)
    , _dir_view(false)
    , _hide_cursor_timer(0)
    , _enlarge_timer(this)
    , _reduce_timer(this)
    , _fit_timer(this)
    , _enlarge_parameter(0.01)
    , _start_zoom(1.0)
    , _start_pos(0,0)
    , _thumb_zoom(0.05)
{

    setAttribute(Qt::WA_StaticContents);

    _connectSignals( &_image_viewer );
    _image_viewer.setSize( width(), height() );

    _connectSignals( &_dir_viewer );
    _dir_viewer.setSize( width(), height() );

    connect( &_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    _timer.start( g_config.timer_duration );
    if ( g_config.multitouch )
    {
        setAttribute(Qt::WA_AcceptTouchEvents);
        setMouseTracking(true);
    } else {
        setMouseTracking(true);
    }
    setAcceptDrops(true);

    connect( &_enlarge_timer, SIGNAL(timeout()), this, SLOT(enlargeImage()) );
    connect( &_reduce_timer, SIGNAL(timeout()),this, SLOT(reduceImage()) );
    connect( &_fit_timer, SIGNAL(timeout()), this, SLOT(fitImage()));

    connect( &_image_viewer, SIGNAL(fitImage()), this, SLOT(onFitImage()));
}

ImageArea::~ImageArea()
{
    if ( _front_viewer )
        delete _front_viewer;
    _cleanOldViewers();
}

/*******************************************************************************
 * PUBLIC METHODS AND SLOTS
 *******************************************************************************/

void ImageArea::passCloseOnTouch( void )
{
    emit closeOnTouch();
}

void ImageArea::passChangeFullScreen( void )
{
    emit changeFullscreen();
}

void ImageArea::passLoadFile( void )
{
    emit loadFile();
}

void ImageArea::passLoadDir( void )
{
    emit loadDir();
}

void ImageArea::passConfig( void )
{
    //emit config();
    if ( _front_viewer ) return;
    _front_viewer = new ScreenSettings();
    _connectSignals( _front_viewer );
    if ( _front_viewer ) _front_viewer->setSize( width(), height() );
    update();
}

void ImageArea::setFiles( QString dir_name, QString current )
{
    if ( current == "" && !_dir_view )
        _dir_view = true;
    ScreenBase * w = getCurrentViewer();
    if ( w ) w->loadFiles( dir_name, current );
}

void ImageArea::reloadFiles( void )
{
    if ( _dir_view )
        _dir_viewer.reloadFiles();
}

void ImageArea::updateSize( int w, int h )
{
    _dir_viewer.setSize( w,h );
    _image_viewer.setSize( w,h );

    if (_front_viewer) _front_viewer->setSize( w,h );

    ScreenBase * v = getCurrentViewer();
    if ( v ) v->onResize();
}

/*******************************************************************************
 * EVENTS
 *******************************************************************************/

void ImageArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if( !_enlarge_timer.isActive() && !_reduce_timer.isActive() )
    {
        if ( g_config.smooth_images )
        {
            painter.setRenderHint(QPainter::SmoothPixmapTransform);
            painter.setRenderHint(QPainter::Antialiasing);
        }
    }

    const QRect rect = event->rect();
    painter.fillRect( rect, g_config.background_color );
    double fitZoom = _image_viewer.computeCurrentFitZoom();
    double zoom = _image_viewer.getZoom();
    double zoomRatio = zoom/fitZoom;
    if(!_front_viewer && !_dir_view && zoomRatio < 0.70 &&
       (_enlarge_timer.isActive() || _reduce_timer.isActive() || _image_viewer.isBeingPinchZoomed() )  )
    {
        double transparency = (zoomRatio-_thumb_zoom/fitZoom)/(0.70-_thumb_zoom/fitZoom);
        QTransform transform;
        double zoom = 1.0+0.2*transparency;
        transform.scale(zoom,zoom);
        transform.translate(this->width()*(1.0-zoom)/2.0,this->height()*(1.0-zoom)/2.0);
        painter.setTransform(transform);
        _dir_viewer.onPaint( painter );
        painter.resetTransform();
        QColor c = g_config.background_color;
        if(transparency > 0.01)
        {
            c.setAlphaF(transparency);
            painter.fillRect( rect, c );
            //painter.setOpacity(transparency);
            _image_viewer.onPaint( painter );
        }
    } else {
        ScreenBase * w = getCurrentViewer();
        if ( w ) w->onPaint( painter );
    }
}

void ImageArea::resizeEvent(QResizeEvent *event)
{
    updateSize( width(), height() );
    QWidget::resizeEvent(event);
}

bool ImageArea::event( QEvent * event )
{
    if ( event && event->type() == QEvent::MouseMove )
    {
        emit showCursor();
        _hide_cursor_timer = 5 * 1000;
    }

    // Try to let the current UI handle the event
    ScreenBase * w = getCurrentViewer();
    bool ret = ( w != NULL ) && w->onEvent( event );

    if ( !ret ) return QWidget::event(event);
    return true;
}

void ImageArea::onKeyPress( QKeyEvent * event )
{
    ScreenBase * w = getCurrentViewer();
    if ( w ) w->onKeyPress( event );
}

void ImageArea::onWheel( QWheelEvent * event )
{
    ScreenBase * w = getCurrentViewer();
    if ( w ) w->onWheel( event );
}

void ImageArea::dropEvent( QDropEvent * de )
{
    //printf("Contents: %s\n", de->mimeData()->text().toLatin1().data());

    QString text = de->mimeData()->text();
    if ( !text.startsWith("file://") ) return;
    text = text.mid(7);

    emit dragFile( text );
}

void ImageArea::dragMoveEvent( QDragMoveEvent * de )
{
    de->accept();
}

void ImageArea::dragEnterEvent( QDragEnterEvent * event )
{
    event->acceptProposedAction();
}

void ImageArea::onTimer( void )
{
    ScreenBase * w = getCurrentViewer();
    if ( w ) w->onTimer();
    if ( _hide_cursor_timer > 0 )
    {
        _hide_cursor_timer -= g_config.timer_duration;
        if ( _hide_cursor_timer <= 0 )
        {
            _hide_cursor_timer = 0;
            emit hideCursor();
        }
    }

    _cleanOldViewers();
}

void ImageArea::onStartTimer( void )
{
    _timer.start( g_config.timer_duration );
}

void ImageArea::onChangeMode( void )
{
    if( _enlarge_timer.isActive() ) return;

    if ( _front_viewer )
    {
        _old_viewers.append(_front_viewer);
        _front_viewer = NULL;
        update();
        _image_viewer.onSettingsChanged();
        _dir_viewer.onSettingsChanged();
        return;
    }

    // start animation
    if ( _dir_view )
    {
        _image_viewer.changeFromOtherViewer( &_dir_viewer );
        _dir_view = false;
        if ( g_config.disable_animations )
        {
            _enlarge_parameter = 1.0;
            enlargeImage();
        } else {
            _thumb_pos = _dir_viewer.currentItemPosition();
            _image_viewer.setZoom(_thumb_zoom);
            _image_viewer.setView(_thumb_pos);
            _enlarge_timer.start(20);
        }
    } else {
        if ( g_config.disable_animations )
        {
            _enlarge_parameter = 0.0;
            reduceImage();
        } else {
            _enlarge_parameter = 1.0;
            _thumb_pos = _dir_viewer.currentItemPosition();
            _start_pos = _image_viewer.getView();
            _start_zoom = _image_viewer.getZoom();
            _start_angle = _image_viewer.getRotation();
            _reduce_timer.start(15);
        }
    }
}

void ImageArea::onFitImage( void )
{
    _enlarge_parameter = 0.013;
    _start_pos = _image_viewer.getView();
    _start_zoom = _image_viewer.getZoom();
    _start_angle = _image_viewer.getRotation();
    _fit_timer.start(15);
}

/*******************************************************************************
 * PRIVATE METHODS
 *******************************************************************************/

void ImageArea::_connectSignals( ScreenBase * w )
{
    if ( w == NULL ) return;
    connect( w, SIGNAL(updateSignal()), this, SLOT(update()) );
    connect( w, SIGNAL(closeOnTouch()), this, SLOT(passCloseOnTouch()) );
    connect( w, SIGNAL(changeFullscreen()), this, SLOT(passChangeFullScreen()) );
    connect( w, SIGNAL(loadFile()), this, SLOT(passLoadFile()) );
    connect( w, SIGNAL(loadDir()), this, SLOT(passLoadDir()) );
    connect( w, SIGNAL(config()), this, SLOT(passConfig()) );
    connect( w, SIGNAL(startTimer()), this, SLOT(onStartTimer()) );
    connect( w, SIGNAL(changeViewer()), this, SLOT(onChangeMode()) );
    connect( w, SIGNAL(indexChanged(int)), this, SLOT(indexChanged(int)) );
}

void ImageArea::indexChanged( int index )
{
    _dir_viewer.changeIndex( index );
}

void ImageArea::enlargeImage( void )
{
    if ( _image_viewer.isReady() )
    {
        _enlarge_parameter = _enlarge_parameter + 0.14;
        double fitZoom = _image_viewer.computeCurrentFitZoom();
        _image_viewer.setZoom( fitZoom*_enlarge_parameter+_thumb_zoom*(1.0-_enlarge_parameter) );
        _image_viewer.setView( _thumb_pos*(1.0-_enlarge_parameter) );
        if( _enlarge_parameter > 1.0 )
        {
            _image_viewer.setView( QPoint(0,0) );
            _enlarge_parameter = 0.0;
            _enlarge_timer.stop();
            _image_viewer.setZoom( fitZoom );
        }
        update();
    }
}
void ImageArea::reduceImage( void )
{
    _enlarge_parameter = _enlarge_parameter - 0.07;
    _image_viewer.setZoom( _start_zoom*_enlarge_parameter + _thumb_zoom*(1.0-_enlarge_parameter) );
    _image_viewer.setView( _start_pos*_enlarge_parameter + _thumb_pos*(1.0-_enlarge_parameter) );
    double _enlarge_parameter2 = 1.0 -( _enlarge_parameter-1.0 ) * ( _enlarge_parameter-1.0 );
    if ( _start_angle < 180.0 )
    {
        _image_viewer.setRotation(_start_angle*_enlarge_parameter2 );
    } else {
        _image_viewer.setRotation(360.0+(_start_angle-360.0)*_enlarge_parameter2 );
    }
    if ( _enlarge_parameter < 0.01 )
    {
        _enlarge_parameter = 0.01;
        _reduce_timer.stop();
        _image_viewer.resetView();
        _dir_viewer.changeFromOtherViewer( &_image_viewer );
        _dir_view = true;
    }
    update();
}

void ImageArea::fitImage( void )
{
    _enlarge_parameter = _enlarge_parameter + 0.07;
    double fitZoom = _image_viewer.computeCurrentFitZoom();
    _image_viewer.setZoom( fitZoom*_enlarge_parameter + _start_zoom*(1.0-_enlarge_parameter) );
    _image_viewer.setView( _start_pos*(1.0-_enlarge_parameter) );

    if ( _enlarge_parameter > 1.0 )
    {
        _enlarge_parameter = 1.0;
        _fit_timer.stop();
        _image_viewer.setZoom( fitZoom );
        _image_viewer.setView( QPoint(0,0) );
    }
    update();
}

void ImageArea::_cleanOldViewers( void )
{
    while ( !_old_viewers.empty() )
    {
        ScreenBase * vptr = _old_viewers.first();
        _old_viewers.pop_front();
        delete vptr;
    }
}
