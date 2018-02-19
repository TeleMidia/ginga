/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "ginga_gtk.h"
#include <glib/gstdio.h>

#define SETTINGS_FILENAME "ginga222.settings"

void
save_settings (void)
{
  GError *error = NULL;
  gchar *file_path = g_build_path (
      G_DIR_SEPARATOR_S, g_get_user_config_dir (), SETTINGS_FILENAME, NULL);
  GKeyFile *key_file = g_key_file_new ();
  g_key_file_set_value (key_file, "ginga-gui", "ginga-ID",
                        g_markup_printf_escaped ("%s", gingaID));

  g_key_file_set_value (
      key_file, "ginga-gui", "aspect-ratio",
      g_markup_printf_escaped ("%d", presentationAttributes.aspectRatio));

  g_key_file_set_value (
      key_file, "ginga-gui", "frame-rate",
      g_markup_printf_escaped ("%d", presentationAttributes.frameRate));

  g_key_file_set_value (
      key_file, "ginga-gui", "gui-theme",
      g_markup_printf_escaped ("%d", presentationAttributes.guiTheme));

  gchar *hist_str = g_markup_printf_escaped (" ");
  GList *childs = gtk_container_get_children (GTK_CONTAINER (historicBox));
  for (GList *l = childs; l != NULL; l = l->next)
    {
      GtkWidget *row = (GtkWidget *) l->data;
      GtkWidget *label = gtk_bin_get_child (GTK_BIN (row));
      hist_str = g_strconcat (hist_str, "##",
                              gtk_label_get_text (GTK_LABEL (label)), NULL);
    }

  g_key_file_set_value (key_file, "ginga-gui", "historic", hist_str);

  g_key_file_set_value (
      key_file, "ginga-gui", "show-tracker-window",
      g_markup_printf_escaped ("%d",
                               presentationAttributes.showTrackerWindow));

  g_key_file_set_value (
      key_file, "ginga-gui", "tracker-accept",
      g_markup_printf_escaped ("%d", trackerFlags.trackerAccept));

  g_key_file_save_to_file (key_file, file_path, &error);

  g_free (hist_str);
  g_free (file_path);
}

void
load_settings (void)
{
  GError *error = NULL;
  gchar *hist_str = NULL;
  gchar *file_path = g_build_path (
      G_DIR_SEPARATOR_S, g_get_user_config_dir (), SETTINGS_FILENAME, NULL);
  GKeyFile *key_file = g_key_file_new ();

  if (!g_key_file_load_from_file (key_file, file_path, G_KEY_FILE_NONE,
                                  &error))
    goto endload;

  gingaID
      = g_key_file_get_value (key_file, "ginga-gui", "ginga-ID", &error);

  presentationAttributes.aspectRatio = atoi (
      g_key_file_get_value (key_file, "ginga-gui", "aspect-ratio", &error));
  presentationAttributes.frameRate = atoi (
      g_key_file_get_value (key_file, "ginga-gui", "frame-rate", &error));
  presentationAttributes.guiTheme = atoi (
      g_key_file_get_value (key_file, "ginga-gui", "gui-theme", &error));

  hist_str
      = g_key_file_get_value (key_file, "ginga-gui", "historic", &error);

  if (hist_str != NULL)
    {
      if (historicBox == NULL)
        {
          historicBox = gtk_list_box_new ();
          g_assert_nonnull (historicBox);
        }
      gchar **str_split = g_strsplit (hist_str, "##", -1);
      guint str_v_len = g_strv_length (str_split);
      for (guint i = 1; i < str_v_len; i++)
        {

          GtkWidget *label = gtk_label_new (str_split[i]);
          gtk_widget_set_halign (label, GTK_ALIGN_START);
          gtk_list_box_insert (GTK_LIST_BOX (historicBox), label, -1);
        }
      g_strfreev (str_split);
    }

  presentationAttributes.showTrackerWindow = atoi (g_key_file_get_value (
      key_file, "ginga-gui", "show-tracker-window", &error));

  trackerFlags.trackerAccept = atoi (g_key_file_get_value (
      key_file, "ginga-gui", "tracker-accept", &error));

endload:
  g_free (hist_str);
  g_free (file_path);
}
