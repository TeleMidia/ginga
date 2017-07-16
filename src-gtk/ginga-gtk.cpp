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

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600

#if defined (GDK_WINDOWING_X11)
#include <gdk/gdkx.h>
#define MENU_BOX_HEIGHT 0
#elif defined (GDK_WINDOWING_WIN32)
#include <gdk/gdkwin32.h>
#define MENU_BOX_HEIGHT 30
#elif defined (GDK_WINDOWING_QUARTZ)
#include <gdk/gdkquartz.h>
#define MENU_BOX_HEIGHT 30
#endif

//annotation
//light #303030
//dark  #EBEBEB

typedef struct{
  GtkWidget *toplevel_window;
  GtkWidget *tvcontrol_window;
  GtkWidget *remote_control_window;
  guint16 default_margin = 5;   
  GtkWidget *canvas;
  guint16 controller_container_height = 48;
  GtkWidget *play_button;
  GtkWidget *stop_button;
  GtkWidget *config_button;
  GtkWidget *fullscreen_button;
  GtkWidget *volume_button;
  gchar *executable_folder = g_strdup("/home/busson/ginga/src-gtk/"); //fix me
  SDL_Rect window_rect = {0,0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT+30};
  SDL_Rect canvas_rect = {0,MENU_BOX_HEIGHT, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT};
  SDL_Rect controll_area_rect = {0,DEFAULT_WINDOW_HEIGHT+MENU_BOX_HEIGHT+15, DEFAULT_WINDOW_WIDTH,20}; 
}Ginga_GUI; 

Ginga_GUI ginga_gui;

//SDL_Window *window;
//SDL_Renderer *renderer;

static gboolean
resize_callback(GtkWidget *widget, gpointer data){
 /* guint16 window_width = (guint16)gtk_widget_get_allocated_width(widget);
  guint16 window_height = (guint16)gtk_widget_get_allocated_height(widget);
  
  gtk_widget_set_size_request(ginga_gui.canvas, 
     window_width - ginga_gui.toplevel_container_margin,
     window_height - ginga_gui.toplevel_container_margin - ginga_gui.controller_container_height);


  printf(" %d - %d \n", window_width, window_height);  */
  return TRUE;
}

static gboolean
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

  return TRUE;
}

static void 
create_tvcontrol_window(GtkWidget *widget, gpointer data){
    
    ginga_gui.tvcontrol_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_assert_nonnull(ginga_gui.tvcontrol_window);
    gtk_window_set_title (GTK_WINDOW (ginga_gui.tvcontrol_window), "TV Control");
    gtk_window_set_default_size (GTK_WINDOW (ginga_gui.tvcontrol_window), 220, 400);
    gtk_window_set_position(GTK_WINDOW(ginga_gui.tvcontrol_window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(ginga_gui.tvcontrol_window), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(ginga_gui.tvcontrol_window), ginga_gui.default_margin);
    
    GtkWidget *fixed_layout = gtk_fixed_new( );
    g_assert_nonnull(fixed_layout);
   //4 -> 55
    GdkColor color;
    color.red = 65535;
    color.blue = 0;
    color.green = 0;
    
    
    GtkWidget *button_1 = gtk_button_new_with_label ("1");
    g_assert_nonnull(button_1);
    gtk_widget_set_size_request(button_1, 40, 40);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_1, 25, 0);

    GtkWidget *button_2 = gtk_button_new_with_label ("2");
    g_assert_nonnull(button_2);
    gtk_widget_set_size_request(button_2, 40, 40);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_2, 80, 0);

    GtkWidget *button_3 = gtk_button_new_with_label ("3");
    g_assert_nonnull(button_3);
    gtk_widget_set_size_request(button_3, 40, 40);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_3, 135, 0);

    //

    GtkWidget *button_4 = gtk_button_new_with_label ("4");
    g_assert_nonnull(button_4);
    gtk_widget_set_size_request(button_4, 40, 40);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_4, 25, 50);

    GtkWidget *button_5 = gtk_button_new_with_label ("5");
    g_assert_nonnull(button_5);
    gtk_widget_set_size_request(button_5, 40, 40);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_5, 80, 50);

    GtkWidget *button_6 = gtk_button_new_with_label ("6");
    g_assert_nonnull(button_6);
    gtk_widget_set_size_request(button_6, 40, 40);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_6, 135, 50);

    //

    GtkWidget *button_red = gtk_button_new_with_label ("C");
    g_assert_nonnull(button_red);
    gtk_widget_set_size_request(button_red, 40, 40);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_red, 0, 200);

    GtkWidget *button_green = gtk_button_new_with_label ("C");
    g_assert_nonnull(button_green);
    gtk_widget_set_size_request(button_green, 40, 40);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_green, 55, 200);

    GtkWidget *button_yellow = gtk_button_new_with_label ("C");
    g_assert_nonnull(button_yellow);
    gtk_widget_set_size_request(button_yellow, 40, 40);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_yellow, 110, 200);

    GtkWidget *button_blue = gtk_button_new_with_label ("C");
    g_assert_nonnull(button_blue);
    gtk_widget_set_size_request(button_blue, 40, 40);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_blue, 165, 200);

    
    gtk_container_add(GTK_CONTAINER(ginga_gui.tvcontrol_window), fixed_layout);
    gtk_widget_show_all(ginga_gui.tvcontrol_window);
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
    gtk_window_set_default_size (GTK_WINDOW (ginga_gui.toplevel_window), ginga_gui.window_rect.w, ginga_gui.window_rect.h);
    gtk_window_set_position(GTK_WINDOW(ginga_gui.toplevel_window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(ginga_gui.toplevel_window), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(ginga_gui.toplevel_window), ginga_gui.default_margin); 

    GtkWidget *fixed_layout = gtk_fixed_new( );
    g_assert_nonnull(fixed_layout);
    //Create Drawing area
    ginga_gui.canvas = gtk_drawing_area_new();
    g_assert_nonnull(ginga_gui.canvas);
    gtk_widget_set_app_paintable(ginga_gui.canvas, TRUE);
    g_signal_connect( ginga_gui.canvas, "draw", G_CALLBACK (draw_callback), NULL );
    gtk_widget_set_size_request(ginga_gui.canvas, ginga_gui.canvas_rect.w, ginga_gui.canvas_rect.h);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  ginga_gui.canvas,  ginga_gui.canvas_rect.x,  ginga_gui.canvas_rect.y);
    
    GtkWidget * canvas_separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_size_request(canvas_separator, ginga_gui.canvas_rect.w, 2);
    gtk_fixed_put(GTK_FIXED (fixed_layout), canvas_separator, 0, ginga_gui.canvas_rect.y + ginga_gui.canvas_rect.h + ginga_gui.default_margin );
    
    // ----- top-menu begin
    
    GtkWidget *menu_bar = gtk_menu_bar_new();
    g_assert_nonnull(menu_bar);
    
    
    //File
    GtkWidget *menu_file = gtk_menu_new();
    g_assert_nonnull(menu_file);  
    GtkWidget *menu_item_file = gtk_menu_item_new_with_label("File");
    g_assert_nonnull(menu_item_file);
    GtkWidget *menu_item_open = gtk_menu_item_new_with_label("Open");
    g_assert_nonnull(menu_item_open);
    GtkWidget *menu_item_quit = gtk_menu_item_new_with_label("Quit");
    g_assert_nonnull(menu_item_quit);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item_file), menu_file);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), menu_item_open);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), menu_item_quit);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item_file);

    //Tools
    GtkWidget *menu_tool = gtk_menu_new();
    g_assert_nonnull(menu_tool);  
    GtkWidget *menu_item_tools = gtk_menu_item_new_with_label("Tools");
    g_assert_nonnull(menu_item_tools);
    GtkWidget *menu_item_tvcontrol = gtk_menu_item_new_with_label("TV Control");
    g_assert_nonnull(menu_item_tvcontrol);
    g_signal_connect(menu_item_tvcontrol, "activate", G_CALLBACK(create_tvcontrol_window), NULL); 
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item_tools),menu_tool);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_tool), menu_item_tvcontrol);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item_tools);

    //Help
    GtkWidget *menu_help = gtk_menu_new();
    g_assert_nonnull(menu_help);
    GtkWidget *menu_item_help = gtk_menu_item_new_with_label("Help");
    g_assert_nonnull(menu_item_help);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item_help),menu_help);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item_help); 
    
    
    GtkWidget *menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(menu_box), menu_bar, TRUE, TRUE, 3);
    gtk_fixed_put(GTK_FIXED (fixed_layout), menu_box, 0, 0);

    // ----- top-menu end
    

    
    GtkWidget *play_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/play-icon.png",NULL));
    g_assert_nonnull(play_icon);
    ginga_gui.play_button = gtk_button_new();
    g_assert_nonnull(ginga_gui.play_button);
    gtk_button_set_image(GTK_BUTTON(ginga_gui.play_button), play_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  ginga_gui.play_button, 0, ginga_gui.controll_area_rect.y);

    GtkWidget *stop_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/stop-icon.png",NULL));
    g_assert_nonnull(stop_icon);
    ginga_gui.stop_button = gtk_button_new();
    g_assert_nonnull(ginga_gui.stop_button);
    gtk_button_set_image(GTK_BUTTON(ginga_gui.stop_button), stop_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  ginga_gui.stop_button, 30, ginga_gui.controll_area_rect.y);

    GtkWidget *time_label = gtk_label_new ("00:00");
    gtk_label_set_markup (GTK_LABEL(time_label), g_markup_printf_escaped("<span font=\"13\"><b>\%s</b></span>", "00:00"));
    g_assert_nonnull(time_label);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  time_label, 65, ginga_gui.controll_area_rect.y+4);

   
    GtkWidget *fullscreen_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/fullscreen-icon.png",NULL));
    g_assert_nonnull(fullscreen_icon);
    ginga_gui.fullscreen_button = gtk_button_new();
    g_assert_nonnull(ginga_gui.fullscreen_button);
    gtk_button_set_image(GTK_BUTTON(ginga_gui.fullscreen_button), fullscreen_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  ginga_gui.fullscreen_button, 740, ginga_gui.controll_area_rect.y);

    GtkWidget *config_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/settings-icon.png",NULL));
    g_assert_nonnull(config_icon);
    ginga_gui.config_button = gtk_button_new();
    g_assert_nonnull(ginga_gui.config_button);
    gtk_button_set_image(GTK_BUTTON(ginga_gui.config_button), config_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  ginga_gui.config_button, 770, ginga_gui.controll_area_rect.y);
    
    ginga_gui.volume_button = gtk_volume_button_new ();
    g_assert_nonnull(ginga_gui.volume_button);
    gtk_fixed_put(GTK_FIXED (fixed_layout), ginga_gui.volume_button, 710, ginga_gui.controll_area_rect.y);

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
