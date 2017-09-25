#include "runview.h"
#include "ui_runview.h"

#include <cairo.h>
#include <glib.h>
#include <stdlib.h>
#include <glib/gstdio.h>

#include "ginga.h"

#include <QTimer>
#include <QPainter>
#include <QPaintEvent>

using namespace std;

// Global formatter.
static Ginga *GINGA = nullptr;
GingaOptions opts;

cairo_surface_t *surface;
cairo_t *cr;

int abc = 0;

RunView::RunView(QWidget *parent) :
  QWidget(parent),
  _ui(new Ui::RunView)
{
  _ui->setupUi(this);

  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 800, 600);
  cr = cairo_create (surface);

  opts.debug = FALSE;
  opts.width = 800;
  opts.height = 600;
  opts.experimental = FALSE;
  opts.background = "";

  char **argv;
  GINGA = Ginga::create (0, argv, &opts);

  g_assert_nonnull (GINGA);

  QTimer *timer = new QTimer(this);
  connect (timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start (30);
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
}

RunView::~RunView()
{
  delete _ui;
}

void
RunView::paintEvent(QPaintEvent *e)
{
  GINGA->redraw (cr);

  QImage img(cairo_image_surface_get_data (surface),
             cairo_image_surface_get_width (surface),
             cairo_image_surface_get_height (surface),
             QImage::Format_ARGB32_Premultiplied);

  g_assert_cmpuint(img.bytesPerLine(), ==, cairo_image_surface_get_stride(surface));

  img.save (QString("/tmp/image") + abc + ".png");
  QPainter painter;
  painter.begin(this);
  painter.setBackgroundMode(Qt::TransparentMode);
  painter.setRenderHint(QPainter::Antialiasing);

  const QRect & rect = e->rect();
  painter.eraseRect(rect);
  painter.setRenderHint(QPainter::Antialiasing);

  painter.drawImage(10,10, img);
  painter.end();
}
