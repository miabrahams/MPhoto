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
    if ( m_mouse_started == Qt::NoButton )
	{
        m_x = x;
        m_y = y;
        m_touch_started = true;
        m_last_touch_start_time.start();
	} else {
        m_touch_started = false;
	}
    return m_touch_started;
}


void TouchMouseControl::endTouchAction()
{
    m_touch_started = false;
    m_last_touch_release_time.start();
}

bool TouchMouseControl::startMouseAction( int x, int y, Qt::MouseButton button )
{
	// don't start mouse action if touch action is active
    if ( !m_touch_started
        && (!g_config.multitouch || abs_i(x-m_x) > 1 || abs_i(y-m_y) > 1 ) )
	{
        m_mouse_started = button;
	} else {
        m_mouse_started = Qt::NoButton;
	}

    // Block mouse events for a bit after touch action has started.
    qint64 time_diff = m_last_touch_release_time.elapsed();
	if ( time_diff < 40 )
        m_mouse_started = Qt::NoButton;

    return m_mouse_started != Qt::NoButton;
}



void TouchMouseControl::endMouseAction( void )
{
    m_mouse_started = Qt::NoButton;
}
