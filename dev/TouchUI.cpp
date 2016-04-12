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

#include "TouchUI.h"
#include "Config.h"
#include "ImageLoadThread.h"
#include <stdio.h>

TouchUI::TouchUI()
{
	_background.load(g_config.install_dir + "/images/back.png" );
	_xoffset = _yoffset = _old_xoffset = _old_yoffset = 0;
	_width = _height = 0;
	_screen_width = _screen_height = 0;
}

TouchUI::~TouchUI()
{
	clean();
}

void TouchUI::draw( QPainter & painter )
{
	_screen_width = painter.window().width();
	_screen_height = painter.window().height();
	limitScroll();

	// draw background
	QRect rect(0,0,painter.window().width(),_height);
	painter.drawTiledPixmap( rect, _background );
	
	// draw icons
	for ( int i = 0; i < _items.count(); i++ )
	{
        UIItem * t = _items[i];
		int posx = t->x1;
		int posy = t->y1;
		if ( posx < 0 ) posx = _screen_width+posx;
		QSvgRenderer * image = t->image;
		if ( t->highlighted )
			painter.setCompositionMode( QPainter::CompositionMode_HardLight );
		else
			painter.setCompositionMode( QPainter::CompositionMode_SourceOver );
		if ( image == NULL ) continue;
		int h = image->defaultSize().height();
		int w = image->defaultSize().width();
		int img_width = g_config.ui_size;
		int img_height = g_config.ui_size;
		ImageLoadThread::fitImage( w,h, img_width, img_height, false );
		QRectF r( posx+_xoffset, posy+_yoffset, w, h );
		image->render( &painter, r );
	}
}

TouchUI::UIAction TouchUI::getAction( int x, int y )
{
	x -= _xoffset;
	y -= _yoffset;
	for ( int i = 0; i < _items.count(); i++ )
	{
        UIItem * t = _items[i];
		t->highlighted = false;
		int tx1 = t->x1;
		int tx2 = t->x2;
		if ( tx1 < 0 ) { tx1 = _screen_width+tx1; tx2 = _screen_width+tx2; }
		if ( x > tx1 && x < tx2 && y > t->y1 && y < t->y2 )
		{
			return t->action;
		}
	}
	return TOUCH_ACTION_NONE;
}

bool TouchUI::highlightItem( int x, int y )
{
	bool change = false;

	x -= _xoffset;
	y -= _yoffset;
	for ( int i = 0; i < _items.count(); i++ )
	{
        UIItem * t = _items[i];
		int tx1 = t->x1;
		int tx2 = t->x2;
		if ( tx1 < 0 ) { tx1 = _screen_width+tx1; tx2 = _screen_width+tx2; }
		if ( x > tx1 && x < tx2 && y > t->y1 && y < t->y2 )
		{
			if ( !t->highlighted ) change = true;
			t->highlighted = true;
		} else {
			if ( t->highlighted ) change = true;
			t->highlighted = false;
		}
	}
	
	return change;

}

void TouchUI::addAction( TouchUI::UIAction a, QString icon_name )
{
	int spacing = TouchUI::scaleUI(20);
	int left_space = spacing;
	if ( _items.count() > 0 )
		left_space += _items.back()->x2;

    TouchUI::UIItem * ta = new TouchUI::UIItem;
	ta->image = new QSvgRenderer( g_config.install_dir + "/icons/" + icon_name );
	if ( !ta->image->isValid() )
	{
		delete ta->image;
		delete ta;
		return;
	}
	
	ta->highlighted = false;
	ta->action = a;
	ta->x1 = left_space;
	ta->x2 = ta->x1 + TouchUI::scaleUI(ta->image->defaultSize().width());
	ta->y1 = spacing;
	ta->y2 = ta->y1 + TouchUI::scaleUI(ta->image->defaultSize().height());

	if ( ta->x2 > _width ) _width = ta->x2;
	if ( ta->y2 + spacing > _height ) _height = ta->y2 + spacing;

	_items.push_back(ta);
}

void TouchUI::addSeparator( void )
{
	addAction( TOUCH_ACTION_NONE, "separator.svg" );
}

void TouchUI::limitScroll( void )
{
	int min_x = - _width + _screen_width - 20;
	if ( min_x > 0 ) min_x = 0;
	if ( _xoffset < min_x ) _xoffset = min_x;

	int max_x = 0;
	if ( _xoffset > max_x ) _xoffset = max_x;
}

void TouchUI::clean( void )
{
	for ( int i = 0; i < _items.count(); i++ )
	{
		delete _items[i]->image;
		delete _items[i];
	}
	_items.clear();

	_xoffset = _yoffset = _old_xoffset = _old_yoffset = 0;
	_width = _height = 0;
	_screen_width = _screen_height = 0;
}
