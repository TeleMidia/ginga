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
#include "ginga-color-table.h"
#include "TextPlayer.h"


#include <cairo.h>
#include <pango/pangocairo.h>
#include <stdio.h>
#include <string.h>


#include "mb/Display.h"
#include "mb/SDLWindow.h"

using namespace ::ginga::mb;

GINGA_PLAYER_BEGIN


// Private methods.
bool
TextPlayer::displayJobCallbackWrapper (DisplayJob *job,
                                        SDL_Renderer *renderer,
                                        void *self)
{
  return ((TextPlayer *) self)->displayJobCallback (job, renderer);
}

bool
TextPlayer::displayJobCallback (arg_unused (DisplayJob *job),
                                   arg_unused (SDL_Renderer *renderer))
{
    gchar *contents;
    GError *err = NULL;
    g_file_get_contents (this->mrl.c_str (), &contents, NULL, &err);
    if (err != NULL) g_error_free (err);
    g_assert_nonnull(contents);

    SDL_Texture *texture;

    SDL_Surface *sfc;
    cairo_t *cr;
    cairo_surface_t *surface_c;

    double vAlign=0;
    int textAreaHeight;

    this->lock ();

#if SDL_VERSION_ATLEAST(2,0,5)
    sfc = SDL_CreateRGBSurfaceWithFormat (0, this->rect.w,
                                           this->rect.h,
                                           32, SDL_PIXELFORMAT_ARGB8888);
#else
    sfc = SDL_CreateRGBSurface (0, this->rect.w, this->rect.h, 32,
                              0xff000000,
                              0x00ff0000,
                              0x0000ff00,
                              0x000000ff); 
#endif

    g_assert_nonnull (sfc);
    this->unlock ();

    SDLx_LockSurface (sfc);
    surface_c = cairo_image_surface_create_for_data ((guchar*) sfc->pixels,
                                                CAIRO_FORMAT_ARGB32,
                                                sfc->w, sfc->h, sfc->pitch);
    cr = cairo_create (surface_c);
    g_assert_nonnull (cr);
    //background
    cairo_set_source_rgba (cr,ginga_color_percent(bgColor.r),
                              ginga_color_percent(bgColor.g),
                              ginga_color_percent(bgColor.b),
                              ginga_color_percent(bgColor.a));

    cairo_paint (cr);

    // Create a PangoLayout, set the font face and text
    PangoLayout * layout = pango_cairo_create_layout (cr);
    pango_layout_set_text (layout,  contents, -1);
    string fontDescription = fontFamily+" "+fontWeight+" "+fontStyle+" "+fontSize;
    PangoFontDescription *desc = pango_font_description_from_string ( fontDescription.c_str() );
    pango_layout_set_font_description (layout, desc);

    if(textAlign == "left")
        pango_layout_set_alignment(layout,PANGO_ALIGN_LEFT);
    else if(textAlign == "center")
        pango_layout_set_alignment(layout,PANGO_ALIGN_CENTER);
    else if(textAlign == "right")
        pango_layout_set_alignment(layout,PANGO_ALIGN_RIGHT);
    else
        pango_layout_set_justify(layout, true);

    pango_layout_set_width (layout,this->rect.w*PANGO_SCALE);
    pango_layout_set_wrap (layout,PANGO_WRAP_WORD);
    pango_layout_get_size (layout, NULL, &textAreaHeight);
    pango_font_description_free (desc);

    cairo_set_source_rgba (cr, ginga_color_percent(fontColor.r),
                               ginga_color_percent(fontColor.g),
                               ginga_color_percent(fontColor.b),
                               ginga_color_percent(fontColor.a));

    pango_cairo_update_layout (cr, layout);

    if(verticalAlign == "top") vAlign = 0;
    else if(verticalAlign == "middle") vAlign = (this->rect.h/2) - ( (textAreaHeight/PANGO_SCALE) /2);
    else vAlign= this->rect.h - (textAreaHeight/PANGO_SCALE);

    cairo_move_to (cr, 0, vAlign);
    pango_cairo_show_layout (cr, layout);
    
    

    // free the layout object
    g_object_unref (layout);
    cairo_destroy (cr);
    cairo_surface_destroy (surface_c);
    g_free(contents);

    this->texture = SDL_CreateTextureFromSurface (renderer, sfc);
    g_assert_nonnull (this->texture);
    
    SDLx_UnlockSurface (sfc);
    SDL_FreeSurface(sfc);
     
   // this->window->setTexture (this->texture);

    this->condDisplayJobSignal ();
    return false;                 // remove job
}


// Public methods.

TextPlayer::TextPlayer (const string &uri) : Player (uri)
{
  //defalts attr values
  ginga_color_input_to_sdl_color("#0", &fontColor); //black
  ginga_color_input_to_sdl_color("#0000", &bgColor); //transparent
  fontFamily = "serif";
  fontStyle ="";
  fontSize ="18px";
  fontVariant="";
  fontWeight="";
  textAlign="left";
  verticalAlign="top";

  this->mutexInit ();
  this->condDisplayJobInit ();
}

TextPlayer::~TextPlayer (void)
{
  this->condDisplayJobClear ();
  this->mutexClear ();
}

bool
TextPlayer::play ()
{

  Ginga_Display->addJob (displayJobCallbackWrapper, this);
  this->condDisplayJobWait ();
  return Player::play ();
}

void
TextPlayer::setPropertyValue (const string &name, const string &value){

   Player::setPropertyValue(name,value);

  if(name == "fontColor"){
      ginga_color_input_to_sdl_color(value, &fontColor);
  }
  else if(name == "backgroundColor"){
      ginga_color_input_to_sdl_color(value, &bgColor);
  }
  else if(name == "fontSize"){
         fontSize = value;
  }
  else if(name == "textAlign"){
         if(value == "left" || value == "right" || value == "center" || value == "justify" )
            textAlign = value;
  }
  else if(name == "verticalAlign"){
         if(value == "top" || value == "middle" || value == "bottom" )
            verticalAlign = value;
  }
  else if(name == "fontStyle"){
         if(value == "italic")
            fontStyle = value;
  }
  else if(name == "fontWeight"){
         if(value == "bold")
            fontWeight = value;
  }
  else if(name == "fontFamily"){
         fontFamily = value;
  }
  else if(name == "fontVariant"){
         if(value == "small-caps")
            fontVariant = value;
  }

  if(status!=OCCURRING)return;
  Ginga_Display->addJob (displayJobCallbackWrapper, this);
  this->condDisplayJobWait ();
}

GINGA_PLAYER_END
