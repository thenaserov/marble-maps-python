/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

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

#include "KmlStyleUrlTagHandler.h"

#include <QString>

#include "KmlElementDictionary.h"
#include "MarbleDebug.h"
#include "GeoDataFeature.h"
#include "GeoDataStyleMap.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( styleUrl )

GeoNode* KmlstyleUrlTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_styleUrl)));

    GeoStackItem parentItem = parser.parentElement();
    GeoStackItem grandParentItem = parser.parentElement();
    
    if( parentItem.represents( kmlTag_Pair ) ) {
        QString content = parser.readElementText().trimmed();

        QString key = parentItem.nodeAs<GeoDataStyleMap>()->lastKey();
        (*parentItem.nodeAs<GeoDataStyleMap>())[ key ] = content;
        parentItem.nodeAs<GeoDataStyleMap>()->setLastKey(QString());

    } else if( parentItem.is<GeoDataFeature>() ) {
        QString content = parser.readElementText().trimmed();

        parentItem.nodeAs<GeoDataFeature>()->setStyleUrl( content );
    }
    return nullptr;
}

}
}
