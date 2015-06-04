/*******************************************************************************

MPhoto - Photo viewer for multi-touch devices
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

#ifndef IMAGEWITHINFO_H
#define IMAGEWITHINFO_H

#include <QImage>
#include <QTransform>

struct ImageWithInfo
{
	QImage * image;
	QImage * small_image;
	double zoom;
	int posx,posy;
	double rotation;

	inline ImageWithInfo( void )
	{
		image = small_image = NULL;
		zoom = 1.0;
		posx = posy = 0;
		rotation = 0.0;
	}

        inline void clear( void )
	{
		image = small_image = 0;
		posx = posy = 0;
		zoom = 1.0;
		rotation = 0.0;
	}

	inline void free( void )
	{
		delete image;
		delete small_image;
		clear();
	}

	inline void setZoom( double f )
	{
		zoom = f;
		rotation = 0.0;
		posx = posy = 0;
	}

	inline int width( void )
	{
		return image != NULL ? image->width() : 0;
	}

	inline int height( void )
	{
		return image != NULL ? image->height() : 0;
	}
};

#endif
