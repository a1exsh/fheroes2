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

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

#include "agg_image.h"
#include "audio.h"
#include "audio_manager.h"
#include "cursor.h"
#include "dialog_audio.h"
#include "game.h"
#include "game_hotkeys.h"
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
    const fheroes2::Size offsetBetweenOptions{ 118, 110 };

    const fheroes2::Point optionOffset{ 69, 47 };
    const int32_t optionWindowSize{ 65 };

    const fheroes2::Rect musicVolumeRoi{ optionOffset.x, optionOffset.y, optionWindowSize, optionWindowSize };
    const fheroes2::Rect soundVolumeRoi{ optionOffset.x + offsetBetweenOptions.width, optionOffset.y, optionWindowSize, optionWindowSize };
    const fheroes2::Rect musicTypeRoi{ optionOffset.x, optionOffset.y + offsetBetweenOptions.height, optionWindowSize, optionWindowSize };
    const fheroes2::Rect audio3DRoi{ optionOffset.x + offsetBetweenOptions.width, optionOffset.y + offsetBetweenOptions.height, optionWindowSize, optionWindowSize };

    void drawMusicVolume( fheroes2::DisplayContext & ctx )
    {
        const Settings & conf = Settings::Get();

        const fheroes2::Sprite & musicVolumeIcon = fheroes2::AGG::GetICN( ICN::SPANEL, Audio::isValid() ? 1 : 0 );
        std::string value;
        if ( Audio::isValid() && conf.MusicVolume() ) {
            value = std::to_string( conf.MusicVolume() );
        }
        else {
            value = _( "off" );
        }

        fheroes2::drawOption( ctx, musicVolumeRoi, musicVolumeIcon, _( "Music" ), value, fheroes2::UiOptionTextWidth::TWO_ELEMENTS_ROW );
    }

    void drawSoundVolume( fheroes2::DisplayContext & ctx )
    {
        const Settings & conf = Settings::Get();

        const fheroes2::Sprite & soundVolumeOption = fheroes2::AGG::GetICN( ICN::SPANEL, Audio::isValid() ? 3 : 2 );
        std::string value;
        if ( Audio::isValid() && conf.SoundVolume() ) {
            value = std::to_string( conf.SoundVolume() );
        }
        else {
            value = _( "off" );
        }

        fheroes2::drawOption( ctx, soundVolumeRoi, soundVolumeOption, _( "Effects" ), value, fheroes2::UiOptionTextWidth::TWO_ELEMENTS_ROW );
    }

    void drawMusicType( fheroes2::DisplayContext & ctx )
    {
        const Settings & conf = Settings::Get();

        const MusicSource musicType = conf.MusicType();
        const fheroes2::Sprite & musicTypeIcon = fheroes2::AGG::GetICN( ICN::SPANEL, musicType == MUSIC_EXTERNAL ? 11 : 10 );
        std::string value;
        if ( musicType == MUSIC_MIDI_ORIGINAL ) {
            value = _( "MIDI" );
        }
        else if ( musicType == MUSIC_MIDI_EXPANSION ) {
            value = _( "MIDI Expansion" );
        }
        else if ( musicType == MUSIC_EXTERNAL ) {
            value = _( "External" );
        }

        fheroes2::drawOption( ctx, musicTypeRoi, musicTypeIcon, _( "Music Type" ), value, fheroes2::UiOptionTextWidth::TWO_ELEMENTS_ROW );
    }

    void drawAudio3D( fheroes2::DisplayContext & ctx )
    {
        const Settings & conf = Settings::Get();

        const bool is3DAudioEnabled = conf.is3DAudioEnabled();
        const fheroes2::Sprite & interfaceStateIcon = is3DAudioEnabled ? fheroes2::AGG::GetICN( ICN::SPANEL, 11 ) : fheroes2::AGG::GetICN( ICN::SPANEL, 10 );
        std::string value;
        if ( is3DAudioEnabled ) {
            value = _( "On" );
        }
        else {
            value = _( "Off" );
        }

        fheroes2::drawOption( ctx, audio3DRoi, interfaceStateIcon, _( "3D Audio" ), value, fheroes2::UiOptionTextWidth::TWO_ELEMENTS_ROW );
    }
}

namespace Dialog
{
    void openAudioSettingsDialog( const bool fromAdventureMap )
    {
        const CursorRestorer cursorRestorer( true, Cursor::POINTER );

        Settings & conf = Settings::Get();
        const bool isEvilInterface = conf.isEvilInterfaceEnabled();

        fheroes2::Display & display = fheroes2::Display::instance();

        const fheroes2::Sprite & dialog = fheroes2::AGG::GetICN( ( isEvilInterface ? ICN::ESPANBKG_EVIL : ICN::ESPANBKG ), 0 );
        const fheroes2::Sprite & dialogShadow = fheroes2::AGG::GetICN( ( isEvilInterface ? ICN::CSPANBKE : ICN::CSPANBKG ), 1 );

        const fheroes2::Point dialogOffset( ( display.width() - dialog.width() ) / 2, ( display.height() - dialog.height() ) / 2 );
        const fheroes2::Point shadowOffset( dialogOffset.x - BORDERWIDTH, dialogOffset.y );

        const fheroes2::Rect windowRoi{ dialogOffset.x, dialogOffset.y, dialog.width(), dialog.height() };

        fheroes2::ImageRestorer restorer( display, shadowOffset.x, shadowOffset.y, dialog.width() + BORDERWIDTH, dialog.height() + BORDERWIDTH );

        fheroes2::Fill( display, windowRoi.x, windowRoi.y, windowRoi.width, windowRoi.height, 0 );
        fheroes2::Blit( dialogShadow, display, windowRoi.x - BORDERWIDTH, windowRoi.y + BORDERWIDTH );
        fheroes2::Blit( dialog, display, windowRoi.x, windowRoi.y );

        fheroes2::DisplayContext ctx = display.getContext( windowRoi.x, windowRoi.y );

        auto drawOptions = []( fheroes2::DisplayContext & c ) {
            drawMusicVolume( c );
            drawSoundVolume( c );
            drawMusicType( c );
            drawAudio3D( c );
        };
        drawOptions( ctx );

        fheroes2::Button buttonOkay( ctx.scale( 112 ), ctx.scale( 252 ), isEvilInterface ? ICN::BUTTON_SMALL_OKAY_EVIL : ICN::BUTTON_SMALL_OKAY_GOOD, 0, 1 );
        buttonOkay.draw( ctx );

        display.render();

        bool saveConfig = false;

        // dialog menu loop
        LocalEvent & le = LocalEvent::Get();
        while ( le.HandleEvents() ) {
            le.MousePressLeft( buttonOkay.area( ctx ) ) ? buttonOkay.drawOnPress( ctx ) : buttonOkay.drawOnRelease( ctx );

            if ( le.MouseClickLeft( buttonOkay.area( ctx ) ) || Game::HotKeyCloseWindow() ) {
                break;
            }

            // set music or sound volume
            bool saveMusicVolume = false;
            bool saveSoundVolume = false;
            if ( Audio::isValid() ) {
                if ( le.MouseClickLeft( ctx.area( musicVolumeRoi ) ) ) {
                    conf.SetMusicVolume( ( conf.MusicVolume() + 1 ) % 11 );
                    saveMusicVolume = true;
                }
                else if ( le.MouseWheelUp( ctx.area( musicVolumeRoi ) ) ) {
                    conf.SetMusicVolume( conf.MusicVolume() + 1 );
                    saveMusicVolume = true;
                }
                else if ( le.MouseWheelDn( ctx.area( musicVolumeRoi ) ) ) {
                    conf.SetMusicVolume( conf.MusicVolume() - 1 );
                    saveMusicVolume = true;
                }

                if ( saveMusicVolume ) {
                    Music::setVolume( 100 * conf.MusicVolume() / 10 );
                }

                if ( le.MouseClickLeft( ctx.area( soundVolumeRoi ) ) ) {
                    conf.SetSoundVolume( ( conf.SoundVolume() + 1 ) % 11 );
                    saveSoundVolume = true;
                }
                else if ( le.MouseWheelUp( ctx.area( soundVolumeRoi ) ) ) {
                    conf.SetSoundVolume( conf.SoundVolume() + 1 );
                    saveSoundVolume = true;
                }
                else if ( le.MouseWheelDn( ctx.area( soundVolumeRoi ) ) ) {
                    conf.SetSoundVolume( conf.SoundVolume() - 1 );
                    saveSoundVolume = true;
                }
                if ( le.MouseClickLeft( ctx.area( audio3DRoi ) ) ) {
                    conf.set3DAudio( !conf.is3DAudioEnabled() );
                    saveSoundVolume = true;
                }

                if ( saveSoundVolume && fromAdventureMap ) {
                    Game::EnvironmentSoundMixer();
                }
            }

            // set music type
            bool saveMusicType = false;
            if ( le.MouseClickLeft( ctx.area( musicTypeRoi ) ) ) {
                int type = conf.MusicType() + 1;
                // If there's no expansion files we skip this option
                if ( type == MUSIC_MIDI_EXPANSION && !conf.isPriceOfLoyaltySupported() ) {
                    ++type;
                }

                conf.SetMusicType( type > MUSIC_EXTERNAL ? 0 : type );

                AudioManager::PlayCurrentMusic();

                saveMusicType = true;
            }

            if ( le.MousePressRight( ctx.area( musicVolumeRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "Music" ), _( "Toggle ambient music level." ), 0 );
            }

            else if ( le.MousePressRight( ctx.area( soundVolumeRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "Effects" ), _( "Toggle foreground sounds level." ), 0 );
            }
            else if ( le.MousePressRight( ctx.area( musicTypeRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "Music Type" ), _( "Change the type of music." ), 0 );
            }
            else if ( le.MousePressRight( ctx.area( audio3DRoi ) ) ) {
                fheroes2::showStandardTextMessage( _( "3D Audio" ), _( "Toggle 3D effects of foreground sounds." ), 0 );
            }
            else if ( le.MousePressRight( buttonOkay.area( ctx ) ) ) {
                fheroes2::showStandardTextMessage( _( "Okay" ), _( "Exit this menu." ), 0 );
            }

            if ( saveMusicVolume || saveSoundVolume || saveMusicType ) {
                // redraw
                fheroes2::Blit( dialog, display, windowRoi.x, windowRoi.y );
                drawOptions( ctx );
                buttonOkay.draw( ctx );
                display.render();

                saveConfig = true;
            }
        }

        if ( saveConfig ) {
            Settings::Get().Save( Settings::configFileName );
        }
    }
}
