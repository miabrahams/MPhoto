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

#ifndef TRASHCAN_H
#define TRASHCAN_H

 #include <QString>

bool sendToTrash( QString dirname, QString filename );


#ifdef Q_OS_LINUX

bool sendToTrashKDE( QString dirname, QString filename );
bool sendToTrashFreeDesktop( QString dirname, QString filename );

#elif defined Q_OS_MAC

bool sendToTrashMac( QString dirname, QString filename );

#elif defined Q_OS_WIN32

bool sendToTrashWindows( QString dirname, QString filename );

#endif


#endif // TRASHCAN_H
