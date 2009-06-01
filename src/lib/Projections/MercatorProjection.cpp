//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>"
//


// Local
#include "MercatorProjection.h"

// Marble
#include "MercatorProjectionHelper.h"
#include "ViewportParams.h"

#include "MathHelper.h"
#include "GeoDataPoint.h"

using namespace Marble;

static MercatorProjectionHelper  theHelper;


MercatorProjection::MercatorProjection()
    : AbstractProjection()
{
    // This is the max value where atanh( sin( lat ) ) is defined.
    m_maxLat  = 85.05113 * DEG2RAD;
    m_minLat  = -85.05113 * DEG2RAD;
    m_traversableMaxLat = false;
    m_repeatX = true;
}

MercatorProjection::~MercatorProjection()
{
}


AbstractProjectionHelper *MercatorProjection::helper()
{
    return &theHelper;
}


bool MercatorProjection::screenCoordinates( qreal lon, qreal lat,
                                            const ViewportParams *viewport,
                                            qreal& x, qreal& y )
{
    bool retval = true;

    if ( lat > maxLat() ) {
        lat = maxLat();
        retval = false;
    }
    if ( lat < minLat() ) {
        lat = minLat();
        retval = false;
    }

    // Convenience variables
    int  radius = viewport->radius();
    qreal  width  = (qreal)(viewport->width());
    qreal  height = (qreal)(viewport->height());

    qreal  rad2Pixel = 2 * radius / M_PI;

    // Calculate translation of center point
    qreal  centerLon;
    qreal  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    // Let (x, y) be the position on the screen of the placemark..
    x = ( width  / 2 + rad2Pixel * ( lon - centerLon ) );
    y = ( height / 2 - rad2Pixel * ( atanh( sin( lat ) ) - atanh( sin( centerLat ) ) ) );

    return retval && ( ( 0 <= y && y < height )
                  && ( ( 0 <= x && x < width )
                  || ( 0 <= x - 4 * radius && x - 4 * radius < width )
                  || ( 0 <= x + 4 * radius && x + 4 * radius < width ) ) );
}

bool MercatorProjection::screenCoordinates( const GeoDataCoordinates &geopoint, 
                                            const ViewportParams *viewport,
                                            qreal &x, qreal &y, bool &globeHidesPoint )
{
    globeHidesPoint = false;
    qreal  lon;
    qreal  lat;

    geopoint.geoCoordinates( lon, lat );

    bool retval = true;

    if ( lat > maxLat() ) {
        GeoDataCoordinates approxCoords( geopoint );
        approxCoords.setLatitude( maxLat() );
        approxCoords.geoCoordinates( lon, lat );
        retval = false;
    }

    if ( lat < minLat() ) {
        GeoDataCoordinates approxCoords( geopoint );
        approxCoords.setLatitude( minLat() );
        approxCoords.geoCoordinates( lon, lat );
        retval = false;
    }

    // Convenience variables
    int  radius = viewport->radius();
    qreal  width  = (qreal)(viewport->width());
    qreal  height = (qreal)(viewport->height());

    qreal  rad2Pixel = 2 * radius / M_PI;

    qreal  centerLon;
    qreal  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );
    
    // Let (x, y) be the position on the screen of the placemark..
    x = ( width  / 2 + rad2Pixel * ( lon - centerLon ) );
    y = ( height / 2 - rad2Pixel * ( atanh( sin( lat ) ) - atanh( sin( centerLat ) ) ) );

    // Return true if the calculated point is inside the screen area,
    // otherwise return false.
    return retval && ( ( 0 <= y && y < height )
                  && ( ( 0 <= x && x < width )
                  || ( 0 <= x - 4 * radius && x - 4 * radius < width )
                  || ( 0 <= x + 4 * radius && x + 4 * radius < width ) ) );
}

bool MercatorProjection::screenCoordinates( const GeoDataCoordinates &coordinates,
                                            const ViewportParams *viewport,
                                            qreal *x, qreal &y, int &pointRepeatNum,
                                            const QSizeF& size,
                                            bool &globeHidesPoint )
{
    // On flat projections the observer's view onto the point won't be 
    // obscured by the target planet itself.
    globeHidesPoint = false;

    qreal  lon;
    qreal  lat;

    coordinates.geoCoordinates( lon, lat );

    bool retval = true;

    if ( lat > maxLat() ) {
        GeoDataCoordinates approxCoords( coordinates );
        approxCoords.setLatitude( maxLat() );
        approxCoords.geoCoordinates( lon, lat );
        retval = false;
    }

    if ( lat < minLat() ) {
        GeoDataCoordinates approxCoords( coordinates );
        approxCoords.setLatitude( minLat() );
        approxCoords.geoCoordinates( lon, lat );
        retval = false;
    }

    // Convenience variables
    int  radius = viewport->radius();
    qreal  width  = (qreal)(viewport->width());
    qreal  height = (qreal)(viewport->height());

    qreal  rad2Pixel = 2.0 * radius / M_PI;

    qreal  centerLon;
    qreal  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    // Let (itX, y) be the first guess for one possible position on screen..
    qreal itX = ( width  / 2.0 + rad2Pixel * ( lon - centerLon ) );
    y = ( height / 2.0 - rad2Pixel * ( atanh( sin( lat ) ) - atanh( sin( centerLat ) ) ) );

    // Make sure that the requested point is within the visible y range:
    if ( 0 <= y + size.height() / 2.0 && y < height + size.height() / 2.0 ) {
        // First we deal with the case where the repetition doesn't happen
        if ( !m_repeatX ) {
            *x = itX;
            if ( 0 < itX + size.width() / 2.0  && itX < width + size.width() / 2.0 ) {
                return retval && true;
            }
            else {
                // the requested point is out of the visible x range:
                return false;
            }
        }
        // For the repetition case the same geopoint gets displayed on 
        // the map many times.across the longitude.

        int xRepeatDistance = 4 * radius;

        // Finding the leftmost positive x value
        if ( itX > xRepeatDistance ) {
            int repeatNum = (int)( itX / xRepeatDistance );  
            itX = itX - repeatNum * xRepeatDistance;
        }
        if ( itX + size.width() / 2.0 < 0 ) {
            itX += xRepeatDistance;
        }
        // the requested point is out of the visible x range:
        if ( itX > width + size.width() / 2.0 ) {
            return false;
        }

        // Now iterate through all visible x screen coordinates for the point 
        // from left to right.
        int itNum = 0;
        while ( itX - size.width() / 2.0 < width ) {
            *x = itX;
            ++x;
            ++itNum;
            itX += xRepeatDistance;
        }

        pointRepeatNum = itNum;

        return retval && true;
    }

    // the requested point is out of the visible y range:
    return false;
}

bool MercatorProjection::screenCoordinates( const GeoDataLineString &_lineString, 
                                    const ViewportParams *viewport,
                                    QVector<QPolygonF *> &polygons )
{
//    qDebug() << "LineString GeometryId:" << lineString.geometryId();

    const TessellationFlags tessellationFlags = _lineString.tessellationFlags();

    // Compare bounding box size of the line string with the angularResolution
    // Immediately return if the latLonAltBox is smaller.
    if ( !viewport->resolves( _lineString.latLonAltBox() ) ) {
//      qDebug() << "Object too small to be resolved";
        return false;
    }

    qreal x = 0;
    qreal y = 0;
    bool globeHidesPoint = false;

    qreal previousX = -1.0;
    qreal previousY = -1.0;
    bool previousGlobeHidesPoint = false;

    QPolygonF  *polygon = new QPolygonF;

    GeoDataLineString lineString;

    if ( lineString.latLonAltBox().containsPole( Marble::AnyPole ) ) {
        lineString = _lineString.toPoleCorrected();
    }
    else {
        lineString = _lineString;
    }

    GeoDataLineString::ConstIterator itCoords = lineString.constBegin();
    GeoDataLineString::ConstIterator itPreviousCoords = lineString.constBegin();

    GeoDataCoordinates previousCoords;
    GeoDataCoordinates currentCoords;

    GeoDataLineString::ConstIterator itEnd = lineString.constEnd();

    bool processingLastNode = false;

    // We use a while loop to be able to cover linestrings as well as linear rings:
    // Linear rings require to tessellate the path from the last node to the first node
    // which isn't really convenient to achieve with a for loop ...

    const bool isLong = lineString.size() > 50;

    while ( itCoords != itEnd )
    {
        // Optimization for line strings with a big amount of nodes
        bool skipNode = isLong && viewport->resolves( *itPreviousCoords, *itCoords);

        if ( !skipNode ) {

            previousCoords = *itPreviousCoords;
            currentCoords  = *itCoords;

            screenCoordinates( currentCoords, viewport, x, y, globeHidesPoint );

            // Initializing variables that store the values of the previous iteration
            if ( !processingLastNode && itCoords == lineString.constBegin() ) {
                previousGlobeHidesPoint = globeHidesPoint;
                itPreviousCoords = itCoords;
                previousX = x;
                previousY = y;
            }

            // TODO: on flat maps we need to take the date line into account right here.

            // <SPHERICAL>
            // Adding interpolated nodes if the current or previous point is visible
            if ( globeHidesPoint || previousGlobeHidesPoint ) {
                if ( globeHidesPoint !=  previousGlobeHidesPoint ) {
                    // the line string disappears behind the visible horizon or
                    // it reappears at the horizon.

                    // Add interpolated "horizon" nodes

                    // Assign the first or last horizon point to the current or
                    // previous point, so that we still get correct results for
                    // the case where we need to geoproject the line segment.
/*
                    if ( globeHidesPoint ) {
                        currentCoords  = createHorizonCoordinates( previousCoords,
                                                                   currentCoords,
                                                                   viewport, lineString.tessellationFlags() );
                    } else {
                        previousCoords = createHorizonCoordinates( previousCoords,
                                                                   currentCoords,
                                                                   viewport, lineString.tessellationFlags() );
                    }
*/
                }
                else {
                    // Both nodes are located on the planet's hemisphere that is
                    // not visible to the user.
                }
            }
            // </SPHERICAL>

            // This if-clause contains the section that tessellates the line
            // segments of a linestring. If you are about to learn how the code of
            // this class works you can safely ignore this section for a start.

            if ( lineString.tessellate() /* && ( isVisible || previousIsVisible ) */ ) {
                // let the line segment follow the spherical surface
                // if the distance between the previous point and the current point
                // on screen is too big

                // We take the manhattan length as a distance approximation
                // that can be too big by a factor of sqrt(2)
                qreal distance = fabs((x - previousX)) + fabs((y - previousY));

                // FIXME: This is a work around: remove as soon as we handle horizon crossing
                if ( globeHidesPoint || previousGlobeHidesPoint ) {
                    distance = 350;
                }

                const qreal safeDistance = - 0.5 * distance;

                // Interpolate additional nodes if the current or previous nodes are visible
                // or if the line segment that connects them might cross the viewport.
                // The latter can pretty safely be excluded for most projections if both points
                // are located on the same side relative to the viewport boundaries and if they are
                // located more than half the line segment distance away from the viewport.
#ifdef SAFE_DISTANCE
                if (   !( x < safeDistance && previousX < safeDistance )
                    || !( y < safeDistance && previousY < safeDistance )
                    || !( x + safeDistance > viewport->width()
                        && previousX + safeDistance > viewport->width() )
                    || !( y + safeDistance > viewport->height()
                        && previousY + safeDistance > viewport->height() )
                )
                {
#endif
                    int tessellatedNodes = (int)( distance / tessellationPrecision );

                    if ( distance > tessellationPrecision ) {
//                      qDebug() << "Distance: " << distance;
                        *polygon << tessellateLineSegment( previousCoords, currentCoords,
                                                        tessellatedNodes, viewport,
                                                        tessellationFlags );
                    }
#ifdef SAFE_DISTANCE
                }
#endif
            }
            else {
                if ( !globeHidesPoint ) {
                    polygon->append( QPointF( x, y ) );
                }
            }

            if ( globeHidesPoint ) {
                if (   !previousGlobeHidesPoint
                    && !lineString.isClosed() // FIXME: this probably needs to take rotation
                                                //        into account for some cases
                    ) {
                    polygons.append( polygon );
                    polygon = new QPolygonF;
                }
            }

            previousGlobeHidesPoint = globeHidesPoint;
            itPreviousCoords = itCoords;
            previousX = x;
            previousY = y;
        }

        // Here we modify the condition to be able to process the
        // first node after the last node in a LinearRing.

        if ( processingLastNode ) {
            break;
        }
        ++itCoords;

        if ( itCoords == itEnd  && lineString.isClosed() ) {
            itCoords = lineString.constBegin();
            processingLastNode = true;
        }
    }

    if ( polygon->size() > 1 ){
        polygons.append( polygon );
    }
    else {
        delete polygon;
    }

    return polygons.isEmpty();
}

bool MercatorProjection::geoCoordinates( int x, int y,
                                         const ViewportParams *viewport,
                                         qreal& lon, qreal& lat,
                                         GeoDataCoordinates::Unit unit )
{
    int           radius             = viewport->radius();
    int           halfImageWidth     = viewport->width() / 2;
    int           halfImageHeight    = viewport->height() / 2;
    bool          noerr              = false;

    // Calculate translation of center point
    qreal  centerLon;
    qreal  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    // Calculate how many pixel are being represented per radians.
    const float rad2Pixel = (qreal)( 2 * radius )/M_PI;

    int yCenterOffset = (int)( asinh( tan( centerLat ) ) * rad2Pixel  );
    int yTop          = halfImageHeight - 2 * radius + yCenterOffset;
    int yBottom       = yTop + 4 * radius;

    if ( y >= yTop && y < yBottom ) {
        int    const  xPixels   = x - halfImageWidth;
        qreal const  pixel2Rad = M_PI / (2 * radius);

        lat = atan( sinh( ( ( halfImageHeight + yCenterOffset ) - y)
                          * pixel2Rad ) );
        lon = xPixels * pixel2Rad + centerLon;

        while ( lon > M_PI )  lon -= 2*M_PI;
        while ( lon < -M_PI ) lon += 2*M_PI;

        noerr = true;
    }

    if ( unit == GeoDataCoordinates::Degree ) {
        lon *= RAD2DEG;
        lat *= RAD2DEG;
    }

    return noerr;
}


GeoDataLatLonAltBox MercatorProjection::latLonAltBox( const QRect& screenRect,
                                                      const ViewportParams *viewport )
{
    // For the case where the whole viewport gets covered there is a 
    // pretty dirty and generic detection algorithm:
    GeoDataLatLonAltBox latLonAltBox = AbstractProjection::latLonAltBox( screenRect, viewport );

    // The remaining algorithm should be pretty generic for all kinds of 
    // flat projections:

    // If the whole globe is visible we can easily calculate
    // analytically the lon-/lat- range.
    // qreal pitch = GeoDataPoint::normalizeLat( viewport->planetAxis().pitch() );

    if ( m_repeatX ) {
        int xRepeatDistance = 4 * viewport->radius();
        if ( viewport->width() >= xRepeatDistance ) {
            latLonAltBox.setWest( -M_PI );
            latLonAltBox.setEast( +M_PI );
        }
    }
    else {
        // We need a point on the screen at maxLat that definetely
        // gets displayed:
        qreal averageLatitude = ( latLonAltBox.north() + latLonAltBox.south() ) / 2.0;
    
        GeoDataCoordinates maxLonPoint( +M_PI, averageLatitude, GeoDataCoordinates::Radian );
        GeoDataCoordinates minLonPoint( -M_PI, averageLatitude, GeoDataCoordinates::Radian );
    
        qreal dummyX, dummyY; // not needed
        bool dummyVal;
    
        if ( screenCoordinates( maxLonPoint, viewport, dummyX, dummyY, dummyVal ) ) {
            latLonAltBox.setEast( +M_PI );
        }
        if ( screenCoordinates( minLonPoint, viewport, dummyX, dummyY, dummyVal ) ) {
            latLonAltBox.setWest( -M_PI );
        }
    }

//    qDebug() << latLonAltBox.text( GeoDataCoordinates::Degree );

    return latLonAltBox;
}


bool MercatorProjection::mapCoversViewport( const ViewportParams *viewport ) const
{
    int           radius = viewport->radius();
    int           height = viewport->height();

    // Calculate translation of center point
    qreal  centerLon;
    qreal  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    // Calculate how many pixel are being represented per radians.
    const float rad2Pixel = (float)( 2 * radius )/M_PI;

    int yCenterOffset = (int)( asinh( tan( centerLat ) ) * rad2Pixel  );
    int yTop          = height / 2 - 2 * radius + yCenterOffset;
    int yBottom       = yTop + 4 * radius;

    if ( yTop >= 0 || yBottom < height )
        return false;

    return true;
}
