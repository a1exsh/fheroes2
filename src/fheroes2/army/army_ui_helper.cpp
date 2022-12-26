/***************************************************************************
 *   fheroes2: https://github.com/ihhub/fheroes2                           *
 *   Copyright (C) 2021 - 2022                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cassert>
#include <cstddef>
#include <string>

#include "agg_image.h"
#include "army.h"
#include "army_troop.h"
#include "army_ui_helper.h"
#include "game.h"
#include "icn.h"
#include "image.h"
#include "ui_text.h"

void fheroes2::drawMiniMonsters( const Troops & troops, int32_t cx, const int32_t cy, const uint32_t width, uint32_t first, uint32_t count, const bool isCompact,
                                 const bool isDetailedView, const bool isGarrisonView, const uint32_t thievesGuildsCount, DisplayContext & ctx )
{
    if ( !troops.isValid() ) {
        return;
    }

    if ( 0 == count ) {
        count = troops.GetOccupiedSlotCount();
    }

    const int chunk = width / count;

    if ( !isCompact ) {
        cx += chunk / 2;
    }

    for ( size_t slot = 0; slot <= troops.Size(); ++slot ) {
        const Troop * troop = troops.GetTroop( slot );
        if ( troop == nullptr || !troop->isValid() ) {
            continue;
        }
        if ( first != 0 || count == 0 ) {
            --first;
            continue;
        }

        std::string monstersCountRepresentation;

        if ( isDetailedView || !isGarrisonView ) {
            monstersCountRepresentation = Game::formatMonsterCount( troop->GetCount(), isDetailedView, isCompact );
        }
        else {
            assert( thievesGuildsCount > 0 );

            if ( thievesGuildsCount == 1 ) {
                monstersCountRepresentation = "???";
            }
            else {
                monstersCountRepresentation = Army::SizeString( troop->GetCount() );
            }
        }

        const fheroes2::Sprite & monster = fheroes2::AGG::GetICN( ICN::MONS32, troop->GetSpriteIndex() );
        fheroes2::Text text( monstersCountRepresentation, fheroes2::FontType::smallWhite() );

        // This is the drawing of army troops in compact form in the small info window beneath resources
        if ( isCompact ) {
            const int offsetY = std::max( 0, ctx.scale( 37 ) - monster.height() );
            const int offsetX = std::max( 0, ( chunk - monster.width() - text.width() ) / 2 );
            fheroes2::Blit( monster, ctx, cx + offsetX, cy + offsetY + monster.y() );

            text.draw( cx + chunk - text.width() - offsetX, cy + 23, ctx );
        }
        else {
            const int offsetY = std::max( 0, ctx.scale( 28 ) - monster.height() );
            fheroes2::Blit( monster, ctx, cx - monster.width() / 2 + monster.x() + 2, cy + offsetY + monster.y() );

            text.draw( cx - text.width() / 2, cy + ctx.scale( 29 ), ctx );
        }
        cx += chunk;
        --count;
    }
}
