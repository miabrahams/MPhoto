/*******************************************************************************

MPhoto - Photo viewer for multi-touch devices
Copyright (C) 2010 Mihai Paslariu
Copyright (C) 2015 Michael Abrahams

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

#ifndef SCREENVIEWER_H
#define SCREENVIEWER_H

#include "ScreenBase.h"
#include "ImageLoadThread.h"
#include "TouchUI.h"
#include "ImageWithInfo.h"

enum FitZoomMode
{
	ZOOM_FIT_WIDTH,
	ZOOM_FIT_HEIGHT,
	ZOOM_FIT
};

class ScreenViewer : public ScreenBase
{
Q_OBJECT

private:

	int _drag_offset;

	ImageWithInfo _previous, _current, _next;

	bool _show_ui;
	bool _show_ui_by_tap;

	int _initial_posx,_initial_posy,_initial_drag_offset;
	double _initial_rotation,_initial_zoom;
	bool _allow_drag; // when changing from one picture to another
	bool _allow_zoom,_allow_pan; // only in single touch (or mouse) mode
	bool _two_fingers; // only on multitouch mode when a two finger operation starts
	bool _two_fingers_valid_operation; // if the finger operation is valid (inside the image)
	bool _changing; // when playing the transition animation from one image to another
	
	ImageLoadThread _load_thread;
	bool _last_load_thread_idle_state; // last idle state viewed by the timer

	TouchUI _ui;
	bool _extra_buttons;
	QSvgRenderer _zoom_in_icon,_zoom_out_icon,_zoom_indicator;

	// only for mouse mode
	int _mouse_start_x;
	int _mouse_start_y;
	bool _mouse_drag;
	
public:

	ScreenViewer( void );
	~ScreenViewer( void );
	
public slots:

	void forward( void );
	void back( void );
	void first( void );
	void last( void );
	void changeUI( void );
	void zoomIn( double speed = 0 );
	void zoomOut( double speed = 0 );
	void zoomToFit( FitZoomMode zoom_mode = ZOOM_FIT );
	void zoomToOriginalSize( void );
	void zoomToOriginalSize( int x, int y );
	void rotateLeft( void );
	void rotateRight( void );
	void moveView( int deltax, int deltay );
	void setView(QPoint pos);
	void setRotation( double angle );
	void resetView();

public:

	void onSetFiles( QString dir_name, QStringList files, QString current );
	void onPaint( QPainter & painter );
	void onResize( void );
	void onKeyPress( QKeyEvent * event );
	void onTimer( void );
	
	bool onEvent(QEvent *event);
	void onSettingsChanged( void );

	void onPan( const QTouchEvent::TouchPoint & point, bool end );
	void onTwoFingers( const QTouchEvent::TouchPoint tp0,
		const QTouchEvent::TouchPoint tp1 );
    bool onTap( int x, int y ); // return false if no action was taken for the point (x,y)
	
	void onMousePressed( QEvent * event );
	void onMouseReleased( QEvent * event );
	void onMouseMove( QEvent * event );
	void onDoubleClick( QEvent * event );
	void onWheel( QWheelEvent * event );

public:

	QPoint getView();
	double getRotation();
	double getZoom();
	double computeCurrentFitZoom();
	void setZoom(double zoom);
	bool isIdle();
	bool isReady();
	bool isBeingPinchZoomed();

signals:

	void fitImage( void );

private:

	void _resetUserActionsParameters( void );
	void _resetTouchParams( void );
	void _loadImage( int k, QImage ** dest, double * zoom_dest );
	double _computeFitZoom( QImage * img, FitZoomMode zoom_mode = ZOOM_FIT );
	double _computeFitZoomWithRotation( QImage * img, FitZoomMode zoom_mode = ZOOM_FIT );
	void _moveForward( void );
	void _moveBack( void );
	void _reloadAll( void );
	void _handleTouchAction( TouchUIAction action );
	void _limitZoom( double & zoom, ImageWithInfo & img );
	void _limitPan( void );
	QTransform _getCurrentTransform( void );
	bool _isScreenPointInsideCurrentImage( qreal x, qreal y );
	void _loadUI( void );
	void _deleteCurrentFile( void );
};

#endif // SCREENVIEWER_H
