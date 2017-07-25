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

#ifndef GINGA_GTK_H
#define GINGA_GTK_H

#include "ginga.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <string.h>

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600

#if defined (GDK_WINDOWING_X11)
#include <gdk/gdkx.h>
#define MENU_BOX_HEIGHT 38
#define BUTTON_SIZE 30
#elif defined (GDK_WINDOWING_WIN32)
#include <gdk/gdkwin32.h>
#define MENU_BOX_HEIGHT 30
#define BUTTON_SIZE 30
#elif defined (GDK_WINDOWING_QUARTZ)
#include <gdk/gdkquartz.h>
#define MENU_BOX_HEIGHT 65
#define BUTTON_SIZE 35
#endif

typedef struct{
  GtkWidget *toplevel_window = NULL;
  GtkWidget *fixed_layout = NULL;
  GtkWidget *notebook = NULL;
  guint16 default_margin = 5;   
  GtkWidget *canvas = NULL;
  GtkWidget *log_view = NULL;
  GtkWidget *menu_bar = NULL;
  guint16 controller_container_height = 48;
  GtkWidget *file_entry = NULL;
  GtkWidget *play_button = NULL;
  GtkWidget *stop_button = NULL;
  GtkWidget *time_label = NULL;
  GtkWidget *config_button = NULL;
  GtkWidget *fullscreen_button = NULL;
  GtkWidget *volume_button = NULL;
  GtkWidget *open_button = NULL;
  GtkWidget *canvas_separator_bottom = NULL;
  gchar *executable_folder = NULL;
  SDL_Rect window_rect = {0,0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT+30};
  SDL_Rect canvas_rect = {0,MENU_BOX_HEIGHT, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT};
  SDL_Rect controll_area_rect = {0,DEFAULT_WINDOW_HEIGHT+MENU_BOX_HEIGHT+10, DEFAULT_WINDOW_WIDTH,20}; 
  gboolean playMode = FALSE;
}Ginga_GUI; 

typedef struct{
  SDL_Rect main_window_rect = {0,0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT+30};
  SDL_Rect canvas_rect = {0,MENU_BOX_HEIGHT, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT};
  SDL_Rect control_area_rect = {0,DEFAULT_WINDOW_HEIGHT+MENU_BOX_HEIGHT+10, DEFAULT_WINDOW_WIDTH,20};   
}gui_layout_param;  

extern Ginga_GUI ginga_gui;

extern GtkWidget *tvcontrolWindow;
extern GtkWidget *fullscreenWindow;
extern GtkWidget *settingsWindow;
extern GtkWidget *aboutWindow;
extern gboolean isFullScreenMode;
extern gboolean isDebugMode;
extern gboolean isCrtlModifierActive;

/* View/MainWindow */ 
void create_main_window(void);
void destroy_main_window(void);
void enable_disable_debug(void);
void select_ncl_file_callback(GtkWidget *widget, gpointer data);
void play_pause_ginga(void);

/* View/TvControlWindow */
void create_tvcontrol_window(void);
void destroy_tvcontrol_window(void);

/* View/FullscreenWindow */
void create_fullscreen_window(void);
void destroy_fullscreen_window(void); 
void set_fullscreen_mode(void);
void set_unfullscreen_mode(void);

/* View/SettingsWindow */
void create_settings_window(void);
void destroy_settings_window(void);

/* View/AboutWindow */
void create_about_window(void);
void destroy_about_window(void);

/* View/Draw */
SDL_Window* create_sdl_window_from_gtk_widget(GtkWidget *gtk_widget);
void draw_callback(GtkWidget *widget, cairo_t *cr, arg_unused (gpointer data));

/* Control/KeyEvent */
void key_press_event_callback(GtkWidget *widget, GdkEventKey *event);
void key_release_event_callback(GtkWidget *widget, GdkEventKey *event);


#endif /* GINGA_GTK_H */