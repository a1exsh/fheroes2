/***************************************************************************
 *   fheroes2: https://github.com/ihhub/fheroes2                           *
 *   Copyright (C) 2019 - 2022                                             *
 *                                                                         *
 *   Free Heroes2 Engine: http://sourceforge.net/projects/fheroes2         *
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include <algorithm>
#include <cstdint>
#include <memory>

#include "agg_image.h"
#include "dialog.h"
#include "icn.h"
#include "image.h"
#include "math_base.h"
#include "screen.h"
#include "settings.h"

namespace
{
    const int32_t WINDOW_WIDTH = 288; // this is measured value
    const int32_t BUTTON_HEIGHT = 40;
    const int32_t ACTIVE_AREA_HEIGHT = 35;

    int32_t topHeight( const bool isEvilInterface )
    {
        const fheroes2::Sprite & image = fheroes2::AGG::GetICN( isEvilInterface ? ICN::BUYBUILE : ICN::BUYBUILD, 0 );
        return image.height();
    }

    int32_t bottomHeight( const bool isEvilInterface )
    {
        const fheroes2::Sprite & image = fheroes2::AGG::GetICN( isEvilInterface ? ICN::BUYBUILE : ICN::BUYBUILD, 2 );
        return image.height();
    }

    int32_t leftWidth( const bool isEvilInterface )
    {
        const int icnId = isEvilInterface ? ICN::BUYBUILE : ICN::BUYBUILD;

        const fheroes2::Sprite & image3 = fheroes2::AGG::GetICN( icnId, 3 );
        const fheroes2::Sprite & image4 = fheroes2::AGG::GetICN( icnId, 4 );
        const fheroes2::Sprite & image5 = fheroes2::AGG::GetICN( icnId, 5 );

        int32_t widthLeft = image3.width();
        widthLeft = std::max( widthLeft, image4.width() );
        widthLeft = std::max( widthLeft, image5.width() );

        return widthLeft;
    }

    int32_t rightWidth( const bool isEvilInterface )
    {
        const int icnId = isEvilInterface ? ICN::BUYBUILE : ICN::BUYBUILD;

        const fheroes2::Sprite & image0 = fheroes2::AGG::GetICN( icnId, 0 );
        const fheroes2::Sprite & image1 = fheroes2::AGG::GetICN( icnId, 1 );
        const fheroes2::Sprite & image2 = fheroes2::AGG::GetICN( icnId, 2 );

        int32_t widthRight = image0.width();
        widthRight = std::max( widthRight, image1.width() );
        widthRight = std::max( widthRight, image2.width() );

        return widthRight;
    }

    int32_t overallWidth( const bool isEvilInterface )
    {
        return leftWidth( isEvilInterface ) + rightWidth( isEvilInterface );
    }

    int32_t leftOffset( fheroes2::DisplayContext & ctx, const bool isEvilInterface )
    {
        return leftWidth( isEvilInterface ) - ctx.scale( WINDOW_WIDTH ) / 2;
    }
}

Dialog::NonFixedFrameBox::NonFixedFrameBox( int height, int startYPos, bool showButtons )
    : _middleFragmentCount( 0 )
    , _middleFragmentHeight( 0 )
{
    fheroes2::Display & display = fheroes2::Display::instance();
    fheroes2::DisplayContext rootCtx = display.getContext();

    if ( showButtons )
        height += BUTTON_HEIGHT;

    const bool evil = Settings::Get().isEvilInterfaceEnabled();

    _middleFragmentCount = ( height <= 2 * ACTIVE_AREA_HEIGHT ? 0 : 1 + ( height - 2 * ACTIVE_AREA_HEIGHT ) / ACTIVE_AREA_HEIGHT );
    _middleFragmentHeight = rootCtx.scale( height <= 2 * ACTIVE_AREA_HEIGHT ? 0 : height - 2 * ACTIVE_AREA_HEIGHT );

    _area.width = rootCtx.scale( BOXAREA_WIDTH );
    _area.height = rootCtx.scale( 2 * ACTIVE_AREA_HEIGHT ) + _middleFragmentHeight;

    const int32_t leftSideOffset = leftOffset( rootCtx, evil );

    _position.x = ( display.width() - rootCtx.scale( WINDOW_WIDTH ) ) / 2 - leftSideOffset;
    _position.y = startYPos;

    if ( startYPos < 0 ) {
        _position.y = ( display.height() - _middleFragmentHeight ) / 2 - topHeight( evil );
    }

    const int32_t heightTopBottom = topHeight( evil ) + bottomHeight( evil );
    _restorer.reset( new fheroes2::ImageRestorer( display, _position.x, _position.y, overallWidth( evil ), heightTopBottom + _middleFragmentHeight ) );

    _area.x = _position.x + rootCtx.scale( WINDOW_WIDTH - BOXAREA_WIDTH ) / 2 + leftSideOffset;
    _area.y = _position.y + topHeight( evil ) - rootCtx.scale( ACTIVE_AREA_HEIGHT );

    redraw();
}

void Dialog::NonFixedFrameBox::redraw() const
{
    const bool isEvilInterface = Settings::Get().isEvilInterfaceEnabled();
    const int buybuild = isEvilInterface ? ICN::BUYBUILE : ICN::BUYBUILD;

    const int32_t overallLeftWidth = leftWidth( isEvilInterface );

    // right-top
    const fheroes2::Sprite & part0 = fheroes2::AGG::GetICN( buybuild, 0 );
    // left-top
    const fheroes2::Sprite & part4 = fheroes2::AGG::GetICN( buybuild, 4 );

    fheroes2::Display & display = fheroes2::Display::instance();
    fheroes2::DisplayContext ctx = display.getContext( _position.x, _position.y );

    fheroes2::Blit( part4, ctx, overallLeftWidth - part4.width(), 0 );
    fheroes2::Blit( part0, ctx, overallLeftWidth, 0 );

    int32_t offsetY = part4.height();

    int32_t middleLeftHeight = _middleFragmentHeight;
    for ( uint32_t i = 0; i < _middleFragmentCount; ++i ) {
        const int32_t chunkHeight = middleLeftHeight >= ctx.scale( ACTIVE_AREA_HEIGHT ) ? ctx.scale( ACTIVE_AREA_HEIGHT ) : middleLeftHeight;
        // left-middle
        const fheroes2::Sprite & sl = fheroes2::AGG::GetICN( buybuild, 5 );
        fheroes2::Blit( sl, 0, 10, ctx, overallLeftWidth - sl.width(), offsetY, sl.width(), chunkHeight );

        // right-middle
        const fheroes2::Sprite & sr = fheroes2::AGG::GetICN( buybuild, 1 );
        fheroes2::Blit( sr, 0, 10, ctx, overallLeftWidth, offsetY, sr.width(), chunkHeight );

        middleLeftHeight -= chunkHeight;
        offsetY += chunkHeight;
    }

    // right-bottom
    const fheroes2::Sprite & part2 = fheroes2::AGG::GetICN( buybuild, 2 );
    // left-bottom
    const fheroes2::Sprite & part6 = fheroes2::AGG::GetICN( buybuild, 6 );

    offsetY = part4.height() + _middleFragmentHeight;

    fheroes2::Blit( part6, ctx, overallLeftWidth - part6.width(), offsetY );
    fheroes2::Blit( part2, ctx, overallLeftWidth, offsetY );
}

Dialog::NonFixedFrameBox::~NonFixedFrameBox()
{
    _restorer->restore();

    fheroes2::Display::instance().render( _restorer->rect() );
}

Dialog::FrameBox::FrameBox( int height, bool buttons )
    : Dialog::NonFixedFrameBox( height, -1, buttons )
{}
