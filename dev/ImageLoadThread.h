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

#ifndef __IMAGE_LOAD_THREAD_H__
#define __IMAGE_LOAD_THREAD_H__

#include <QThread>
#include "ImageLoadItem.h"
#include "ImageLoadQueue.h"
#include <QImage>


class ImageLoadThread : public QThread
{
Q_OBJECT

private:

	ImageLoadQueue _in;
	int _load;
	QMutex _load_mutex;

	volatile bool _finished;

public:

	ImageLoadThread( void ) : QThread()
	{
		_finished = false;
		_load = 0;
	}

public:

	void run();

private:

	QImage * _loadImage( QString name, int area_width, int area_height, bool force_size );
	qreal _getExifRotation( QString fullname, qreal * out_rotation, bool * out_mirror );

public:

	inline ImageLoadQueue & getQueue( void )
	{
		return _in;
	}

	inline void addLoadImage( ImageLoadItem & ili )
	{
		_load_mutex.lock();
		_load++;
		_load_mutex.unlock();
		_in.push( ili );
	}

	inline bool isIdle( void )
	{
		_load_mutex.lock();
		int k = _load;
		_load_mutex.unlock();
		return k == 0;
	}

	inline void waitUntilIdle( void )
	{
		while ( !isIdle() ) msleep(60);
	}

	inline void addDummyElement( void )
	{
		ImageLoadItem ili;
		ili.destination = NULL;
		ili.w = ili.h = 0.0;
		_in.push(ili);
	}

	inline void stopThread( void )
	{
		_finished = true;
		addDummyElement();
	}
	
	inline void clear( void )
	{
		_load_mutex.lock();
		_in.clear();
		// put dummy element
		addDummyElement();
		_load_mutex.unlock();
	}

public:
	static void fitImage( int & w, int & h, int fitw, int fith, bool shrink_only );
};

#endif
