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

#ifndef __TOUCH_UI_H__
#define __TOUCH_UI_H__

#include <QPainter>
#include <QString>
#include <QImage>
#include <QVector>
#include <QtSvg/QSvgRenderer>

#include "Config.h"

/**
 * Touch UI layout engine. Allows a ScreenBase to store widgets with locations.
 * Provides a single draw call to place them as requested, and an engine to
 * retrieve the correct action to perform after the UI has been manipulated.
 */

class TouchUI
{
public:
    enum UIAction
    {
        TOUCH_ACTION_NONE,
        TOUCH_ACTION_EXIT,
        TOUCH_ACTION_FULLSCREEN,
        TOUCH_ACTION_CONFIG,
        TOUCH_ACTION_UP,

        TOUCH_ACTION_ZOOM_IN,
        TOUCH_ACTION_ZOOM_OUT,
        TOUCH_ACTION_ZOOM_FIT,
        TOUCH_ACTION_ZOOM_FIT_WIDTH,
        TOUCH_ACTION_ZOOM_FIT_HEIGHT,
        TOUCH_ACTION_ZOOM_ORIGINAL,
        TOUCH_ACTION_ROTATE_LEFT,
        TOUCH_ACTION_ROTATE_RIGHT,

        TOUCH_ACTION_GO_FIRST,
        TOUCH_ACTION_GO_LAST,
        TOUCH_ACTION_GO_NEXT,
        TOUCH_ACTION_GO_PREVIOUS,
        TOUCH_ACTION_OPEN,
        TOUCH_ACTION_THUMBS,

        TOUCH_ACTION_CONFIRM,
        TOUCH_ACTION_CANCEL
    };

    struct UIItem
    {
        UIAction action;
        int x1,y1,x2,y2;
        QSvgRenderer * image;
        bool highlighted;
    };

private:

    QVector<UIItem*> _items;
	QPixmap _background;
	int _width,_height;
	int _xoffset,_yoffset; // _yoffset is currently not used
	int _old_xoffset,_old_yoffset;
	int _screen_width,_screen_height; // taken from the last draw command

public:

	TouchUI();
	~TouchUI();

public:

	void draw( QPainter & painter );
    UIAction getAction( int x, int y );
	bool highlightItem( int x, int y );
    void addAction( UIAction a, QString icon_name );
	void addSeparator( void );
	void limitScroll( void );
	void clean( void );

public:

	inline QPixmap * getBackground( void )
	{
		return &_background;
	}
	
	inline int height( void )
	{
		return _height;
	}

	inline void startScroll( void )
	{
		_old_xoffset = _xoffset;
		_old_yoffset = _yoffset;
	}

	void scroll( int dx, int dy )
	{
		_xoffset = _old_xoffset + dx;
		_yoffset = _old_yoffset + dy;
	}

	static inline int scaleUI( int size )
	{
		return size * g_config.ui_size / 48;
	}
};

#endif
