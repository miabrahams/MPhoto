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

#include "Config.h"
#include "ScreenBase.h"

ScreenBase::ScreenBase( void ) : QObject()
{
	_sb_width = _sb_height = 0;
	m_current_index = 0;
}

ScreenBase::~ScreenBase( void )
{
	// nothing for now
}

void ScreenBase::loadFiles( QString dir, QString current_file )
{
	g_config.current_dir = dir;
	g_config.last_open_dir = dir;
	QDir myDir( dir );
	QStringList name_filters;
	name_filters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp"  << "*.tif"  << "*.gif";

	onSetFiles( dir,
		myDir.entryList( name_filters, QDir::Files | QDir::Readable, QDir::Name | QDir::IgnoreCase ),
		current_file );
}

void ScreenBase::reloadFiles( void )
{
	if ( m_current_index < m_files.size() )
		loadFiles( m_dir_name, m_files[m_current_index] );
	else if ( m_files.size() > 0 )
		loadFiles( m_dir_name, m_files[0] );
	else
		loadFiles( m_dir_name, "" );
}

void ScreenBase::changeFromOtherViewer( ScreenBase * other )
{
	if ( other == NULL ) return;

	onSetFiles( other->getDirName(),
		other->getFiles(), other->getCurrentFile() );
	m_action.copyParameters( other->m_action );
	onResize();
}

QString ScreenBase::getCurrentFile( void )
{
	if ( m_current_index < m_files.size() )
		return m_files.at( m_current_index );
	return QString("");
}
