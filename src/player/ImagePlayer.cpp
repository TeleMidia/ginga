/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#include "ginga.h"
#include "ImagePlayer.h"

#include "mb/Display.h"
#include "mb/SDLWindow.h"

#include "librsvg/rsvg.h"


using namespace ::ginga::mb;

GINGA_PLAYER_BEGIN


// Private methods.
SDL_Surface *
ImagePlayer::decodeSVG(int width, int height){
   
   RsvgHandle* h;
   RsvgDimensionData dim;
  
   GError* e = NULL;
   cairo_surface_t *cairoSurface;
   cairo_t *cairoState;

   /* RSVG initiation */
   h = rsvg_handle_new_from_file(this->mrl.c_str (), &e);
   rsvg_handle_get_dimensions (h, &dim);

   // precisamos ver o quanto vamos escalar... Usando 1920x1080 como pior caso...
   // Ver: http://www.svgopen.org/2009/presentations/62-Rendering_SVG_graphics_with_libSDL_a_crossplatform_multimedia_library/index.pdf
   double scale = (dim.width > dim.height)? (double) 1920 / dim.width : (double) 1080 / dim.height;

   int x = floor(dim.width * scale) + 1;
   int y = floor(dim.height * scale) + 1;
   int stride = x * 4; // ARGB

   /* Cairo Initiation */
   uint8_t *image = (uint8_t *) malloc (stride * y); // ARGB uses 4 bytes / pixel

   // here the correct would be...
   cairoSurface = cairo_image_surface_create_for_data (image, CAIRO_FORMAT_ARGB32, x, y, stride);
   cairoState = cairo_create (cairoSurface);

   cairo_scale(cairoState, scale, scale);
   rsvg_handle_render_cairo (h, cairoState);

   // Use the following line for debug purposes
//        cairo_surface_write_to_png (cairoSurface, "/tmp/out.png");

        // Match ARGB32 format masks
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        Uint32 rmask = 0x0000ff00;
        Uint32 gmask = 0x00ff0000;
        Uint32 bmask = 0xff000000;
        Uint32 amask = 0x000000ff;
#else
        Uint32 rmask = 0x00ff0000;
        Uint32 gmask = 0x0000ff00;
        Uint32 bmask = 0x000000ff;
        Uint32 amask = 0xff000000;
#endif

        SDL_Surface *sdlSurface = SDL_CreateRGBSurfaceFrom ( (void *) image, x, y, 32, stride, rmask, gmask, bmask, amask);

        cairo_surface_destroy (cairoSurface);
        cairo_destroy (cairoState);

        return sdlSurface;

}


bool
ImagePlayer::displayJobCallbackWrapper (DisplayJob *job,
                                        SDL_Renderer *renderer,
                                        void *self)
{
  return ((ImagePlayer *) self)->displayJobCallback (job, renderer);
}

bool
ImagePlayer::displayJobCallback (arg_unused (DisplayJob *job),
                                 SDL_Renderer *renderer)
{
  SDL_Texture *texture;
  SDLWindow *window;
  
   if( mrl.substr(mrl.find_last_of(".") + 1) == "svg" ||
       mrl.substr(mrl.find_last_of(".") + 1) == "svgz"){
       texture = SDL_CreateTextureFromSurface(renderer, this->decodeSVG(100,100));
  }
  else{ 
      texture = IMG_LoadTexture (renderer, this->mrl.c_str ());
      if (unlikely (texture == NULL))
          g_error ("cannot load image file %s: %s", this->mrl.c_str (),
      IMG_GetError ());
  }           

  this->lock ();
  window = surface->getParentWindow ();
  g_assert_nonnull (window);
  window->setTexture (texture);
  this->unlock ();
  this->condDisplayJobSignal ();
  return false;                 // remove job
}


// Public methods.

ImagePlayer::ImagePlayer (const string &uri) : Player (uri)
{
  this->mutexInit ();
  this->condDisplayJobInit ();
  this->surface = new SDLSurface ();
}

ImagePlayer::~ImagePlayer (void)
{
  this->condDisplayJobClear ();
  this->mutexClear ();
}

bool
ImagePlayer::play ()
{
  Ginga_Display->addJob (displayJobCallbackWrapper, this);
  this->condDisplayJobWait ();
  return Player::play ();
}

GINGA_PLAYER_END
