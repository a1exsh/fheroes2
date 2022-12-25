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

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

#include "agg_image.h"
#include "cursor.h"
#include "dialog_audio.h"
#include "dialog_hotkeys.h"
#include "dialog_system_options.h"
#include "game_delays.h"
#include "game_hotkeys.h"
#include "game_interface.h"
#include "gamedefs.h"
#include "icn.h"
#include "image.h"
#include "localevent.h"
#include "math_base.h"
#include "screen.h"
#include "settings.h"
#include "translations.h"
#include "ui_button.h"
#include "ui_dialog.h"
#include "ui_option_item.h"

namespace
{
    enum class DialogAction : int
    {
        Open,
        ChangeInterfaceTheme,
        UpdateInterface,
        AudioSettings,
        HotKeys,
        CursorType,
        Close
    };

    const fheroes2::Size offsetBetweenOptions{ 92, 110 };
    const fheroes2::Point optionOffset{ 36, 47 };
    const int32_t optionWindowSize{ 65 };

    const fheroes2::Rect audioSettingsRoi{ optionOffset.x, optionOffset.y, optionWindowSize, optionWindowSize };
    const fheroes2::Rect hotkeysRoi{ optionOffset.x + offsetBetweenOptions.width, optionOffset.y, optionWindowSize, optionWindowSize };
    const fheroes2::Rect cursorTypeRoi{ optionOffset.x + offsetBetweenOptions.width * 2, optionOffset.y, optionWindowSize, optionWindowSize };
    const fheroes2::Rect heroSpeedRoi{ optionOffset.x, optionOffset.y + offsetBetweenOptions.height, optionWindowSize, optionWindowSize };
    const fheroes2::Rect aiSpeedRoi{ optionOffset.x + offsetBetweenOptions.width, optionOffset.y + offsetBetweenOptions.height, optionWindowSize, optionWindowSize };
    const fheroes2::Rect scrollSpeedRoi{ optionOffset.x + offsetBetweenOptions.width * 2, optionOffset.y + offsetBetweenOptions.height, optionWindowSize,
                                         optionWindowSize };
    const fheroes2::Rect interfaceThemeRoi{ optionOffset.x, optionOffset.y + offsetBetweenOptions.height * 2, optionWindowSize, optionWindowSize };
    const fheroes2::Rect interfaceStateRoi{ optionOffset.x + offsetBetweenOptions.width, optionOffset.y + offsetBetweenOptions.height * 2, optionWindowSize,
                                            optionWindowSize };
    const fheroes2::Rect autoBattleRoi{ optionOffset.x + offsetBetweenOptions.width * 2, optionOffset.y + offsetBetweenOptions.height * 2, optionWindowSize,
                                        optionWindowSize };

    void drawAudioSettings( fheroes2::DisplayContext & ctx )
    {
        const fheroes2::Sprite & audioSettingsIcon = fheroes2::AGG::GetICN( ICN::SPANEL, 1 );
        fheroes2::drawOption( ctx, audioSettingsRoi, audioSettingsIcon, _( "Audio" ), _( "Settings" ), fheroes2::UiOptionTextWidth::THREE_ELEMENTS_ROW );
    }

    void drawHotkeys( fheroes2::DisplayContext & ctx )
    {
        const fheroes2::Sprite & hotkeysIcon = fheroes2::AGG::GetICN( ICN::CSPANEL, 5 );
        fheroes2::drawOption( ctx, hotkeysRoi, hotkeysIcon, _( "Hot Keys" ), _( "Configure" ), fheroes2::UiOptionTextWidth::THREE_ELEMENTS_ROW );
    }

    void drawCursorType( fheroes2::DisplayContext & ctx )
    {
        const bool isMonoCursor = Settings::Get().isMonochromeCursorEnabled();
        const fheroes2::Sprite & cursorTypeIcon = fheroes2::AGG::GetICN( ICN::SPANEL, isMonoCursor ? 20 : 21 );
        fheroes2::drawOption( ctx, cursorTypeRoi, cursorTypeIcon, _( "Mouse Cursor" ), isMonoCursor ? _( "Black & White" ) : _( "Color" ),
                              fheroes2::UiOptionTextWidth::THREE_ELEMENTS_ROW );
    }

    void drawHeroSpeed( fheroes2::DisplayContext & ctx )
    {
        const int heroSpeed = Settings::Get().HeroesMoveSpeed();
        uint32_t heroSpeedIconId = 9;
        if ( heroSpeed >= 4 ) {
            heroSpeedIconId = 3 + heroSpeed / 2;
        }
        else if ( heroSpeed > 0 ) {
            heroSpeedIconId = 4;
        }

        const fheroes2::Sprite & heroSpeedIcon = fheroes2::AGG::GetICN( ICN::SPANEL, heroSpeedIconId );
        std::string value;
        if ( heroSpeed == 10 ) {
            value = _( "Jump" );
        }
        else {
            value = std::to_string( heroSpeed );
        }
        fheroes2::drawOption( ctx, heroSpeedRoi, heroSpeedIcon, _( "Hero Speed" ), value, fheroes2::UiOptionTextWidth::THREE_ELEMENTS_ROW );
    }

    void drawAiSpeed( fheroes2::DisplayContext & ctx )
    {
        const int aiSpeed = Settings::Get().AIMoveSpeed();
        uint32_t aiSpeedIconId = 9;
        if ( aiSpeed >= 4 ) {
            aiSpeedIconId = 3 + aiSpeed / 2;
        }
        else if ( aiSpeed > 0 ) {
            aiSpeedIconId = 4;
        }

        const fheroes2::Sprite & aiSpeedIcon = fheroes2::AGG::GetICN( ICN::SPANEL, aiSpeedIconId );
        std::string value;
        if ( aiSpeed == 0 ) {
            value = _( "Don't Show" );
        }
        else if ( aiSpeed == 10 ) {
            value = _( "Jump" );
        }
        else {
            value = std::to_string( aiSpeed );
        }

        fheroes2::drawOption( ctx, aiSpeedRoi, aiSpeedIcon, _( "Enemy Speed" ), value, fheroes2::UiOptionTextWidth::THREE_ELEMENTS_ROW );
    }

    void drawScrollSpeed( fheroes2::DisplayContext & ctx )
    {
        const int scrollSpeed = Settings::Get().ScrollSpeed();
        int32_t scrollSpeedIconIcn = ICN::UNKNOWN;
        uint32_t scrollSpeedIconId = 0;
        std::string scrollSpeedName;

        if ( scrollSpeed == SCROLL_SPEED_NONE ) {
            scrollSpeedName = _( "Off" );
            scrollSpeedIconIcn = ICN::SPANEL;
            scrollSpeedIconId = 9;
        }
        else if ( scrollSpeed == SCROLL_SPEED_SLOW ) {
            scrollSpeedName = _( "Slow" );
            scrollSpeedIconIcn = ICN::CSPANEL;
            scrollSpeedIconId = 0;
        }
        else if ( scrollSpeed == SCROLL_SPEED_NORMAL ) {
            scrollSpeedName = _( "Normal" );
            scrollSpeedIconIcn = ICN::CSPANEL;
            scrollSpeedIconId = 0;
        }
        else if ( scrollSpeed == SCROLL_SPEED_FAST ) {
            scrollSpeedName = _( "Fast" );
            scrollSpeedIconIcn = ICN::CSPANEL;
            scrollSpeedIconId = 1;
        }
        else if ( scrollSpeed == SCROLL_SPEED_VERY_FAST ) {
            scrollSpeedName = _( "Very Fast" );
            scrollSpeedIconIcn = ICN::CSPANEL;
            scrollSpeedIconId = 2;
        }

        assert( scrollSpeedIconIcn != ICN::UNKNOWN );

        const fheroes2::Sprite & scrollSpeedIcon = fheroes2::AGG::GetICN( scrollSpeedIconIcn, scrollSpeedIconId );
        fheroes2::drawOption( ctx, scrollSpeedRoi, scrollSpeedIcon, _( "Scroll Speed" ), scrollSpeedName, fheroes2::UiOptionTextWidth::THREE_ELEMENTS_ROW );
    }

    void drawInterfaceTheme( fheroes2::DisplayContext & ctx )
    {
        const bool isEvilInterface = Settings::Get().isEvilInterfaceEnabled();
        const fheroes2::Sprite & interfaceThemeIcon = fheroes2::AGG::GetICN( ICN::SPANEL, isEvilInterface ? 17 : 16 );
        std::string value;
        if ( isEvilInterface ) {
            value = _( "Evil" );
        }
        else {
            value = _( "Good" );
        }

        fheroes2::drawOption( ctx, interfaceThemeRoi, interfaceThemeIcon, _( "Interface Type" ), value, fheroes2::UiOptionTextWidth::THREE_ELEMENTS_ROW );
    }

    void drawInterfaceState( fheroes2::DisplayContext & ctx )
    {
        const Settings & conf = Settings::Get();
        const bool isHiddenInterface = conf.isHideInterfaceEnabled();
        const fheroes2::Sprite & interfaceStateIcon
            = isHiddenInterface ? fheroes2::AGG::GetICN( ICN::ESPANEL, 4 ) : fheroes2::AGG::GetICN( ICN::SPANEL, conf.isEvilInterfaceEnabled() ? 17 : 16 );
        std::string value;
        if ( isHiddenInterface ) {
            value = _( "Hide" );
        }
        else {
            value = _( "Show" );
        }

        fheroes2::drawOption( ctx, interfaceStateRoi, interfaceStateIcon, _( "Interface" ), value, fheroes2::UiOptionTextWidth::THREE_ELEMENTS_ROW );
    }

    void drawAutoBattle( fheroes2::DisplayContext & ctx )
    {
        const Settings & conf = Settings::Get();
        if ( conf.BattleAutoResolve() ) {
            const bool spellcast = conf.BattleAutoSpellcast();
            std::string value = spellcast ? _( "Auto Resolve" ) : _( "Auto, No Spells" );

            const fheroes2::Sprite & autoBattleIcon = fheroes2::AGG::GetICN( ICN::CSPANEL, spellcast ? 7 : 6 );
            fheroes2::drawOption( ctx, autoBattleRoi, autoBattleIcon, _( "Battles" ), value, fheroes2::UiOptionTextWidth::THREE_ELEMENTS_ROW );
        }
        else {
            const fheroes2::Sprite & autoBattleIcon = fheroes2::AGG::GetICN( ICN::SPANEL, 18 );
            fheroes2::drawOption( ctx, autoBattleRoi, autoBattleIcon, _( "Battles" ), _( "autoBattle|Manual" ), fheroes2::UiOptionTextWidth::THREE_ELEMENTS_ROW );
        }
    }

    DialogAction openSystemOptionsDialog( bool & saveConfiguration )
    {
        const CursorRestorer cursorRestorer( true, Cursor::POINTER );

        Settings & conf = Settings::Get();
        const bool isEvilInterface = conf.isEvilInterfaceEnabled();

        fheroes2::Display & display = fheroes2::Display::instance();

        const fheroes2::Sprite & dialog = fheroes2::AGG::GetICN( ( isEvilInterface ? ICN::SPANBKGE : ICN::SPANBKG ), 0 );
        const fheroes2::Sprite & dialogShadow = fheroes2::AGG::GetICN( ( isEvilInterface ? ICN::SPANBKGE : ICN::SPANBKG ), 1 );

        const fheroes2::Point dialogOffset( ( display.width() - dialog.width() ) / 2, ( display.height() - dialog.height() ) / 2 );
        const fheroes2::Point shadowOffset( dialogOffset.x - BORDERWIDTH, dialogOffset.y );

        fheroes2::ImageRestorer restorer( display, shadowOffset.x, shadowOffset.y, dialog.width() + BORDERWIDTH, dialog.height() + BORDERWIDTH );
        const fheroes2::Rect windowRoi( dialogOffset.x, dialogOffset.y, dialog.width(), dialog.height() );

        fheroes2::Fill( display, windowRoi.x, windowRoi.y, windowRoi.width, windowRoi.height, 0 );
        fheroes2::Blit( dialogShadow, display, windowRoi.x - BORDERWIDTH, windowRoi.y + BORDERWIDTH );
        fheroes2::Blit( dialog, display, windowRoi.x, windowRoi.y );

        fheroes2::DisplayContext ctx = display.getContext( windowRoi.x, windowRoi.y );

        auto drawOptions = []( fheroes2::DisplayContext & c ) {
            drawAudioSettings( c );
            drawHotkeys( c );
            drawCursorType( c );
            drawHeroSpeed( c );
            drawAiSpeed( c );
            drawInterfaceTheme( c );
            drawInterfaceState( c );
            drawScrollSpeed( c );
            drawAutoBattle( c );
        };
        drawOptions( ctx );

        fheroes2::Button buttonOkay( ctx.scale( 112 ), ctx.scale( 252 ), isEvilInterface ? ICN::BUTTON_SMALL_OKAY_EVIL : ICN::BUTTON_SMALL_OKAY_GOOD, 0, 1 );
        buttonOkay.draw( ctx );

        display.render();

        // dialog menu loop
        LocalEvent & le = LocalEvent::Get();
        while ( le.HandleEvents() ) {
            le.MousePressLeft( buttonOkay.area( ctx ) ) ? buttonOkay.drawOnPress( ctx ) : buttonOkay.drawOnRelease( ctx );

            if ( le.MouseClickLeft( buttonOkay.area( ctx ) ) || Game::HotKeyCloseWindow() ) {
                break;
            }

            // Open audio settings window.
            if ( le.MouseClickLeft( ctx.area( audioSettingsRoi ) ) ) {
                return DialogAction::AudioSettings;
            }

            // Open Hotkeys window.
            if ( le.MouseClickLeft( ctx.area( hotkeysRoi ) ) ) {
                return DialogAction::HotKeys;
            }

            // Change Cursor Type.
            if ( le.MouseClickLeft( ctx.area( cursorTypeRoi ) ) ) {
                return DialogAction::CursorType;
            }

            // set hero speed
            bool saveHeroSpeed = false;
            if ( le.MouseClickLeft( ctx.area( heroSpeedRoi ) ) ) {
                conf.SetHeroesMoveSpeed( conf.HeroesMoveSpeed() % 10 + 1 );
                saveHeroSpeed = true;
            }
            else if ( le.MouseWheelUp( ctx.area( heroSpeedRoi ) ) ) {
                conf.SetHeroesMoveSpeed( conf.HeroesMoveSpeed() + 1 );
                saveHeroSpeed = true;
            }
            else if ( le.MouseWheelDn( ctx.area( heroSpeedRoi ) ) ) {
                conf.SetHeroesMoveSpeed( conf.HeroesMoveSpeed() - 1 );
                saveHeroSpeed = true;
            }

            // set ai speed
            bool saveAISpeed = false;
            if ( le.MouseClickLeft( ctx.area( aiSpeedRoi ) ) ) {
                conf.SetAIMoveSpeed( ( conf.AIMoveSpeed() + 1 ) % 11 );
                saveAISpeed = true;
            }
            else if ( le.MouseWheelUp( ctx.area( aiSpeedRoi ) ) ) {
                conf.SetAIMoveSpeed( conf.AIMoveSpeed() + 1 );
                saveAISpeed = true;
            }
            else if ( le.MouseWheelDn( ctx.area( aiSpeedRoi ) ) ) {
                conf.SetAIMoveSpeed( conf.AIMoveSpeed() - 1 );
                saveAISpeed = true;
            }

            if ( saveHeroSpeed || saveAISpeed ) {
                Game::UpdateGameSpeed();
            }

            // set scroll speed
            bool saveScrollSpeed = false;
            if ( le.MouseClickLeft( ctx.area( scrollSpeedRoi ) ) ) {
                conf.SetScrollSpeed( ( conf.ScrollSpeed() + 1 ) % ( SCROLL_SPEED_VERY_FAST + 1 ) );
                saveScrollSpeed = true;
            }
            else if ( le.MouseWheelUp( ctx.area( scrollSpeedRoi ) ) ) {
                conf.SetScrollSpeed( conf.ScrollSpeed() + 1 );
                saveScrollSpeed = true;
            }
            else if ( le.MouseWheelDn( ctx.area( scrollSpeedRoi ) ) ) {
                conf.SetScrollSpeed( conf.ScrollSpeed() - 1 );
                saveScrollSpeed = true;
            }

            // set interface theme
            if ( le.MouseClickLeft( ctx.area( interfaceThemeRoi ) ) ) {
                return DialogAction::ChangeInterfaceTheme;
            }

            // set interface hide/show
            if ( le.MouseClickLeft( ctx.area( interfaceStateRoi ) ) ) {
                return DialogAction::UpdateInterface;
            }

            // toggle manual/auto battles
            bool saveAutoBattle = false;
            if ( le.MouseClickLeft( ctx.area( autoBattleRoi ) ) ) {
                if ( conf.BattleAutoResolve() ) {
                    if ( conf.BattleAutoSpellcast() ) {
                        conf.setBattleAutoSpellcast( false );
                    }
                    else {
                        conf.setBattleAutoResolve( false );
                    }
                }
                else {
                    conf.setBattleAutoResolve( true );
                    conf.setBattleAutoSpellcast( true );
                }
                saveAutoBattle = true;
            }

            if ( le.MousePressRight( ctx.area( audioSettingsRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "Audio" ), _( "Change the audio settings of the game." ), 0 );
            }
            else if ( le.MousePressRight( ctx.area( hotkeysRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "Hot Keys" ), _( "Check and configure all the hot keys present in the game." ), 0 );
            }
            else if ( le.MousePressRight( ctx.area( cursorTypeRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "Mouse Cursor" ), _( "Toggle colored cursor on or off. This is only an esthetic choice." ), 0 );
            }
            else if ( le.MousePressRight( ctx.area( heroSpeedRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "Hero Speed" ), _( "Change the speed at which your heroes move on the main screen." ), 0 );
            }
            else if ( le.MousePressRight( ctx.area( aiSpeedRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "Enemy Speed" ),
                                                   _( "Sets the speed that A.I. heroes move at.  You can also elect not to view A.I. movement at all." ), 0 );
            }
            else if ( le.MousePressRight( ctx.area( scrollSpeedRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "Scroll Speed" ), _( "Sets the speed at which you scroll the window." ), 0 );
            }
            else if ( le.MousePressRight( ctx.area( interfaceThemeRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "Interface Type" ), _( "Toggle the type of interface you want to use." ), 0 );
            }
            else if ( le.MousePressRight( ctx.area( interfaceStateRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "Interface" ), _( "Toggle interface visibility." ), 0 );
            }
            else if ( le.MousePressRight( ctx.area( autoBattleRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "Battles" ), _( "Toggle instant battle mode." ), 0 );
            }
            else if ( le.MousePressRight( buttonOkay.area( ctx ) ) ) {
                fheroes2::showStandardTextMessage( _( "Okay" ), _( "Exit this menu." ), 0 );
            }

            if ( saveHeroSpeed || saveAISpeed || saveScrollSpeed || saveAutoBattle ) {
                // redraw
                fheroes2::Blit( dialog, display, windowRoi.x, windowRoi.y );
                drawOptions( ctx );
                buttonOkay.draw( ctx );
                display.render();

                saveConfiguration = true;
            }
        }

        return DialogAction::Close;
    }
}

namespace fheroes2
{
    void showSystemOptionsDialog()
    {
        // We should make file writing only once.
        bool saveConfiguration = false;
        Settings & conf = Settings::Get();

        DialogAction action = DialogAction::Open;

        while ( action != DialogAction::Close ) {
            switch ( action ) {
            case DialogAction::Open:
                action = openSystemOptionsDialog( saveConfiguration );
                break;
            case DialogAction::ChangeInterfaceTheme: {
                conf.setEvilInterface( !conf.isEvilInterfaceEnabled() );
                saveConfiguration = true;

                Interface::Basic & basicInterface = Interface::Basic::Get();
                basicInterface.Reset();
                basicInterface.Redraw( Interface::REDRAW_ALL );

                action = DialogAction::Open;
                break;
            }
            case DialogAction::UpdateInterface: {
                conf.setHideInterface( !conf.isHideInterfaceEnabled() );
                saveConfiguration = true;

                Interface::Basic & basicInterface = Interface::Basic::Get();
                basicInterface.Reset();

                // We need to redraw radar first due to the nature of restorers. Only then we can redraw everything.
                basicInterface.Redraw( Interface::REDRAW_RADAR );
                basicInterface.Redraw( Interface::REDRAW_ALL );

                action = DialogAction::Open;
                break;
            }
            case DialogAction::AudioSettings:
                Dialog::openAudioSettingsDialog( true );
                action = DialogAction::Open;
                break;
            case DialogAction::HotKeys:
                fheroes2::openHotkeysDialog();
                action = DialogAction::Open;
                break;
            case DialogAction::CursorType: {
                conf.setMonochromeCursor( !conf.isMonochromeCursorEnabled() );
                saveConfiguration = true;
                action = DialogAction::Open;
                break;
            }
            default:
                break;
            }
        }

        if ( saveConfiguration ) {
            conf.Save( Settings::configFileName );
        }
    }
}
