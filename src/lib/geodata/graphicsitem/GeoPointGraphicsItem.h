//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef GEOPOINTGRAPHICSITEM_H
#define GEOPOINTGRAPHICSITEM_H

#include "GeoDataPoint.h"
#include "GeoGraphicsItem.h"

namespace Marble
{

class GeoPointGraphicsItem : public GeoGraphicsItem
{
public:
    GeoPointGraphicsItem();

    void setPoint( const GeoDataPoint& point );
    
    virtual void paint( GeoPainter* painter, ViewportParams *viewport,
                        const QString &renderPos, GeoSceneLayer *layer );
protected:
    GeoDataPoint    m_point;
};

}

#endif // GEOPOINTGRAPHICSITEM_H
