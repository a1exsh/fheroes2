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

#include <cstdint>
#include <string>
#include <utility>

#include "agg_image.h"
#include "dialog_graphics_settings.h"
#include "dialog_resolution.h"
#include "game_hotkeys.h"
#include "game_mainmenu_ui.h"
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
#include "ui_text.h"

namespace
{
    enum class SelectedWindow : int
    {
        Configuration,
        Resolution,
        Mode,
        VSync,
        SystemInfo,
        Exit
    };

    const fheroes2::Size offsetBetweenOptions{ 118, 110 };
    const fheroes2::Point optionOffset{ 69, 47 };
    const int32_t optionWindowSize{ 65 };

    const fheroes2::Rect resolutionRoi{ optionOffset.x, optionOffset.y, optionWindowSize, optionWindowSize };
    const fheroes2::Rect modeRoi{ optionOffset.x + offsetBetweenOptions.width, optionOffset.y, optionWindowSize, optionWindowSize };
    const fheroes2::Rect vSyncRoi{ optionOffset.x, optionOffset.y + offsetBetweenOptions.height, optionWindowSize, optionWindowSize };
    const fheroes2::Rect systemInfoRoi{ optionOffset.x + offsetBetweenOptions.width, optionOffset.y + offsetBetweenOptions.height, optionWindowSize, optionWindowSize };

    void drawResolution( fheroes2::DisplayContext & ctx )
    {
        const fheroes2::Display & display = fheroes2::Display::instance();
        std::string resolutionName = std::to_string( display.width() ) + 'x' + std::to_string( display.height() );

        fheroes2::drawOption( ctx, resolutionRoi, fheroes2::AGG::GetICN( ICN::SPANEL, Settings::Get().isEvilInterfaceEnabled() ? 17 : 16 ), _( "Resolution" ),
                              std::move( resolutionName ), fheroes2::UiOptionTextWidth::TWO_ELEMENTS_ROW );
    }

    void drawMode( fheroes2::DisplayContext & ctx )
    {
        const fheroes2::Sprite & originalIcon = fheroes2::AGG::GetICN( ICN::SPANEL, Settings::Get().isEvilInterfaceEnabled() ? 17 : 16 );

        if ( fheroes2::engine().isFullScreen() ) {
            fheroes2::Sprite icon = originalIcon;
            fheroes2::Resize( originalIcon, 6, 6, 53, 53, icon, 2, 2, 61, 61 );

            fheroes2::drawOption( ctx, modeRoi, icon, _( "window|Mode" ), _( "Fullscreen" ), fheroes2::UiOptionTextWidth::TWO_ELEMENTS_ROW );
        }
        else {
            fheroes2::drawOption( ctx, modeRoi, originalIcon, _( "window|Mode" ), _( "Windowed" ), fheroes2::UiOptionTextWidth::TWO_ELEMENTS_ROW );
        }
    }

    void drawVSync( fheroes2::DisplayContext & ctx )
    {
        const bool isVSyncEnabled = Settings::Get().isVSyncEnabled();

        fheroes2::drawOption( ctx, vSyncRoi, fheroes2::AGG::GetICN( ICN::SPANEL, isVSyncEnabled ? 18 : 19 ), _( "V-Sync" ), isVSyncEnabled ? _( "on" ) : _( "off" ),
                              fheroes2::UiOptionTextWidth::TWO_ELEMENTS_ROW );
    }

    void drawSystemInfo( fheroes2::DisplayContext & ctx )
    {
        const bool isSystemInfoDisplayed = Settings::Get().isSystemInfoEnabled();

        fheroes2::Sprite image = fheroes2::Crop( fheroes2::AGG::GetICN( ICN::ESPANBKG, 0 ), ctx.scale( 69 ), ctx.scale( 47 ), ctx.scale( 65 ), ctx.scale( 65 ) );
        fheroes2::Text info;
        if ( isSystemInfoDisplayed ) {
            info.set( _( "FPS" ), fheroes2::FontType( fheroes2::FontSize::NORMAL, fheroes2::FontColor::YELLOW ) );
        }
        else {
            info.set( _( "N/A" ), fheroes2::FontType( fheroes2::FontSize::NORMAL, fheroes2::FontColor::GRAY ) );
        }
        info.draw( ( image.width() - info.width() ) / 2, ( image.height() - info.height() ) / 2, image );

        fheroes2::drawOption( ctx, systemInfoRoi, image, _( "System Info" ), isSystemInfoDisplayed ? _( "on" ) : _( "off" ),
                              fheroes2::UiOptionTextWidth::TWO_ELEMENTS_ROW );
    }

    SelectedWindow showConfigurationWindow()
    {
        fheroes2::Display & display = fheroes2::Display::instance();

        const Settings & conf = Settings::Get();
        const bool isEvilInterface = conf.isEvilInterfaceEnabled();
        const fheroes2::Sprite & dialog = fheroes2::AGG::GetICN( ( isEvilInterface ? ICN::ESPANBKG_EVIL : ICN::ESPANBKG ), 0 );
        const fheroes2::Sprite & dialogShadow = fheroes2::AGG::GetICN( ( isEvilInterface ? ICN::CSPANBKE : ICN::CSPANBKG ), 1 );

        const fheroes2::Point dialogOffset( ( display.width() - dialog.width() ) / 2, ( display.height() - dialog.height() ) / 2 );
        const fheroes2::Point shadowOffset( dialogOffset.x - BORDERWIDTH, dialogOffset.y );

        const fheroes2::Rect windowRoi{ dialogOffset.x, dialogOffset.y, dialog.width(), dialog.height() };

        const fheroes2::ImageRestorer restorer( display, shadowOffset.x, shadowOffset.y, dialog.width() + BORDERWIDTH, dialog.height() + BORDERWIDTH );

        fheroes2::Blit( dialogShadow, display, windowRoi.x - BORDERWIDTH, windowRoi.y + BORDERWIDTH );
        fheroes2::Blit( dialog, display, windowRoi.x, windowRoi.y );

        fheroes2::ImageRestorer emptyDialogRestorer( display, windowRoi.x, windowRoi.y, windowRoi.width, windowRoi.height );
        fheroes2::DisplayContext ctx = display.getContext( windowRoi.x, windowRoi.y );

        auto drawOptions = []( fheroes2::DisplayContext & c ) {
            drawResolution( c );
            drawMode( c );
            drawVSync( c );
            drawSystemInfo( c );
        };
        drawOptions( ctx );

        fheroes2::Button okayButton( ctx.scale( 112 ), ctx.scale( 252 ), isEvilInterface ? ICN::BUTTON_SMALL_OKAY_EVIL : ICN::BUTTON_SMALL_OKAY_GOOD, 0, 1 );
        okayButton.draw( ctx );

        display.render();

        bool isFullScreen = fheroes2::engine().isFullScreen();

        LocalEvent & le = LocalEvent::Get();
        while ( le.HandleEvents() ) {
            if ( le.MousePressLeft( okayButton.area( ctx ) ) ) {
                okayButton.drawOnPress( ctx );
            }
            else {
                okayButton.drawOnRelease( ctx );
            }

            if ( le.MouseClickLeft( okayButton.area( ctx ) ) || Game::HotKeyCloseWindow() ) {
                break;
            }
            if ( le.MouseClickLeft( ctx.area( resolutionRoi ) ) ) {
                return SelectedWindow::Resolution;
            }
            if ( le.MouseClickLeft( ctx.area( modeRoi ) ) ) {
                return SelectedWindow::Mode;
            }
            if ( le.MouseClickLeft( ctx.area( vSyncRoi ) ) ) {
                return SelectedWindow::VSync;
            }
            if ( le.MouseClickLeft( ctx.area( systemInfoRoi ) ) ) {
                return SelectedWindow::SystemInfo;
            }

            if ( le.MousePressRight( ctx.area( resolutionRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "Select Game Resolution" ), _( "Change the resolution of the game." ), 0 );
            }
            else if ( le.MousePressRight( ctx.area( modeRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "window|Mode" ), _( "Toggle between fullscreen and windowed modes." ), 0 );
            }
            else if ( le.MousePressRight( ctx.area( vSyncRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "V-Sync" ), _( "The V-Sync option can be enabled to resolve flickering issues on some monitors." ), 0 );
            }
            if ( le.MousePressRight( ctx.area( systemInfoRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "System Info" ), _( "Show extra information such as FPS and current time." ), 0 );
            }
            else if ( le.MousePressRight( okayButton.area( ctx ) ) ) {
                fheroes2::showStandardTextMessage( _( "Okay" ), _( "Exit this menu." ), 0 );
            }

            // Fullscreen mode can be toggled using a global hotkey, we need to properly reflect this change in the UI
            if ( isFullScreen != fheroes2::engine().isFullScreen() ) {
                isFullScreen = fheroes2::engine().isFullScreen();

                emptyDialogRestorer.restore();
                drawOptions( ctx );

                display.render( emptyDialogRestorer.rect() );
            }
        }

        return SelectedWindow::Exit;
    }
}

namespace fheroes2
{
    void openGraphicsSettingsDialog()
    {
        drawMainMenuScreen();

        Settings & conf = Settings::Get();

        SelectedWindow windowType = SelectedWindow::Configuration;
        while ( windowType != SelectedWindow::Exit ) {
            switch ( windowType ) {
            case SelectedWindow::Configuration:
                windowType = showConfigurationWindow();
                break;
            case SelectedWindow::Resolution:
                if ( Dialog::SelectResolution() ) {
                    conf.Save( Settings::configFileName );
                }
                drawMainMenuScreen();
                windowType = SelectedWindow::Configuration;
                break;
            case SelectedWindow::Mode:
                conf.setFullScreen( !conf.FullScreen() );
                conf.Save( Settings::configFileName );
                windowType = SelectedWindow::Configuration;
                break;
            case SelectedWindow::VSync:
                conf.setVSync( !conf.isVSyncEnabled() );
                conf.Save( Settings::configFileName );
                windowType = SelectedWindow::Configuration;
                break;
            case SelectedWindow::SystemInfo:
                conf.setSystemInfo( !conf.isSystemInfoEnabled() );
                conf.Save( Settings::configFileName );
                windowType = SelectedWindow::Configuration;
                break;
            default:
                return;
            }
        }
    }
}
