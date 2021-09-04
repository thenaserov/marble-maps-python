/*
    This file is part of the Marble Virtual Globe.

    SPDX-License-Identifier: LGPL-2.1-or-later

    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/


#include "GeoSceneTextureTileDataset.h"
#include "GeoSceneTypes.h"

namespace Marble
{

GeoSceneTextureTileDataset::GeoSceneTextureTileDataset( const QString& name )
    : GeoSceneTileDataset( name ){
}

const char* GeoSceneTextureTileDataset::nodeType() const
{
    return GeoSceneTypes::GeoSceneTextureTileType;
}

GeoDataLatLonBox GeoSceneTextureTileDataset::latLonBox() const
{
    return m_latLonBox;
}

void GeoSceneTextureTileDataset::setLatLonBox( const GeoDataLatLonBox &box )
{
    m_latLonBox = box;
}


}
