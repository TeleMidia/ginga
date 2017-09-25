#include "runview.h"
#include "ui_runview.h"

#include <cairo.h>
#include <glib.h>
#include <stdlib.h>
#include <glib/gstdio.h>

#include "ginga.h"

#include <QDebug>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>

using namespace std;

// Global formatter.
static Ginga *GINGA = nullptr;
GingaOptions opts;

cairo_surface_t *surface;
cairo_t *cr;

RunView::RunView (QWidget *parent) :
  QWidget (parent),
  _ui (new Ui::RunView)
{
  _ui->setupUi(this);

  setUpdatesEnabled (true);

  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 800, 600);
  cr = cairo_create (surface);

  opts.debug = FALSE;
  opts.width = 800;
  opts.height = 600;
  opts.experimental = FALSE;
  opts.background = "black";

  char **argv;
  GINGA = Ginga::create (0, argv, &opts);

  g_assert_nonnull (GINGA);
}

void
RunView::start (const string &file)
{
  string errmsg;
  if (!GINGA->start (file, &errmsg))
    {
      g_printerr ("error: ");
      g_printerr ("%s\n", errmsg.c_str ());
    }

  QTimer *timer = new QTimer(this);
  connect (timer, SIGNAL(timeout()), this, SLOT(redrawGinga()));
  timer->start (33);
}

void
RunView::stop ()
{
  GINGA->stop ();
  close ();
}

RunView::~RunView()
{
  delete _ui;
}

void
RunView::redrawGinga ()
{
  GINGA->redraw (cr);
  img = QImage (cairo_image_surface_get_data (surface),
                cairo_image_surface_get_width (surface),
                cairo_image_surface_get_height (surface),
                QImage::Format_ARGB32_Premultiplied);
  g_assert_cmpuint(img.bytesPerLine(), ==, cairo_image_surface_get_stride(surface));

  update ();
}

void
RunView::paintEvent (QPaintEvent *e)
{
  QPainter painter (this);

  if (!img.isNull())
    {
      painter.drawImage (0, 0, img);
    }
}
