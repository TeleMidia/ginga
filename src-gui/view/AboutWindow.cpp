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

GtkWidget *aboutWindow = NULL;

void
create_about_window (void)
{
  if (aboutWindow != NULL)
    return;

  aboutWindow = gtk_about_dialog_new ();
  gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG (aboutWindow),
                                     "Ginga");

  GtkWidget *header_bar = gtk_header_bar_new ();
  g_assert_nonnull (header_bar);
  gtk_header_bar_set_title (GTK_HEADER_BAR (header_bar), "About Ginga");
  gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (header_bar), true);
  gtk_header_bar_set_decoration_layout (GTK_HEADER_BAR (header_bar),
                                        "menu:close");

  gtk_window_set_titlebar (GTK_WINDOW (aboutWindow), header_bar);

  gtk_about_dialog_set_copyright (
      GTK_ABOUT_DIALOG (aboutWindow),
      "Copyright (C) 2006-2017 PUC-Rio/TeleMidia Lab");
  gtk_about_dialog_set_license (
      GTK_ABOUT_DIALOG (aboutWindow),
      "\nGNU General Public License v2.0 (GPL-2.0) \n"
      "https://www.gnu.org/licenses/old-licenses/gpl-2.0.html");
  gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (aboutWindow),
                                 "The iDTV middleware.");
  gtk_about_dialog_set_website_label (GTK_ABOUT_DIALOG (aboutWindow),
                                      "http://ginga.org.br");

  const gchar **authors = (const gchar **) malloc (sizeof (gchar *) * 2);
  authors[0] = g_strconcat (
      "PUC-Rio/TeleMidia Lab. \nwww.telemidia.puc-rio.br", NULL);
  authors[1] = NULL;
  gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (aboutWindow), authors);
  gtk_about_dialog_set_wrap_license (GTK_ABOUT_DIALOG (aboutWindow), TRUE);
  gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (aboutWindow), NULL);

  g_signal_connect (aboutWindow, "destroy",
                    G_CALLBACK (destroy_about_window), NULL);

  g_signal_connect (aboutWindow, "delete-event",
                    G_CALLBACK (destroy_about_window), NULL);

  gtk_widget_show_all (aboutWindow);
  // gtk_show_about_dialog(aboutWindow);
}
void
destroy_about_window (void)
{
  gtk_widget_destroy (aboutWindow);
  aboutWindow = NULL;
}
