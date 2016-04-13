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

#ifndef IMAGEWITHINFO_H
#define IMAGEWITHINFO_H

#include <QImage>
#include <QTransform>
#include <QSize>


enum FitZoomMode
{
    ZOOM_FIT_WIDTH,
    ZOOM_FIT_HEIGHT,
    ZOOM_FIT
};

class ImageWithInfo
{
public:
    QImage * image = nullptr;
    QImage * small_image = nullptr;
    double zoom = 1.0;
    int posx = 0;
    int posy = 0;
    double rotation = 0.0;

    inline void clear( void )
    {
        image = small_image = 0;
        zoom = 1.0;
        recenter();
    }


    inline bool isNull()
    {
        return (image == nullptr);
    }

    inline int width( void )
    {
        return image != NULL ? image->width() : 0;
    }

    inline int height( void )
    {
        return image != NULL ? image->height() : 0;
    }

    inline void recenter()
    {
        posx = posy = 0;
        rotation = 0;
    }

    ImageWithInfo& operator=(ImageWithInfo &rhs);

    double computeFitZoomWithRotation( QSize screenSize, FitZoomMode zoom_mode = ZOOM_FIT );
    double computeFitZoom( QSize screenSize, FitZoomMode zoom_mode = ZOOM_FIT );
    void rotate(int degrees, QSize screenSize);
    void rotateLeft(QSize screenSize);
    void rotateRight(QSize screenSize);
    ~ImageWithInfo();
};

#endif
