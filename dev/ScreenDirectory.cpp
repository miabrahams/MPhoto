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

#include <QtGui>
#include <stdio.h>

#include "Config.h"
#include "ScreenDirectory.h"

ScreenDirectory::ScreenDirectory()
    : ScreenBase()
{
  _total_height = 0;
  _scroll_pos = 0;
  _scroll_pos_dest = 0;
  _thumbs_per_row = 1;
  _image_name_height = _computeImageNameHeight();

  _resetUserActionsParameters();

  _thumbs = NULL;
  _last_number_of_loaded_images = 0;

    _ui.addAction( TouchUI::TOUCH_ACTION_OPEN, "document-open.svg" );
    _ui.addAction( TouchUI::TOUCH_ACTION_UP, "up.svg" );
    _ui.addAction( TouchUI::TOUCH_ACTION_FULLSCREEN, "view-fullscreen.svg" );
    _ui.addAction( TouchUI::TOUCH_ACTION_CONFIG, "settings.svg" );
  _ui.addSeparator();
    _ui.addAction( TouchUI::TOUCH_ACTION_EXIT, "application-exit.svg" );

  _loadIcons();
  _load_thread.start();
}

ScreenDirectory::~ScreenDirectory()
{
  _load_thread.stopThread();
  _load_thread.wait();
  _clearThumbs();
}

/*******************************************************************************
* EVENTS
*******************************************************************************/

void ScreenDirectory::onSetFiles( QString dir_name, QStringList files, QString current )
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
  int priority = (int)files.size();
  if ( !same_files )
  {
    // wait for load thread to become idle
    _load_thread.clear();
    _load_thread.waitUntilIdle();

    _clearThumbs();
    m_files = files;
    m_current_index = current_index;

    // load new thumbs
    _thumbs = new QImage*[ files.size() ];
    for ( int i = 0; i < files.size(); i++ )
    {
      _thumbs[i] = NULL;
      _addThumbnailToLoad( files.at(i), &_thumbs[i], priority-- );

    }
  } else {
    // wait for load thread to become idle
    _load_thread.clear();
    _load_thread.waitUntilIdle();

    // update index
    m_current_index = current_index;

    // load thumbnails for images that don't have one
    for ( int i = 0; i < files.size(); i++ )
    {
      if ( _thumbs[i] == NULL )
        _addThumbnailToLoad( files.at(i), &_thumbs[i], priority-- );

    }
  }

  // get list of folders
  if ( g_config.show_folders )
  {
    QDir myDir( dir_name );
    QStringList name_filters;
    _folders = myDir.entryList( name_filters, QDir::Dirs | QDir::Drives, QDir::Name | QDir::IgnoreCase );
    _folders.removeAll(".");
    _folders.removeAll("..");
    m_current_index += _folders.size();

    // however, if there is no image in this folder, select first folder
    if ( m_current_index - _folders.size() >= files.size() )
      m_current_index = 0;
  } else {
    // just to be sure
    _folders.clear();
  }

  // update positions
  //_scroll_pos = _scroll_pos_dest = 0;
  _updateThumbsLocations();
  _last_number_of_loaded_images = 0;

  _resetUserActionsParameters();
  scrollToCurrent();
}

void ScreenDirectory::onPaint( QPainter & painter )
{
        QFont font = painter.font();
  int point_size = TouchUI::scaleUI(font.pointSize());
  font.setPointSize( point_size > 0 ? point_size : 1 );
        painter.setFont( font );

  if ( m_files.size() == 0 && _folders.size() == 0 )
  {
    QString s1 = "", s2 = "";
    s1 = tr("No images.");
    s2 = tr("Click on the \"Open\" icon and select a folder!");

    int w,h,x,y;

    QFontMetrics fm( painter.font() );
    painter.setPen( g_config.text_color );
    w = fm.width(s1);
    h = fm.height();
    x = ( width() - w ) / 2;
    y = height()/2 - h - 2;
    painter.drawText( x,y, s1 );
    w = fm.width(s2);
    x = ( width() - w ) / 2;
    y = height()/2 + 2;
    painter.drawText( x,y, s2 );

    _ui.draw( painter );
    return;
  }

  int th_width = (int)( g_config.thumb_size / 100.0 * width() );
  int th_height = th_width * 3 / 4;
  if ( g_config.thumbnails_square )
    th_height = th_width;
  int img_width = th_width;
  int img_height = th_height;
  if ( g_config.thumbnails_space )
  {
    img_width = img_width * 90 / 100;
    img_height = img_height * 90 / 100;
  }

  int priority = (int)m_files.size();
  ImageLoadQueue & q = _load_thread.getQueue();
  q.clearPriorities();

  int number_of_items = m_files.size() + _folders.size();

  for ( int i = 0; i < number_of_items; i++ )
  {
    // is it an image or a folder
    bool is_image = ( i >= _folders.size() );

    // top-left corner of thumb area
    int x = th_width * ( i % _thumbs_per_row );
    int y = _ui.height()
      + ( th_height + _image_name_height ) * ( i / _thumbs_per_row )
      - _scroll_pos;

    // don't draw items outside the screen
    if ( y > height() ) break;
    if ( y + th_height + _image_name_height < 0 ) continue;

    // center
    int cx = x + th_width / 2;
    int cy = y + th_height / 2;

    // draw scaled thumbnail
    if ( is_image )
    {
      // it's an image
      QImage * img = _thumbs[i-_folders.size()];
      if ( img )
      {
        if ( g_config.thumbnails_crop )
        {
          QRectF target(cx-img_width/2, cy-img_height/2, img_width, img_height);
          float w = (float)img->width();
          float h = (float)img->height();
          float w2 = (float)img_width * img->height() / (float)img_height;
          float h2 = (float)img_height * img->width() / (float)img_width;
          if ( w2 > w )
          {
            h = h2;
          } else {
            w = w2;
          }
          float dx = ( (float)img->width() - w ) /2;
          float dy = ( (float)img->height() - h ) /2;
          QRectF source(dx,dy, w,h);
          /*
          printf("Image %d,%d Source %.1f;%.1f;%.1f;%.1f Target %.1f;%.1f;%.1f;%.1f\n",
            img->width(),img->height(),
            source.x(),source.y(),source.width(),source.height(),
            target.x(),target.y(),target.width(),target.height());
          */
          painter.drawImage(target, *img, source);

        } else {
          int w = img->width();
          int h = img->height();
          ImageLoadThread::fitImage( w,h, img_width, img_height, false );
          QRectF r( cx-w/2, cy-h/2, w, h );
          painter.drawImage( r, *img );
        }

      } else {
        q.updatePriority( &_thumbs[i], priority-- );
        QRectF thumb_rect( cx-img_width/2,cy-img_height/2,
          img_width,img_height );
        QBrush brush1( Qt::darkGray );
        painter.setBrush( brush1 );
        painter.setPen( Qt::transparent );
        painter.drawRect( thumb_rect );
      }
    } else {
      // it's a folder
      int w = 512;
      int h = 512;
      ImageLoadThread::fitImage( w,h, img_width, img_height, false );
      QRectF r( cx-w/2, cy-h/2, w, h );
      _folder_icon.render( &painter, r );
    }

    // draw border for selected item
    if ( i == m_current_index )
    {
      QBrush brush2( Qt::transparent );
      QPen pen2( g_config.text_color );
      pen2.setWidth(5);
      painter.setBrush( brush2 );
      painter.setPen( pen2 );
      painter.drawRect( cx-img_width/2,cy-img_height/2,
        img_width-4,img_height-3 );
    }

    // draw name
    bool draw_name = g_config.thumbnails_show_name || !is_image;

    if ( draw_name )
    {
      // get the name to be displayed
      QString s;
      if ( is_image )
      {
        // it's an image
        s = m_files.at(i-_folders.size());
      } else {
        // it's a folder
        s = _folders.at(i);
      }

      // draw the name
      painter.setPen( g_config.text_color );
      QFontMetrics fm( painter.font() );
      int h = fm.height();
      if ( _image_name_height == 0 )
      {
        int x1 = x + (th_width - img_width)/2;
        int y1 = y + (th_height - img_height)/2;
        painter.setClipRect(x1,y1, img_width-15, h+4 );

        painter.setPen( Qt::black );
        painter.drawText( x1+0, y1+h+0, s );
        painter.drawText( x1+0, y1+h+2, s );
        painter.drawText( x1+2, y1+h+0, s );
        painter.drawText( x1+2, y1+h+2, s );
        painter.setPen( g_config.text_color );
        painter.drawText( x1+1, y1+h+1, s );
        //painter.drawText( x1, y1+h, s );
      } else {
        int x1 = x + (th_width - img_width);
        int y1 = y + th_height;
        painter.setClipRect(x1,y + th_height, img_width,_image_name_height );
        painter.drawText( x1, y1+h, s );
      }
      painter.setClipRect( 0,0, width(), height() );
    }
  }

  // draw top menu
  _ui.draw( painter );

  // draw scroll indicator
  if ( ( _dragging || g_config.always_show_scroll )
    && _total_height > height() )
  {
    int sw = TouchUI::scaleUI( _scroll_indicator.defaultSize().width() );
    int sh = TouchUI::scaleUI( _scroll_indicator.defaultSize().height() );
    int th = height() - _ui.height() - sh;
    int scroll_height = _total_height - height() + _ui.height();
    int scroll_pos = _scroll_pos;
    _limitScroll( scroll_pos );
    int sx = width() - sw;
    int sy = _ui.height() + th * scroll_pos / scroll_height;
    QRectF r( sx,sy, sw, sh );
    _scroll_indicator.render( &painter, r );
  }
}

void ScreenDirectory::onResize( void )
{
  _updateThumbsLocations();
  update();
}

bool ScreenDirectory::onEvent( QEvent * event )
{
  switch (event->type())
  {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
      QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
      QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();

      if ( event->type() == QEvent::TouchBegin && touchPoints.count() == 1 )
      {
        const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
        int x = (int)touchPoint0.pos().x();
        int y = (int)touchPoint0.pos().y();
        if ( m_action.startTouchAction(x,y) )
        {
          _resetTouchParams();
          _updateScrollSpeed( touchPoint0.pos().x(), touchPoint0.pos().y(), true );
          _scroll_speed = 0.0;
        }
      }

      if ( m_action.isTouchAction() )
      {
        if ( touchPoints.count() == 1
             && !_zooming )
        {
          const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
          onDrag( touchPoint0, event->type() == QEvent::TouchEnd );
          _updateScrollSpeed( touchPoint0.pos().x(), touchPoint0.pos().y() );
        }

        if ( touchPoints.count() >= 2 && !_dragging )
        {
          _two_fingers = true;

          const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
          const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
          onTwoFingers( touchPoint0, touchPoint1 );

          update();
        }

        if ( event->type() == QEvent::TouchEnd )
        {
          if ( !_zooming && !_dragging
            && touchPoints.count() >= 1 )
          {
            const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
            int x = (int)touchPoint0.pos().x();
            int y = (int)touchPoint0.pos().y();
            if ( y > _ui.height() )
            {
              // select picture under mouse
              int th_width = (int)( g_config.thumb_size / 100.0 * width() );
              int th_height = th_width * 3 / 4;
              int column = x / th_width;
              int row = ( y + _scroll_pos - _ui.height() ) / ( th_height + _image_name_height );
              int pos = row * _thumbs_per_row + column;
              int number_of_items = m_files.size() + _folders.size();
              if ( column < _thumbs_per_row && pos >= 0 && pos < number_of_items )
              {
                m_current_index = pos;
                if ( pos < _folders.size() )
                {
                  // it's a folder
                  _changeToFolder( _folders[pos] );
                } else {
                  // it's an image
                  m_current_index -= _folders.size();
                  _load_thread.clear();
                  emit changeViewer();
                }
              }
            } else {
              onTap(x,y);
            }
            update();
          }
          if ( _dragging )
            _cineticScroll();
          if ( touchPoints.count() == 1 )
          {
            m_action.endTouchAction();
            _dragging = false;
          }
        }
      }
    }
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

void ScreenDirectory::onDrag( const QTouchEvent::TouchPoint & point, bool end )
{
  (void)end;

  int dx0 = point.pos().x() - point.startPos().x();
  int dy0 = point.pos().y() - point.startPos().y();
  if ( abs(dy0) > g_config.drag_sensitivity
       || abs(dx0) > g_config.drag_sensitivity  )
    _dragging = true;

  if ( _dragging )
  {
    _scroll_pos_dest = _initial_scroll - dy0;
    onTimer();
    emit startTimer();
  }
}

void  ScreenDirectory::onTwoFingers( const QTouchEvent::TouchPoint tp0,
  const QTouchEvent::TouchPoint tp1 )
{
  qreal d_current = QLineF(tp0.pos(), tp1.pos()).length();
  qreal d_start = QLineF(tp0.startPos(), tp1.startPos()).length();

  if ( fabs(d_current-d_start) > 2*g_config.drag_sensitivity )
    _zooming = true;

  if ( _zooming )
  {
    g_config.thumb_size = _initial_thumb_size * d_current / d_start
      * g_config.thumb_zoom_ratio;

    // limit zoom
    if ( g_config.thumb_size > g_config.thumb_zoom_max )
      g_config.thumb_size = g_config.thumb_zoom_max;
    if ( g_config.thumb_size < g_config.thumb_zoom_min )
      g_config.thumb_size = g_config.thumb_zoom_min;

    // update dimensions
    _updateThumbsLocations();
    _limitScroll();
  }

}

void ScreenDirectory::onTap( int x, int y )
{
  // ui action
  if ( y < _ui.height() )
  {
    _handleTouchAction( _ui.getAction(x,y) );
    return;
  }

  // click on picture
  int th_width = (int)( g_config.thumb_size / 100.0 * width() );
  int th_height = th_width * 3 / 4;
  int column = x / th_width;
  int row = ( y + _scroll_pos - _ui.height() ) / ( th_height + _image_name_height );
  int pos = row * _thumbs_per_row + column;
  int number_of_items = m_files.size() + _folders.size();
  if ( column < _thumbs_per_row && pos >= 0 && pos < number_of_items )
  {
    m_current_index = pos;
    if ( pos < _folders.size() )
    {
      // it's a folder
      _changeToFolder( _folders[pos] );
    } else {
      // it's an image
      //m_current_index -= _folders.size();
      _load_thread.clear();
      emit changeViewer();
    }
    return;
  }
}

void ScreenDirectory::onMousePressed( QEvent * event )
{
  QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);

    if ( mouseEvent->y() < _ui.height() )
  {
    // highlight interface icon
    _ui.highlightItem( mouseEvent->x(),mouseEvent->y());
  }

  if ( !m_action.startMouseAction(mouseEvent->x(), mouseEvent->y(), mouseEvent->button()) )
    return;

  _initial_scroll = _scroll_pos;
  _dragging = false;
  _zooming = false;

  //if ( mouseEvent->button() == Qt::LeftButton )
  {
    _mouse_drag = true;
    _mouse_start_x = mouseEvent->x();
    _mouse_start_y = mouseEvent->y();
    _updateScrollSpeed( mouseEvent->x(), mouseEvent->y(), true );
    _scroll_speed = 0.0;
  }
  update();
}

void ScreenDirectory::onMouseReleased( QEvent * event )
{
  if ( !m_action.isMouseAction() )
    return;

  QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
  if ( !_dragging && !_zooming )
  {
    int x = mouseEvent->x();
    int y = mouseEvent->y();
    onTap(x,y);
  }
  if ( _dragging )
    _cineticScroll();
  _mouse_drag = _dragging = _zooming = false;
  update();
  m_action.endMouseAction();
}

void ScreenDirectory::onMouseMove( QEvent * event )
{
  QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);

  int th_width = (int)( g_config.thumb_size / 100.0 * width() );
  int th_height = th_width * 3 / 4;
  if ( g_config.thumbnails_square ) th_height = th_width;

  if ( _mouse_drag )
  {
    int dx0 = mouseEvent->x() - _mouse_start_x;
    int dy0 = mouseEvent->y() - _mouse_start_y;
    if ( abs(dy0) > g_config.drag_sensitivity
         || abs(dx0) > g_config.drag_sensitivity )
      _dragging = true;
    if ( _dragging )
    {
      _scroll_pos_dest = _initial_scroll - dy0;
      // update();
      emit startTimer();
      onTimer();
    }
  } else {
    if ( mouseEvent->y() < _ui.height() )
    {
      // highlight interface icon
      if ( _ui.highlightItem( mouseEvent->x(),mouseEvent->y() )  )
        update();
    } else {
      // select picture under mouse
      int column = mouseEvent->x() / th_width;
      int row = ( mouseEvent->y() + _scroll_pos - _ui.height() ) / ( th_height + _image_name_height );
      int pos = row * _thumbs_per_row + column;
      int number_of_items = m_files.size() + _folders.size();
      if ( column < _thumbs_per_row && pos >= 0 && pos < number_of_items )
        m_current_index = pos;
      update();
    }
        }

  _updateScrollSpeed( mouseEvent->x(), mouseEvent->y() );
}

void ScreenDirectory::onDoubleClick( QEvent * event )
{
  QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
  // nothing for now
  // use  mouseEvent->x(), mouseEvent->y()
  (void)mouseEvent;
}

void ScreenDirectory::onKeyPress( QKeyEvent * event )
{
  switch ( event->key() )
  {
    case Qt::Key_Enter:
    case Qt::Key_Return:
      if ( m_current_index < _folders.size() )
      {
        // it's a folder
        _changeToFolder( _folders[m_current_index] );
      } else {
        // it's an image
        //m_current_index -= _folders.size();
        _load_thread.clear();
        emit changeViewer();
      }
      break;
    case Qt::Key_Left:
      if ( m_current_index > 0 )
      {
        m_current_index--;
        scrollToCurrent();
        update();
      }
      break;
    case Qt::Key_Right:
      if ( m_current_index < m_files.size() + _folders.size() - 1 )
      {
        m_current_index++;
        scrollToCurrent();
        update();
      }
      break;
    case Qt::Key_Up:
      if ( m_current_index - _thumbs_per_row >= 0 )
      {
        m_current_index -= _thumbs_per_row;
        scrollToCurrent();
        update();
      }
      break;
    case Qt::Key_Down:
      if ( m_current_index + _thumbs_per_row <= m_files.size() + _folders.size() - 1 )
      {
        m_current_index += _thumbs_per_row;
        scrollToCurrent();
        update();
      }
      break;
    case Qt::Key_O:
      emit loadDir();
      break;
    case Qt::Key_U:
      _changeToFolder("..");
      break;
    case Qt::Key_Plus:
      _zoomIn();
      break;
    case Qt::Key_Minus:
      _zoomOut();
      break;
    case Qt::Key_PageUp:
      m_current_index -= _thumbs_per_row * 4;
      if ( m_current_index < 0 ) m_current_index = 0;
      scrollToCurrent();
      update();;
      break;
    case Qt::Key_PageDown:
      m_current_index += _thumbs_per_row * 4;
      if ( m_current_index > m_files.size() + _folders.size() - 1 )
        m_current_index = m_files.size() + _folders.size() - 1;
      scrollToCurrent();
      update();;
      break;

    default:
      event->ignore();
      break;
  }
}

void ScreenDirectory::onWheel( QWheelEvent * event )
{
  // change thumbnail size if ctrl is pressed
  if ( event->modifiers() & Qt::ControlModifier )
  {
    if ( event->delta() > 0 )
      _zoomIn();
    else
      _zoomOut();
    return;
  }

  // by default: scroll through thumbnails
  int delta = height() / 6;
  if ( event->delta() > 0 )
    _scroll_pos_dest -= delta;
  else
    _scroll_pos_dest += delta;
  _limitScroll();
  emit startTimer();
}

void ScreenDirectory::onTimer( void )
{
  int x_int = abs(_scroll_pos_dest - _scroll_pos);

  int delta = 1;
  double r = (double)height() * g_config.folder_view_scroll_speed;
  double x = (double)x_int;
  double r_fixed = r * 0.05;
  double r_var = r * 0.95;
  double r_part = 2;
  if ( x < r_var * r_part )
  {
    double xp = x / r_part;
    double y = sqrt( r_var*r_var - xp*xp );
    delta = (int)( r_fixed + r_var - y );
  } else {
    delta = (int)r;
  }
  if ( delta < 1 ) delta = 1;
  //printf("x=%d r=%d delta=%d\n", x_int, (int)r, delta );

  bool change = true;
  if ( x_int < delta )
  {
    if ( _scroll_pos != _scroll_pos_dest )
      _scroll_pos = _scroll_pos_dest;
    else
      change = false;
  } else if ( _scroll_pos_dest > _scroll_pos )
    _scroll_pos += delta;
  else
    _scroll_pos -= delta;

  // also trigger a change if the load thread has loaded a new image
  int img_loaded_count = 0;
  for ( int i = 0; i < m_files.size(); i++ )
  {
    if ( _thumbs[i] != NULL )
      img_loaded_count ++;
  }
  if ( img_loaded_count != _last_number_of_loaded_images)
  {
    _last_number_of_loaded_images = img_loaded_count;
    change = true;
  }

  if ( change )
    update();
}

void ScreenDirectory::onSettingsChanged( void )
{
        _loadIcons();
  _updateThumbsLocations();
  _resetUserActionsParameters();
  scrollToCurrent();
}

/*******************************************************************************
* PUBLIC METHODS
*******************************************************************************/

void ScreenDirectory::scrollToCurrent( void )
{
  int th_width = (int)( g_config.thumb_size / 100.0 * width() );
  int th_height = th_width * 3 / 4;
  if ( g_config.thumbnails_square )
    th_height = th_width;
  th_height += _image_name_height;
  int y = th_height
    * ( m_current_index / _thumbs_per_row ) - _scroll_pos_dest;

  if ( y < 0 )
    _scroll_pos_dest = _scroll_pos_dest + y;
  if ( y > height() - th_height - _ui.height() )
    _scroll_pos_dest = _scroll_pos_dest + ( y - height() + th_height + _ui.height() );

  _limitScroll();
  _scroll_pos = _scroll_pos_dest;
}

QPoint ScreenDirectory::itemPosition(int i)
{
  int th_width = (int)( g_config.thumb_size / 100.0 * width() );
  int th_height = th_width * 3 / 4;
  // top-left corner of thumb area
  int x = th_width * ( i % _thumbs_per_row );
  int y = _ui.height()
    + ( th_height + _image_name_height ) * ( i / _thumbs_per_row )
    - _scroll_pos;
  return QPoint(x-width()/2+th_width/2,y-height()/2+th_height/2);
}

QPoint ScreenDirectory::currentItemPosition()
{
  int index = m_current_index;
  // if(g_config.show_folders) index += _folders.size();
  return itemPosition(index);
}

void ScreenDirectory::changeIndex(int index)
{
  m_current_index = index + _folders.size();
}

/*******************************************************************************
* PRIVATE METHODS
*******************************************************************************/

void ScreenDirectory::_handleTouchAction( TouchUI::UIAction action )
{
  switch ( action )
  {
        case TouchUI::TOUCH_ACTION_EXIT:
      emit closeOnTouch();
      break;
        case TouchUI::TOUCH_ACTION_FULLSCREEN:
      emit changeFullscreen();
      break;
        case TouchUI::TOUCH_ACTION_OPEN:
      emit loadDir();
      break;
        case TouchUI::TOUCH_ACTION_THUMBS:
      _load_thread.clear();
      emit changeViewer();
      break;
        case TouchUI::TOUCH_ACTION_CONFIG:
      emit config();
      break;
        case TouchUI::TOUCH_ACTION_UP:
      _changeToFolder("..");
      break;
    default:
      break;
  }
}

void ScreenDirectory::_updateThumbsLocations( void )
{
  if ( g_config.thumbnails_text_under_image && g_config.thumbnails_show_name )
    _image_name_height = _computeImageNameHeight();
  else
    _image_name_height = 0;

  int old_pos = _scroll_pos_dest;
  int old_height = _total_height;
  _thumbs_per_row = (int)( 100.0 / g_config.thumb_size );
  int th_width = (int)( g_config.thumb_size / 100.0 * width() );
  int th_height = th_width * 3 / 4;
  if ( g_config.thumbnails_square )
    th_height = th_width;

  int number_of_items = m_files.size() + _folders.size();
  //int number_of_images = m_files.size();
  int rows = number_of_items / _thumbs_per_row
    + ( ( number_of_items % _thumbs_per_row != 0 ) ? 1 : 0 );
  int row_height = th_height + _image_name_height;
  _total_height = rows * row_height;

  int new_scroll = 0;
    if ( old_height > 0 )
        new_scroll = old_pos * _total_height / old_height;
  _scroll_pos = _scroll_pos_dest = new_scroll;
    _limitScroll();
}

void ScreenDirectory::_limitScroll( int & scroll )
{
  int scroll_height = _total_height - height() + _ui.height();
  if ( scroll > scroll_height )
    scroll = scroll_height;
  if ( scroll < 0 )
    scroll = 0;
}

void ScreenDirectory::_limitScroll( void )
{
  _limitScroll( _scroll_pos_dest );
}

void ScreenDirectory::_loadIcons( void )
{
        if ( g_config.background_color.value() < 128 )
  {
                _scroll_indicator.load( g_config.install_dir + "/icons/scroll-indicator.svg" );
          _folder_icon.load( g_config.install_dir + "/images/folder.svg" );
        } else {
                _scroll_indicator.load( g_config.install_dir + "/icons/scroll-indicator-dark.svg" );
          _folder_icon.load( g_config.install_dir + "/images/folder-dark.svg" );
        }
}

void ScreenDirectory::_clearThumbs( void )
{
  if ( _thumbs == NULL ) return;
  for ( int i = 0; i < m_files.size(); i++ )
    delete _thumbs[i];
  delete[] _thumbs;
  _thumbs = NULL;
}

void ScreenDirectory::_addThumbnailToLoad( const QString name, QImage ** location, int priority )
{
  ImageLoadItem ili;
  ili.name = name;
  ili.destination = location;
  //ili.w = width() / 3;
  //ili.h = height() / 3;
  ili.w = 512;
  ili.h = 512;
  ili.priority = priority;
  ili.force_fit_in_size = true;
  _load_thread.addLoadImage(ili);
}

void ScreenDirectory::_resetUserActionsParameters( void )
{
  _mouse_start_x = _mouse_start_y = 0;
  _last_mouse_x = _last_mouse_y = 0;
  _mouse_drag = false;
  _scroll_speed = 0.0;

  _initial_scroll = _scroll_pos;
  _initial_thumb_size = g_config.thumb_size;
  _zooming = _dragging = false;
  _two_fingers = false;

  m_action.endMouseAction();
  m_action.endTouchAction();
}

void ScreenDirectory::_resetTouchParams( void )
{
  _initial_scroll = _scroll_pos;
  _initial_thumb_size = g_config.thumb_size;
  _zooming = _dragging = false;
  _two_fingers = false;
}

void ScreenDirectory::_updateScrollSpeed( int x, int y, bool reset )
{
  QDateTime dt = QDateTime::currentDateTime();
  if ( reset )
  {
    _scroll_speed = 0.0;
  } else {
    double mouse_distance = (double)( y - _last_mouse_y );
    double time_dif = (double)_last_mouse_time.msecsTo(dt);
    if ( time_dif > 200.0 ) time_dif = 200.0;
    if ( time_dif < 1.0 ) time_dif = 1.0;
    double new_speed = mouse_distance / time_dif;
        if ( m_action.isTouchAction() )
            new_speed *= 5; // touch gestures are slower than mouse movements!?!

    double k = g_config.folder_scroll_average_speed_coef;
    if ( fabs(_scroll_speed) < 0.1 )
      _scroll_speed = new_speed;
    else
      _scroll_speed = k * _scroll_speed + ( 1.0 - k ) * new_speed;
  }
  _last_mouse_x = x;
  _last_mouse_y = y;
  _last_mouse_time = dt;
}

void ScreenDirectory::_cineticScroll( void )
{
  if ( fabs(_scroll_speed) > 0.5 )
    _scroll_pos_dest -= _scroll_speed
      * g_config.folder_view_scroll_speed * height() * g_config.folder_scroll_inertia;
  _limitScroll();
}

void ScreenDirectory::_changeToFolder( QString name )
{
  QDir old_dir(m_dir_name);
  QDir my_dir(m_dir_name);
  if ( name != ".." )
    my_dir.cd(name);
  else
    my_dir.cdUp();
  loadFiles( my_dir.absolutePath(), "" );

  if ( g_config.show_folders )
  {
    int number_of_items = _folders.size();
    for ( int i = 0; i < number_of_items; i++ )
    {
      if ( _folders[i] == old_dir.dirName() )
      {
        m_current_index = i;
        break;
      }
    }
  }

  scrollToCurrent();
  update();
}

QString ScreenDirectory::getCurrentFile( void )
{
  int index = m_current_index - _folders.size();
  if ( index < m_files.size() && index > -1)
    return m_files.at( index );
  return QString("");
}

void ScreenDirectory::_zoomIn( void )
{
  g_config.thumb_size = g_config.thumb_size + g_config.thumb_zoom_ratio;

  // limit zoom
  if ( g_config.thumb_size > g_config.thumb_zoom_max )
    g_config.thumb_size = g_config.thumb_zoom_max;
  if ( g_config.thumb_size < g_config.thumb_zoom_min )
    g_config.thumb_size = g_config.thumb_zoom_min;

  _updateThumbsLocations();
  _limitScroll();
  update();
}

void ScreenDirectory::_zoomOut( void )
{
  g_config.thumb_size = g_config.thumb_size - g_config.thumb_zoom_ratio;

  // limit zoom
  if ( g_config.thumb_size > g_config.thumb_zoom_max )
    g_config.thumb_size = g_config.thumb_zoom_max;
  if ( g_config.thumb_size < g_config.thumb_zoom_min )
    g_config.thumb_size = g_config.thumb_zoom_min;

  _updateThumbsLocations();
  _limitScroll();
  update();
}

int ScreenDirectory::_computeImageNameHeight( void )
{
        return TouchUI::scaleUI(20);
}
