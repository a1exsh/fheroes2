/***************************************************************************
 *   fheroes2: https://github.com/ihhub/fheroes2                           *
 *   Copyright (C) 2020 - 2022                                             *
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

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "image.h"
#include "math_base.h"

namespace fheroes2
{
    class Cursor;
    class Display;
    class DisplayContext;

    class BaseRenderEngine
    {
    public:
        friend class Cursor;
        friend class Display;

        virtual ~BaseRenderEngine() = default;

        virtual void toggleFullScreen()
        {
            _isFullScreen = !_isFullScreen;
        }

        virtual bool isFullScreen() const
        {
            return _isFullScreen;
        }

        virtual std::vector<Size> getAvailableResolutions() const
        {
            return {};
        }

        virtual void setTitle( const std::string & )
        {
            // Do nothing.
        }

        virtual void setIcon( const Image & )
        {
            // Do nothing.
        }

        virtual fheroes2::Rect getActiveWindowROI() const
        {
            return {};
        }

        virtual fheroes2::Size getCurrentScreenResolution() const
        {
            return {};
        }

        virtual void setVSync( const bool )
        {
            // Do nothing.
        }

        void setNearestScaling( const bool enable )
        {
            _nearestScaling = enable;
        }

        bool isNearestScaling() const
        {
            return _nearestScaling;
        }

    protected:
        BaseRenderEngine()
            : _isFullScreen( false )
            , _nearestScaling( false )
        {
            // Do nothing.
        }

        virtual void clear()
        {
            // Do nothing.
        }

        virtual void render( const Display &, const Rect & )
        {
            // Do nothing.
        }

        virtual bool allocate( int32_t &, int32_t &, bool )
        {
            return false;
        }

        virtual bool isMouseCursorActive() const
        {
            return false;
        }

        // To support color cycling we need to update palette.
        virtual void updatePalette( const std::vector<uint8_t> & )
        {
            // Do nothing.
        }

        void linkRenderSurface( uint8_t * surface ) const; // declaration of this method is in source file

    private:
        bool _isFullScreen;

        bool _nearestScaling;
    };

    // A drawable that knows how to render itself on the screen.
    class AbstractDisplay : public virtual AbstractDrawable
    {
    public:
        virtual void render() = 0;
        virtual void render( const Rect & roi ) = 0;
    };

    class Display : public AbstractDisplay, public Image
    {
    public:
        friend class BaseRenderEngine;

        enum : int32_t
        {
            DEFAULT_WIDTH = 640,
            DEFAULT_HEIGHT = 480
        };

        static Display & instance();

        ~Display() override = default;

        // Render a full frame on screen.
        void render() override
        {
            render( { 0, 0, Image::width(), Image::height() } );
        }

        // render a part of image on screen. Prefer this method over full image if you don't draw full screen.
        void render( const Rect & roi ) override;

        // Update the area which will be rendered on the next render() call.
        void updateNextRenderRoi( const Rect & roi );

        void resize( int32_t width_, int32_t height_, int32_t scaleFactor_ ) override;

        bool isDefaultSize() const
        {
            return Image::width() == DEFAULT_WIDTH && Image::height() == DEFAULT_HEIGHT;
        }

        // this function must return true if new palette has been generated
        using PreRenderProcessing = bool ( * )( std::vector<uint8_t> & palette );
        using PostRenderProcessing = void ( * )();

        void subscribe( PreRenderProcessing preprocessing, PostRenderProcessing postprocessing )
        {
            _preprocessing = preprocessing;
            _postprocessing = postprocessing;
        }

        DisplayContext getContext( int32_t x, int32_t y );

        // For 8-bit mode we return a pointer to direct surface which we draw on screen
        uint8_t * image() override;
        const uint8_t * image() const override;

        void release(); // to release all allocated resources. Should be used at the end of the application

        // Change the whole color representation on the screen. Make sure that palette exists all the time!!!
        // nullptr input parameter is used to reset pallette to default one.
        void changePalette( const uint8_t * palette = nullptr, const bool forceDefaultPaletteUpdate = false ) const;

        friend BaseRenderEngine & engine();
        friend Cursor & cursor();

    private:
        std::unique_ptr<BaseRenderEngine> _engine;
        std::unique_ptr<Cursor> _cursor;
        PreRenderProcessing _preprocessing;
        PostRenderProcessing _postprocessing;

        uint8_t * _renderSurface;

        // Previous area drawn on the screen.
        Rect _prevRoi;

        // Only for cases of direct drawing on rendered 8-bit image.
        void linkRenderSurface( uint8_t * surface )
        {
            _renderSurface = surface;
        }

        Display();

        void _renderFrame( const Rect & roi ) const; // prepare and render a frame
    };

    class DisplayContext : public AbstractDisplay
    {
    public:
        DisplayContext( Display & display, int32_t x, int32_t y )
            : _display( display )
            , _x( std::max( 0, std::min( x, display.width() - 1 ) ) )
            , _y( std::max( 0, std::min( y, display.height() - 1 ) ) )
        {}

        int32_t width() const override
        {
            return _display.width();
        }

        int32_t height() const override
        {
            return _display.height();
        }

        int32_t scaleFactor() const override
        {
            return _display.scaleFactor();
        }

        bool singleLayer() const override
        {
            return _display.singleLayer(); // actually should be always true
        }

        bool empty() const override
        {
            return _display.empty();
        }

        uint8_t * image() override
        {
            return _display.image() + _x + _y * _display.width();
        }

        const uint8_t * image() const override
        {
            return _display.image() + _x + _y * _display.width();
        }

        uint8_t * transform() override
        {
            return _display.transform() + _x + _y * _display.width();
        }

        const uint8_t * transform() const override
        {
            return _display.transform() + _x + _y * _display.width();
        }

        void render() override
        {
            _display.render();
        }

        void render( const Rect & roi ) override
        {
            _display.render( roi );
        }

        // Scaling and translation of coordinates.
        int32_t coordX( int32_t x ) const
        {
            return translateX( scale( x ) );
        }

        int32_t coordY( int32_t y ) const
        {
            return translateY( scale( y ) );
        }

        int32_t scale( int32_t xy ) const
        {
            return xy * _display.scaleFactor();
        }

        int32_t translateX( int32_t x ) const
        {
            return _x + x;
        }

        int32_t translateY( int32_t y ) const
        {
            return _y + y;
        }

        Rect area( const Rect & r ) const
        {
            return translate( scale( r ) );
        }

        Rect scale( const Rect & r ) const
        {
            return { scale( r.x ), scale( r.y ), scale( r.width ), scale( r.height ) };
        }

        Rect translate( const Rect & r ) const
        {
            return { translateX( r.x ), translateY( r.y ), r.width, r.height };
        }

    private:
        Display & _display;
        int32_t _x;
        int32_t _y;
    };

    class Cursor
    {
    public:
        friend Display;
        virtual ~Cursor() = default;

        virtual void show( const bool enable )
        {
            _show = enable;
        }

        virtual bool isVisible() const
        {
            return _show;
        }

        bool isFocusActive() const;

        virtual void update( const fheroes2::Image & image, int32_t offsetX, int32_t offsetY )
        {
            _image = fheroes2::Sprite( image, offsetX, offsetY );
        }

        void setPosition( int32_t x, int32_t y )
        {
            _image.setPosition( x, y );
        }

        // Default implementation of Cursor uses software emulation.
        virtual void enableSoftwareEmulation( const bool )
        {
            // Do nothing.
        }

        bool isSoftwareEmulation() const
        {
            return _emulation;
        }

        void registerUpdater( void ( *cursorUpdater )() )
        {
            _cursorUpdater = cursorUpdater;
        }

    protected:
        Sprite _image;
        bool _emulation;
        bool _show;
        void ( *_cursorUpdater )();

        Cursor()
            : _emulation( true )
            , _show( false )
            , _cursorUpdater( nullptr )
        {}
    };

    BaseRenderEngine & engine();
    Cursor & cursor();
}
