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

#include "ginga_gtk.h"

void
draw_callback(GtkWidget *widget, cairo_t *cr, arg_unused (gpointer data)){
  int w, h;

  w = gtk_widget_get_allocated_width (widget);
  h = gtk_widget_get_allocated_height (widget);

  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_set_source_rgb (cr,0, 0, 0);
  cairo_rectangle (cr, 0, 0, w, h);
  cairo_fill (cr);

 /*
  SDL_SetRenderDrawColor (renderer, 255, 0, 255, 255);
  SDL_RenderClear (renderer); 
  SDL_RenderPresent(renderer); 
 */

}

SDL_Window*
create_sdl_window_from_gtk_widget(GtkWidget *gtk_widget){
    GdkWindow* gdk_window = gtk_widget_get_window (gtk_widget); 
    g_assert_nonnull (gdk_window);

#if defined (GDK_WINDOWING_WIN32)
    return SDL_CreateWindowFrom((void*)GDK_WINDOW_HWND(gdk_window));
#elif defined (GDK_WINDOWING_QUARTZ)    
    return SDL_CreateWindowFrom((void*)gdk_quartz_window_get_nswindow(gdk_window));
#elif defined (GDK_WINDOWING_X11)
    return SDL_CreateWindowFrom((void*)gdk_x11_window_get_xid(GDK_X11_WINDOW(gdk_window)));
#endif      
}
