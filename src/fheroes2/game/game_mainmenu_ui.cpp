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

#include "game_mainmenu_ui.h"

#include <cstdint>

#include "agg_image.h"
#include "icn.h"
#include "image.h"
#include "screen.h"

namespace fheroes2
{
    void drawSprite( Drawable & ctx, const int icnId, const uint32_t index )
    {
        const Sprite & sprite = AGG::GetICN( icnId, index );
        Blit( sprite, 0, 0, ctx, sprite.x(), sprite.y(), sprite.width(), sprite.height() );
    }

    void drawMainMenuScreen()
    {
        Display & display = Display::instance();

        const Image & background = AGG::GetICN( ICN::HEROES, 0 );
        const int32_t offX = ( display.width() - background.width() ) / 2;
        const int32_t offY = ( display.height() - background.height() ) / 2;

        DisplayContext ctx = display.getContext( offX, offY );
        Blit( background, ctx, 0, 0 );

        drawSprite( ctx, ICN::BTNSHNGL, 1 );
        drawSprite( ctx, ICN::BTNSHNGL, 5 );
        drawSprite( ctx, ICN::BTNSHNGL, 9 );
        drawSprite( ctx, ICN::BTNSHNGL, 13 );
        drawSprite( ctx, ICN::BTNSHNGL, 17 );
    }
}
