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

#include "ImageLoadThread.h"
#include "Config.h"
#include <QFile>
#include <QDir>
#include <QImageReader>
#include <math.h>

void ImageLoadThread::run()
{
	while ( !_finished )
	{
		ImageLoadItem ili = _in.popWithPriority();
		if ( _finished ) break;

		if ( ili.destination != NULL )
		{
			QImage * img = _loadImage( ili.name, ili.w, ili.h, ili.force_fit_in_size );
			*ili.destination = img;
		}

		_load_mutex.lock();
		_load = _in.count();
		_load_mutex.unlock();
	}
}

QImage * ImageLoadThread::_loadImage( QString name, int area_width, int area_height, bool force_size )
{
	QString fullname = g_config.current_dir + QDir::separator() + name;
	QImage * img = NULL;

	// get exif rotation if needed
	qreal rotation = 0;
	bool mirror = false;
	if ( g_config.rotate_by_exif )
	{
		_getExifRotation( fullname, &rotation, &mirror );
	}

	if ( force_size )
	{
		//printf("Loading %s\n", fullname.toUtf8().data() );

		// create image reader
		QImageReader * reader = new QImageReader( fullname );
		QSize size = reader->size();
		int w = size.width();
		int h = size.height();

		#define INTERCHAGE_WH(w,h) { int x = w; w = h; h = x; }

		// compute the new size
		if ( (int)rotation % 180 != 0 )
			INTERCHAGE_WH(w,h);
		ImageLoadThread::fitImage(w,h, area_width, area_height, true );
		if ( (int)rotation % 180 != 0 )
			INTERCHAGE_WH(w,h);
		size.setWidth(w);
		size.setHeight(h);
		//printf("%d,%d\n", w,h);

		// actually load the image
		img = new QImage(size, reader->imageFormat());
		reader->setScaledSize( size );
		if ( !reader->read(img) )
		{
			delete reader;
			delete img;
			return NULL;
		}
		delete reader;
	} else {
			(void)area_width;
			(void)area_height;
			img = new QImage( fullname );
	}

	// rotate the image
	QImage * img2 = img;
	if ( rotation != 0.0 )
	{
		QTransform tr;
		if ( mirror )
			tr.scale(-1.0, 1.0);
		tr.rotate(rotation);
		img2 = new QImage();
		*img2 = img->transformed( tr, g_config.smooth_images ? Qt::SmoothTransformation : Qt::FastTransformation );
		delete img;
	}

	return img2;
}

qreal ImageLoadThread::_getExifRotation( QString fullname, qreal * out_rotation, bool * out_mirror )
{
	// verification
	if ( out_mirror == NULL || out_rotation == NULL )
		return 0.0;

	// only check exif for jpg files
	if ( !fullname.endsWith(".jpg", Qt::CaseInsensitive) && !fullname.endsWith(".jpeg", Qt::CaseInsensitive) )
		return 0.0;

	// try to load exif orientation
	QFile f( fullname );
	quint16 orientation = 1;
	if ( f.open(QIODevice::ReadOnly))
	{
		// printf("File opened %s\n", fullname.toUtf8().data());
		// find exif start marker

		unsigned short w;
		bool in_exif = false;
		quint16 type = 0;
		quint32 len = 0;
		quint8 last_byte, current_byte;
		bool full_word = false;
		while ( f.read((char*)&current_byte, sizeof(current_byte)) )
		{
			if ( !full_word )
			{
				last_byte = current_byte;
				full_word = true;
				continue;
			}
			w = ( current_byte << 8 ) | last_byte;
			if ( in_exif )
			{
				if ( current_byte == 0xFF )
				{
					in_exif = false;
					full_word = false;
				} else if ( w == 0x0112 ) {
					// HACK
					f.read((char*)&type, sizeof(type)) > 0
						&& f.read((char*)&len, sizeof(len)) > 0
						&& f.read((char*)&orientation, sizeof(orientation)) > 0;
					break;
				} else if ( w == 0x1201 ) {
					f.read((char*)&type, sizeof(type)) > 0
						&& f.read((char*)&len, sizeof(len)) > 0
						&& f.read((char*)&orientation, sizeof(orientation)) > 0;
					orientation = ( ( orientation >> 8 ) & 0xFF ) | ( ( orientation << 8 ) & 0xFF00 );
					break;
				}
			} else {
				if ( w == 0xE1FF || w == 0xFFE1 )
				{
					in_exif = true;
					full_word = false;
				}
			}
			last_byte = current_byte;
		}

		f.close();
	}

	qreal rotation = 0;
	bool mirror = false;
	switch ( orientation )
	{
		case 1: mirror = false; rotation =   0; break;
		case 2: mirror =  true; rotation =   0; break;
		case 3: mirror = false; rotation = 180; break;
		case 4: mirror =  true; rotation = 180; break;
		case 5: mirror =  true; rotation = -90; break;
		case 6: mirror = false; rotation =  90; break;
		case 7: mirror =  true; rotation =  90; break;
		case 8: mirror = false; rotation = -90; break;
		default: break;
	}
	
	/*
	printf("Rotation %u-> rotation=%d mirror=%s\n",
		(unsigned int)orientation, (int)rotation, (mirror?"true":"false"));
	*/

	*out_rotation = rotation;
	*out_mirror = mirror;
	return rotation;
}

void ImageLoadThread::fitImage( int & w, int & h, int fitw, int fith, bool shrink_only )
{
	double ratio_w = (double)fitw / (double)w;
	double ratio_h = (double)fith / (double)h;
	double ratio = ( ratio_w < ratio_h ? ratio_w : ratio_h );
	if ( shrink_only && ratio > 1.0 )
		ratio = 1.0;
	w = (int)( (double)w * ratio );
	h = (int)( (double)h * ratio );
}
