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
#include "NewTextPlayer.h"


#include <cairo.h>
#include <pango/pangocairo.h>
#include <stdio.h>

#include "mb/Display.h"
#include "mb/SDLWindow.h"
using namespace ::ginga::mb;

GINGA_PLAYER_BEGIN


// Private methods.
bool
NewTextPlayer::displayJobCallbackWrapper (DisplayJob *job,
                                        SDL_Renderer *renderer,
                                        void *self)
{
  return ((NewTextPlayer *) self)->displayJobCallback (job, renderer);
}

bool
NewTextPlayer::displayJobCallback (arg_unused (DisplayJob *job),
                                   arg_unused (SDL_Renderer *renderer))
{
   
    gchar *contents;
    GError *err = NULL;
    g_file_get_contents (this->mrl.c_str (), &contents, NULL, &err);
    if (err != NULL) g_error_free (err);
    g_assert_nonnull(contents);
  
    SDLWindow *window;
    SDL_Texture *texture;

    SDL_Surface *sfc;
    cairo_t *cr;
    cairo_status_t status;
    cairo_surface_t *surface_c;
  
    this->lock ();
    sfc = SDL_CreateRGBSurfaceWithFormat (0, this->surface->getParentWindow()->getRect().w, 
                                           this->surface->getParentWindow()->getRect().h, 
                                           32, SDL_PIXELFORMAT_ARGB32);
    g_assert_nonnull (sfc);                                       
    this->unlock ();                                      

    SDLx_LockSurface (sfc); 
    surface_c = cairo_image_surface_create_for_data ((guchar*) sfc->pixels,
                                                CAIRO_FORMAT_ARGB32,
                                                sfc->w, sfc->h, sfc->pitch);
    cr = cairo_create (surface_c);
    g_assert_nonnull (cr);
    //background
    //cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
    //cairo_paint (cr);
 
    PangoLayout *layout;
    PangoFontDescription *desc;

    // Center coordinates to origin
    cairo_translate (cr, 0, 0);

    // Create a PangoLayout, set the font and text 
    layout = pango_cairo_create_layout (cr);

    pango_layout_set_text (layout,  contents, -1);
    //"Sans Bold Italic Condensed 52.5px"
    string fontDescription = fontFamily+" "+fontWeight+" "+fontStyle+" "+fontSize;
    desc = pango_font_description_from_string ( fontDescription.c_str() );
    pango_layout_set_font_description (layout, desc);
    pango_font_description_free (desc);
    cairo_save (cr);
  
    //text rgb   
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
    pango_cairo_update_layout (cr, layout);  
    pango_cairo_show_layout (cr, layout);
    cairo_restore (cr);
  
    // free the layout object 
    g_object_unref (layout);
    cairo_destroy (cr);
    cairo_surface_destroy (surface_c);

    texture = SDL_CreateTextureFromSurface (renderer, sfc);
    g_assert_nonnull (texture); 
    SDLx_UnlockSurface (sfc);
              
    this->lock ();
    window = surface->getParentWindow ();
    g_assert_nonnull (window);
    window->setTexture (texture);
    this->unlock ();
    this->condDisplayJobSignal ();
    return false;                 // remove job
}


// Public methods.

NewTextPlayer::NewTextPlayer (const string &uri) : Player (uri)
{
  this->mutexInit ();
  this->condDisplayJobInit ();
  this->surface = new SDLSurface ();
}

NewTextPlayer::~NewTextPlayer (void)
{
  this->condDisplayJobClear ();
  this->mutexClear ();
}

bool
NewTextPlayer::play ()
{
 
  Ginga_Display->addJob (displayJobCallbackWrapper, this);
  this->condDisplayJobWait ();
  return Player::play ();
}

void 
NewTextPlayer::setPropertyValue (const string &name, const string &value){

  if(name == "fontColor"){
     //generate color
  }
  else if(name == "backgroundColor"){
     //generate color 
  }
  else if(name == "fontSize") fontSize = value;
  else if(name == "textAlign") textAlign = value;
  else if(name == "fontStyle") fontStyle = value;
  else if(name == "fontWeight") fontWeight = value;
  else if(name == "fontFamily") fontFamily = value;
  else if(name == "fontVariant") fontVariant = value; 
  
 // g_debug("%s ",name.c_str());
}

GINGA_PLAYER_END
