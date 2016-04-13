/*******************************************************************************

MihPhoto - Photo viewer for multi-touch devices
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

#include <QProcess>
#include <QDebug>
#include <stdio.h>

#include "Config.h"
#include "ScreenViewer.h"
#include "Trashcan.h"

ScreenViewer::ScreenViewer()
    : ScreenBase()
{

    m_current_index = 0;
    _resetUserActionsParameters();

    _loadUI();

    _zoom_in_icon.load( g_config.install_dir + "/icons/zoom-in.svg" );
    _zoom_out_icon.load( g_config.install_dir + "/icons/zoom-out.svg" );
    _zoom_indicator.load( g_config.install_dir + "/icons/position.svg" );

    _last_load_thread_idle_state = true;

    _load_thread.start();
    _show_ui = false;
    _show_ui_by_tap = false;
}

ScreenViewer::~ScreenViewer()
{
    _load_thread.stopThread();
    _load_thread.wait();
}

/*******************************************************************************
 * PUBLIC METHODS AND SLOTS
 *******************************************************************************/

void ScreenViewer::forward( void )
{
    if ( !_changing && m_current_index < m_files.size() - 1 && _load_thread.isIdle() )
    {
        _allow_drag = false;
        resetFitZoom( _current );
        resetFitZoom( _next );
        _current.recenter();
        emit startTimer();

        _drag_offset = width();
        _moveForward();
        _changing = true;
    }
}

void ScreenViewer::back( void )
{
    if ( !_changing && m_current_index > 0 && _load_thread.isIdle() )
    {
        _allow_drag = false;
        resetFitZoom( _current );
        resetFitZoom( _previous );
        _current.recenter();
        emit startTimer();

        _drag_offset = -width();
        _moveBack();
        _changing = true;
    }
}

void ScreenViewer::gotoIndex( int new_index )
{
    if ( !_changing && m_current_index != new_index && _load_thread.isIdle() )
    {
        _current.rotation = 0.0;
        _allow_drag = false;
        _drag_offset = 0;
        m_current_index = new_index;
        _reloadAll();
        emit indexChanged( new_index );
    }
}

void ScreenViewer::last( void )
{
    gotoIndex(m_files.count()-1);
}

void ScreenViewer::first( void )
{
    gotoIndex(0);
}

void ScreenViewer::changeUI( void )
{
    _show_ui = !_show_ui;
    update();
}

void ScreenViewer::zoomIn( double speed )
{
    if ( _current.isNull() || _changing )
        return;

    speed = ( speed > 0.0 ? speed : g_config.zoom_speed );
    _current.zoom /= speed;

    _limitZoom(_current.zoom, _current);
    _limitPan();

    update();
}

void ScreenViewer::zoomOut( double speed )
{
    if ( _current.isNull() || _changing )
        return;

    speed = ( speed > 0.0 ? speed : g_config.zoom_speed );
    _current.zoom *= speed;

    _limitZoom(_current.zoom, _current);
    _limitPan();

    update();
}

void ScreenViewer::zoomToFit( FitZoomMode zoom_mode )
{
    if ( _current.isNull() || _changing )
        return;

    _current.recenter();
    resetFitZoomWithRotation(_current, zoom_mode);
    update();
}

void ScreenViewer::zoomToOriginalSize( void )
{
    if ( _current.isNull() || _changing )
        return;

    _current.zoom = 1.0;
    _current.recenter();
    update();
}

void ScreenViewer::zoomToOriginalSize( int x, int y )
{
    if ( _current.isNull() || _changing )
        return;

    double old_zoom = _current.zoom;
    _current.zoom = 1.0;
    _current.rotation = 0.0;
    int dx = (int)( (double)( x - width() / 2 ) / old_zoom );
    int dy = (int)( (double)( y - height() / 2 ) / old_zoom );
    _current.posx = -dx;
    _current.posy = -dy;
    _limitPan();
    update();
}

void ScreenViewer::rotateLeft( void )
{
    if ( _current.isNull() || _changing )
        return;

    _current.rotateLeft(size());
    update();
}

void ScreenViewer::rotateRight( void )
{
    if ( _current.isNull() || _changing )
        return;

    _current.rotateRight(size());
    update();
}

void ScreenViewer::moveView( int deltax, int deltay )
{
    if ( _current.isNull() || _changing )
        return;

    _current.posx -= deltax;
    _current.posy -= deltay;
    _limitPan();
}

QPoint ScreenViewer::getView()
{
    return QPoint(_current.posx,_current.posy);
}

void ScreenViewer::setView( QPoint pos )
{
    _current.posx = pos.x();
    _current.posy = pos.y();
}

void ScreenViewer::resetView()
{
    //_current.zoom = computeCurrentFitZoom();
    _current.recenter();
}

/*******************************************************************************
 * EVENTS
 *******************************************************************************/

void ScreenViewer::onSetFiles( QString dir_name, QStringList files, QString current )
{
    m_dir_name = dir_name;

    // are these the same files
    bool same_files = true;
    if ( files.size() == m_files.size() )
    {
        for ( int i = 0; i < files.size(); i++ )
            if ( files[i] != m_files[i] )
            {
                same_files = false;
                break;
            }
    } else {
        same_files = false;
    }

    // get current index
    int current_index = 0;
    for ( int i = 0; i < files.size(); i++ )
    {
        if ( current == files[i] )
        {
            current_index = i;
            break;
        }
    }

    // load files
    if ( same_files )
    {
        if ( m_current_index == current_index )
        {
            // nothing
        } else {
            m_current_index = current_index;
            if ( current_index == m_current_index - 1 ) {
                _moveBack();
            } else if ( current_index == m_current_index + 1 ) {
                _moveForward();
            } else {
                _reloadAll();
            }
        }
    } else {
        m_current_index = current_index;
        m_files = files;
        _reloadAll();
    }

    // reset user actions
    _show_ui = false;
    _resetUserActionsParameters();
}

void ScreenViewer::onPaint( QPainter & painter )
{
    int cx = width() / 2;
    int cy = height() / 2;

    QFont font = painter.font();
    int point_size = TouchUI::scaleUI(font.pointSize());
    font.setPointSize( point_size > 0 ? point_size : 1 );
    painter.setFont( font );

    /*if ( _current.image )
      {
      int sw = (int)( (double)_current.width() * _current.zoom );
      int sh = (int)( (double)_current.height() * _current.zoom );
      int x = cx - sw/2 + _current.posx + _drag_offset;
      int y = cy - sh/2 + _current.posy;
      QRect r(x,y,sw,sh);
      painter.drawImage( r, *_current );
      }*/

    if ( !(_current.isNull()) )
    {
        if ( _current.zoom == 0.0 )
            resetFitZoom( _current );
        QTransform tr = _getCurrentTransform();

        bool alternative_smooth = false;
#ifdef Q_OS_LINUX
        alternative_smooth = ( _current.rotation == 0.0
                               && _drag_offset == 0
                               && _current.zoom == fit_zoom
                               && g_config.smooth_images );
#endif
        if ( alternative_smooth )
        {
            QImage img2 = _current.image->transformed( tr, Qt::SmoothTransformation );

            qreal ox = (qreal)( (this->width() - img2.width()) / 2 ) + _current.posx + _drag_offset;
            qreal oy = (qreal)( (this->height() - img2.height()) / 2 ) + _current.posy;
            QPointF origin( ox, oy );
            painter.drawImage( origin, img2 );
        } else {
            painter.setTransform(tr);
            painter.drawImage( 0,0, *_current.image );
            painter.resetTransform();
        }

    } else {
        QString s1 = "", s2 = "";
        if ( m_files.empty() )
        {
            s1 = tr("No image.");
            s2 = tr("Click on the screen to bring the menu!");
        } else {
            if ( _load_thread.isIdle() )
            {
                s1 = tr("Cannot load image.");
                s2 = m_files[m_current_index];
            } else {
                s1 = tr("Loading.");
                s2 = tr("Please wait!");
            }
        }

        int w,h,x,y;
        QFontMetrics fm( painter.font() );
        painter.setPen( g_config.text_color );
        w = fm.width(s1);
        h = fm.height();
        x = ( width() - w ) / 2 + _drag_offset;
        y = height()/2 - h - 2;
        painter.drawText( x,y, s1 );
        w = fm.width(s2);
        x = ( width() - w ) / 2 + _drag_offset;
        y = height()/2 + 2;
        painter.drawText( x,y, s2 );

    }

    if ( !(_next.isNull()) && _drag_offset < 0 )
    {
        if ( _next.zoom == 0.0 )
            resetFitZoom( _next );
        int sw = (int)( (double)_next.width() * _next.zoom );
        int sh = (int)( (double)_next.height() * _next.zoom );
        int x = cx - sw/2 + _current.posx + _drag_offset + width();
        int y = cy - sh/2 + _current.posy;
        QRect r(x,y,sw,sh);
        painter.drawImage( r, *_next.image );
    }

    if ( !(_previous.isNull()) && _drag_offset > 0 )
    {
        if ( _previous.zoom == 0.0 )
            resetFitZoom( _previous );
        int sw = (int)( (double)_previous.width() * _previous.zoom );
        int sh = (int)( (double)_previous.height() * _previous.zoom );
        int x = cx - sw/2 + _current.posx + _drag_offset - width();
        int y = cy - sh/2 + _current.posy;
        QRect r(x,y,sw,sh);
        painter.drawImage( r, *_previous.image );
    }

    if ( g_config.show_file_name )
    {
        QFont font = painter.font();
        font.setPointSizeF( font.pointSizeF() * g_config.file_name_size );
        painter.setFont( font );

        QString s;
        if ( m_files.count() != 0 )
        {
            s.sprintf("[%d/%d] ", (m_current_index+1), m_files.count() );
            s += m_files[m_current_index];
        }

        int vert_p = g_config.file_name_position / 3;
        if ( vert_p < 0 ) vert_p = 0;
        if ( vert_p > 1 ) vert_p = 1;
        int horz_p = g_config.file_name_position % 3;
        if ( horz_p < 0 ) horz_p = 0;
        if ( horz_p > 2 ) horz_p = 2;
        int space_x = 10;
        int space_y = 5;

        QFontMetrics fm( font );
        int w = fm.width(s);
        int h = fm.height();
        int x = space_x;
        if ( horz_p == 1 ) x = ( width() - w ) / 2;
        else if ( horz_p > 1 ) x = width() - w - space_x;
        int y = space_y;
        if ( vert_p > 0 ) y = height() - h - space_y;
        painter.setBrush( g_config.background_color );
        painter.setPen( Qt::transparent );
        painter.drawRect( x - space_x, y - space_y, w + space_x*2, h + space_y*2 );
        painter.setPen( g_config.text_color );
        painter.drawText( x,y+h, s );
    }

    if ( _show_ui )
    {
        if ( g_config.show_extra_buttons != _extra_buttons )
            _loadUI(); // reload icons
        _ui.draw( painter );
    }
    if ( _show_ui && !g_config.multitouch )
    {
        int x,y;

        int indicator_width = TouchUI::scaleUI(_zoom_indicator.defaultSize().width());
        int indicator_height = TouchUI::scaleUI(_zoom_indicator.defaultSize().height());
        QRect rect(0,height() - indicator_height,
                   painter.window().width(),painter.window().height());
        painter.drawTiledPixmap( rect, *_ui.getBackground() );

        int icon_out_width = TouchUI::scaleUI(_zoom_out_icon.defaultSize().width());
        int icon_out_height = TouchUI::scaleUI(_zoom_out_icon.defaultSize().height());
        x = 0;
        y = height() - icon_out_height;
        QRectF r1( x,y, icon_out_width, icon_out_height );
        _zoom_out_icon.render( &painter, r1 );

        int icon_in_width = TouchUI::scaleUI(_zoom_in_icon.defaultSize().width());
        int icon_in_height = TouchUI::scaleUI(_zoom_in_icon.defaultSize().height());
        x = width() - icon_in_width;
        y = height() - icon_in_height;
        QRectF r2( x,y, icon_in_width, icon_in_height );
        _zoom_in_icon.render( &painter, r2 );


        if ( !(_current.isNull()) )
        {
            double fit_zoom = _current.computeFitZoomWithRotation(size());
            double min_zoom = ( fit_zoom < 1.0 ? fit_zoom : 1.0 );
            double max_zoom = g_config.max_zoom;
            double pos = ( _current.zoom - min_zoom ) / ( max_zoom - min_zoom );
            int w = width()
                    - icon_in_width - icon_out_width
                    - indicator_width;
            x = indicator_width + pos * w;//  - _zoom_indicator.width()/2;
        } else {
            x = width() / 2 - indicator_width/2;
        }
        y = height() - indicator_height;
        QRectF r3( x,y, indicator_width, indicator_height );
        _zoom_indicator.render( &painter, r3 );
    }
}

void ScreenViewer::onResize( void )
{
    if ( !(_current.isNull()) && _current.posx == 0 && _current.posy == 0 )
    {
        resetFitZoom( _previous );
        resetFitZoomWithRotation(_current);
        resetFitZoom( _next );
    }
}

bool ScreenViewer::onEvent( QEvent * event )
{
    switch (event->type())
    {

    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        onTouchEvent(static_cast<QTouchEvent *>(event));
        break;

    case QEvent::MouseButtonPress:
        onMousePressed( event );
        break;

    case QEvent::MouseButtonRelease:
        onMouseReleased( event );
        break;

    case QEvent::MouseMove:
        onMouseMove( event );
        break;

    case QEvent::MouseButtonDblClick:
        onDoubleClick( event );
        break;

    default:
        return false;
    }
    return true;
}

void ScreenViewer::onSettingsChanged( void )
{
    // currently nothing to do
}


void ScreenViewer::onTouchEvent(QTouchEvent *touchEvent)
{
    auto touchPoints = touchEvent->touchPoints();

    if ( touchEvent->type() == QEvent::TouchBegin && touchPoints.count() == 1 )
    {
        // Start a new touch stroke
        const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
        int x = (int)touchPoint0.pos().x();
        int y = (int)touchPoint0.pos().y();
        if ( m_action.startTouchAction(x,y) )
            commitTouchParams();
    }

    if ( m_action.isTouchAction() ) // Don't interrupt mouse actions with touch
    {
        if ( touchPoints.count() == 1 && !_two_fingers && !_changing )
        {
            const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
            onPan( touchPoint0, touchEvent->type() == QEvent::TouchEnd );
        }

        if ( touchPoints.count() >= 2 && !_allow_drag && !_changing)
        {
            _two_fingers = true;

            const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
            const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
            onTwoFingers( touchPoint0, touchPoint1 );

            //_limitPan();
            update();
        }

        if ( touchEvent->type() == QEvent::TouchEnd )
        {
            if ( !_allow_drag && !_two_fingers && touchPoints.count() >= 1 )
            {
                const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
                int x = (int)touchPoint0.pos().x();
                int y = (int)touchPoint0.pos().y();
                onTap(x,y);
                update();
            }
            if ( touchPoints.count() == 1 )
            {
                _allow_drag = false;
                m_action.endTouchAction();
            }
            double zoomRatio = (getZoom()/computeCurrentFitZoom());
            if( zoomRatio < 0.70 )
            {
                emit changeViewer();
            } else if( zoomRatio < 1.00 ) {
                emit fitImage();
            }
        }
    }
}

void ScreenViewer::onPan( const QTouchEvent::TouchPoint & point, bool end )
{
    int dx = point.pos().x() - point.startPos().x();
    int dy = point.pos().y() - point.startPos().y();

    // Strange, not sure when this happens
    if ( point.startPos().y() < _ui.height()
         && abs(dx) > g_config.drag_sensitivity )
    {
        if ( !_allow_drag )
        {
            _ui.startScroll();
        }
        _allow_drag = true;
        _ui.scroll(dx,0);
        update();
        qDebug() << "Strange pan happened" << QTime::currentTime();
        return;
    }

    // We get this once we go too far
    if ( !_two_fingers && abs(dx) > g_config.drag_sensitivity )
    {
        _allow_drag = true;
        // _current.recenter();
        // resetFitZoom( _current );
        resetFitZoom( _previous );
        resetFitZoom( _next );
        // qDebug() << "Resetting stuff during pan";
        emit startTimer();
    }

    if ( _allow_drag )
    {
        _drag_offset = _committed_drag_offset + dx;
        _drag_offset_y = _committed_drag_offset_y + dy;
        // qDebug() << "Pan" << QTime::currentTime();
        update();
        if ( end )
        {
            if ( dx < -g_config.flip_distance && m_current_index < m_files.size() - 1
                 && _load_thread.isIdle() )
            {
                _drag_offset = dx + width();
                _moveForward();
            }
            if ( dx > g_config.flip_distance && m_current_index > 0
                 && _load_thread.isIdle() )
            {
                _drag_offset = dx - width();
                _moveBack();
            }
            _changing = true;
            emit startTimer();
        }
    }
}

void  ScreenViewer::onTwoFingers( const QTouchEvent::TouchPoint tp0, const QTouchEvent::TouchPoint tp1 )
{
    // when starting, verify that at least one finger is inside the image
    if ( !_two_fingers_valid_operation )
    {
        if ( _isScreenPointInsideCurrentImage( tp0.startPos().x(), tp0.startPos().y() )
             || _isScreenPointInsideCurrentImage( tp1.startPos().x(), tp1.startPos().y() ) )
        {
            _two_fingers_valid_operation = true;
        } else {
            return;
        }
    }
    QLineF currentPos(tp1.pos(), tp0.pos());
    QLineF startPos(tp1.startPos(), tp0.startPos());

    // compute zoom
    double new_zoom = _committed_zoom * currentPos.length() / startPos.length();
    _limitZoom(new_zoom, _current);

    // compute rotation
    qreal new_rotation;
    if (g_config.allow_rotation) {
        new_rotation = _committed_rotation + currentPos.angle() - startPos.angle();
    } else {
        new_rotation = _committed_rotation;
    }

    // window size need in the formula
    qreal sw = (qreal)width();
    qreal sh = (qreal)height();

    // compute translation
    double rot_a = - _committed_rotation * M_PI / 180.0;
    double rot_b = - new_rotation * M_PI / 180.0;
    qreal cosa = (qreal)cos( rot_a );
    qreal sina = (qreal)sin( rot_a );
    qreal cosb = (qreal)cos( rot_b );
    qreal sinb = (qreal)sin( rot_b );
    qreal xt1 = tp0.startPos().x();
    qreal yt1 = tp0.startPos().y();
    qreal xt1p = tp0.pos().x();
    qreal yt1p = tp0.pos().y();
    qreal x1 = (  (xt1-_committed_posx-sw/2)*cosa + (yt1-_committed_posy-sh/2)*sina ) / _committed_zoom;
    qreal y1 = ( -(xt1-_committed_posx-sw/2)*sina + (yt1-_committed_posy-sh/2)*cosa ) / _committed_zoom;
    qreal px = xt1p - sw/2 - (x1*cosb - y1*sinb) * new_zoom;
    qreal py = yt1p - sh/2 - (x1*sinb + y1*cosb) * new_zoom;

    /*
      printf("Old: p=%d,%d rot=%d scale=%.2lf\n", _initial_posx,_initial_posy,
      (int)_initial_rotation, _initial_zoom);
      printf("New: p=%d,%d rot=%d scale=%.2lf\n", (int)px,(int)py,
      (int)new_rotation, new_zoom);
      printf("Touch1=%d,%d Touch1p=%d,%d Image_touch=%d,%d\n",
      (int)xt1,(int)yt1, (int)xt1p, (int)yt1p, (int)x1, (int)y1 );
      printf("Image_size=%d,%d Screen_size=%d,%d Alpha=%d\n",
      (int)w, (int)h, (int)sw, (int)sh, (int)r);
      printf("=====================================================\n");
    */

    // assign new values
    _current.zoom = new_zoom;
    _current.posx = (int)px;
    _current.posy = (int)py;
    _current.rotation = fmod(new_rotation, 360.0);
}

bool ScreenViewer::onTap( int x, int y )
{
    if ( _changing )
    {
        if ( !_show_ui ) _show_ui = true;
        return true;
    }

    TouchUI::UIAction action = TouchUI::TOUCH_ACTION_NONE;
    if ( _show_ui )
    {
        action = _ui.getAction(x,y);
    }

    _handleTouchAction( action );

    int zby1 = ( height() - TouchUI::scaleUI(_zoom_in_icon.defaultSize().height()) );
    int zby2 = height();
    bool zoom_zone = ( !g_config.multitouch
                       && _show_ui
                       && y > zby1
                       && y < zby2 );
    if ( zoom_zone )
    {
        if ( x < TouchUI::scaleUI(_zoom_out_icon.defaultSize().width()) )
        {
            zoomOut();
        } else if ( x > width() - TouchUI::scaleUI(_zoom_in_icon.defaultSize().width()) ) {
            zoomIn();
        }
        return true;
    }

    if ( action == TouchUI::TOUCH_ACTION_NONE )
    {
        _show_ui = !_show_ui;
        _show_ui_by_tap = _show_ui;
    }
    return false;
}

void ScreenViewer::onMousePressed( QEvent * event )
{
    QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);

    if ( !m_action.startMouseAction(mouseEvent->x(), mouseEvent->y(), mouseEvent->button()) )
        return;

    _committed_posx = _current.posx;
    _committed_posy = _current.posy;
    _committed_drag_offset = _drag_offset;
    _committed_drag_offset_y = _drag_offset_y;
    _allow_drag = false;
    _allow_zoom = false;
    _allow_pan = false;

    if ( mouseEvent->button() == Qt::LeftButton )
    {
        _mouse_drag = true;
        _mouse_start_x = mouseEvent->x();
        _mouse_start_y = mouseEvent->y();
    }
    update();
}

void ScreenViewer::onMouseReleased( QEvent * event )
{
    QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
    if ( !m_action.isMouseAction() )
        return;

    if ( _allow_drag )
    {
        int dx = mouseEvent->x() - _mouse_start_x;

        if ( _mouse_start_y < _ui.height() ) {
            // dragging the menu
            // nothing for now
        } else if ( dx < -g_config.flip_distance && m_current_index < m_files.size() - 1
                    && !_changing && _load_thread.isIdle() )
        {
            _drag_offset = dx + width();
            _moveForward();
            _changing = true;
        } else if ( dx > g_config.flip_distance && m_current_index > 0
                    && !_changing && _load_thread.isIdle() )
        {
            _drag_offset = dx - width();
            _moveBack();
            _changing = true;

        } else {
            _drag_offset = dx;
            _changing = true;
            emit startTimer();
        }
    } else if ( !_allow_zoom && !_allow_pan ) {
        int x = mouseEvent->x();
        int y = mouseEvent->y();
        onTap(x,y);
    }

    _mouse_drag = _allow_pan = _allow_zoom = false;
    update();
    m_action.endMouseAction();
}

void ScreenViewer::onMouseMove( QEvent * event )
{
    QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);

    bool mouse_in_ui_zone = mouseEvent->y() < _ui.height();
    bool mouse_in_zoom_zone = !g_config.multitouch
                                              && mouseEvent->y() >  height() - TouchUI::scaleUI(_zoom_in_icon.defaultSize().height())
                              && mouseEvent->y() < height()
                                                   && mouseEvent->x() > TouchUI::scaleUI(_zoom_out_icon.defaultSize().width())
                              && mouseEvent->x() < width() - TouchUI::scaleUI(_zoom_in_icon.defaultSize().width());

    // update UI
    bool ui_change = false;
    if ( mouse_in_ui_zone || mouse_in_zoom_zone )
    {
        // highlight interface icon
        if ( !_show_ui )
        {
            _show_ui = true;
            ui_change = true;
        }
        if ( _ui.highlightItem( mouseEvent->x(), mouseEvent->y() ) )
            ui_change = true;
    } else {
        if ( _show_ui && !_show_ui_by_tap )
        {
            _show_ui = false;
            ui_change = true;
        }
    }
    if ( ui_change ) update();

    if ( !m_action.isMouseAction() )
        return;

    if ( !_changing && _mouse_drag )
    {
        _allow_zoom = _allow_zoom || ( _show_ui && mouse_in_zoom_zone && !_allow_drag );

        double fit_zoom = _current.computeFitZoomWithRotation(size());
        double min_zoom = ( fit_zoom < 1.0 ? fit_zoom : 1.0 );
        double max_zoom = g_config.max_zoom;

        if ( _allow_zoom )
        {
            // zoom

            int posx = mouseEvent->x()
                       - TouchUI::scaleUI(_zoom_out_icon.defaultSize().width())
                       - TouchUI::scaleUI(_zoom_indicator.defaultSize().width()/2);
            int maxx = width()
                       - TouchUI::scaleUI(_zoom_in_icon.defaultSize().width())
                       - TouchUI::scaleUI(_zoom_out_icon.defaultSize().width());
            if ( posx < 0 ) posx = 0;
            if ( posx > maxx ) posx = maxx;

            _current.zoom = min_zoom + (double)posx * (max_zoom - min_zoom) / (double) maxx;
            _limitZoom(_current.zoom, _current);
            _limitPan();
            update();

        } else if ( fabs( _current.zoom - min_zoom ) > 0.01 ) {

            // pan

            _allow_pan = true;
            int dx0 = mouseEvent->x() - _mouse_start_x;
            int dy0 = mouseEvent->y() - _mouse_start_y;
            if ( dx0 > g_config.drag_sensitivity || dy0 > g_config.drag_sensitivity )
                _allow_pan = true;
            if ( _allow_pan )
            {
                _current.posx = _committed_posx + dx0;
                _current.posy = _committed_posy + dy0;
                _limitPan();
                update();
            }
        } else if ( _mouse_start_y < _ui.height() ) {

            int dx = mouseEvent->x() - _mouse_start_x;
            if ( !_allow_drag )
            {
                _ui.startScroll();
            }
            _allow_drag = true;
            _ui.scroll(dx,0);
            update();
        } else {

            // drag to next image

            int dx = mouseEvent->x() - _mouse_start_x;
            if ( abs(dx) > g_config.drag_sensitivity )
            {
                _allow_drag = true;
                resetFitZoom( _current );
                _current.posx = _current.posy = 0;
                _current.rotation = 0.0;
            }
            if ( _allow_drag )
            {
                _drag_offset = _committed_drag_offset + dx;
                update();
            }
        }
    }
}

void ScreenViewer::onDoubleClick( QEvent * event )
{
    QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
    int x = mouseEvent->x();
    int y = mouseEvent->y();

    if (y > _ui.height()
        && y < height() - TouchUI::scaleUI(_zoom_indicator.defaultSize().height()) )
    {
        if ( fabs( _current.zoom - 1.0 ) < 0.01 )
            zoomToFit();
        else
            zoomToOriginalSize( x,y );
    }
}

void ScreenViewer::onKeyPress( QKeyEvent * event )
{
    switch ( event->key() )
    {
    case Qt::Key_Space:
        this->forward();
        break;
    case Qt::Key_Backspace:
        this->back();
        break;
    case Qt::Key_Plus:
    case Qt::Key_Greater:
        this->zoomIn();
        break;
    case Qt::Key_Minus:
    case Qt::Key_Less:
        this->zoomOut();
        break;
    case Qt::Key_BracketLeft:
    case Qt::Key_L:
        this->rotateRight();
        break;
    case Qt::Key_BracketRight:
    case Qt::Key_R:
        this->rotateLeft();
        break;
    case Qt::Key_Slash:
        this->zoomToFit();
        break;
    case Qt::Key_0:
        this->zoomToOriginalSize();
        break;
    case Qt::Key_Left:
        if ( event->modifiers() & ( Qt::ShiftModifier | Qt::ControlModifier ) )
            this->moveView(-50,0);
        else
            this->back();
        break;
    case Qt::Key_Right:
        if ( event->modifiers() & ( Qt::ShiftModifier | Qt::ControlModifier ) )
            this->moveView(50,0);
        else
            this->forward();
        break;
    case Qt::Key_Up:
        if ( event->modifiers() & ( Qt::ShiftModifier | Qt::ControlModifier ) )
            this->moveView(0,-50);
        break;
    case Qt::Key_Down:
        if ( event->modifiers() & ( Qt::ShiftModifier | Qt::ControlModifier ) )
            this->moveView(0,50);
        break;
    case Qt::Key_O:
        emit loadFile();
        break;
    case Qt::Key_T:
    case Qt::Key_D:
        emit changeViewer();
        break;
    case Qt::Key_Home:
        first();
        break;
    case Qt::Key_End:
        last();
        break;
    case Qt::Key_Escape:
        closeOnTouch();
        break;
    case Qt::Key_Delete:
        _deleteCurrentFile();
        break;
    default:
        event->ignore();
        break;
    }
}

void ScreenViewer::onWheel( QWheelEvent * event )
{
    if ( event->modifiers() & ( Qt::ShiftModifier | Qt::ControlModifier ) )
    {
        if ( event->delta() > 0 )
            zoomIn( 0.90 );
        else
            zoomOut( 0.90 );
    } else {
        if ( event->delta() > 0 )
            back();
        else
            forward();
    }
}

double ScreenViewer::getRotation()
{
    return _current.rotation;
}

void ScreenViewer::setRotation( double angle )
{
    _current.rotation = angle;
}

double ScreenViewer::getZoom()
{
    return _current.zoom;
}

void ScreenViewer::setZoom( double zoom )
{
    _current.zoom = zoom;
}

double ScreenViewer::computeCurrentFitZoom()
{
    return _current.computeFitZoom(size());
}

bool ScreenViewer::isIdle()
{
    return _load_thread.isIdle();
}

bool ScreenViewer::isReady()
{
    if(!(_current.isNull())) return true;
    return false;
}

bool ScreenViewer::isBeingPinchZoomed()
{
    return _two_fingers;
}

void ScreenViewer::onTimer( void )
{
    if ( _changing )
    {
        if ( _drag_offset > 0 )
        {
            _drag_offset = std::max(_drag_offset - g_config.image_scroll_speed, 0);
        } else if ( _drag_offset < 0 ) {
            _drag_offset = std::min(_drag_offset + g_config.image_scroll_speed, 0);
        }
        update();
    }
    if ( _drag_offset == 0 ) _changing = false;

    bool load_thread_idle_state = _load_thread.isIdle();
    if ( load_thread_idle_state != _last_load_thread_idle_state )
    {
        update();
        _last_load_thread_idle_state = load_thread_idle_state;
    }
}

/*******************************************************************************
 * PRIVATE METHODS
 *******************************************************************************/

void  ScreenViewer::loadImage( int index, ImageWithInfo &i )
{
    // Warning: breaking encapsulation here.
    delete i.image;
    delete i.small_image;

    i.image = NULL;
    i.zoom = 1.0;
    if ( index < 0 || index >= m_files.size() )
        return;

    ImageLoadItem ili;
    ili.name = m_files[index];
    ili.destination = &i.image;
    ili.w = width();
    ili.h = height();
    i.zoom = 0.0;
    ili.force_fit_in_size = false;
    _load_thread.addLoadImage(ili);
}

void ScreenViewer::resetFitZoom( ImageWithInfo &i, FitZoomMode zoom_mode )
{
    i.zoom = i.computeFitZoom(size(), zoom_mode);
}

void ScreenViewer::resetFitZoomWithRotation( ImageWithInfo &i, FitZoomMode zoom_mode )
{
    i.zoom = i.computeFitZoomWithRotation(size(), zoom_mode);
}

void ScreenViewer::_handleTouchAction( TouchUI::UIAction action )
{
    switch ( action )
    {
    case TouchUI::TOUCH_ACTION_EXIT:
        emit closeOnTouch();
        break;
    case TouchUI::TOUCH_ACTION_FULLSCREEN:
        emit changeFullscreen();
        break;
    case TouchUI::TOUCH_ACTION_ZOOM_IN:
        zoomIn();
        break;
    case TouchUI::TOUCH_ACTION_ZOOM_OUT:
        zoomOut();
        break;
    case TouchUI::TOUCH_ACTION_ZOOM_FIT:
        zoomToFit();
        break;
    case TouchUI::TOUCH_ACTION_ZOOM_FIT_WIDTH:
        zoomToFit(ZOOM_FIT_WIDTH);
        break;
    case TouchUI::TOUCH_ACTION_ZOOM_FIT_HEIGHT:
        zoomToFit(ZOOM_FIT_HEIGHT);
        break;
    case TouchUI::TOUCH_ACTION_ZOOM_ORIGINAL:
        zoomToOriginalSize();
        break;
    case TouchUI::TOUCH_ACTION_ROTATE_LEFT:
        rotateLeft();
        break;
    case TouchUI::TOUCH_ACTION_ROTATE_RIGHT:
        rotateRight();
        break;
    case TouchUI::TOUCH_ACTION_GO_FIRST:
        first();
        break;
    case TouchUI::TOUCH_ACTION_GO_LAST:
        last();
        break;
    case TouchUI::TOUCH_ACTION_GO_NEXT:
        forward();
        break;
    case TouchUI::TOUCH_ACTION_GO_PREVIOUS:
        back();
        break;
    case TouchUI::TOUCH_ACTION_OPEN:
        emit loadFile();
        break;
    case TouchUI::TOUCH_ACTION_THUMBS:
        emit changeViewer();
        break;
    case TouchUI::TOUCH_ACTION_CONFIG:
        emit config();
        break;
    default:
        break;
    }
}

void ScreenViewer::_limitZoom( double & zoom, ImageWithInfo & img )
{
    double min_zoom = g_config.min_zoom;
    double max_zoom = g_config.max_zoom;

    int w = img.width();
    int h = img.height();
    if ( w > 0 && h > 0 )
    {
        double z = ( w > h ) ? ( 80.0 / (double)h ) : ( 80.0 / (double)w );
        if ( z > min_zoom ) min_zoom = z;
    }
    if ( min_zoom > 1.0 ) min_zoom = 1.0;

    if ( zoom > max_zoom )
        zoom = max_zoom;
    else if ( zoom < min_zoom )
        zoom = min_zoom;
}

void ScreenViewer::_limitPan( void )
{
    if ( _current.isNull() ) return;
    //int sw = (int)( (double)_current.width() * _current.zoom );
    //int sh = (int)( (double)_current.height() * _current.zoom );

    double rot = _current.rotation * M_PI / 180.0;
    qreal c = (qreal)cos( rot );
    qreal s = (qreal)sin( rot );
    double w = (double)_current.width() * _current.zoom;
    double h = (double)_current.height() * _current.zoom;
    int sw = (int)fabs(w * c + h * s);
    int sh = (int)fabs(w * s + h * c);

    if ( sw > width() )
    {
        int max_x = ( sw - width() ) / 2;
        int min_x = - sw + max_x + width();
        if ( _current.posx > max_x ) _current.posx = max_x;
        if ( _current.posx < min_x ) _current.posx = min_x;
    } else {
        _current.posx = 0;
    }
    if ( sh > height() )
    {
        int max_y = ( sh - height() ) / 2;
        int min_y = - sh + max_y + height();
        if ( _current.posy > max_y ) _current.posy = max_y;
        if ( _current.posy < min_y ) _current.posy = min_y;
    } else {
        _current.posy = 0;
    }
}

void ScreenViewer::_resetUserActionsParameters( void )
{
    _drag_offset = 0;
    _committed_rotation = 0.0;
    _committed_zoom = 0.0;

    _mouse_start_x = _mouse_start_y = 0;
    _mouse_drag = false;

    _committed_posx = _committed_posy = _committed_drag_offset = _committed_drag_offset_y = 0;
    _allow_zoom = _allow_pan = _allow_drag = false;
    _changing = false;
    _two_fingers = _two_fingers_valid_operation = false;

    m_action.endMouseAction();
    m_action.endTouchAction();
}

void ScreenViewer::commitTouchParams( void )
{
    _committed_posx = _current.posx;
    _committed_posy = _current.posy + _drag_offset_y;
    _committed_drag_offset = _drag_offset;
    _committed_drag_offset_y = 0;
    _committed_zoom = _current.zoom;
    _committed_rotation = _current.rotation;
    _allow_zoom = false;
    _allow_pan = false;
    _allow_drag = false;
    _two_fingers = false;
    _two_fingers_valid_operation = false;

}

void ScreenViewer::_reloadAll( void )
{
    // wait for load thread to stop
    // (it may still have pointers to the images)
    _load_thread.clear();
    _load_thread.waitUntilIdle();

    loadImage( m_current_index,   _current);
    loadImage( m_current_index-1, _previous);
    loadImage( m_current_index+1, _next);
}

void ScreenViewer::_moveForward( void )
{
    m_current_index++;
    _current.recenter();

    _previous = _current;
    _current = _next;

    loadImage( m_current_index+1, _next );
    emit indexChanged( m_current_index );
}

void ScreenViewer::_moveBack( void )
{
    m_current_index--;
    _current.recenter();

    _next = _current;
    _current = _previous;

    loadImage( m_current_index-1, _previous );
    emit indexChanged( m_current_index );
}

QTransform ScreenViewer::_getCurrentTransform( void )
{
    QTransform tr;

    qreal dx = (qreal)_current.width();
    qreal dy = (qreal)_current.height();

    tr.translate( (qreal)width() / 2, (qreal)height() / 2 );
    tr.translate( (qreal)(_current.posx + _drag_offset), (qreal)(_current.posy + _drag_offset_y) );
    tr.rotate(-_current.rotation);
    tr.scale( _current.zoom, _current.zoom );
    tr.translate( -dx/2,-dy/2);

    return tr;
}

bool ScreenViewer::_isScreenPointInsideCurrentImage( qreal x, qreal y )
{
    // x,y are the coordinates on the screen

    // window and image size need in the formula
    qreal sw = (qreal)width();
    qreal sh = (qreal)height();
    qreal w = (qreal)_current.width();
    qreal h = (qreal)_current.height();

    // compute coordinates inside the image (x1,y1)
    qreal rot = (qreal)( - _current.rotation * M_PI / 180.0 );
    qreal cosa = (qreal)cos( rot );
    qreal sina = (qreal)sin( rot );
    qreal px = (qreal)_current.posx;
    qreal py = (qreal)_current.posy;
    qreal x1 = (  (x-px-sw/2)*cosa + (y-py-sh/2)*sina ) / _current.zoom + w/2;
    qreal y1 = ( -(x-px-sw/2)*sina + (y-py-sh/2)*cosa ) / _current.zoom + h/2;

    // verify that the coordinates are inside the image
    bool ret = ( x1 >= 0.0 && y1 >= 0.0 && x1 < w && y1 < h );

    return ret;
}

void ScreenViewer::_loadUI( void )
{
    _extra_buttons = g_config.show_extra_buttons;
    _ui.clean();

    bool draw_separator = true;

    _ui.addAction( TouchUI::TOUCH_ACTION_OPEN, "document-open.svg" );
    _ui.addAction( TouchUI::TOUCH_ACTION_THUMBS, "folder-photos.svg" );
    _ui.addAction( TouchUI::TOUCH_ACTION_FULLSCREEN, "view-fullscreen.svg" );
    _ui.addAction( TouchUI::TOUCH_ACTION_CONFIG, "settings.svg" );

    if ( draw_separator ) _ui.addSeparator();

    _ui.addAction( TouchUI::TOUCH_ACTION_GO_FIRST, "go-first.svg" );
    //_ui.addAction( TouchUI::TOUCH_ACTION_GO_PREVIOUS, "go-previous.svg" );
    //_ui.addAction( TouchUI::TOUCH_ACTION_GO_NEXT, "go-next.svg" );
    _ui.addAction( TouchUI::TOUCH_ACTION_GO_LAST, "go-last.svg" );

    if ( draw_separator ) _ui.addSeparator();

    //_ui.addAction( TouchUI::TOUCH_ACTION_ZOOM_IN, "zoom-in.svg" );
    //_ui.addAction( TouchUI::TOUCH_ACTION_ZOOM_OUT, "zoom-out.svg" );
    _ui.addAction( TouchUI::TOUCH_ACTION_ZOOM_FIT, "zoom-fit-best.svg" );
    _ui.addAction( TouchUI::TOUCH_ACTION_ZOOM_ORIGINAL, "zoom-original.svg" );
    if ( g_config.show_extra_buttons )
    {
        _ui.addAction( TouchUI::TOUCH_ACTION_ZOOM_FIT_WIDTH, "zoom-fit-width.svg" );
        _ui.addAction( TouchUI::TOUCH_ACTION_ZOOM_FIT_HEIGHT, "zoom-fit-height.svg" );
    }

    if ( !g_config.multitouch )
    {
        _ui.addAction( TouchUI::TOUCH_ACTION_ROTATE_LEFT, "object-rotate-left.svg" );
        _ui.addAction( TouchUI::TOUCH_ACTION_ROTATE_RIGHT, "object-rotate-right.svg" );
    }

    if ( draw_separator ) _ui.addSeparator();
    _ui.addAction( TouchUI::TOUCH_ACTION_EXIT, "application-exit.svg" );
}

void ScreenViewer::_deleteCurrentFile( void )
{
    // get file name
    if ( m_files.size() == 0 )
        return;
    QString filename = m_files.at(m_current_index);

    // send file to trash if possible
    printf("Delete %s\n", QDir(m_dir_name).filePath(filename).toUtf8().data());
    if ( !sendToTrash( m_dir_name, filename ) )
    {
        fprintf( stderr, "[WARNING] Cannot send file to trash.\n" );
        return;
    }

    // remove file from vector
    m_files.removeAt(m_current_index);

    // Load new images
    if ( m_files.size() >= 0 )
    {
        if ( m_current_index == m_files.size() )
        {
            m_current_index--;
            _current = _previous;
            if ( m_current_index > 0 )
                loadImage( m_current_index-1, _previous );
            else
                _previous.clear();
        } else {
            _current = _next;
            if ( m_current_index < m_files.size() - 1 )
                loadImage( m_current_index+1, _next );
            else
                _next.clear();
        }
        resetFitZoom(_current);
    }
    _current.recenter();
    update();
}
