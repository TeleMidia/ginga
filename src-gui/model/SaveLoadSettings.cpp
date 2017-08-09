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

#define SETTINGS_FILENAME "ginga-gtk6.settings"

void
save_settings (void)
{
  GFile *file = NULL;
  GError *error = NULL;
  gsize *length = NULL;
  GFileOutputStream *outputStream = NULL;

  file = g_file_new_for_path (SETTINGS_FILENAME);
  if (g_file_query_exists (file, NULL))
    g_remove (SETTINGS_FILENAME);

  outputStream = g_file_create (file, G_FILE_CREATE_REPLACE_DESTINATION,
                                NULL, &error);
  g_assert_nonnull (outputStream);
  GDataOutputStream *data
      = g_data_output_stream_new ((GOutputStream *)outputStream);

 // if(presentationAttributes.lastFileName==NULL)
 //     strcpy( presentationAttributes.lastFileName ," ");

  gchar *str = g_markup_printf_escaped (
      "%d#%d#%d#%d", presentationAttributes.aspectRatio,
      presentationAttributes.resolutionWidth,
      presentationAttributes.resolutionHeight,
      presentationAttributes.frameRate);

  g_data_output_stream_put_string (data, str, NULL, &error);

  g_object_unref (outputStream);
  g_object_unref (file);
}

void
load_settings (void)
{
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
}
