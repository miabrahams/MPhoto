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

#include "ImageLoadQueue.h"

// synchronized against all other methods
ImageLoadItem ImageLoadQueue::pop ( void )
{
	ImageLoadItem x;
	_sem.acquire();
	_mutex.lock();
	x = QQueue<ImageLoadItem>::front();
	QQueue<ImageLoadItem>::pop_front();
	_mutex.unlock();

	return x;
}

// synchronized against all other methods
ImageLoadItem ImageLoadQueue::popWithPriority ( void )
{
	ImageLoadItem x;
	_sem.acquire();
	_mutex.lock();

	QQueue<ImageLoadItem>::Iterator it;
	QQueue<ImageLoadItem>::Iterator it_max;
	int max_prio = -1;
	for ( it = this->begin(); it != this->end(); it++ )
	{
		if ( it->priority > max_prio )
		{
			max_prio =  it->priority;
			it_max = it;
		}
	}

	x = *it_max;
	this->erase( it_max );
	_mutex.unlock();

	return x;
}

// synchronized only against pop
void ImageLoadQueue::push ( const ImageLoadItem &x )
{
	_mutex.lock();
	QQueue<ImageLoadItem>::push_back(x);
	_sem.release();
	_mutex.unlock();
}

// synchronized only against pop
QList<ImageLoadItem> ImageLoadQueue::clear( void )
{
	QList<ImageLoadItem> cleared_items = QList<ImageLoadItem>();
	while ( _sem.tryAcquire() )
	{
		ImageLoadItem x;
		_mutex.lock();
		x = QQueue<ImageLoadItem>::front();
		QQueue<ImageLoadItem>::pop_front();
		_mutex.unlock();
		cleared_items.append( x );
	}
	return cleared_items;
}

void ImageLoadQueue::clearPriorities( void )
{
	_mutex.lock();
	QQueue<ImageLoadItem>::Iterator it;
	for ( it = this->begin(); it != this->end(); it++ )
	{
		it->priority = 0;
	}
	_mutex.unlock();
}

void ImageLoadQueue::updatePriority( QImage ** dest, int priority )
{
	_mutex.lock();
	QQueue<ImageLoadItem>::Iterator it;
	for ( it = this->begin(); it != this->end(); it++ )
	{
		if ( it->destination == dest )
			it->priority = priority;
	}
	_mutex.unlock();
}
