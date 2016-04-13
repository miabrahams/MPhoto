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

#define __CONFIGURATION_MAIN__
#include "Config.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <stdio.h>
#include <QtCore>

Config::Config()
{
    loadDefaults();
}

void Config::loadDefaults( void )
{
    background_color.setRgb(0,0,0);
    text_color.setRgb(255,255,255);
    drag_sensitivity = 30;
    flip_distance = 200;
    min_zoom = 0.05;
    show_file_name = true;
    file_name_position = 0;
    file_name_size = 1.0f;
    show_folders = true;
    install_dir = ".";
    rotate_by_exif = true;
    zoom_speed = 0.8;
    always_show_scroll = false;
    last_open_dir = "";
    start_dir = "";
    thumb_size = 19.0;
    thumb_zoom_min = 10.0;
    thumb_zoom_max = 50.0;
    thumb_zoom_ratio = 0.5;
    start_with_last_folder = false;
    start_with_custom_folder = false;
    show_extra_buttons = false;
    thumbnails_crop = true;
    thumbnails_square = false;
    thumbnails_space = false;
    thumbnails_text_under_image = true;
    thumbnails_show_name = true;
    image_scroll_speed = 50;
    smooth_images = false;
    disable_animations = false;
    choose_ui_size = 0;
    ui_size = 48;
    allow_rotation = false;
    multitouch = true;
    max_zoom = 10.0;

    // try to guess the default image folder
    last_open_dir = QDir::homePath() + "/Pictures";
    if ( !_dirExists(last_open_dir) )
        last_open_dir = QDir::homePath();
    started_with_file = false;
    disable_settings_dialog = false;
    timer_duration = 30;
    folder_view_scroll_speed = 0.1;
    folder_scroll_inertia = 10.5;
    folder_scroll_average_speed_coef = 0.2;
}

// for saving and loading, the name of the key is the
// same as the name of the variable

bool Config::save()
{
    QString filename = QDir::homePath() + "/" + CONFIG_FILE;

    // save configuration to file
    QFile f(filename);

    if ( f.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text) )
    {
        QTextStream ts(&f);
        ts << "background_color = " << _fromColor(background_color) << "\n";
        ts << "text_color = " << _fromColor(text_color) << "\n";
        ts << "drag_sensitivity = " << drag_sensitivity << "\n";
        ts << "flip_distance = " << flip_distance << "\n";
        ts << "image_scroll_speed = " << image_scroll_speed << "\n";
        ts << "min_zoom = " << min_zoom << "\n";
        ts << "max_zoom = " << max_zoom << "\n";
        ts << "show_file_name = " << _fromBool(show_file_name) << "\n";
        ts << "file_name_position = " << file_name_position << "\n";
        ts << "file_name_size = " << file_name_size << "\n";
        ts << "show_folders = " << _fromBool(show_folders) << "\n";
        ts << "rotate_by_exif = " << _fromBool(rotate_by_exif) << "\n";
        ts << "always_show_scroll = " << _fromBool(always_show_scroll) << "\n";
        ts << "start_with_last_folder = " << _fromBool(start_with_last_folder) << "\n";
        ts << "start_with_custom_folder = " << _fromBool(start_with_custom_folder) << "\n";
        ts << "show_folders = " << _fromBool(show_folders) << "\n";
        ts << "show_extra_buttons = " << _fromBool(show_extra_buttons) << "\n";
        ts << "multitouch = " << _fromBool(multitouch) << "\n";
        ts << "zoom_speed = " << zoom_speed << "\n";
        ts << "last_dir = " << last_open_dir << "\n";
        ts << "start_dir = " << start_dir << "\n";
        ts << "thumb_zoom_ratio = " << thumb_zoom_ratio << "\n";
        ts << "thumb_size = " << thumb_size << "\n";
        ts << "thumbnails_crop = " << _fromBool(thumbnails_crop) << "\n";
        ts << "thumbnails_square = " << _fromBool(thumbnails_square) << "\n";
        ts << "thumbnails_space = " << _fromBool(thumbnails_space) << "\n";
        ts << "thumbnails_show_name = " << _fromBool(thumbnails_show_name) << "\n";
        ts << "thumbnails_text_under_image = " << _fromBool(thumbnails_text_under_image) << "\n";
        ts << "smooth_images = " << _fromBool(smooth_images) << "\n";
        ts << "disable_animations = " << _fromBool(disable_animations) << "\n";
        ts << "choose_ui_size = " << choose_ui_size << "\n";
        ts << "allow_rotation = " << allow_rotation << "\n";
        f.close();
        return true;
    }

    printf("Cannot save config file %s\n", filename.toUtf8().data());
    return false;
}

bool Config::load()
{
    QString filename = QDir::homePath() + "/" + CONFIG_FILE;

    QFile f(filename);
    if ( f.open(QIODevice::ReadOnly|QIODevice::Text) )
    {
        QTextStream ts(&f);
        while ( !ts.atEnd() )
        {
            QString line = ts.readLine(1024);
            int k = line.indexOf("=");
            if ( k < 0 ) continue;
            QString key = line.mid(0,k).trimmed();
            QString value = line.mid(k+1).trimmed();

            if ( key == "background_color" )
                background_color = _toColor(value);
            else if ( key == "text_color" )
                text_color = _toColor(value);
            else if ( key == "drag_sensitivity" )
                drag_sensitivity = value.toInt();
            else if ( key == "flip_distance" )
                flip_distance = value.toInt();
            else if ( key == "image_scroll_speed" )
                image_scroll_speed = value.toInt();
            else if ( key == "min_zoom" )
                min_zoom = value.toDouble();
            else if ( key == "show_file_name" )
                show_file_name = _toBool(value);
            else if ( key == "file_name_position" )
                file_name_position = value.toInt();
            else if ( key == "file_name_size" )
                file_name_size = (float)value.toDouble();
            else if ( key == "show_folders" )
                show_folders = _toBool(value);
            else if ( key == "show_extra_buttons" )
                show_extra_buttons = _toBool(value);
            else if ( key == "rotate_by_exif" )
                rotate_by_exif = _toBool(value);
            else if ( key == "always_show_scroll" )
                always_show_scroll = _toBool(value);
            else if ( key == "start_with_last_folder" )
                start_with_last_folder = _toBool(value);
            else if ( key == "start_with_custom_folder" )
                start_with_custom_folder = _toBool(value);
            else if ( key == "max_zoom" )
                max_zoom = value.toDouble();
            else if ( key == "multitouch" )
                multitouch = _toBool(value);

            else if ( key == "last_dir" )
                last_open_dir = value;
            else if ( key == "start_dir" )
                start_dir = value;

            else if ( key == "zoom_speed" )
            {
                double z = value.toDouble();
                if ( z > 0.01 ) zoom_speed = z ;
            }
            else if ( key == "thumb_zoom_ratio" )
            {
                double z = value.toDouble();
                thumb_zoom_ratio = z;
                if ( z < 0.001 ) thumb_zoom_ratio = 0.001;
            }
            else if ( key == "thumb_size" )
            {
                double z = value.toDouble();
                thumb_size = z;
                if ( z < thumb_zoom_min ) thumb_size = thumb_zoom_min;
                if ( z > thumb_zoom_max ) thumb_size = thumb_zoom_max;
            }
            else if ( key == "thumbnails_crop" )
                thumbnails_crop = _toBool(value);
            else if ( key == "thumbnails_square" )
                thumbnails_square = _toBool(value);
            else if ( key == "thumbnails_space" )
                thumbnails_space = _toBool(value);
            else if ( key == "thumbnails_text_under_image" )
                thumbnails_text_under_image = _toBool(value);
            else if ( key == "thumbnails_show_name" )
                thumbnails_show_name = _toBool(value);
            else if ( key == "smooth_images" )
                smooth_images = _toBool(value);
            else if ( key == "disable_animations" )
                disable_animations = _toBool(value);
            else if ( key == "choose_ui_size" )
            {
                choose_ui_size = value.toInt();
                if ( choose_ui_size < 0 ) choose_ui_size = 0;
                if ( choose_ui_size > 1024 ) choose_ui_size = 0;
            }
            else if ( key == "allow_rotation" )
            {
                allow_rotation = _toBool(value);
            }

            // else => ignore unknown key
            f.close();
        }
        return true;
    }

    printf("Cannot load config file\n");
    return false;
}

void Config::computeUiSize( int screen_dpi )
{
    // manual UI size
    if ( choose_ui_size != 0 )
    {
        g_config.ui_size = g_config.choose_ui_size;
        return;
    }

    // automatically choose UI size
    // 135 dpi    => 48px
    // screen_dpi => ui_size
    // (only scale to bigger)
    g_config.ui_size = screen_dpi * 48 / 135;
    if ( g_config.ui_size < 48 )
        g_config.ui_size = 48;
}

QString Config::_fromColor( QColor &c )
{
    QString s;
    s.sprintf("%d %d %d", c.red(), c.green(), c.blue() );
    return s;
}

QColor Config::_toColor( QString &s )
{
    int r,g,b;
    sscanf( s.toUtf8().data(), "%d %d %d", &r,&g,&b);
    return QColor(r,g,b);
}

QString Config::_fromBool( bool b )
{
    return b ? "true" : "false";
}

bool Config::_toBool( QString &s )
{
    QString s1 = s.toLower();
    return s == "yes" || s == "true";
}

bool Config::_dirExists( QString s )
{
    QDir d(s);
    return d.exists();
}
