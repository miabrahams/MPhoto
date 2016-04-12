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
#include <QFileDialog>
#include <stdio.h>

#include "Config.h"
#include "ScreenSettings.h"

ScreenSettings::ScreenSettings()
    : ScreenBase()
{
	g_config.disable_settings_dialog = true;
	_total_height = 0;
	_scroll_pos = 0;
	_scroll_pos_dest = 0;

	_resetUserActionsParameters();

    _ui.addAction( TouchUI::TOUCH_ACTION_CONFIRM, "confirm.svg" );
    _ui.addAction( TouchUI::TOUCH_ACTION_CANCEL, "cancel.svg" );
	_ui.addSeparator();
    _ui.addAction( TouchUI::TOUCH_ACTION_FULLSCREEN, "view-fullscreen.svg" );
	if ( g_config.background_color.value() < 128 )
	{
		_check.load( g_config.install_dir + "/icons/check.svg" );
		_uncheck.load( g_config.install_dir + "/icons/uncheck.svg" );
		_scroll_indicator.load( g_config.install_dir + "/icons/scroll-indicator.svg" );
	} else {
		_check.load( g_config.install_dir + "/icons/check_dark.svg" );
		_uncheck.load( g_config.install_dir + "/icons/uncheck_dark.svg" );
		_scroll_indicator.load( g_config.install_dir + "/icons/scroll-indicator-dark.svg" );
	}
	_y_spacing = TouchUI::scaleUI( _check.defaultSize().height()/6 );

	_items.append( new SettingItem( tr("General") ) );
	_items.append( new SettingItem( "show_file_name",
					tr("show name"), g_config.show_file_name ) );
	_items.append( new SettingItem( "show_extra_buttons",
					tr("show extra buttons"), g_config.show_extra_buttons ) );
	_items.append( new SettingItem( "always_show_scroll",
					tr("always show scroll indicator"), g_config.always_show_scroll ) );
	_items.append( new SettingItem( "smooth_images",
					tr("smooth images"), g_config.smooth_images ) );
	_items.append( new SettingItem( "disable_animations",
					tr("disable animations"), g_config.disable_animations ) );
	_items.append( new SettingItem( "allow_rotation",
					tr("allow rotation"), g_config.allow_rotation ) );

	_items.append( new SettingItem( tr("Startup") ) );
	_items.append( new SettingItem( "start_with_empty_folder",
					tr("empty screen"), !g_config.start_with_last_folder && !g_config.start_with_custom_folder, 2 ) );
	_items.append( new SettingItem( "start_with_last_folder",
					tr("last folder"),  g_config.start_with_last_folder, 2 ) );
	_items.append( new SettingItem( "start_with_current_folder",
					tr("current folder"),
					!g_config.start_with_last_folder && g_config.start_with_custom_folder && g_config.last_open_dir == g_config.start_dir, 2 ) );

	_items.append( new SettingItem( tr("Filename Position") ) );
	_items.append( new SettingItem( "file_name_position0",
					tr("top-left"), g_config.file_name_position == 0, 3 ) );
	_items.append( new SettingItem( "file_name_position1",
					tr("top-center"), g_config.file_name_position == 1, 3 ) );
	_items.append( new SettingItem( "file_name_position2",
					tr("top-right"), g_config.file_name_position == 2, 3 ) );
	_items.append( new SettingItem( "file_name_position3",
					tr("bottom-left"), g_config.file_name_position == 3, 3 ) );
	_items.append( new SettingItem( "file_name_position4",
					tr("bottom-center"), g_config.file_name_position == 4, 3 ) );
	_items.append( new SettingItem( "file_name_position5",
					tr("bottom-right"), g_config.file_name_position == 5, 3 ) );

	_items.append( new SettingItem( tr("Transition Speed") ) );
	_items.append( new SettingItem( "image_scroll_speed10000",
					tr("no transition"), g_config.image_scroll_speed==10000, 1 ) );
	_items.append( new SettingItem( "image_scroll_speed30",
					tr("slow"), g_config.image_scroll_speed==30, 1 ) );
	_items.append( new SettingItem( "image_scroll_speed50",
					tr("normal"), g_config.image_scroll_speed==50, 1 ) );
	_items.append( new SettingItem( "image_scroll_speed100",
					tr("fast"), g_config.image_scroll_speed==100, 1 ) );
	_items.append( new SettingItem( "image_scroll_speed200",
					tr("very fast"), g_config.image_scroll_speed==200, 1 ) );

	_items.append( new SettingItem( tr("Folder View") ) );
	_items.append( new SettingItem( "show_folders",
					tr("show folders"), g_config.show_folders ) );
	_items.append( new SettingItem( "thumbnails_square",
					tr("square thumbnails"), g_config.thumbnails_square ) );
	_items.append( new SettingItem( "thumbnails_show_name",
					tr("show thumbnail name"), g_config.thumbnails_show_name ) );
	_items.append( new SettingItem( "thumbnails_text_under_image",
					tr("text under thumbnails"), g_config.thumbnails_text_under_image ) );
	_items.append( new SettingItem( "thumbnails_crop",
					tr("crop thumbnails"), g_config.thumbnails_crop ) );
	_items.append( new SettingItem( "thumbnails_space",
					tr("spacing between thumbnails"), g_config.thumbnails_space ) );
	_items_are_arranged = false;

	_items.append( new SettingItem( tr("Colors") ) );
	bool dark_colors = g_config.background_color.value() == 0 && g_config.text_color.value() == 255;
	bool light_colors = g_config.background_color.value() == 255 && g_config.text_color.value() == 0;
	_items.append( new SettingItem( "dark_colors",
					tr("white on black"), dark_colors, 4 ) );
	_items.append( new SettingItem( "light_colors",
					tr("black on white"), light_colors, 4 ) );
}

ScreenSettings::~ScreenSettings()
{
	QList<SettingItem*>::iterator it;
	for ( it = _items.begin(); it != _items.end(); it++ )
		delete *it;
	_items.clear();
	g_config.disable_settings_dialog = false;
}

/*******************************************************************************
* EVENTS
*******************************************************************************/

void ScreenSettings::onSetFiles( QString dir_name, QStringList files, QString current )
{
	// nothing to do: not handling any files
	(void)dir_name;
	(void)files;
	(void)current;
}

void ScreenSettings::onPaint( QPainter & painter )
{
	// arranged items
	if ( !_items_are_arranged )
		_arrangeItems(painter);

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

	// draw items
	bool first_item = true;
	for ( QList<SettingItem*>::iterator it = _items.begin(); it != _items.end(); it++ )
	{
		SettingItem * si = *it;
		int x,y;

		painter.setFont( _getFont(painter, si) );
		painter.setPen( g_config.text_color );

		x = si->x1;
		y = _ui.height() + si->y1 - _scroll_pos;
		if ( si->has_checkbox )
		{
                        int check_width = TouchUI::scaleUI( _check.defaultSize().width() );
                        int check_height = TouchUI::scaleUI( _check.defaultSize().height() );
                        QSvgRenderer * check_image = ( si->checked? &_check : &_uncheck );
                        QRectF r( x,y, check_width, check_height );
			check_image->render( &painter, r );
		} else if ( !first_item )
			painter.drawLine( 10, y-_y_spacing, width()-10, y-_y_spacing );

		x = x + si->text_off_x;
		y = y + si->text_off_y;
		painter.drawText( x,y, si->text );
		first_item = false;
	}

	// draw top menu
	_ui.draw( painter );
}

void ScreenSettings::onResize( void )
{
	_items_are_arranged = false;
	update();
}

bool ScreenSettings::onEvent( QEvent * event )
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
				if ( touchPoints.count() == 1 )
				{
					const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
					onDrag( touchPoint0, event->type() == QEvent::TouchEnd );
					_updateScrollSpeed( touchPoint0.pos().x(), touchPoint0.pos().y() );
				}

				if ( touchPoints.count() >= 2 && !_dragging )
				{
					const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
					const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
					onTwoFingers( touchPoint0, touchPoint1 );

					update();
				}

				if ( event->type() == QEvent::TouchEnd )
				{
					if ( !_dragging && touchPoints.count() >= 1 )
					{
						const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
						int x = (int)touchPoint0.pos().x();
						int y = (int)touchPoint0.pos().y();
						onTap(x,y);
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

void ScreenSettings::onDrag( const QTouchEvent::TouchPoint & point, bool end )
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

void  ScreenSettings::onTwoFingers( const QTouchEvent::TouchPoint tp0,
	const QTouchEvent::TouchPoint tp1 )
{
	// no two finger gesture on this screen
	(void)tp0;
	(void)tp1;
}

void ScreenSettings::onTap( int x, int y )
{
	// ui action
	if ( y < _ui.height() )
	{
		_handleTouchAction( _ui.getAction(x,y) );
		return;
	}

	// click on item
	SettingItem * selected_item = NULL;
	y = y + _scroll_pos - _ui.height();
	for ( QList<SettingItem*>::iterator it = _items.begin(); it != _items.end(); it++ )
	{
		SettingItem * si = *it;
		if ( x > si->x1-5 && x < si->x2+5
			&& y > si->y1 && y < si->y2 && si->has_checkbox )
		{
			selected_item = si;
			break;
		}
	}

	// select item
	if ( selected_item != NULL )
	{
		// unselect other items if they are exclusive
		bool new_value = !selected_item->checked;
		if ( selected_item->group > 0 )
		{
			for ( QList<SettingItem*>::iterator it = _items.begin(); it != _items.end(); it++ )
			{
				SettingItem * si = *it;
				if ( si->group == selected_item->group )
					si->checked = false;
			}
		}
		selected_item->checked = new_value;
		update();
	}
}

void ScreenSettings::onMousePressed( QEvent * event )
{
	QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);

	if ( !m_action.startMouseAction(mouseEvent->x(), mouseEvent->y(), mouseEvent->button()) )
		return;

	_initial_scroll = _scroll_pos;
	_dragging = false;

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

void ScreenSettings::onMouseReleased( QEvent * event )
{
	if ( !m_action.isMouseAction() )
		return;

	QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
	m_action.endMouseAction();

	if ( !_dragging )
	{
		int x = mouseEvent->x();
		int y = mouseEvent->y();
		_resetUserActionsParameters();
		onTap(x,y);
		return;
	}

	if ( _dragging )
		_cineticScroll();
	_resetUserActionsParameters();
	update();

}

void ScreenSettings::onMouseMove( QEvent * event )
{
	QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);

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
	} else if ( mouseEvent->y() > _ui.height() ) {
		// TODO: highlight some item
		update();
	} else {
		// highlight interface icon
		if ( _ui.highlightItem( mouseEvent->x(),mouseEvent->y() ) )
			update();
	}

	_updateScrollSpeed( mouseEvent->x(), mouseEvent->y() );
}

void ScreenSettings::onDoubleClick( QEvent * event )
{
	QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
	// nothing for now
	// use  mouseEvent->x(), mouseEvent->y()
	(void)mouseEvent;
}

void ScreenSettings::onKeyPress( QKeyEvent * event )
{
	switch ( event->key() )
	{
		case Qt::Key_Enter:
		case Qt::Key_Return:
			emit changeViewer();
			break;
		default:
			event->ignore();
			break;
	}
}

void ScreenSettings::onWheel( QWheelEvent * event )
{
	int delta = height() / 6;
	if ( event->delta() > 0 )
		_scroll_pos_dest -= delta;
	else
		_scroll_pos_dest += delta;
	_limitScroll();
	emit startTimer();
}

void ScreenSettings::onTimer( void )
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
	
	if ( change ) 
		update();
}

void ScreenSettings::onSettingsChanged( void )
{
	// nothing to do
	// settings are changed right from this screen
}

/*******************************************************************************
* PUBLIC METHODS
*******************************************************************************/

// none

/*******************************************************************************
* PRIVATE METHODS
*******************************************************************************/

void ScreenSettings::_handleTouchAction( TouchUI::UIAction action )
{
	switch ( action )
	{
        case TouchUI::TOUCH_ACTION_EXIT:
            emit closeOnTouch();
            break;
        case TouchUI::TOUCH_ACTION_CONFIRM:
			_applySettings();
			emit changeViewer();
			break;
        case TouchUI::TOUCH_ACTION_CANCEL:
			emit changeViewer();
			break;
        case TouchUI::TOUCH_ACTION_FULLSCREEN:
			emit changeFullscreen();
			break;
		default:
			break;
	}
}

void ScreenSettings::_limitScroll( int & scroll )
{
	int scroll_height = _total_height - height() + _ui.height();
	if ( scroll > scroll_height )
		scroll = scroll_height;
	if ( scroll < 0 )
		scroll = 0;
}

void ScreenSettings::_limitScroll( void )
{
	_limitScroll( _scroll_pos_dest );
}

void ScreenSettings::_resetUserActionsParameters( void )
{
	_mouse_start_x = _mouse_start_y = 0;
	_last_mouse_x = _last_mouse_y = 0;
	_mouse_drag = false;
	_scroll_speed = 0.0;

	_initial_scroll = _scroll_pos;
	_dragging = false;

	m_action.endMouseAction();
	m_action.endTouchAction();
}

void ScreenSettings::_resetTouchParams( void )
{
	_initial_scroll = _scroll_pos;
	_dragging = false;
}

void ScreenSettings::_updateScrollSpeed( int x, int y, bool reset )
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

void ScreenSettings::_cineticScroll( void )
{
	if ( fabs(_scroll_speed) > 0.5 )
		_scroll_pos_dest -= _scroll_speed
			* g_config.folder_view_scroll_speed * height() * g_config.folder_scroll_inertia;
	_limitScroll();
}

QFont ScreenSettings::_getFont( QPainter &painter, SettingItem * item )
{
	QFont font = painter.font();
	font.setPixelSize( TouchUI::scaleUI(_check.defaultSize().height())/2 );
	font.setBold( !item->has_checkbox );
	return font;
}

void ScreenSettings::_arrangeItems( QPainter &painter )
{
	int current_y = 0;
	_total_height = 0;
	SettingItem * previous_item = NULL;
	for ( QList<SettingItem*>::iterator it = _items.begin(); it != _items.end(); it++ )
	{
		SettingItem * si = *it;

		QFont font = _getFont( painter, si );
		QFontMetrics fm( font );
		QRect text_rect = fm.boundingRect(si->text);
		int check_width = TouchUI::scaleUI( _check.defaultSize().width() );
		int check_height = TouchUI::scaleUI( _check.defaultSize().height() );

		if ( !si->has_checkbox )
		{
			current_y = _total_height = _total_height + _y_spacing*2;
			previous_item = NULL;
		}
		si->width = check_width + 10 + text_rect.width();
		si->height = check_height;
		if ( previous_item == NULL || ( previous_item->x2 + check_width*2 + si->width > width() ) )
		{
			si->x1 = 10;
			si->x2 = si->x1 + si->width;
			si->y1 = _total_height;
			si->y2 = _total_height + check_height;
			current_y = si->y1;
		} else {
			si->x1 = previous_item->x2 + check_width;
			si->x2 = si->x1 + si->width;
			si->y1 = current_y;
			si->y2 = current_y + check_height;
		}
		si->text_off_x = ( si->has_checkbox ? (check_width + 10) : 0 );
		si->text_off_y = ( check_height + text_rect.height() ) / 2;
		_total_height = si->y2 + _y_spacing;
		if ( !si->has_checkbox )
		{
			current_y = _total_height;
			previous_item = NULL;
		} else {
			previous_item = si;
		}

	}
	_items_are_arranged = true;
}

bool ScreenSettings::_getValue( QString id )
{
	for ( QList<SettingItem*>::iterator it = _items.begin(); it != _items.end(); it++ )
	{
		SettingItem * si = *it;
		if ( si->id == id )
			return si->checked;
	}
	return false;
}

void ScreenSettings::_applySettings( void )
{
	// general
	g_config.show_file_name = _getValue( "show_file_name" );
	g_config.show_extra_buttons = _getValue( "show_extra_buttons" );
	g_config.always_show_scroll = _getValue( "always_show_scroll" );
	g_config.smooth_images = _getValue( "smooth_images" );
	g_config.disable_animations = _getValue( "disable_animations" );	
	g_config.allow_rotation = _getValue( "allow_rotation" );
	
	// startup
	if ( _getValue( "start_with_empty_folder" ) )
	{
		g_config.start_with_last_folder = false;
		g_config.start_with_custom_folder = false;
	} else if ( _getValue( "start_with_last_folder" ) ) {
		g_config.start_with_last_folder = true;
		g_config.start_with_custom_folder = false;
	} else if ( _getValue( "start_with_current_folder" ) ) {
		g_config.start_with_last_folder = false;
		g_config.start_with_custom_folder = true;
		g_config.start_dir = g_config.last_open_dir;
	}

	// filename positon
	if ( _getValue( "file_name_position5" ) )
		g_config.file_name_position = 5;
	else if ( _getValue( "file_name_position4" ) )
		g_config.file_name_position = 4;
	else if ( _getValue( "file_name_position3" ) )
		g_config.file_name_position = 3;
	else if ( _getValue( "file_name_position2" ) )
		g_config.file_name_position = 2;
	else if ( _getValue( "file_name_position1" ) )
		g_config.file_name_position = 1;
	else if ( _getValue( "file_name_position0" ) )
		g_config.file_name_position = 0;

	// transiton speed
	if ( _getValue( "image_scroll_speed10000" ) )
		g_config.image_scroll_speed = 10000;
	else if ( _getValue( "image_scroll_speed30" ) )
		g_config.image_scroll_speed = 30;
	else if ( _getValue( "image_scroll_speed50" ) )
		g_config.image_scroll_speed = 50;
	else if ( _getValue( "image_scroll_speed100" ) )
		g_config.image_scroll_speed = 100;
	else if ( _getValue( "image_scroll_speed200" ) )
		g_config.image_scroll_speed = 200;

	// folder view
	g_config.show_folders = _getValue( "show_folders" );
	g_config.thumbnails_square = _getValue( "thumbnails_square" );
	g_config.thumbnails_text_under_image = _getValue( "thumbnails_text_under_image" );
	g_config.thumbnails_crop = _getValue( "thumbnails_crop" );
	g_config.thumbnails_space = _getValue( "thumbnails_space" );
	g_config.thumbnails_show_name = _getValue( "thumbnails_show_name" );

	// colors
	if ( _getValue("dark_colors") )
	{
		g_config.background_color = QColor(0,0,0);
		g_config.text_color = QColor(255,255,255);
	} else if ( _getValue("light_colors") ) {
		g_config.background_color = QColor(255,255,255);
		g_config.text_color = QColor(0,0,0);
	}

}

