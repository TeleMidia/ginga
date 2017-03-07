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
#include "DisplayDebug.h"
#include "SDLSurface.h"

#include <cairo.h>
#include <pango/pangocairo.h>

GINGA_MB_BEGIN

DisplayDebug::DisplayDebug(int width, int height){
   
    this->accTime = 1.0;
    this->totalTime = 0;
    this->fps = 0;

    this->iniTime= (gdouble)g_get_monotonic_time()/G_USEC_PER_SEC;

    this->width = width;
    this->height = height;

    this->fpsTexture = NULL;
    this->timerTexture = NULL;
    this->fileTexture = NULL; 

    //fps texture area
    fps_rect.w = 70; 
    fps_rect.h = 23; 
    fps_rect.x = this->width - fps_rect.w;  
    fps_rect.y = this->height - fps_rect.h; 

    timer_rect.w = 135; 
    timer_rect.h = 23; 
    timer_rect.x = this->width - timer_rect.w;  
    timer_rect.y = 0; 
}

DisplayDebug::~DisplayDebug(){
    SDL_DestroyTexture(this->fpsTexture);
    SDL_DestroyTexture(this->timerTexture);
    SDL_DestroyTexture(this->fileTexture);
}

void
DisplayDebug::update(gdouble elapsedTime){

    this->totalTime = ((gdouble)g_get_monotonic_time()/G_USEC_PER_SEC) - iniTime;
    this->accTime += elapsedTime;
    this->fps = (guint)(1/elapsedTime);
  
}

void
DisplayDebug::draw(SDL_Renderer * renderer){
    
    if(accTime >= 0.5){ //every 0.33s  
        //update textures
        SDL_DestroyTexture(this->fpsTexture);
        this->fpsTexture = updateTexture(renderer, fps_rect,
                               g_strdup_printf("%d FPS", fps));
        g_assert_nonnull (this->fpsTexture);

        SDL_DestroyTexture(this->timerTexture);
        this->timerTexture = updateTexture(renderer, timer_rect,
                               g_strdup_printf("Time %02d:%02d:%02d", 
                               (guint)this->totalTime/60,
                               ((guint)this->totalTime)%60,
                               (guint)(this->totalTime*100)%100));                     
        g_assert_nonnull (this->timerTexture);                         
        
        accTime=0; 
     }
    
     SDL_RenderCopy(renderer, this->fpsTexture, NULL, &fps_rect); 
     SDL_RenderCopy(renderer, this->timerTexture, NULL, &timer_rect); 
  
}

SDL_Texture *
DisplayDebug::updateTexture(SDL_Renderer * renderer, SDL_Rect rect, gchar * fps_str){
    SDL_Surface *sfc;
    cairo_t *cr;
    cairo_surface_t *surface_c;

#if SDL_VERSION_ATLEAST(2,0,5)
    sfc = SDL_CreateRGBSurfaceWithFormat (0, rect.w,
                                           rect.h,
                                           32, SDL_PIXELFORMAT_ARGB8888);
#else
    sfc = SDL_CreateRGBSurface (0, rect.w, rect.h, 32,
                              0xff000000,
                              0x00ff0000,
                              0x0000ff00,
                              0x000000ff); 
#endif

    g_assert_nonnull (sfc);
   
    SDLx_LockSurface (sfc);
    surface_c = cairo_image_surface_create_for_data ((guchar*) sfc->pixels,
                                                CAIRO_FORMAT_ARGB32,
                                                sfc->w, sfc->h, sfc->pitch);
    cr = cairo_create (surface_c);
    g_assert_nonnull (cr);
    //background
    cairo_set_source_rgba (cr,0,0,0,0.5);

    cairo_paint (cr);
    // Create a PangoLayout, set the font face and text
    PangoLayout * layout = pango_cairo_create_layout (cr);
    pango_layout_set_text (layout, fps_str, -1);
    PangoFontDescription *desc = pango_font_description_from_string ( "Arial 20px" );
    pango_layout_set_font_description (layout, desc);

    pango_font_description_free (desc);
    cairo_set_source_rgba (cr,1,1,1,10.5);
    pango_cairo_update_layout (cr, layout);
    pango_cairo_show_layout (cr, layout);
    
    // free the layout object
    g_object_unref (layout);
    cairo_destroy (cr);
    cairo_surface_destroy (surface_c);
    g_free(fps_str);
   
    SDL_Texture * texture = SDL_CreateTextureFromSurface (renderer, sfc);
    SDLx_UnlockSurface (sfc);
    SDL_FreeSurface(sfc);
    return texture; 
}

GINGA_MB_END