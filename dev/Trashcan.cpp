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

#include "Trashcan.h"
#include <QStringList>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include <QSet>

#ifdef Q_OS_LINUX
#include <unistd.h>
#include <sys/types.h>
#endif

#ifdef Q_OS_WIN32
#include <windows.h>
#include <shellapi.h>
#endif

bool sendToTrash(QString dirname, QString filename )
{
	// verify if the user has permissions to move/copy this file
	QFileInfo fileinfo( QDir(dirname).filePath(filename) );
	if ( !fileinfo.isWritable() || !fileinfo.isReadable() )
		return false;

#ifdef Q_OS_LINUX

	return sendToTrashFreeDesktop( dirname, filename );

#elif defined Q_OS_MAC

	return sendToTrashMac( dirname, filename );

#elif defined Q_OS_WIN32

	return sendToTrashWindows( dirname, filename );

#else

	fprintf( stderr, "[WARNING] Sending to trash not supported on this operating system." );
	return false;

#endif
}

#ifdef Q_OS_LINUX

bool sendToTrashKDE( QString dirname, QString filename )
{
	QStringList params;
	params << "move";
	params << dirname + filename;
	params << "trash://";
	QProcess process;
	process.start( "kioclient", params );
	process.waitForFinished();

	return true;
}

QString getPartitionTrashFolder( QString directory_of_deleted_file )
{
	QString trash_dir;

	// get partition root
	QString root = "";
	QString filesystem = "";
	QFile mounts_file( "/proc/mounts" );
	mounts_file.open( QIODevice::ReadOnly );
	QTextStream in(&mounts_file);
	while ( true )
	{
		QString line = in.readLine();
		if ( line.isNull() ) break;
		QStringList words = line.split(" ");
		if ( words.length() < 3 ) continue;
		QString mount_point = words[1];
		QString fs = words[2];
		if ( directory_of_deleted_file.startsWith(mount_point) && mount_point.length() > root.length() )
		{
			root = mount_point;
			filesystem = fs;
		}
	}
	mounts_file.close();
	if ( root.isEmpty() )
		return "";

	// skip remote filesystems, temporary filesystems and some fuse based filesystems
	// in fact, only allow a few known types
	QSet<QString> set;
	set << "ext2" << "ext3" << "ext4" << "btrfs" << "fat32" << "vfat" << "ntfs" << "fuse.encfs";
	if ( !set.contains(filesystem) )
		return "";

	// try the global partition trash (".Trash")
	trash_dir = root + "/.Trash";
	QDir global_trash(trash_dir);
	QFileInfo global_trash_info(trash_dir);
	if ( global_trash.exists() && global_trash_info.isWritable() && global_trash_info.isReadable() )
		return trash_dir;

	// otherwise, try the partition trash folder with the user id (".Trash-<uid>")
	uid_t uid = getuid();
	QString uid_str;
	uid_str.sprintf("%u", (unsigned int)uid);
	trash_dir = root + "/.Trash-" + uid_str;
	QDir user_trash(trash_dir);
	if ( !user_trash.exists() && !user_trash.mkpath(".") )
		return "";
	QFileInfo user_trash_info(trash_dir);
	if ( user_trash_info.isWritable() && user_trash_info.isReadable() )
		return trash_dir;

	// cannot get partition trash
	return "";
}

bool sendToTrashFreeDesktop( QString dirname, QString filename )
{
	/*
	Desktop Trash Can Specification
	http://freedesktop.org/wiki/Specifications/trash-spec/
	http://www.ramendik.ru/docs/trashspec.html
	*/

	// detect trashcan directory
	QString trash_dir;
	bool home_trash = false;
	QString home_dir = QString( qgetenv("HOME") );
	if ( home_dir.isEmpty() || !dirname.startsWith(home_dir) )
		trash_dir = getPartitionTrashFolder(dirname);
	if ( trash_dir == "" )
	{
		QString xdg_user_data( qgetenv("XDG_DATA_HOME") );
		if ( xdg_user_data.isEmpty() )
		{
			if ( !home_dir.isEmpty() )
				trash_dir = home_dir + "/.local/share/Trash";
		} else {
			trash_dir = xdg_user_data + "/Trash";
		}
		home_trash = true;
	}
	if ( trash_dir == "" )
		return false;
	QString info_dir = trash_dir + "/info/";
	QString files_dir = trash_dir + "/files/";

	// create trash directories if they don't exist
	QDir d_info( info_dir );
	if ( !d_info.exists() && !d_info.mkpath(".") )
		return false;
	QDir d_files( info_dir );
	if ( !d_files.exists() && !d_files.mkpath(".") )
		return false;

	printf("Trash dir: %s\n", trash_dir.toUtf8().data() );
	//return false;

	// get a name for the file when it is moved to trash
	// first try with it's original name
	QString new_name = filename;
	QString number;
	QFileInfo fi( files_dir + filename );
	int max_tries = 100000;
	if ( fi.exists() )
	{
		int i;
		for ( i = 0; i < max_tries; i++ )
		{
			number.sprintf("%d", i);
			new_name = filename + " " + number;
			QFileInfo nfi( files_dir + new_name );
			if ( !nfi.exists() )
				break;
		}
		if ( i == max_tries )
			// we can't find a name for the file
			return false;
	}

	// move file to trash
	QString old_path = QDir(dirname).filePath(filename);
	QString trash_path = files_dir + new_name;
	if ( !QFile::rename( old_path, trash_path ) )
		return false;

	// get date and time
	QDateTime dt = QDateTime::currentDateTime();
	QString deletion_date = dt.toString( Qt::ISODate );

	// get the restore path of the file
	// - absolute path, if when using the home trash
	// - relative path to the top directory where Trash resides, otherwise
	QString restore_path = old_path;
	if ( !home_trash )
	{
		int k = trash_dir.lastIndexOf( QDir::separator() );
		if ( k > 0 )
			restore_path = old_path.mid(k+1);
	}

	// write info file
	QFile info_file( info_dir + new_name + ".trashinfo" );
	info_file.open( QIODevice::WriteOnly );
	QTextStream out(&info_file);
	out << QString( "[Trash Info]\n" );
	out << QString( "Path=" + restore_path + "\n" );
	out << QString( "DeletionDate=" + deletion_date + "\n" );
	info_file.close();

	return true;
}

#elif defined Q_OS_MAC

bool sendToTrashMac( QString dirname, QString filename )
{
	fprintf( stderr, "[WARNING] Sending to trash is not yet implemented on Mac." );
	return false;
}

#elif defined Q_OS_WIN32

bool sendToTrashWindows( QString dirname, QString filename )
{
    bool result = false;

	// create a wchar_t buffer with the file name, null terminated
    QString name = QDir(dirname).filePath(filename);
	int length = name.length();
    wchar_t * buffer = new wchar_t[ length + 2 ];
	name.toWCharArray( buffer );
	buffer[length] = 0;
    buffer[length+1] = 0;

	// fill the file operation structure and execute the operation
	SHFILEOPSTRUCT FileOPStruct;
	FileOPStruct.hwnd = NULL;
	FileOPStruct.wFunc = FO_DELETE;
	FileOPStruct.pFrom = buffer;
	FileOPStruct.pTo = NULL;
	FileOPStruct.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_ALLOWUNDO | FOF_SILENT;
	FileOPStruct.hNameMappings = NULL;
	FileOPStruct.lpszProgressTitle = NULL;
	result = ( SHFileOperation(&FileOPStruct) == 0 );

	// clear the buffer
	delete[] buffer;

	return result;
}

#endif
