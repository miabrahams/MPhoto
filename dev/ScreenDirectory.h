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

#ifndef SCREENDIRECTORY_H
#define SCREENDIRECTORY_H

#include <QtSvg/QSvgRenderer>
#include "ScreenBase.h"
#include "ImageLoadThread.h"
#include "TouchUI.h"

/**
 * UI state for browsing files.
 */

class ScreenDirectory : public ScreenBase
{
Q_OBJECT

private:

	QImage ** _thumbs;
	QStringList _folders;

	int _total_height;
	int _scroll_pos;
	int _scroll_pos_dest;
	int _thumbs_per_row;
	int _image_name_height;

	int _initial_scroll;
	double _initial_thumb_size;
	bool _dragging,_zooming;
	bool _two_fingers;
	
	ImageLoadThread _load_thread;
	int _last_number_of_loaded_images;

	TouchUI _ui;
	QSvgRenderer _scroll_indicator;
	QSvgRenderer _folder_icon;

	int _mouse_start_x;
	int _mouse_start_y;
	bool _mouse_drag;
	int _last_mouse_x;
	int _last_mouse_y;
	QDateTime _last_mouse_time;
	double _scroll_speed;
	
public:

	ScreenDirectory( void );
	~ScreenDirectory( void );

public:

	void onSetFiles( QString dir_name, QStringList files, QString current );
	void onPaint( QPainter & painter );
	void onResize( void );
	void onKeyPress( QKeyEvent * event );
	void onWheel( QWheelEvent * event );
	void onTimer( void );
	
	bool onEvent(QEvent *event);
	void onSettingsChanged( void );

	void onDrag( const QTouchEvent::TouchPoint & point, bool end );
	void onTwoFingers( const QTouchEvent::TouchPoint tp0,
		const QTouchEvent::TouchPoint tp1 );
	void onTap( int x, int y );
	
	void onMousePressed( QEvent * event );
	void onMouseReleased( QEvent * event );
	void onMouseMove( QEvent * event );
	void onDoubleClick( QEvent * event );

public:

	void scrollToCurrent( void );
	QPoint itemPosition( int i );
	QPoint currentItemPosition( void );
	QString getCurrentFile( void );
	void changeIndex( int );

private:

    void _handleTouchAction( TouchUI::UIAction action );
	void _updateThumbsLocations( void );
	void _limitScroll( void );
	void _limitScroll( int & scroll );
	void _loadIcons( void );
	void _clearThumbs( void );
	void _addThumbnailToLoad( const QString name, QImage ** location,
		int priority = 0 );
	void _resetUserActionsParameters( void );
	void _resetTouchParams( void );
	void _updateScrollSpeed( int x, int y, bool reset = false );
	void _cineticScroll( void );
	void _changeToFolder( QString name );
	void _zoomIn( void );
	void _zoomOut( void );
	int _computeImageNameHeight( void );
};

#endif // SCREENDIRECTORY_H
