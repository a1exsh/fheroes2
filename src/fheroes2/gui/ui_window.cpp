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

#include "ui_window.h"
#include "agg_image.h"
#include "icn.h"
#include "settings.h"

namespace
{
    const int32_t BORDER_SIZE = 16;
}

namespace fheroes2
{
    StandardWindow::StandardWindow( const int32_t width, const int32_t height, DisplayContext ctx )
        : StandardWindow( ( ctx.width() - width ) / 2, ( ctx.height() - height ) / 2, width, height, ctx )
    {
    }

    StandardWindow::StandardWindow( const int32_t x, const int32_t y, const int32_t width, const int32_t height, DisplayContext ctx )
        : _ctx( ctx )
        , _activeArea( x, y, width, height )
        , _borderSize( _ctx.scale( BORDER_SIZE ) )
        , _windowArea( _activeArea.x - _borderSize, _activeArea.y - _borderSize, _activeArea.width + 2 * _borderSize, _activeArea.height + 2 * _borderSize )
        , _shadowSize( _borderSize )
        , _restorer( _ctx, _windowArea.x - _shadowSize, _windowArea.y, _windowArea.width + _shadowSize, _windowArea.height + _shadowSize )
        , _background( stretchBackground() )
    {
        redraw();
        renderShadow();
    }

    void StandardWindow::redraw()
    {
        fheroes2::Blit( _background, _ctx, _windowArea.x, _windowArea.y );
    }

    void StandardWindow::renderShadow()
    {
        fheroes2::ApplyTransform( _ctx, _windowArea.x - _shadowSize, _windowArea.y + _shadowSize, 1, _windowArea.height - _shadowSize, 5 );
        fheroes2::ApplyTransform( _ctx, _windowArea.x - _shadowSize + _ctx.scale( 1 ), _windowArea.y + _shadowSize, _ctx.scale( 1 ), _windowArea.height - _shadowSize,
                                  4 );
        fheroes2::ApplyTransform( _ctx, _windowArea.x - _shadowSize + _ctx.scale( 2 ), _windowArea.y + _shadowSize, _shadowSize - _ctx.scale( 2 ),
                                  _windowArea.height - _shadowSize, 3 );
        fheroes2::ApplyTransform( _ctx, _windowArea.x - _shadowSize, _windowArea.y + _windowArea.height, _windowArea.width, _shadowSize - _ctx.scale( 2 ), 3 );
        fheroes2::ApplyTransform( _ctx, _windowArea.x - _shadowSize, _windowArea.y + _windowArea.height + _shadowSize - _ctx.scale( 2 ), _windowArea.width,
                                  _ctx.scale( 1 ), 4 );
        fheroes2::ApplyTransform( _ctx, _windowArea.x - _shadowSize, _windowArea.y + _windowArea.height + _shadowSize - _ctx.scale( 1 ), _windowArea.width,
                                  _ctx.scale( 1 ), 5 );
    }

    fheroes2::Image StandardWindow::stretchBackground()
    {
        const fheroes2::Sprite & sprite = fheroes2::AGG::GetICN( ( Settings::Get().isEvilInterfaceEnabled() ? ICN::SURDRBKE : ICN::SURDRBKG ), 0 );
        return fheroes2::Stretch( sprite, _shadowSize, 0, sprite.width() - _shadowSize, sprite.height() - _shadowSize, _windowArea.width, _windowArea.height );
    }
}
