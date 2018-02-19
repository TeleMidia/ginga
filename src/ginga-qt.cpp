#include <glib.h>

#include <QApplication>
#include <QElapsedTimer>
#include <QPainter>
#include <QTimer>
#include <QWidget>
#include <QDebug>

#include "ginga.h"

using namespace std;

class GingaQt : public QWidget
{
  Q_OBJECT

public:
  //! Constructor.
  explicit GingaQt (QWidget *parent = 0) : QWidget (parent), _timer (this)
  {
    _ginga_surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
                                                 width (), height ());
    _cr = cairo_create (_ginga_surface);

    _ginga_opts.debug = TRUE;
    _ginga_opts.width = width ();
    _ginga_opts.height = height ();
    _ginga_opts.experimental = FALSE;
    _ginga_opts.background = "black";
    _ginga_opts.opengl = false;

    _ginga = Ginga::create (&_ginga_opts);

    g_assert_nonnull (_ginga);

    connect (&_timer, SIGNAL (timeout ()), this, SLOT (redrawGinga ()));
  }

  //! Destructor.
  virtual ~GingaQt ()
  {
    delete _ginga;
    cairo_destroy (_cr);
    cairo_surface_destroy (_ginga_surface);
  }

  //! Play the file passed as parameter.
  void
  start (const std::string &file)
  {
    string errmsg;
    if (!_ginga->start (file, &errmsg))
      {
        g_printerr ("error: ");
        g_printerr ("%s\n", errmsg.c_str ());
      }

    _timer.start (33);
    _elapsedTime.start ();
  }

  //! Stop the ginga player.
  void
  stop ()
  {
    _ginga->stop ();
    _timer.stop ();
  }

protected:
  void
  paintEvent (QPaintEvent *e)
  {
    Q_UNUSED (e);

    QPainter painter (this);
    if (!_img.isNull ())
      {
        painter.drawImage (0, 0, _img);
      }
  }

  void
  resizeEvent (QResizeEvent *event)
  {
    QWidget::resizeEvent (event);
    // _ginga->resize (width(), height());
  }

protected Q_SLOTS:
  //! Redraw the ginga frame in the QWidget.
  void
  redrawGinga ()
  {
    static guint64 frame = (guint64) -1;
    static guint64 last, first;
    guint64 time = _elapsedTime.elapsed () * 1000000;

    frame++;
    if (frame == 0)
      {
        first = time;
        last = time;
      }

    _ginga->sendTick (time - first, time - last, frame);
    last = time;

    _ginga->redraw (_cr);

    _img = QImage (cairo_image_surface_get_data (_ginga_surface),
                   cairo_image_surface_get_width (_ginga_surface),
                   cairo_image_surface_get_height (_ginga_surface),
                   QImage::Format_ARGB32_Premultiplied);

    g_assert_cmpuint (_img.bytesPerLine (), ==,
                      cairo_image_surface_get_stride (_ginga_surface));

    _img = _img.scaled (width (), height ());
    update ();
  }

private:
  Ginga *_ginga;
  GingaOptions _ginga_opts;

  cairo_surface_t *_ginga_surface;
  cairo_t *_cr;

  QImage _img;
  QTimer _timer;
  QElapsedTimer _elapsedTime;
};

int
main (int argc, char **argv)
{
  QApplication a (argc, argv);
  GingaQt w;

  w.show ();
  if (argc > 1)
    w.start (argv[1]);
  else
    qFatal (
        "ginga-qt: Missing file operand.\nUsage: ./ginga-qt <FILE_URI>");

  return a.exec ();
}

#include "ginga-qt.moc" // Used to call the moc system.
