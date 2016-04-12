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

#include <QApplication>
#include <QDir>
#include <QDesktopWidget>
#include "Config.h"
#include "MainWindow.h"

void print_console_help( char * appname )
{
	printf("Usage: %s [options] [file or directory name]\n", appname );
	printf("\nAvailable options:\n");
	printf("%-20s - %s\n", "--fullscreen, -f", "start in fullscreen mode");
	printf("%-20s - %s\n", "--window", "start in a window (not in fullscreen)");
	printf("%-20s - %s\n", "--install-dir=<dir>", "get resource files from <dir> instead of the default directory");
	printf("%-20s - %s\n", "--multitouch, -m", "enable multitouch support");
	printf("%-20s - %s\n", "--no-multitouch", "disable multitouch support");
	printf("%-20s - %s\n", "--help, -h", "print this help");
	printf("%-20s - %s\n", "--version, -v", "print the version number");
}

void print_console_version( void )
{
	printf("MihPhoto - Photo viewer for multi-touch devices\n");
	printf("Copyright (C) 2010 Mihai Paslariu\n");
	printf("Version %.2lf\n", VERSION );
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.addLibraryPath( app.applicationDirPath() );
	g_config.load();
#ifdef DATADIR
  g_config.install_dir = DATADIR;
#else
  g_config.install_dir = app.applicationDirPath() + QDir::separator() + "..";
#endif
	QStringList args = app.arguments();
	QString startfile = "";
	bool fullscreen = true;

	for ( int i = 1; i < args.size(); i++ )
	{
		QString v = args[i];
		if ( v == "--fullscreen" || v == "-f" )
			fullscreen = true;
		else if ( v == "--window" )
			fullscreen = false;
		else if ( v.startsWith("--install-dir=") )
			g_config.install_dir = v.mid(14);
		else if ( v == "--multitouch" || v == "-m" )
			g_config.multitouch = true;
		else if ( v == "--no-multitouch" )
			g_config.multitouch = false;
		else if ( v == "--help" || v == "-h" )
		{
			print_console_help(argv[0]);
			return 0;
		} else if ( v == "--version" || v == "-v" ) {
			print_console_version();
			return 0;
		} else if ( i == args.size() - 1 ) {
			startfile = v;
		} else {
			printf("WARNING: Unknown argument '%s'\n", v.toUtf8().data());
		}
	}
	g_config.computeUiSize( app.desktop()->logicalDpiX() );

	MainWindow window(startfile, fullscreen);
	return app.exec();
}
