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
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#if defined (GDK_WINDOWING_X11)
#include <gdk/gdkx.h>
#elif defined (GDK_WINDOWING_WIN32)
#include <gdk/gdkwin32.h>
#elif defined (GDK_WINDOWING_QUARTZ)
#include <gdk/gdkquartz.h>
#endif

SDL_Window *window;
SDL_Renderer *renderer;

static gboolean
draw_callback (GtkWidget *widget, arg_unused (gpointer data))
{
  SDL_SetRenderDrawColor (renderer, 255, 0, 255, 255);
  SDL_RenderClear (renderer); 
  SDL_RenderPresent(renderer); 

  return TRUE;
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

int
main (int argc, char **argv)
{
  GtkWidget *toplevel_window;

  gtk_init (&argc, &argv);

  // Create application window.
  toplevel_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_assert_nonnull(toplevel_window);
  gtk_window_set_title (GTK_WINDOW (toplevel_window), PACKAGE_STRING);
  gtk_window_set_default_size (GTK_WINDOW (toplevel_window), 800, 600);
  gtk_window_set_position(GTK_WINDOW(toplevel_window), GTK_WIN_POS_CENTER);
  gtk_container_set_border_width(GTK_CONTAINER(toplevel_window), 5);
  
  //Create Drawing area
  GtkWidget *draw_area = gtk_gl_area_new();
//  gtk_widget_set_app_paintable(draw_area, TRUE);
  gtk_gl_area_set_auto_render((GtkGLArea*)draw_area,TRUE);
  g_signal_connect (draw_area, "render", G_CALLBACK (draw_callback), NULL );
  gtk_container_add(GTK_CONTAINER(toplevel_window), draw_area);

  // Setup GTK+ callbacks.
  g_signal_connect (toplevel_window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  

  gtk_widget_show_all (toplevel_window);
  
  // SDL TEST
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 1;
  }
  window = create_sdl_window_from_gtk_widget(toplevel_window);
  g_assert_nonnull(window);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_TARGETTEXTURE);
  g_assert_nonnull(renderer);  

  // Enter event loop.
  gtk_main ();

  exit (EXIT_SUCCESS);
}
