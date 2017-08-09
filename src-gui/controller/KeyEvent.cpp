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

gboolean isCrtlModifierActive = FALSE;

void
key_press_event_callback (GtkWidget *widget, GdkEventKey *event)
{
  gchar *key_name = gdk_keyval_name (event->keyval);
  
  if (!g_strcmp0 (key_name, "Control_L")
      || !g_strcmp0 (key_name, "Control_R")
      || !g_strcmp0 (key_name, "Meta_R") || !g_strcmp0 (key_name, "Meta_L"))
    isCrtlModifierActive = TRUE;
  else if ((!g_strcmp0 (key_name, "r") || !g_strcmp0 (key_name, "R"))
           && isCrtlModifierActive && !isFullScreenMode)
    create_tvcontrol_window ();
  else if ((!g_strcmp0 (key_name, "d") || !g_strcmp0 (key_name, "D"))
           && isCrtlModifierActive && !isFullScreenMode)
    enable_disable_debug ();
  else if ((!g_strcmp0 (key_name, "f") || !g_strcmp0 (key_name, "F"))
           && isCrtlModifierActive && !isFullScreenMode)
    set_fullscreen_mode ();
  else if (!g_strcmp0 (key_name, "Escape") && isFullScreenMode)
    set_unfullscreen_mode ();
   /*
  if(_Ginga_Display==NULL)
     return;
 
  if (!g_strcmp0 (key_name, "1"))
    _Ginga_Display->insertKeyEvent(SDLK_1, true);
  else if (!g_strcmp0 (key_name, "2"))
    _Ginga_Display->insertKeyEvent(SDLK_2, true);
  else if (!g_strcmp0 (key_name, "3"))
    _Ginga_Display->insertKeyEvent(SDLK_3, true);
  else if (!g_strcmp0 (key_name, "4"))
    _Ginga_Display->insertKeyEvent(SDLK_4, true);
  else if (!g_strcmp0 (key_name, "5"))
    _Ginga_Display->insertKeyEvent(SDLK_5, true);    
  else if (!g_strcmp0 (key_name, "6"))
    _Ginga_Display->insertKeyEvent(SDLK_6, true);
  else if (!g_strcmp0 (key_name, "7"))
    _Ginga_Display->insertKeyEvent(SDLK_7, true);
  else if (!g_strcmp0 (key_name, "8"))
    _Ginga_Display->insertKeyEvent(SDLK_8, true); 
  else if (!g_strcmp0 (key_name, "9"))
    _Ginga_Display->insertKeyEvent(SDLK_9, true);     
  else if (!g_strcmp0 (key_name, "Up"))
    _Ginga_Display->insertKeyEvent(SDLK_UP, true);  
  else if (!g_strcmp0 (key_name, "Left"))
    _Ginga_Display->insertKeyEvent(SDLK_LEFT, true);      
  else if (!g_strcmp0 (key_name, "Right"))
    _Ginga_Display->insertKeyEvent(SDLK_RIGHT, true);
  else if (!g_strcmp0 (key_name, "Down"))
    _Ginga_Display->insertKeyEvent(SDLK_DOWN, true);
  else if (!g_strcmp0 (key_name, "F1"))
    _Ginga_Display->insertKeyEvent(SDLK_F1, true);       
  else if (!g_strcmp0 (key_name, "F2"))
    _Ginga_Display->insertKeyEvent(SDLK_F2, true); 
  else if (!g_strcmp0 (key_name, "F3"))
    _Ginga_Display->insertKeyEvent(SDLK_F3, true); 
  else if (!g_strcmp0 (key_name, "F4"))
    _Ginga_Display->insertKeyEvent(SDLK_F4, true); 
  else if (!g_strcmp0 (key_name, "F5"))
    _Ginga_Display->insertKeyEvent(SDLK_F5, true); 
  else if (!g_strcmp0 (key_name, "F6"))
    _Ginga_Display->insertKeyEvent(SDLK_F6, true); 
  else if (!g_strcmp0 (key_name, "F7"))
    _Ginga_Display->insertKeyEvent(SDLK_F7, true);
  else if (!g_strcmp0 (key_name, "F8"))
    _Ginga_Display->insertKeyEvent(SDLK_F8, true); 
  else if (!g_strcmp0 (key_name, "F9"))
    _Ginga_Display->insertKeyEvent(SDLK_F9, true); 
  else if (!g_strcmp0 (key_name, "F10"))
    _Ginga_Display->insertKeyEvent(SDLK_F10, true);      
  else if (!g_strcmp0 (key_name, "F11"))
    _Ginga_Display->insertKeyEvent(SDLK_F11, true); 
  else if (!g_strcmp0 (key_name, "F12"))
    _Ginga_Display->insertKeyEvent(SDLK_F12, true);    
 */
}

void
key_release_event_callback (GtkWidget *widget, GdkEventKey *event)
{
  gchar *key_name = gdk_keyval_name (event->keyval);

  if (!g_strcmp0 (key_name, "Control_L")
      || !g_strcmp0 (key_name, "Control_R")
      || !g_strcmp0 (key_name, "Meta_R") || !g_strcmp0 (key_name, "Meta_L"))
    isCrtlModifierActive = FALSE;
/*
  if(_Ginga_Display==NULL)
     return;
  
  if (!g_strcmp0 (key_name, "1"))
    _Ginga_Display->insertKeyEvent(SDLK_1, false);
  else if (!g_strcmp0 (key_name, "2"))
    _Ginga_Display->insertKeyEvent(SDLK_2, false);
  else if (!g_strcmp0 (key_name, "3"))
    _Ginga_Display->insertKeyEvent(SDLK_3, false);
  else if (!g_strcmp0 (key_name, "4"))
    _Ginga_Display->insertKeyEvent(SDLK_4, false);
  else if (!g_strcmp0 (key_name, "5"))
    _Ginga_Display->insertKeyEvent(SDLK_5, false);  
  else if (!g_strcmp0 (key_name, "6"))
    _Ginga_Display->insertKeyEvent(SDLK_6, false);
  else if (!g_strcmp0 (key_name, "7"))
    _Ginga_Display->insertKeyEvent(SDLK_7, false);
  else if (!g_strcmp0 (key_name, "8"))
    _Ginga_Display->insertKeyEvent(SDLK_8, false);
  else if (!g_strcmp0 (key_name, "9"))
    _Ginga_Display->insertKeyEvent(SDLK_9, false);    
  else if (!g_strcmp0 (key_name, "Up"))
    _Ginga_Display->insertKeyEvent(SDLK_UP, false);
  else if (!g_strcmp0 (key_name, "Left"))
    _Ginga_Display->insertKeyEvent(SDLK_LEFT, false);    
  else if (!g_strcmp0 (key_name, "Right"))
    _Ginga_Display->insertKeyEvent(SDLK_RIGHT, false);
  else if (!g_strcmp0 (key_name, "Down"))
    _Ginga_Display->insertKeyEvent(SDLK_DOWN, false);
  else if (!g_strcmp0 (key_name, "F1"))
    _Ginga_Display->insertKeyEvent(SDLK_F1, false);      
  else if (!g_strcmp0 (key_name, "F2"))
    _Ginga_Display->insertKeyEvent(SDLK_F2, false); 
  else if (!g_strcmp0 (key_name, "F3"))
    _Ginga_Display->insertKeyEvent(SDLK_F3, false);
  else if (!g_strcmp0 (key_name, "F4"))
    _Ginga_Display->insertKeyEvent(SDLK_F4, false);
  else if (!g_strcmp0 (key_name, "F5"))
    _Ginga_Display->insertKeyEvent(SDLK_F5, false);
  else if (!g_strcmp0 (key_name, "F6"))
    _Ginga_Display->insertKeyEvent(SDLK_F6, false); 
  else if (!g_strcmp0 (key_name, "F7"))
    _Ginga_Display->insertKeyEvent(SDLK_F7, false);
  else if (!g_strcmp0 (key_name, "F8"))
    _Ginga_Display->insertKeyEvent(SDLK_F8, false);
  else if (!g_strcmp0 (key_name, "F9"))
    _Ginga_Display->insertKeyEvent(SDLK_F9, false);
  else if (!g_strcmp0 (key_name, "F10"))
    _Ginga_Display->insertKeyEvent(SDLK_F10, false);     
  else if (!g_strcmp0 (key_name, "F11"))
    _Ginga_Display->insertKeyEvent(SDLK_F11, false); 
  else if (!g_strcmp0 (key_name, "F12"))
    _Ginga_Display->insertKeyEvent(SDLK_F12, false);
*/
}


void
key_tvremote_press_event_callback(GtkWidget *widget, gpointer data){
   
   const gchar * widget_name = gtk_widget_get_name(widget);
   /*
   if(strcmp(widget_name, "b_1")==0)
     _Ginga_Display->insertKeyEvent(SDLK_1, true);
   else  if(strcmp(widget_name, "b_2")==0)
     _Ginga_Display->insertKeyEvent(SDLK_2, true);
   else  if(strcmp(widget_name, "b_3")==0)
     _Ginga_Display->insertKeyEvent(SDLK_3, true);
   else  if(strcmp(widget_name, "b_4")==0)
     _Ginga_Display->insertKeyEvent(SDLK_4, true);
   else  if(strcmp(widget_name, "b_5")==0)
     _Ginga_Display->insertKeyEvent(SDLK_5, true);
   else  if(strcmp(widget_name, "b_6")==0)
     _Ginga_Display->insertKeyEvent(SDLK_6, true);
   else  if(strcmp(widget_name, "b_7")==0)
     _Ginga_Display->insertKeyEvent(SDLK_7, true);
   else  if(strcmp(widget_name, "b_8")==0)
     _Ginga_Display->insertKeyEvent(SDLK_8, true);
   else  if(strcmp(widget_name, "b_9")==0)
     _Ginga_Display->insertKeyEvent(SDLK_9, true);
   else  if(strcmp(widget_name, "b_0")==0)
     _Ginga_Display->insertKeyEvent(SDLK_0, true);
   else  if(strcmp(widget_name, "b_red")==0)
     _Ginga_Display->insertKeyEvent(SDLK_F1, true);
   else  if(strcmp(widget_name, "b_green")==0)
     _Ginga_Display->insertKeyEvent(SDLK_F2, true);
   else  if(strcmp(widget_name, "b_blue")==0)
     _Ginga_Display->insertKeyEvent(SDLK_F3, true);
   else  if(strcmp(widget_name, "b_yellow")==0)
     _Ginga_Display->insertKeyEvent(SDLK_F4, true);
   else  if(strcmp(widget_name, "b_up")==0)
     _Ginga_Display->insertKeyEvent(SDLK_UP, true);
   else  if(strcmp(widget_name, "b_left")==0)
     _Ginga_Display->insertKeyEvent(SDLK_LEFT, true);
   else  if(strcmp(widget_name, "b_right")==0)
     _Ginga_Display->insertKeyEvent(SDLK_RIGHT, true);
   else  if(strcmp(widget_name, "b_down")==0)
     _Ginga_Display->insertKeyEvent(SDLK_DOWN, true);
   else  if(strcmp(widget_name, "b_info")==0)
     _Ginga_Display->insertKeyEvent(SDLK_F5, true);
 */

}
