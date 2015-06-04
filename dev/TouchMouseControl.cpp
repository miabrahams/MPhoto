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

#include "TouchMouseControl.h"
#include "Config.h"

// Redefine abs so we don't have to include anything
static inline int abs_i( int x )
{
	return ( x >= 0 ? x : -x );
}

bool TouchMouseControl::startTouchAction( int x, int y )
{
	if ( _mouse_started == Qt::NoButton )
	{
		_x = x;
		_y = y;
		_touch_started = true;
	} else {
		_touch_started = false;
	}
	return _touch_started;
}

bool TouchMouseControl::startMouseAction( int x, int y, Qt::MouseButton button )
{
	// don't start mouse action if touch action is active
	if ( !_touch_started
		&& (!g_config.multitouch || abs_i(x-_x) > 1 || abs_i(y-_y) > 1 ) )
	{
		_mouse_started = button;
	} else {
		_mouse_started = Qt::NoButton;
	}

	// don't start mouse action one second after the last touch action has finished
	qint64 time_diff = _last_touch_time.msecsTo( QDateTime::currentDateTime() );
	if ( time_diff < 40 )
		_mouse_started = Qt::NoButton;

	return _mouse_started != Qt::NoButton;
}
