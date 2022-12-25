/***************************************************************************
 *   fheroes2: https://github.com/ihhub/fheroes2                           *
 *   Copyright (C) 2022                                                    *
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

#include "ui_option_item.h"

#include <cstdint>
#include <utility>

#include "image.h"
#include "screen.h"
#include "ui_text.h"

namespace
{
    const int32_t titleVerticalOffset = 12;
    const int32_t valueVerticalOffset = 6;
}

namespace fheroes2
{
    void drawOption( DisplayContext & ctx, const Rect & optionRoi, const Sprite & icon, const std::string & titleText, const std::string & valueText,
                     const int32_t textMaxWidth )
    {
        const Text title( titleText, FontType::smallWhite() );
        const Text value( valueText, FontType::smallWhite() );

        // Calculate the text field left border position to horizontally align the text to the icon center.
        const int32_t titleHorizontalOffset = ctx.scale( optionRoi.x ) + ( icon.width() - ctx.scale( textMaxWidth ) ) / 2;

        title.draw( titleHorizontalOffset, ctx.scale( optionRoi.y - titleVerticalOffset ) + title.height() - title.height( ctx.scale( textMaxWidth ) ),
                    ctx.scale( textMaxWidth ), ctx );
        value.draw( titleHorizontalOffset, ctx.scale( optionRoi.y + optionRoi.height + valueVerticalOffset ), ctx.scale( textMaxWidth ), ctx );

        Blit( icon, 0, 0, ctx, ctx.scale( optionRoi.x ), ctx.scale( optionRoi.y ), icon.width(), icon.height() );
    }
}
