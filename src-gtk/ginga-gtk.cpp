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

#include <string.h>
#include <gdk/gdkkeysyms.h>

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
#define MENU_BOX_HEIGHT 72
#define BUTTON_SIZE 35
#endif

//annotation
//light #303030
//dark  #EBEBEB

typedef struct{
  GtkWidget *toplevel_window;
  GtkWidget *tvcontrol_window;
  GtkWidget *remote_control_window;
  GtkWidget *fixed_layout;
  GtkWidget *notebook;
  guint16 default_margin = 5;   
  GtkWidget *canvas;
  GtkWidget *menu_bar;
  guint16 controller_container_height = 48;
  GtkWidget *file_entry;
  GtkWidget *play_button;
  GtkWidget *stop_button;
  GtkWidget *time_label;
  GtkWidget *config_button;
  GtkWidget *fullscreen_button;
  GtkWidget *volume_button;
  GtkWidget *open_button;
  GtkWidget *canvas_separator_bottom;
  gchar *executable_folder; 
  SDL_Rect window_rect = {0,0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT+30};
  SDL_Rect canvas_rect = {0,MENU_BOX_HEIGHT, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT};
  SDL_Rect controll_area_rect = {0,DEFAULT_WINDOW_HEIGHT+MENU_BOX_HEIGHT+10, DEFAULT_WINDOW_WIDTH,20}; 
  gboolean debugMode = FALSE;
  gboolean playMode = FALSE;
  gboolean fullScreenMode = FALSE;
  gboolean crtlModifier = FALSE;
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
select_ncl_file(GtkWidget *widget, gpointer data){
    GtkWidget *dialog = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW(ginga_gui.toplevel_window),
                                      GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel",
                                      GTK_RESPONSE_CANCEL, "_Open",
                                      GTK_RESPONSE_ACCEPT, NULL);

    gint res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
      gchar *filename;
      GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
      filename = gtk_file_chooser_get_filename (chooser);

      gchar* ext = strrchr(filename, '.');

      if(g_strcmp0( ext , ".ncl")){
           GtkWidget * dialog_error = gtk_message_dialog_new ( GTK_WINDOW(ginga_gui.toplevel_window),
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_ERROR,
                                 GTK_BUTTONS_CLOSE,
                                 "Error reading “%s”: %s",
                                 filename,
                                 "is a invalid formart file.");
           gtk_dialog_run (GTK_DIALOG (dialog_error));
           gtk_widget_destroy (dialog_error);
      } 
      else{
           gtk_entry_set_text(GTK_ENTRY(ginga_gui.file_entry), filename); 
      }

      g_free (filename);
    }

    gtk_widget_destroy (dialog);                                  

}

static void
play_pause_ginga(GtkWidget *widget, gpointer data){
    ginga_gui.playMode = !ginga_gui.playMode;
    GtkWidget *play_icon = gtk_image_new_from_icon_name ("media-playback-start", GTK_ICON_SIZE_BUTTON);
    if(ginga_gui.playMode)
         play_icon = gtk_image_new_from_icon_name ("media-playback-pause", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(ginga_gui.play_button), play_icon);   
}

static void
set_fullscreen_mode(GtkWidget *widget, gpointer data){

    gtk_widget_set_visible(ginga_gui.play_button, FALSE);
    gtk_widget_set_visible(ginga_gui.stop_button, FALSE);
    gtk_widget_set_visible(ginga_gui.volume_button, FALSE);
    gtk_widget_set_visible(ginga_gui.fullscreen_button, FALSE);
    gtk_widget_set_visible(ginga_gui.config_button, FALSE);
    gtk_widget_set_visible(ginga_gui.time_label, FALSE);
    gtk_widget_set_visible(ginga_gui.open_button, FALSE);
    gtk_widget_set_visible(ginga_gui.file_entry, FALSE);
    gtk_widget_set_visible(ginga_gui.menu_bar, FALSE);

    ginga_gui.fullScreenMode = TRUE;
    GdkScreen *screen = gdk_screen_get_default();
    gtk_fixed_move(GTK_FIXED(ginga_gui.fixed_layout), ginga_gui.notebook, 0, 0);
    gtk_container_set_border_width(GTK_CONTAINER(ginga_gui.toplevel_window), 0); 
    gtk_widget_set_size_request(ginga_gui.canvas, gdk_screen_get_width(screen), gdk_screen_get_height(screen));
    gtk_window_fullscreen (GTK_WINDOW(ginga_gui.toplevel_window));

}

static void
set_unfullscreen_mode(GtkWidget *widget, gpointer data){
    ginga_gui.fullScreenMode = FALSE;
    gtk_window_unfullscreen (GTK_WINDOW(ginga_gui.toplevel_window));
    gtk_container_set_border_width(GTK_CONTAINER(ginga_gui.toplevel_window), ginga_gui.default_margin);
    gtk_fixed_move(GTK_FIXED(ginga_gui.fixed_layout), ginga_gui.notebook, ginga_gui.canvas_rect.x, ginga_gui.canvas_rect.y);
    gtk_widget_set_size_request(ginga_gui.canvas, ginga_gui.canvas_rect.w, ginga_gui.canvas_rect.h);
    gtk_window_resize(GTK_WINDOW (ginga_gui.toplevel_window), ginga_gui.window_rect.w, ginga_gui.window_rect.h); 
 //   gtk_widget_set_size_request(ginga_gui.toplevel_window, ginga_gui.window_rect.w, ginga_gui.window_rect.h); 
 //   gtk_window_set_default_size (GTK_WINDOW (ginga_gui.toplevel_window), ginga_gui.window_rect.w, ginga_gui.window_rect.h); 

   
    gtk_widget_set_visible(ginga_gui.play_button, TRUE);
    gtk_widget_set_visible(ginga_gui.stop_button, TRUE);
    gtk_widget_set_visible(ginga_gui.volume_button, TRUE);
    gtk_widget_set_visible(ginga_gui.fullscreen_button, TRUE);
    gtk_widget_set_visible(ginga_gui.config_button, TRUE);
    gtk_widget_set_visible(ginga_gui.time_label, TRUE);
    gtk_widget_set_visible(ginga_gui.open_button, TRUE);
    gtk_widget_set_visible(ginga_gui.file_entry, TRUE);
    gtk_widget_set_visible(ginga_gui.menu_bar, TRUE);
}

static void 
enable_disable_debug(GtkWidget *widget, gpointer data){
    ginga_gui.debugMode=!ginga_gui.debugMode;
    
    guint offset=0;
    if(ginga_gui.debugMode){ 
        offset = 30;
        gtk_notebook_set_show_tabs(GTK_NOTEBOOK(ginga_gui.notebook), TRUE); 
    }
    else 
       gtk_notebook_set_show_tabs(GTK_NOTEBOOK(ginga_gui.notebook), FALSE); 
    

   gtk_fixed_move(GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.play_button, 0, ginga_gui.controll_area_rect.y + offset);
   gtk_fixed_move(GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.stop_button, BUTTON_SIZE, ginga_gui.controll_area_rect.y + offset);
   gtk_fixed_move(GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.time_label, (BUTTON_SIZE*2)+5, ginga_gui.window_rect.h + MENU_BOX_HEIGHT - (BUTTON_SIZE/3) + offset);
   gtk_fixed_move(GTK_FIXED (ginga_gui.fixed_layout),  ginga_gui.fullscreen_button, ginga_gui.window_rect.w - (BUTTON_SIZE*2), ginga_gui.controll_area_rect.y + offset);
   gtk_fixed_move(GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.config_button,ginga_gui.window_rect.w - (BUTTON_SIZE), ginga_gui.controll_area_rect.y + offset);
   gtk_fixed_move(GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.volume_button, ginga_gui.window_rect.w - (BUTTON_SIZE*3), ginga_gui.controll_area_rect.y + offset);
   gtk_fixed_move(GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.canvas_separator_bottom, 0, ginga_gui.canvas_rect.y + ginga_gui.canvas_rect.h + ginga_gui.default_margin + offset);
   
   

 //  gtk_window_set_default_size (GTK_WINDOW (ginga_gui.toplevel_window), ginga_gui.window_rect.w, ginga_gui.window_rect.h);
     gtk_window_resize(GTK_WINDOW (ginga_gui.toplevel_window),  ginga_gui.window_rect.w, ginga_gui.window_rect.h);
}

static void 
create_tvcontrol_window(GtkWidget *widget, gpointer data){
    
    guint16 control_width = (BUTTON_SIZE*4);
    guint16 control_height = (BUTTON_SIZE*11);

    ginga_gui.tvcontrol_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_assert_nonnull(ginga_gui.tvcontrol_window);
    gtk_window_set_title (GTK_WINDOW (ginga_gui.tvcontrol_window), "Control");
    gtk_window_set_default_size (GTK_WINDOW (ginga_gui.tvcontrol_window), control_width, control_height);
    gtk_window_set_position(GTK_WINDOW(ginga_gui.tvcontrol_window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(ginga_gui.tvcontrol_window), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(ginga_gui.tvcontrol_window), ginga_gui.default_margin);
   
    GtkWidget *fixed_layout = gtk_fixed_new( );
    g_assert_nonnull(fixed_layout);
   //4 -> 55
    
    guint middle_button_pos = (control_width/2) - (BUTTON_SIZE/2);

    GtkWidget *button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/number_one.png",NULL));
    GtkWidget *button_1 = gtk_button_new();
    g_assert_nonnull(button_1);
    gtk_button_set_image(GTK_BUTTON(button_1), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_1, middle_button_pos - BUTTON_SIZE, 0);
    
    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/number_two.png",NULL));
    GtkWidget *button_2 = gtk_button_new();
    g_assert_nonnull(button_2);
    gtk_button_set_image(GTK_BUTTON(button_2), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_2, middle_button_pos , 0);

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/number_three.png",NULL));
    GtkWidget *button_3 = gtk_button_new();
    g_assert_nonnull(button_3);
    gtk_button_set_image(GTK_BUTTON(button_3), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_3, middle_button_pos + BUTTON_SIZE, 0);

    //

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/number_four.png",NULL));
    GtkWidget *button_4 = gtk_button_new();
    g_assert_nonnull(button_4);
    gtk_button_set_image(GTK_BUTTON(button_4), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_4, middle_button_pos - BUTTON_SIZE, BUTTON_SIZE);

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/number_five.png",NULL));
    GtkWidget *button_5 = gtk_button_new();
    g_assert_nonnull(button_5);
    gtk_button_set_image(GTK_BUTTON(button_5), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_5, middle_button_pos, BUTTON_SIZE);

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/number_six.png",NULL));
    GtkWidget *button_6 = gtk_button_new();
    g_assert_nonnull(button_6);
    gtk_button_set_image(GTK_BUTTON(button_6), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_6, middle_button_pos + BUTTON_SIZE, BUTTON_SIZE);

    //
    
    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/number_seven.png",NULL));
    GtkWidget *button_7 = gtk_button_new();
    g_assert_nonnull(button_7);
    gtk_button_set_image(GTK_BUTTON(button_7), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_7, middle_button_pos - BUTTON_SIZE, (BUTTON_SIZE*2));

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/number_eight.png",NULL));
    GtkWidget *button_8 = gtk_button_new();
    g_assert_nonnull(button_8);
    gtk_button_set_image(GTK_BUTTON(button_8), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_8, middle_button_pos, (BUTTON_SIZE*2));

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/number_nine.png",NULL));
    GtkWidget *button_9 = gtk_button_new();
    g_assert_nonnull(button_9);
    gtk_button_set_image(GTK_BUTTON(button_9), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_9, middle_button_pos + BUTTON_SIZE, (BUTTON_SIZE*2));

    //
    
    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/back_arrow.png",NULL));
    GtkWidget *button_back = gtk_button_new();
    g_assert_nonnull(button_back);
    gtk_button_set_image(GTK_BUTTON(button_back), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_back, middle_button_pos - BUTTON_SIZE, (BUTTON_SIZE*3));

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/number_zero.png",NULL));
    GtkWidget *button_0 = gtk_button_new();
    g_assert_nonnull(button_0);
    gtk_button_set_image(GTK_BUTTON(button_0), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_0, middle_button_pos, (BUTTON_SIZE*3));

    button_icon = gtk_image_new_from_icon_name ("dialog-information", GTK_ICON_SIZE_BUTTON);
    GtkWidget *button_info = gtk_button_new();
    g_assert_nonnull(button_info);
    gtk_button_set_image(GTK_BUTTON(button_info), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_info, middle_button_pos + BUTTON_SIZE, (BUTTON_SIZE*3));

    //

    button_icon = gtk_image_new_from_icon_name ("go-top", GTK_ICON_SIZE_BUTTON);
    GtkWidget *button_up = gtk_button_new();
    g_assert_nonnull(button_up);
    gtk_button_set_image(GTK_BUTTON(button_up), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_up, middle_button_pos, 10+(BUTTON_SIZE*4));
    

    //

    button_icon = gtk_image_new_from_icon_name ("go-first", GTK_ICON_SIZE_BUTTON);
    GtkWidget *button_left = gtk_button_new(); 
    g_assert_nonnull(button_left);
    gtk_button_set_image(GTK_BUTTON(button_left), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_left, middle_button_pos - BUTTON_SIZE, 10+(BUTTON_SIZE*5));
    
    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/ok_button.png",NULL));
    GtkWidget *button_ok = gtk_button_new();
    g_assert_nonnull(button_ok);
    gtk_button_set_image(GTK_BUTTON(button_ok), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_ok, middle_button_pos, 10+(BUTTON_SIZE*5));

    button_icon = gtk_image_new_from_icon_name ("go-last", GTK_ICON_SIZE_BUTTON);
    GtkWidget *button_right = gtk_button_new(); 
    g_assert_nonnull(button_right);
    gtk_button_set_image(GTK_BUTTON(button_right), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_right, middle_button_pos + BUTTON_SIZE, 10+(BUTTON_SIZE*5));

    //
    
    button_icon = gtk_image_new_from_icon_name ("go-bottom", GTK_ICON_SIZE_BUTTON);
    GtkWidget *button_down = gtk_button_new();
    g_assert_nonnull(button_down);
    gtk_button_set_image(GTK_BUTTON(button_down), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_down, middle_button_pos, 10+(BUTTON_SIZE*6));

    //

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/settings-icon.png",NULL));
    GtkWidget *button_vol_up = gtk_button_new(); 
    g_assert_nonnull(button_vol_up);
    gtk_button_set_image(GTK_BUTTON(button_vol_up), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout), button_vol_up,  0, 20+(BUTTON_SIZE*7));
    
    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/menu.png",NULL));
    GtkWidget *button_menu = gtk_button_new();
    g_assert_nonnull(button_menu);
    gtk_button_set_image(GTK_BUTTON(button_menu), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout), button_menu, BUTTON_SIZE, 20+(BUTTON_SIZE*7));

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/close.png",NULL));
    GtkWidget *button_close = gtk_button_new(); 
    g_assert_nonnull(button_close);
    gtk_button_set_image(GTK_BUTTON(button_close), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout), button_close, (BUTTON_SIZE*2), 20+(BUTTON_SIZE*7));

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/settings-icon.png",NULL));
    GtkWidget *button_ch_up = gtk_button_new(); 
    g_assert_nonnull(button_ch_up);
    gtk_button_set_image(GTK_BUTTON(button_ch_up), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout), button_ch_up, (BUTTON_SIZE*3), 20+(BUTTON_SIZE*7));

    //

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/settings-icon.png",NULL));
    GtkWidget *button_vol_down = gtk_button_new(); 
    g_assert_nonnull(button_vol_down);
    gtk_button_set_image(GTK_BUTTON(button_vol_down), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout), button_vol_down,  0, 20+(BUTTON_SIZE*8));
    
    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/settings-icon.png",NULL));
    GtkWidget *button_menu2 = gtk_button_new();
    g_assert_nonnull(button_menu2);
    gtk_button_set_image(GTK_BUTTON(button_menu2), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout), button_menu2, BUTTON_SIZE, 20+(BUTTON_SIZE*8));

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/settings-icon.png",NULL));
    GtkWidget *button_info2 = gtk_button_new(); 
    g_assert_nonnull(button_info2);
    gtk_button_set_image(GTK_BUTTON(button_info2), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout), button_info2, (BUTTON_SIZE*2), 20+(BUTTON_SIZE*8));

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/light-theme/settings-icon.png",NULL));
    GtkWidget *button_ch_down = gtk_button_new(); 
    g_assert_nonnull(button_ch_down);
    gtk_button_set_image(GTK_BUTTON(button_ch_down), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout), button_ch_down, (BUTTON_SIZE*3), 20+(BUTTON_SIZE*8));

    //

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/common/red_icon.png",NULL));
    GtkWidget *button_red = gtk_button_new();
    g_assert_nonnull(button_red);
    gtk_button_set_image(GTK_BUTTON(button_red), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_red, 0, 20+(BUTTON_SIZE*9));

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/common/green_icon.png",NULL));
    GtkWidget *button_green = gtk_button_new();
    g_assert_nonnull(button_green);
    gtk_button_set_image(GTK_BUTTON(button_green), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_green, BUTTON_SIZE, 20+(BUTTON_SIZE*9));

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/common/yellow_icon.png",NULL));
    GtkWidget *button_yellow = gtk_button_new();
    g_assert_nonnull(button_yellow);
    gtk_button_set_image(GTK_BUTTON(button_yellow), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_yellow, (BUTTON_SIZE*2), 20+(BUTTON_SIZE*9));

    button_icon = gtk_image_new_from_file (g_strconcat(ginga_gui.executable_folder,"icons/common/blue_icon.png",NULL));
    GtkWidget *button_blue = gtk_button_new();
    g_assert_nonnull(button_blue);
    gtk_button_set_image(GTK_BUTTON(button_blue), button_icon);
    gtk_fixed_put(GTK_FIXED (fixed_layout),  button_blue, (BUTTON_SIZE*3), 20+(BUTTON_SIZE*9));
    
    GtkWidget *label = gtk_label_new ("...");
    g_assert_nonnull(label);
    gtk_fixed_put(GTK_FIXED (fixed_layout), label, middle_button_pos+10, 20+(BUTTON_SIZE*10));
    
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

static void
key_event_press(GtkWidget *widget, GdkEventKey *event){  

    char *key_name = gdk_keyval_name (event->keyval);
   //g_printerr("press %s %d\n", gdk_keyval_name (event->keyval),ginga_gui.crtlModifier);

    if(!g_strcmp0(key_name, "Control_L") || !g_strcmp0(key_name, "Control_R") || !g_strcmp0(key_name, "Meta_R") || !g_strcmp0(key_name, "Meta_L"))
        ginga_gui.crtlModifier=TRUE;
    else if((!g_strcmp0(key_name, "r") || !g_strcmp0(key_name, "R")) && ginga_gui.crtlModifier)
        create_tvcontrol_window(NULL,0); 
    else if((!g_strcmp0(key_name, "d") || !g_strcmp0(key_name, "D")) && ginga_gui.crtlModifier)
        enable_disable_debug(NULL,0);     
    else if((!g_strcmp0(key_name, "f") || !g_strcmp0(key_name, "F")) && ginga_gui.crtlModifier)
        set_fullscreen_mode(NULL,0);
    else if(!g_strcmp0(key_name, "Escape") && ginga_gui.fullScreenMode)
        set_unfullscreen_mode(NULL,0);    
    
}

static void
key_event_release(GtkWidget *widget, GdkEventKey *event){  

    char *key_name = gdk_keyval_name (event->keyval);
    
    if(!g_strcmp0( key_name , "Control_L") || !g_strcmp0( key_name , "Control_R") || !g_strcmp0(key_name, "Meta_R") || !g_strcmp0(key_name, "Meta_L"))
        ginga_gui.crtlModifier=FALSE;
    
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

    ginga_gui.fixed_layout = gtk_fixed_new( );
    g_assert_nonnull(ginga_gui.fixed_layout);
    
    GtkWidget *icon = gtk_image_new_from_icon_name ("document-open", GTK_ICON_SIZE_BUTTON);
    ginga_gui.open_button = gtk_button_new();
    g_assert_nonnull(ginga_gui.open_button);
    gtk_button_set_image(GTK_BUTTON( ginga_gui.open_button), icon);
    g_signal_connect( ginga_gui.open_button, "clicked", G_CALLBACK(select_ncl_file), NULL); 
    gtk_fixed_put(GTK_FIXED (ginga_gui.fixed_layout),  ginga_gui.open_button, 0, MENU_BOX_HEIGHT - 38);

    ginga_gui.file_entry = gtk_entry_new();
    g_assert_nonnull(ginga_gui.file_entry);
    gtk_widget_set_size_request(ginga_gui.file_entry,  ginga_gui.window_rect.w -BUTTON_SIZE, BUTTON_SIZE);
    gtk_entry_set_text(GTK_ENTRY(ginga_gui.file_entry),g_get_user_special_dir(G_USER_DIRECTORY_DOCUMENTS));
    gtk_fixed_put(GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.file_entry, BUTTON_SIZE, MENU_BOX_HEIGHT - 38);

    GtkWidget * canvas_separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_size_request(canvas_separator, ginga_gui.canvas_rect.w, 2);
    gtk_fixed_put(GTK_FIXED (ginga_gui.fixed_layout), canvas_separator, ginga_gui.canvas_rect.x, ginga_gui.canvas_rect.y-5 );
  
    //Create Drawing area
    ginga_gui.canvas = gtk_drawing_area_new();
    g_assert_nonnull(ginga_gui.canvas);
    gtk_widget_set_app_paintable(ginga_gui.canvas, TRUE);
    g_signal_connect( ginga_gui.canvas, "draw", G_CALLBACK (draw_callback), NULL );
    gtk_widget_set_size_request(ginga_gui.canvas, ginga_gui.canvas_rect.w, ginga_gui.canvas_rect.h);
   // gtk_fixed_put(GTK_FIXED (ginga_gui.fixed_layout),  ginga_gui.canvas,  ginga_gui.canvas_rect.x,  ginga_gui.canvas_rect.y);
    
    ginga_gui.canvas_separator_bottom = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_size_request(ginga_gui.canvas_separator_bottom, ginga_gui.canvas_rect.w, 2);
    gtk_fixed_put(GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.canvas_separator_bottom, 0, ginga_gui.canvas_rect.y + ginga_gui.canvas_rect.h + ginga_gui.default_margin );
    
    ginga_gui.notebook = gtk_notebook_new();
    g_assert_nonnull(ginga_gui.notebook);
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(ginga_gui.notebook), FALSE);
    gtk_notebook_append_page(GTK_NOTEBOOK(ginga_gui.notebook), ginga_gui.canvas, gtk_label_new ("Presentation"));
    gtk_notebook_append_page(GTK_NOTEBOOK(ginga_gui.notebook), ginga_gui.canvas, gtk_label_new ("Timeline"));
    gtk_notebook_append_page(GTK_NOTEBOOK(ginga_gui.notebook), ginga_gui.canvas, gtk_label_new ("Log"));
    gtk_fixed_put(GTK_FIXED(ginga_gui.fixed_layout), ginga_gui.notebook, ginga_gui.canvas_rect.x, ginga_gui.canvas_rect.y);
    
    // ----- top-menu begin
    ginga_gui.menu_bar = gtk_menu_bar_new();
    g_assert_nonnull(ginga_gui.menu_bar);
    
    //File
    GtkWidget *menu_file = gtk_menu_new();
    g_assert_nonnull(menu_file);  
    GtkWidget *menu_item_file = gtk_menu_item_new_with_label("File");
    g_assert_nonnull(menu_item_file);
    GtkWidget *menu_item_open = gtk_menu_item_new_with_label("Open  (Ctrl+O)");
    g_assert_nonnull(menu_item_open);
    GtkWidget *menu_item_quit = gtk_menu_item_new_with_label("Quit  (Ctrl+Q)");
    g_assert_nonnull(menu_item_quit);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item_file), menu_file);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), menu_item_open);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), menu_item_quit);
    gtk_menu_shell_append(GTK_MENU_SHELL(ginga_gui.menu_bar), menu_item_file);

    //Tools
    GtkWidget *menu_tool = gtk_menu_new();
    g_assert_nonnull(menu_tool);  
    GtkWidget *menu_item_tools = gtk_menu_item_new_with_label("Tools");
    g_assert_nonnull(menu_item_tools);
    GtkWidget *menu_item_tvcontrol = gtk_menu_item_new_with_label("TV Control  (Ctrl+R)");
    g_assert_nonnull(menu_item_tvcontrol);
    GtkWidget *menu_item_debug = gtk_menu_item_new_with_label("Debug Mode  (Ctrl+D)");
    g_assert_nonnull(menu_item_debug); 
    g_signal_connect(menu_item_tvcontrol, "activate", G_CALLBACK(create_tvcontrol_window), NULL); 
    g_signal_connect(menu_item_debug, "activate", G_CALLBACK(enable_disable_debug), NULL); 
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item_tools),menu_tool);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_tool), menu_item_tvcontrol);
     gtk_menu_shell_append(GTK_MENU_SHELL(menu_tool), menu_item_debug);
    gtk_menu_shell_append(GTK_MENU_SHELL(ginga_gui.menu_bar), menu_item_tools);

    //Help
    GtkWidget *menu_help = gtk_menu_new();
    g_assert_nonnull(menu_help);
    GtkWidget *menu_item_help = gtk_menu_item_new_with_label("Help");
    g_assert_nonnull(menu_item_help);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item_help),menu_help);
    gtk_menu_shell_append(GTK_MENU_SHELL(ginga_gui.menu_bar), menu_item_help); 
    
    
    GtkWidget *menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(menu_box), ginga_gui.menu_bar, TRUE, TRUE, 3);
    gtk_fixed_put(GTK_FIXED (ginga_gui.fixed_layout), menu_box, 0, 0);

    // ----- top-menu end

    GtkWidget *play_icon = gtk_image_new_from_icon_name ("media-playback-start", GTK_ICON_SIZE_BUTTON);
    g_assert_nonnull(play_icon);
    ginga_gui.play_button = gtk_button_new();
    g_assert_nonnull(ginga_gui.play_button);
    gtk_button_set_image(GTK_BUTTON(ginga_gui.play_button), play_icon);
    g_signal_connect(ginga_gui.play_button, "clicked", G_CALLBACK(play_pause_ginga), NULL); 
    gtk_fixed_put(GTK_FIXED (ginga_gui.fixed_layout),  ginga_gui.play_button, 0, ginga_gui.controll_area_rect.y);

    GtkWidget *stop_icon = gtk_image_new_from_icon_name ("media-playback-stop", GTK_ICON_SIZE_BUTTON);
    g_assert_nonnull(stop_icon);
    ginga_gui.stop_button = gtk_button_new();
    g_assert_nonnull(ginga_gui.stop_button);
    gtk_button_set_image(GTK_BUTTON(ginga_gui.stop_button), stop_icon);
    gtk_fixed_put(GTK_FIXED (ginga_gui.fixed_layout),  ginga_gui.stop_button, BUTTON_SIZE, ginga_gui.controll_area_rect.y);

    ginga_gui.time_label = gtk_label_new ("00:00");
    gtk_label_set_markup (GTK_LABEL(ginga_gui.time_label), g_markup_printf_escaped("<span font=\"13\"><b>\%s</b></span>", "00:00"));
    g_assert_nonnull(ginga_gui.time_label);
    gtk_fixed_put(GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.time_label, (BUTTON_SIZE*2)+5, ginga_gui.window_rect.h + MENU_BOX_HEIGHT - (BUTTON_SIZE/3));

    GtkWidget *fullscreen_icon = gtk_image_new_from_icon_name ("view-fullscreen", GTK_ICON_SIZE_BUTTON);
    g_assert_nonnull(fullscreen_icon);
    ginga_gui.fullscreen_button = gtk_button_new();
    g_assert_nonnull(ginga_gui.fullscreen_button);
    gtk_button_set_image(GTK_BUTTON(ginga_gui.fullscreen_button), fullscreen_icon);
    g_signal_connect(ginga_gui.fullscreen_button, "clicked", G_CALLBACK(set_fullscreen_mode), NULL); 
    gtk_fixed_put(GTK_FIXED (ginga_gui.fixed_layout),  ginga_gui.fullscreen_button, ginga_gui.window_rect.w - (BUTTON_SIZE*2), ginga_gui.controll_area_rect.y);

    GtkWidget *config_icon = gtk_image_new_from_icon_name ("emblem-system", GTK_ICON_SIZE_BUTTON);
    g_assert_nonnull(config_icon);
    ginga_gui.config_button = gtk_button_new();
    g_assert_nonnull(ginga_gui.config_button);
    gtk_button_set_image(GTK_BUTTON(ginga_gui.config_button), config_icon);
    gtk_fixed_put(GTK_FIXED (ginga_gui.fixed_layout),  ginga_gui.config_button, ginga_gui.window_rect.w - (BUTTON_SIZE), ginga_gui.controll_area_rect.y);
    
    ginga_gui.volume_button = gtk_volume_button_new ();
    g_assert_nonnull(ginga_gui.volume_button);
    gtk_fixed_put(GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.volume_button, ginga_gui.window_rect.w - (BUTTON_SIZE*3), ginga_gui.controll_area_rect.y);
   
    gtk_container_add(GTK_CONTAINER(ginga_gui.toplevel_window), ginga_gui.fixed_layout);
     // Setup GTK+ callbacks.
 
    g_signal_connect(ginga_gui.toplevel_window, "key-press-event",G_CALLBACK(key_event_press), NULL); 
    g_signal_connect(ginga_gui.toplevel_window, "key-release-event",G_CALLBACK(key_event_release), NULL); 
    g_signal_connect(ginga_gui.toplevel_window, "check-resize", G_CALLBACK(resize_callback), NULL); 
    g_signal_connect(ginga_gui.toplevel_window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  
    gtk_widget_show_all (ginga_gui.toplevel_window);
}

int
main (int argc, char **argv){
    ginga_gui.executable_folder = g_strconcat(g_get_current_dir(), g_path_get_dirname (argv[0])+1,"/",NULL);
    printf("PATH: %s \n", ginga_gui.executable_folder);  

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
