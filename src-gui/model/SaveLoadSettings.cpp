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
#include <glib/gstdio.h>

#define SETTINGS_FILENAME "ginga-gtk.settings"

void
save_settings (void)
{
 GError *error = NULL;
// gchar * file_path = g_strconcat ( g_get_user_config_dir(),"/", SETTINGS_FILENAME, NULL);


 GKeyFile *key_file =	g_key_file_new ();
 g_key_file_set_value (key_file,
                      "ginga-gui",
                      "aspect-ratio",
                      g_markup_printf_escaped("%d",presentationAttributes.aspectRatio));

 g_key_file_set_value (key_file,
                      "ginga-gui",
                      "frame-rate",
                      g_markup_printf_escaped("%d",0));   

 g_key_file_set_value (key_file,
                      "ginga-gui",
                      "gui-theme",
                      g_markup_printf_escaped("%d",2));                                        

  if( g_key_file_save_to_file(key_file, SETTINGS_FILENAME, &error) ){
    printf("SALVOU !!!");
  }                     
  
 // g_free(file_path);

}

void
load_settings (void)
{
  GError *error = NULL;
  GKeyFile *key_file;
  /*
  if(!g_key_file_load_from_file(key_file, SETTINGS_FILENAME, G_KEY_FILE_NONE, &error) ){
    printf("NN CARREGOU !!!");
  }
  */

 // g_free(file_path);

  /*
  GError *error = NULL;
  gsize *length = NULL;
  GFile *file = g_file_new_for_path (SETTINGS_FILENAME);
  if (!g_file_query_exists (file, NULL))
    return;

  GFileInputStream *inputStream = g_file_read (file, NULL, &error);
  g_assert_nonnull (inputStream);
  GDataInputStream *data
      = g_data_input_stream_new ((GInputStream *)inputStream);
  g_assert_nonnull (data);
  gchar *line = g_data_input_stream_read_line (data, length, NULL, &error);

  gchar **str_split = g_strsplit(line,"#",-1);
  presentationAttributes.aspectRatio = atoi(str_split[0]); 
  presentationAttributes.resolutionWidth = atoi(str_split[1]); 
  presentationAttributes.resolutionHeight = atoi(str_split[2]); 
  presentationAttributes.frameRate = atoi(str_split[3]);
 // strcpy( presentationAttributes.lastFileName ,str_split[4]);
  
  g_strfreev(str_split);
  g_object_unref (data);
  g_object_unref (inputStream);
  g_object_unref (file);
  */
}
