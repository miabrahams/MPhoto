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
#include <QTime>

/**
 * @brief Manages input strokes from the UI.
 */

class TouchMouseControl
{
private:
    int m_x = -1;
    int m_y = -1;
    bool m_touch_started = false;
    Qt::MouseButton m_mouse_started = Qt::NoButton;
    QTime m_last_touch_start_time;
    QTime m_last_touch_release_time;

public:

    bool startTouchAction( int x, int y );
    bool startMouseAction( int x, int y, Qt::MouseButton button );
    void endTouchAction( void );
    void endMouseAction( void );

public:


    inline bool isTouchAction( void )
    {
        return m_touch_started;
    }

    inline bool isMouseAction( void )
    {
        return m_mouse_started != Qt::NoButton;
    }

    inline void copyParameters( TouchMouseControl & other )
    {
        m_x = other.m_x;
        m_y = other.m_y;
    }

    inline int touchElapsedTime()
    {
        if (m_touch_started)
        {
            return m_last_touch_start_time.elapsed();
        }
        else
        {
            return -1;
        }
    }
};

#endif // TOUCHMOUSECONTROL_H
