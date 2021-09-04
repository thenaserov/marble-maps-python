/*
    SPDX-FileCopyrightText: 2008-2009 Patrick Spendrin <ps_ml@gmx.de>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "KmlStyleMapTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataFeature.h"
#include "GeoDataDocument.h"
#include "GeoDataStyleMap.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( StyleMap )

GeoNode* KmlStyleMapTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_StyleMap)));

    GeoStackItem parentItem = parser.parentElement();
    
    if( parentItem.represents( kmlTag_Document ) ) {
        GeoDataStyleMap styleMap;
        KmlObjectTagHandler::parseIdentifiers( parser, &styleMap );
        parentItem.nodeAs<GeoDataDocument>()->addStyleMap( styleMap );
        return &parentItem.nodeAs<GeoDataDocument>()->styleMap( styleMap.id() );
    } else if( parentItem.is<GeoDataFeature>() ) {
/*        GeoDataStyleMap styleMap;
        parentItem.nodeAs<GeoDataFeature>()->setStyleMap( styleMap );*/
    }

    return nullptr;
}

}
}
