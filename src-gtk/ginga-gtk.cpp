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

//annotation
//light #303030
//dark  #EBEBEB

typedef struct{
  GtkWidget *toplevel_window;
  GtkWidget *remote_control_window;
  guint16 toplevel_container_margin = 5;   
  GtkWidget *canvas;
  guint16 controller_container_height = 48;
  GtkWidget *play_button;
  GtkWidget *stop_button;
  GtkWidget *config_button;
  GtkWidget *fullscreen_button;
  GtkWidget *volume_button;

}Ginga_GUI; 

Ginga_GUI ginga_gui;

//SDL_Window *window;
//SDL_Renderer *renderer;

static gboolean
resize_callback(GtkWidget *widget, gpointer data){
  guint16 window_width = (guint16)gtk_widget_get_allocated_width(widget);
  guint16 window_height = (guint16)gtk_widget_get_allocated_height(widget);
  
  gtk_widget_set_size_request(ginga_gui.canvas, 
     window_width - ginga_gui.toplevel_container_margin,
     window_height - ginga_gui.toplevel_container_margin - ginga_gui.controller_container_height);


  printf("hahaha %d - %d \n", window_width, window_height);  
  return TRUE;
}

static gboolean
draw_callback(GtkWidget *widget, cairo_t *cr, arg_unused (gpointer data)){
  int w, h;

  w = gtk_widget_get_allocated_width (widget);
  h = gtk_widget_get_allocated_height (widget);

  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_set_source_rgb (cr, 255, 0, 0);
  cairo_rectangle (cr, 0, 0, w, h);
  cairo_fill (cr);

 /*
  SDL_SetRenderDrawColor (renderer, 255, 0, 255, 255);
  SDL_RenderClear (renderer); 
  SDL_RenderPresent(renderer); 
 */

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

void 
create_gtk_layout(){

    // Create application window.
    ginga_gui.toplevel_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_assert_nonnull(ginga_gui.toplevel_window);
    gtk_window_set_title (GTK_WINDOW (ginga_gui.toplevel_window), PACKAGE_STRING);
    gtk_window_set_default_size (GTK_WINDOW (ginga_gui.toplevel_window), 800, 660);
    gtk_window_set_position(GTK_WINDOW(ginga_gui.toplevel_window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(ginga_gui.toplevel_window), ginga_gui.toplevel_container_margin); 

    GtkWidget *fixed_layout = gtk_fixed_new( );
    g_assert_nonnull(fixed_layout);
    //Create Drawing area
    ginga_gui.canvas = gtk_drawing_area_new();
    g_assert_nonnull(ginga_gui.canvas);
    gtk_widget_set_app_paintable(ginga_gui.canvas, TRUE);
    g_signal_connect( ginga_gui.canvas, "draw", G_CALLBACK (draw_callback), NULL );
    gtk_fixed_put(GTK_FIXED (fixed_layout),  ginga_gui.canvas, 0, 0);
   

 

    GtkWidget *play_icon = gtk_image_new_from_file ("/home/busson/ginga/src-gtk/icons/light-theme/play-icon.png");
    g_assert_nonnull(play_icon);
    ginga_gui.play_button = gtk_button_new();
    g_assert_nonnull(ginga_gui.play_button);
    gtk_button_set_image(GTK_BUTTON(ginga_gui.play_button), play_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  ginga_gui.play_button, 0, 630);

    GtkWidget *stop_icon = gtk_image_new_from_file ("/home/busson/ginga/src-gtk/icons/light-theme/stop-icon.png");
    g_assert_nonnull(stop_icon);
    ginga_gui.stop_button = gtk_button_new();
    g_assert_nonnull(ginga_gui.stop_button);
    gtk_button_set_image(GTK_BUTTON(ginga_gui.stop_button), stop_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  ginga_gui.stop_button, 40, 630);

    GtkWidget *time_label = gtk_label_new ("00:00");
    gtk_label_set_markup (GTK_LABEL(time_label), g_markup_printf_escaped("<span font=\"13\"><b>\%s</b></span>", "00:00"));
    g_assert_nonnull(time_label);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  time_label, 85, 638);

   
    GtkWidget *fullscreen_icon = gtk_image_new_from_file ("/home/busson/ginga/src-gtk/icons/light-theme/fullscreen-icon.png");
    g_assert_nonnull(fullscreen_icon);
    ginga_gui.fullscreen_button = gtk_button_new();
    g_assert_nonnull(ginga_gui.fullscreen_button);
    gtk_button_set_image(GTK_BUTTON(ginga_gui.fullscreen_button), fullscreen_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  ginga_gui.fullscreen_button, 750, 630);

    GtkWidget *config_icon = gtk_image_new_from_file ("/home/busson/ginga/src-gtk/icons/light-theme/settings-icon.png");
    g_assert_nonnull(config_icon);
    ginga_gui.config_button = gtk_button_new();
    g_assert_nonnull(ginga_gui.config_button);
    gtk_button_set_image(GTK_BUTTON(ginga_gui.config_button), config_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  ginga_gui.config_button, 790, 630);
    
    ginga_gui.volume_button = gtk_volume_button_new ();
    g_assert_nonnull(ginga_gui.volume_button);
    gtk_fixed_put(GTK_FIXED (fixed_layout), ginga_gui.volume_button, 600, 630);

    gtk_container_add(GTK_CONTAINER(ginga_gui.toplevel_window), fixed_layout);
     // Setup GTK+ callbacks.
    g_signal_connect(ginga_gui.toplevel_window, "check-resize", G_CALLBACK(resize_callback), NULL); 
    g_signal_connect(ginga_gui.toplevel_window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  
    gtk_widget_show_all (ginga_gui.toplevel_window);
}

int
main (int argc, char **argv)
{
    gtk_init (&argc, &argv);
    create_gtk_layout();
  
  // SDL TEST
 /* if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 1;
  }
  window = create_sdl_window_from_gtk_widget(toplevel_window);
  g_assert_nonnull(window);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_TARGETTEXTURE);
  g_assert_nonnull(renderer);  */

  // Enter event loop.
  gtk_main ();

  exit (EXIT_SUCCESS);
}
