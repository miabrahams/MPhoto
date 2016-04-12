/*******************************************************************************

MPhoto - Photo viewer for multi-touch devices
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

#include "ImageWithInfo.h"

#define _USE_MATH_DEFINES
#include <math.h>

double ImageWithInfo::computeFitZoomWithRotation( QSize screenSize, FitZoomMode zoom_mode )
{
    if ( image == nullptr ) return 1.0f;
    double rot = rotation * M_PI / 180.0;
    qreal c = (qreal)cos( rot );
    qreal s = (qreal)sin( rot );
    double w = (double)image->width();
    double h = (double)image->height();
    double img_w = fabs(w * c + h * s);
    double img_h = fabs(w * s + h * c);
    double xz = (double)screenSize.width() / img_w;
    double yz = (double)screenSize.height() / img_h;
    double z = 1.0;
    switch ( zoom_mode )
    {
        case ZOOM_FIT_WIDTH:
            z = xz;
            break;
        case ZOOM_FIT_HEIGHT:
            z = yz;
            break;
        default:
            z = ( xz < yz ? xz : yz );
            break;
    }
    return z > 1.0f ? 1.0f : z;
}

double ImageWithInfo::computeFitZoom( QSize screenSize, FitZoomMode zoom_mode )
{
    if ( image == nullptr ) return 1.0f;
    double xz = (double)screenSize.width() / (double)image->width();
    double yz = (double)screenSize.height() / (double)image->height();
    double z = 1.0;
    switch ( zoom_mode )
    {
        case ZOOM_FIT_WIDTH:
            z = xz;
            break;
        case ZOOM_FIT_HEIGHT:
            z = yz;
            break;
        default:
            z = ( xz < yz ? xz : yz );
            break;
    }
    return z > 1.0f ? 1.0f : z;
}

void ImageWithInfo::rotateLeft(QSize screenSize)
{
    rotate(90, screenSize);
}

void ImageWithInfo::rotateRight(QSize screenSize)
{
    rotate(-90, screenSize);
}

void ImageWithInfo::rotate(int degrees, QSize screenSize)
{
    double fz = computeFitZoomWithRotation(screenSize);
    int r = (int)rotation;
    r = r/90*90 + ( r % 90 < 45 ? 0 : 90 ) + degrees;
    if ( r < 0 ) r = r + 360 * (abs(r)/360 + 1);
    if ( r >= 360 ) r = r - 360 * r/360;
    rotation = (double)r;
    double zdif = fz - zoom;
    if ( fabs(zdif) < 0.01 )
        zoom = computeFitZoomWithRotation(screenSize);
}
