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

#ifndef SCREENSETTINGS_H
#define SCREENSETTINGS_H

#include "ScreenBase.h"
#include "ImageLoadThread.h"
#include "TouchUI.h"

struct SettingItem
{
	bool has_checkbox;
	bool checked;
	int group;
	QString id;
	QString text;
	int width,height;
	int x1,x2,y1,y2;
	int text_off_x,text_off_y;

public:
	SettingItem( QString text )
	{
		this->id = "";
		has_checkbox = false;
		checked = false;
		group = -1;
		this->text = text;
		x1 = x2 = y1 = y2 = 0;
		width = height = 0;
		text_off_x = text_off_y = 0;
	}

	SettingItem( QString id, QString text, bool value, int group = -1 )
	{
		has_checkbox = true;
		checked = value;
		this->group = group;
		this->text = text;
		this->id = id;
		x1 = x2 = y1 = y2 = 0;
		width = height = 0;
		text_off_x = text_off_y = 0;
	}
};

class ScreenSettings : public ScreenBase
{
Q_OBJECT

private:

	int _total_height;
	int _scroll_pos;
	int _scroll_pos_dest;

	int _initial_scroll;
	bool _dragging;

	TouchUI _ui;
	QSvgRenderer _scroll_indicator;
	QSvgRenderer _check,_uncheck;
	int _y_spacing;

	int _mouse_start_x;
	int _mouse_start_y;
	bool _mouse_drag;
	int _last_mouse_x;
	int _last_mouse_y;
	QDateTime _last_mouse_time;
	double _scroll_speed;

	QList<SettingItem*> _items;
	bool _items_are_arranged;

public:

	ScreenSettings( void );
	~ScreenSettings( void );

public:

	void onSetFiles( QString dir_name, QStringList files, QString current );
	void onPaint( QPainter & painter );
	void onResize( void );
	void onKeyPress( QKeyEvent * event );
	void onWheel( QWheelEvent * event );
	void onTimer( void );

	bool onEvent(QEvent *event);
	void onSettingsChanged( void );

	void onDrag( const QTouchEvent::TouchPoint & point, bool end );
	void onTwoFingers( const QTouchEvent::TouchPoint tp0,
		const QTouchEvent::TouchPoint tp1 );
	void onTap( int x, int y );

	void onMousePressed( QEvent * event );
	void onMouseReleased( QEvent * event );
	void onMouseMove( QEvent * event );
	void onDoubleClick( QEvent * event );

private:

	void _handleTouchAction( TouchUIAction action );
	void _limitScroll( void );
	void _limitScroll( int & scroll );
	void _resetUserActionsParameters( void );
	void _resetTouchParams( void );
	void _updateScrollSpeed( int x, int y, bool reset = false );
	void _cineticScroll( void );
	QFont _getFont( QPainter &painter, SettingItem * item );
	void _arrangeItems( QPainter &painter );
	bool _getValue( QString id );
	void _applySettings( void );

};

#endif // SCREENSETTINGS_H
