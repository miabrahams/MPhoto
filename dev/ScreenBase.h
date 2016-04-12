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

#ifndef SCREENBASE_H
#define SCREENBASE_H

#include <QtGui>
#include <QSize>
#include "TouchMouseControl.h"

/**
 * Base class for different UI states. (Settings screen, viewer screen, file browser...)
 */


class ScreenBase : public QObject
{
Q_OBJECT

private:

	int _sb_width;
	int _sb_height;

protected:

	QString m_dir_name;
	QStringList m_files;
	int m_current_index;
	TouchMouseControl m_action;

public:

	ScreenBase( void );
	~ScreenBase( void );

public:

	// non-virtual public methods
	void loadFiles( QString dir, QString current_file );
	void reloadFiles( void );
	void changeFromOtherViewer( ScreenBase * other );

public:
	
	// pure virtual methods
	virtual void onSetFiles( QString dir_name, QStringList files, QString current ) = 0;
	virtual void onPaint( QPainter & painter ) = 0;
	virtual void onResize( void ) = 0;
	virtual void onKeyPress( QKeyEvent * event ) = 0;
	virtual void onWheel( QWheelEvent * event ) = 0;
	virtual void onTimer( void ) = 0;
	virtual bool onEvent(QEvent *event) = 0;
	virtual void onSettingsChanged( void ) = 0;

public:

	// public virtual methods
	virtual QString getCurrentFile( void );

signals:

	void updateSignal( void );
	void closeOnTouch( void );
	void changeFullscreen( void );
	void loadFile( void );
	void loadDir( void );
	void startTimer( void );
	void changeViewer( void );
	void config( void );
	void indexChanged( int );
	
public:

	inline void update( void )
	{
		emit updateSignal();
	}
	
	inline int width( void )
	{
		return _sb_width;
	}
	
	inline int height( void )
	{
		return _sb_height;
	}
	
	inline void setSize( int w, int h )
	{
		_sb_width = w;
		_sb_height = h;
	}

    inline QSize size()
    {
        return QSize(_sb_width, _sb_height);
    }
	
	inline QStringList & getFiles( void )
	{
		return m_files;
	}

	inline QString getDirName( void )
	{
		return m_dir_name;
	}
	
	inline int getCurrentFileIndex( void )
	{
		return m_current_index;
	}
};

#endif // SCREENBASE_H
