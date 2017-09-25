#include "runview.h"
#include "ui_runview.h"

#include <glib.h>

#include <QPainter>
#include <QPaintEvent>

using namespace std;

RunView::RunView (QWidget *parent) :
  QWidget (parent),
  _ui (new Ui::RunView),
  _timer (this)
{
  _ui->setupUi(this);

  _ginga_surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 800, 600);
  _cr = cairo_create (_ginga_surface);

  _ginga_opts.debug = FALSE;
  _ginga_opts.width = 800;
  _ginga_opts.height = 600;
  _ginga_opts.experimental = FALSE;
  _ginga_opts.background = "black";

  char **argv;
  _ginga = Ginga::create (0, argv, &_ginga_opts);

  g_assert_nonnull (_ginga);

  connect (&_timer, SIGNAL(timeout()), this, SLOT(redrawGinga()));
}

void
RunView::start (const string &file)
{
  string errmsg;
  if (!_ginga->start (file, &errmsg))
    {
      g_printerr ("error: ");
      g_printerr ("%s\n", errmsg.c_str ());
    }

  _timer.start (33);
}

void
RunView::stop ()
{
  _ginga->stop ();
  _timer.stop();
}

RunView::~RunView()
{
  delete _ui;
  delete _ginga;
}

void
RunView::redrawGinga ()
{
  _ginga->redraw (_cr);
  _img = QImage (cairo_image_surface_get_data (_ginga_surface),
                cairo_image_surface_get_width (_ginga_surface),
                cairo_image_surface_get_height (_ginga_surface),
                QImage::Format_ARGB32_Premultiplied);
  g_assert_cmpuint(_img.bytesPerLine(), ==, cairo_image_surface_get_stride(_ginga_surface));

  update ();
}

void
RunView::paintEvent (QPaintEvent *e)
{
  QPainter painter (this);

  if (!_img.isNull())
    {
      painter.drawImage (0, 0, _img);
    }
}
