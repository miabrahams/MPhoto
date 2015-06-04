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

#ifndef __TOUCHMOUSECONTROL_H__
#define __TOUCHMOUSECONTROL_H__

#include <Qt>
#include <QDateTime>

class TouchMouseControl
{
private:
	int _x,_y;
	bool _touch_started;
	Qt::MouseButton _mouse_started;
	QDateTime _last_touch_time;

public:
	inline TouchMouseControl( void )
	{
		_x = _y = -1;
		_touch_started = false;
		_mouse_started = Qt::NoButton;
		_last_touch_time.setTime_t(0);
	}

public:

	bool startTouchAction( int x, int y );
	bool startMouseAction( int x, int y, Qt::MouseButton button );

public:

	inline void endTouchAction( void )
	{
		_touch_started = false;
		_last_touch_time = QDateTime::currentDateTime();
	}

	inline bool isTouchAction( void )
	{
		return _touch_started;
	}

	inline void endMouseAction( void )
	{
		_mouse_started = Qt::NoButton;
	}

	inline bool isMouseAction( void )
	{
		return _mouse_started != Qt::NoButton;
	}

	inline void copyParameters( TouchMouseControl & other )
	{
		_x = other._x;
		_y = other._y;
	}
};

#endif // TOUCHMOUSECONTROL_H
