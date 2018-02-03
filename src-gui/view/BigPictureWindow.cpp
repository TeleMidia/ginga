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

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "ginga_gtk.h"
#include "aux-glib.h"
#include "BigPictureWindow.h"

GList *cards_list = NULL;
gboolean inBigPictureMode = FALSE;
gboolean isPlayingApp = FALSE;
GtkWidget *bigPictureWindow = NULL;
guint timeOutTagB = 0;
cairo_pattern_t *background_pattern;

gint currentCard = 0;
gint carrouselDir = 0;
gdouble mid = 0;
gdouble screen_width = 0;
gdouble speed = 3000.0; /* pixels/s */
gdouble frameRate = 1.000 / 60.0;
gdouble gradient = 1.0;

guint numCards = 0;
gdouble cardWidth = 300;
gdouble cardHeight = 169;

void
instantiateInAppLibrary (unused (xmlDocPtr doc), xmlNodePtr cur)
{
  guint i = 0;
  cur = cur->xmlChildrenNode;
  while (cur != NULL)
    {
      if ((!xmlStrcmp (cur->name, (const xmlChar *) "app")))
        {
          xmlChar *src = xmlGetProp (cur, (const xmlChar *) "src");
          xmlChar *thumbnail
              = xmlGetProp (cur, (const xmlChar *) "thumbnail");
          xmlChar *cover = xmlGetProp (cur, (const xmlChar *) "cover");
          xmlChar *title = xmlGetProp (cur, (const xmlChar *) "title");
          xmlChar *description
              = xmlGetProp (cur, (const xmlChar *) "description");
          printf ("src: %s\n", src);
          printf ("thumbnail: %s\n", thumbnail);
          printf ("cover: %s\n", cover);
          printf ("title: %s\n", title);
          printf ("description: %s\n", description);

          BigPictureCard *bigPictureCard
              = (BigPictureCard *) malloc (sizeof (BigPictureCard));

          bigPictureCard->index = i;
          bigPictureCard->drawOrder = i;
          bigPictureCard->position
              = mid + (bigPictureCard[i].index * 200.0);
          bigPictureCard->animate = FALSE;

          if (g_file_test ((gchar *) thumbnail, G_FILE_TEST_EXISTS))
            bigPictureCard->surface
                = cairo_image_surface_create_from_png ((gchar *) thumbnail);
          else
            bigPictureCard->surface = cairo_image_surface_create_from_png (
                g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                              "common", "app_thumbnail.png", NULL));

          if (g_file_test ((gchar *) cover, G_FILE_TEST_EXISTS))
            bigPictureCard->print_surface
                = cairo_image_surface_create_from_png ((gchar *) cover);
          else
            bigPictureCard->print_surface
                = cairo_image_surface_create_from_png (
                    g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                                  "common", "app_cover.png", NULL));

          bigPictureCard->src = g_strdup ((gchar *) src);
          bigPictureCard->appName = g_strdup ((gchar *) title);
          bigPictureCard->appDesc = g_strdup ((gchar *) description);

          cards_list
              = g_list_append (cards_list, (gpointer) bigPictureCard);
          i++;
          numCards = i;

          xmlFree (src);
          xmlFree (thumbnail);
          xmlFree (cover);
          xmlFree (title);
          xmlFree (description);
        }
      cur = cur->next;
    }
  return;
}

void
loadApplicationsXML ()
{
  const char *docname
      = g_strdup ("/home/busson/ginga/src-gui/ncl-apps.xml");

  xmlDocPtr doc;
  xmlNodePtr cur;

  doc = xmlParseFile (docname);

  if (doc == NULL)
    {
      fprintf (stderr, "Document not parsed successfully. \n");
      return;
    }

  cur = xmlDocGetRootElement (doc);

  if (cur == NULL)
    {
      fprintf (stderr, "empty document\n");
      xmlFreeDoc (doc);
      return;
    }

  if (xmlStrcmp (cur->name, (const xmlChar *) "ncl-apps"))
    {
      fprintf (stderr, "document of the wrong type, root node != ncl-apps");
      xmlFreeDoc (doc);
      return;
    }

  instantiateInAppLibrary (doc, cur);
  xmlFreeDoc (doc);
}

gint
comp_card_list (gconstpointer a, gconstpointer b)
{
  BigPictureCard *card_a = (BigPictureCard *) a;
  BigPictureCard *card_b = (BigPictureCard *) b;

  if (card_a->drawOrder < card_b->drawOrder)
    return 1;
  else if (card_a->drawOrder > card_b->drawOrder)
    return -1;
  else
    return 0;
}

void
draw_ginga_surface (GtkWidget *widget, cairo_t *cr, unused (gpointer data))
{
  int w, h;
  w = gtk_widget_get_allocated_width (widget);
  h = gtk_widget_get_allocated_height (widget);

  cairo_set_source_rgb (cr, 0., 0., 0.);
  cairo_rectangle (cr, 0, 0, w, h);
  cairo_fill (cr);

  if (presentationAttributes.aspectRatio == 0)
    {
      cairo_translate (cr, (w - (w * 0.75)) / 2, 0);
      cairo_scale (cr, 0.75, 1.0);
    }
  else if (presentationAttributes.aspectRatio == 1)
    {
      cairo_translate (cr, 0, (h - (h * 0.5625)) / 2);
      cairo_scale (cr, 1.0, 0.5625);
    }
  else if (presentationAttributes.aspectRatio == 2)
    {
      cairo_translate (cr, 0, (h - (h * 0.625)) / 2);
      cairo_scale (cr, 1.0, 0.625);
    }

  GINGA->redraw (cr);
}

gboolean
update_bigpicture_callback (GtkWidget *widget)
{

  if (gradient < 1.0)
    gradient += (4.0 * frameRate);
  if (gradient > 1.0)
    gradient = 1.0;

  GList *l;
  for (l = cards_list; l != NULL; l = l->next)
    {
      BigPictureCard *card = (BigPictureCard *) l->data;

      if (!card->animate)
        continue;

      card->position += (speed * frameRate) * carrouselDir;

      if ((carrouselDir > 0) && (card->position >= card->destPosition))
        {
          card->position = card->destPosition;
          card->animate = FALSE;
        }
      else if ((carrouselDir < 0) && (card->position <= card->destPosition))
        {
          card->position = card->destPosition;
          card->animate = FALSE;
        }
    }

  gtk_widget_queue_draw (widget);
  return G_SOURCE_CONTINUE;
}

void
draw_bigpicture_callback (GtkWidget *widget, cairo_t *cr,
                          unused (gpointer data))
{
  if (isPlayingApp)
    {
      draw_ginga_surface (widget, cr, data);
      return;
    }

  int h = gtk_widget_get_allocated_height (widget);

  // clear with pattern
  cairo_set_source (cr, background_pattern);
  cairo_paint (cr);

  GList *l;
  for (l = cards_list; l != NULL; l = l->next)
    {
      BigPictureCard *card = (BigPictureCard *) l->data;

      gdouble posX = card->position - (cardWidth / 2);
      gdouble posY = (h - cardHeight - 100);

      if (card->index == 0)
        {
          cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, gradient);
          cairo_rectangle (cr, posX - 10, posY - 10, cardWidth + 20,
                           cardHeight + 20);
        }
      cairo_fill (cr);

      cairo_set_source_surface (cr, card->surface, posX, posY);

      gdouble b = screen_width - card->position;
      if (card->index < 0)
        b = card->position;
      cairo_paint_with_alpha (cr, b / (screen_width - mid) + 0.3);

      cairo_text_extents_t extents;
      cairo_select_font_face (cr, "Arial", CAIRO_FONT_SLANT_NORMAL,
                              CAIRO_FONT_WEIGHT_BOLD);

      if (card->index == 0 && !card->animate)
        {
          gdouble info_height = 600;
          gdouble info_weight = 1200;

          // cairo_set_source_rgba (cr, 0.3, 0.3, 0.3, gradient);
          // cairo_rectangle (cr, mid - (info_weight / 2), 100, info_weight,
          //                 info_height);
          // cairo_fill (cr);
          cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, gradient);

          PangoLayout *layout = pango_cairo_create_layout (cr);
          g_assert_nonnull (layout);

          cairo_set_font_size (cr, 50);
          cairo_text_extents (cr, card->appName, &extents);
          cairo_move_to (cr, mid - (info_weight / 2) + 20, 200);
          cairo_show_text (cr, card->appName);

          cairo_set_font_size (cr, 40);
          pango_layout_set_text (layout, card->appDesc, -1);

          PangoFontDescription *desc
              = pango_font_description_from_string ("Arial 20");
          g_assert_nonnull (desc);
          pango_layout_set_font_description (layout, desc);
          pango_font_description_free (desc);

          pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);
          pango_layout_set_width (layout,
                                  ((info_weight / 2) - 50) * PANGO_SCALE);
          pango_layout_set_wrap (layout, PANGO_WRAP_WORD);
          pango_cairo_update_layout (cr, layout);
          cairo_move_to (cr, mid - (info_weight / 2) + 30,
                         (h / 2) - (info_height / 2));
          pango_cairo_show_layout (cr, layout);

          g_object_unref (layout);

          cairo_set_source_surface (cr, card->print_surface, mid,
                                    (h / 2) - (info_height / 2));
          cairo_paint_with_alpha (cr, gradient);
        }
    }

  // cairo_paint (cr);
}
void
play_application_in_bigpicture ()
{
  GList *l;
  for (l = cards_list; l != NULL; l = l->next)
    {
      BigPictureCard *card = (BigPictureCard *) l->data;

      if (card->index == 0)
        {
          isPlayingApp = TRUE;
          GINGA->start (card->src, nullptr);
        }
    }
}

void
carrousel_rotate (gint dir)
{
  carrouselDir = dir;

  if (dir > 0 && currentCard == 0)
    return;
  else if (dir > 0)
    currentCard--;

  if (dir < 0 && currentCard == (gint) (numCards) -1)
    return;
  else if (dir < 0)
    currentCard++;

  GList *l;
  for (l = cards_list; l != NULL; l = l->next)
    {
      BigPictureCard *card = (BigPictureCard *) l->data;

      if (dir > 0)
        card->index++;
      else if (dir < 0)
        card->index--;

      if (card->index < 0)
        card->drawOrder = card->index * -1;
      else
        card->drawOrder = card->index;

      card->destPosition
          = mid + (card->index * cardWidth) + (card->index * 20);
      card->animate = TRUE;
    }

  gradient = 0;
  cards_list = g_list_sort (cards_list, (GCompareFunc) comp_card_list);
}

void
create_bigpicture_window ()
{
  loadApplicationsXML ();

  if (bigPictureWindow != NULL)
    return;

  cairo_surface_t *image_p = cairo_image_surface_create_from_png (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons", "common",
                    "background_pattern.png", NULL));

  g_assert_nonnull (image_p);
  background_pattern = cairo_pattern_create_for_surface (image_p);
  cairo_pattern_set_extend (background_pattern, CAIRO_EXTEND_REPEAT);

  inBigPictureMode = TRUE;

  GdkRectangle rect;
  GdkDisplay *display = gdk_display_get_default ();
  g_assert_nonnull (display);

#if GTK_CHECK_VERSION(3, 22, 0)
  GdkMonitor *monitor = gdk_display_get_monitor (GDK_DISPLAY (display), 0);
  g_assert_nonnull (monitor);
  gdk_monitor_get_geometry (GDK_MONITOR (monitor), &rect);
#else
  GdkScreen *screen = gdk_display_get_screen (GDK_DISPLAY (display), 0);
  g_assert_nonnull (screen);
  gdk_screen_get_monitor_geometry (GDK_SCREEN (screen), 0, &rect);
#endif

  screen_width = rect.width;
  mid = (rect.width / 2.0);

  bigPictureWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_assert_nonnull (bigPictureWindow);
  gtk_window_set_title (GTK_WINDOW (bigPictureWindow), "Ginga");
  gtk_window_set_default_size (GTK_WINDOW (bigPictureWindow), rect.width,
                               rect.height);
  gtk_window_set_position (GTK_WINDOW (bigPictureWindow),
                           GTK_WIN_POS_CENTER);
  g_signal_connect (bigPictureWindow, "key-press-event",
                    G_CALLBACK (keyboard_callback), (void *) "press");
  g_signal_connect (bigPictureWindow, "key-release-event",
                    G_CALLBACK (keyboard_callback), (void *) "release");
  gtk_container_set_border_width (GTK_CONTAINER (bigPictureWindow), 0);

  // Create Drawing area
  GtkWidget *canvas = gtk_drawing_area_new ();
  g_assert_nonnull (canvas);
  gtk_widget_set_app_paintable (canvas, TRUE);
  g_signal_connect (canvas, "draw", G_CALLBACK (draw_bigpicture_callback),
                    NULL);
  gtk_widget_set_size_request (canvas, rect.width, rect.height);
  gtk_container_add (GTK_CONTAINER (bigPictureWindow), canvas);
  timeOutTagB = g_timeout_add (
      1000 / 60, (GSourceFunc) update_bigpicture_callback, canvas);

  gtk_window_fullscreen (GTK_WINDOW (bigPictureWindow));

  g_signal_connect (bigPictureWindow, "destroy",
                    G_CALLBACK (destroy_bigpicture_window), NULL);

  gtk_widget_show_all (bigPictureWindow);

  carrousel_rotate (-1);
  carrousel_rotate (1);

  GINGA->resize (rect.width, rect.height);
}

void
destroy_card_list (gpointer data)
{
  BigPictureCard *card = (BigPictureCard *) data;
  g_free (card->appName);
  g_free (card->appDesc);
  cairo_surface_destroy (card->surface);
  cairo_surface_destroy (card->print_surface);
}
void
destroy_bigpicture_window ()
{
  if (isPlayingApp)
    {
      isPlayingApp = FALSE;
      GINGA->stop ();
      return;
    };

  inBigPictureMode = FALSE;
  g_source_remove (timeOutTagB);
  gtk_widget_destroy (bigPictureWindow);
  bigPictureWindow = NULL;
  g_list_free_full (cards_list, destroy_card_list);
  cards_list = NULL;
  currentCard = 0;

  GINGA->resize (presentationAttributes.resolutionWidth,
                 presentationAttributes.resolutionHeight);
}
