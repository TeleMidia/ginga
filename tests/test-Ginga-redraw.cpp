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

#include "tests.h"
#include <gtk/gtk.h>

Formatter *fmt;
Media *m1, *m2;
int tickCounter;
int WIDTH = 800, HEIGHT = 600;

#if GTK_CHECK_VERSION(3, 8, 0)
static gboolean
tick_callback (GtkWidget *widget, GdkFrameClock *frame_clock,
               unused (gpointer data))
#else
static gboolean
tick_callback (GtkWidget *widget)
#endif
{
  gtk_widget_queue_draw (widget);
  return G_SOURCE_CONTINUE;
}

static gboolean
draw_callback (unused (GtkWidget *widget), cairo_t *cr,
               unused (gpointer data))
{
  int rowstride, n_channels, x, y;
  GdkPixbuf *pixbuf;
  guchar *pixels, *p;
  guchar alpha, red, green, blue;

  // ginga redraw
  fmt->redraw (cr);

  // get center pixel
  pixbuf = gdk_pixbuf_get_from_surface (cairo_get_target (cr), 0, 0, WIDTH,
                                        HEIGHT);
  n_channels = gdk_pixbuf_get_n_channels (pixbuf);

  g_assert (gdk_pixbuf_get_colorspace (pixbuf) == GDK_COLORSPACE_RGB);
  g_assert_cmpint (gdk_pixbuf_get_bits_per_sample (pixbuf), ==, 8);
  // g_assert (gdk_pixbuf_get_has_alpha (pixbuf));
  // g_assert (n_channels == 4);
  g_assert_cmpint (n_channels, ==, 3);

  WIDTH = gdk_pixbuf_get_width (pixbuf);
  HEIGHT = gdk_pixbuf_get_height (pixbuf);
  rowstride = gdk_pixbuf_get_rowstride (pixbuf);
  pixels = gdk_pixbuf_get_pixels (pixbuf);

  x = WIDTH / 2;
  y = HEIGHT / 2;
  p = pixels + y * rowstride + x * n_channels;
  red = p[0];
  green = p[1];
  blue = p[2];
  alpha = p[3];

  // printf ("\n--------tickCounter=%d\n", tickCounter);
  // printf ("center pixel: red=%d, green=%d, blue=%d, \n", red, green,
  // blue);
  // printf ("m1 state=%d, m1.zIndex=%s, m2 state=%d, m2.zIndex=%s\n",
  //         m1->isOccurring (), m1->getProperty ("zIndex").c_str (),
  //         m2->isOccurring (), m2->getProperty ("zIndex").c_str ());
  if (tickCounter == 0)
    {
      g_assert (m1->getProperty ("zIndex") == "1");
      g_assert (m2->getProperty ("zIndex") == "2");
      g_assert_cmpint (red, ==, 0);
      g_assert_cmpint (green, ==, 128);
      g_assert_cmpint (blue, ==, 0);
    }
  else if (tickCounter == 1)
    {
      g_assert (m1->getProperty ("zIndex") == "2");
      g_assert (m2->getProperty ("zIndex") == "1");
      g_assert_cmpint (red, ==, 255);
      g_assert_cmpint (green, ==, 0);
      g_assert_cmpint (blue, ==, 0);
    }
  else if (tickCounter == 2)
    {
      g_assert (m1->getProperty ("zIndex") == "1");
      g_assert (m2->getProperty ("zIndex") == "2");
      g_assert_cmpint (red, ==, 0);
      g_assert_cmpint (green, ==, 128);
      g_assert_cmpint (blue, ==, 0);
    }
  else if (tickCounter == 3)
    {
      gtk_main_quit ();
    }

  // advance time and trigger links to change zIndex
  fmt->sendTick (1.02 * GINGA_SECOND, 1.02 * GINGA_SECOND, 0);
  tickCounter = tickCounter + 1;

  return TRUE;
}

int
main (void)
{
  // zIndex defined by properties
  {
    gtk_init (nullptr, nullptr);
    Document *doc;
    GtkWidget *app;
    GingaOptions opts;

    app = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_assert_nonnull (app);
    gtk_window_set_default_size (GTK_WINDOW (app), WIDTH, HEIGHT);
    gtk_widget_set_app_paintable (app, TRUE);
    g_signal_connect (app, "draw", G_CALLBACK (draw_callback), NULL);
    gtk_widget_add_tick_callback (app, (GtkTickCallback) tick_callback,
                                  NULL, NULL);

    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body>\n\
    <port id='startTimer' component='timer'/>\n\
    <port id='start1' component='m1'/>\n\
    <port id='start2' component='m2'/>\n\
    <media id='timer'>\n\
      <area id='a1' begin='1s'/>\n\
      <area id='a2' begin='2s'/>\n\
    </media>\n\
    <media id='m1'>\n\
      <property name='background' value='red'/>\n\
      <property name='zIndex' value='1'/>\n\
    </media>\n\
    <media id='m2'>\n\
      <property name='background' value='green'/>\n\
      <property name='zIndex' value='2'/>\n\
    </media>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='timer' interface='a1'/>\n\
      <bind role='set' component='m1' interface='zIndex'>\n\
        <bindParam name='var' value='2'/>\n\
      </bind>\n\
      <bind role='set' component='m2' interface='zIndex'>\n\
        <bindParam name='var' value='1'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='timer' interface='a2'/>\n\
      <bind role='set' component='m1' interface='zIndex'>\n\
        <bindParam name='var' value='1'/>\n\
      </bind>\n\
      <bind role='set' component='m2' interface='zIndex'>\n\
        <bindParam name='var' value='2'/>\n\
      </bind>\n\
    </link>\n\
  </body>\n\
</ncl>\n");

    Context *body = cast (Context *, doc->getRoot ());
    g_assert_nonnull (body);
    Event *body_lambda = body->getLambda ();
    g_assert_nonnull (body_lambda);

    m1 = cast (Media *, doc->getObjectById ("m1"));
    g_assert_nonnull (m1);
    Event *m1_lambda = m1->getLambda ();
    g_assert_nonnull (m1_lambda);

    m2 = cast (Media *, doc->getObjectById ("m2"));
    g_assert_nonnull (m2);
    Event *m2_lambda = m2->getLambda ();
    g_assert_nonnull (m2_lambda);

    // --------------------------------
    // check start document

    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    // when start document, m1 is OCCURRING
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::OCCURRING);

    // --------------------------------
    // main check
    tickCounter = 0;
    gtk_widget_show_all (app);
    gtk_main ();

    delete fmt;
  }

  // zIndex defined by region
  {
    gtk_init (nullptr, nullptr);
    Document *doc;
    GtkWidget *app;
    GingaOptions opts;

    app = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_assert_nonnull (app);
    gtk_window_set_default_size (GTK_WINDOW (app), WIDTH, HEIGHT);
    gtk_widget_set_app_paintable (app, TRUE);
    g_signal_connect (app, "draw", G_CALLBACK (draw_callback), NULL);
    gtk_widget_add_tick_callback (app, (GtkTickCallback) tick_callback,
                                  NULL, NULL);

    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
  <head>\n\
    <regionBase>\n\
      <region id='reg1' width='100%%' height='100%%' zIndex='1'/>\n\
      <region id='reg2' width='100%%' height='100%%' zIndex='2'/>\n\
    </regionBase>\n\
    <descriptorBase>\n\
      <descriptor id='desc1' region='reg1'/>\n\
      <descriptor id='desc2' region='reg2'/>\n\
    </descriptorBase>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body>\n\
    <port id='startTimer' component='timer'/>\n\
    <port id='start1' component='m1'/>\n\
    <port id='start2' component='m2'/>\n\
    <media id='timer'>\n\
      <area id='a1' begin='1s'/>\n\
      <area id='a2' begin='2s'/>\n\
    </media>\n\
    <media id='m1' descriptor='desc1'>\n\
      <property name='background' value='red'/>\n\
    </media>\n\
    <media id='m2' descriptor='desc2'>\n\
      <property name='background' value='green'/>\n\
    </media>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='timer' interface='a1'/>\n\
      <bind role='set' component='m1' interface='zIndex'>\n\
        <bindParam name='var' value='2'/>\n\
      </bind>\n\
      <bind role='set' component='m2' interface='zIndex'>\n\
        <bindParam name='var' value='1'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='timer' interface='a2'/>\n\
      <bind role='set' component='m1' interface='zIndex'>\n\
        <bindParam name='var' value='1'/>\n\
      </bind>\n\
      <bind role='set' component='m2' interface='zIndex'>\n\
        <bindParam name='var' value='2'/>\n\
      </bind>\n\
    </link>\n\
  </body>\n\
</ncl>\n");

    Context *body = cast (Context *, doc->getRoot ());
    g_assert_nonnull (body);
    Event *body_lambda = body->getLambda ();
    g_assert_nonnull (body_lambda);

    m1 = cast (Media *, doc->getObjectById ("m1"));
    g_assert_nonnull (m1);
    Event *m1_lambda = m1->getLambda ();
    g_assert_nonnull (m1_lambda);

    m2 = cast (Media *, doc->getObjectById ("m2"));
    g_assert_nonnull (m2);
    Event *m2_lambda = m2->getLambda ();
    g_assert_nonnull (m2_lambda);

    // --------------------------------
    // check start document

    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    // when start document, m1 is OCCURRING
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::OCCURRING);

    // --------------------------------
    // main check
    tickCounter = 0;
    gtk_widget_show_all (app);
    gtk_main ();

    delete fmt;
  }

  // zIndex defined by descriptor
  {
    gtk_init (nullptr, nullptr);
    Document *doc;
    GtkWidget *app;
    GingaOptions opts;

    app = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_assert_nonnull (app);
    gtk_window_set_default_size (GTK_WINDOW (app), WIDTH, HEIGHT);
    gtk_widget_set_app_paintable (app, TRUE);
    g_signal_connect (app, "draw", G_CALLBACK (draw_callback), NULL);
    gtk_widget_add_tick_callback (app, (GtkTickCallback) tick_callback,
                                  NULL, NULL);

    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
  <head>\n\
    <regionBase>\n\
      <region id='reg1' width='100%%' height='100%%'/>\n\
      <region id='reg2' width='100%%' height='100%%'/>\n\
    </regionBase>\n\
    <descriptorBase>\n\
      <descriptor id='desc1' region='reg1' zIndex='1'/>\n\
      <descriptor id='desc2' region='reg2' zIndex='2'/>\n\
    </descriptorBase>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body>\n\
    <port id='startTimer' component='timer'/>\n\
    <port id='start1' component='m1'/>\n\
    <port id='start2' component='m2'/>\n\
    <media id='timer'>\n\
      <area id='a1' begin='1s'/>\n\
      <area id='a2' begin='2s'/>\n\
    </media>\n\
    <media id='m1' descriptor='desc1'>\n\
      <property name='background' value='red'/>\n\
    </media>\n\
    <media id='m2' descriptor='desc2'>\n\
      <property name='background' value='green'/>\n\
    </media>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='timer' interface='a1'/>\n\
      <bind role='set' component='m1' interface='zIndex'>\n\
        <bindParam name='var' value='2'/>\n\
      </bind>\n\
      <bind role='set' component='m2' interface='zIndex'>\n\
        <bindParam name='var' value='1'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='timer' interface='a2'/>\n\
      <bind role='set' component='m1' interface='zIndex'>\n\
        <bindParam name='var' value='1'/>\n\
      </bind>\n\
      <bind role='set' component='m2' interface='zIndex'>\n\
        <bindParam name='var' value='2'/>\n\
      </bind>\n\
    </link>\n\
  </body>\n\
</ncl>\n");

    Context *body = cast (Context *, doc->getRoot ());
    g_assert_nonnull (body);
    Event *body_lambda = body->getLambda ();
    g_assert_nonnull (body_lambda);

    m1 = cast (Media *, doc->getObjectById ("m1"));
    g_assert_nonnull (m1);
    Event *m1_lambda = m1->getLambda ();
    g_assert_nonnull (m1_lambda);

    m2 = cast (Media *, doc->getObjectById ("m2"));
    g_assert_nonnull (m2);
    Event *m2_lambda = m2->getLambda ();
    g_assert_nonnull (m2_lambda);

    // --------------------------------
    // check start document

    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    // when start document, m1 is OCCURRING
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::OCCURRING);

    // --------------------------------
    // main check
    tickCounter = 0;
    gtk_widget_show_all (app);
    gtk_main ();

    delete fmt;
  }

  exit (EXIT_SUCCESS);
}
