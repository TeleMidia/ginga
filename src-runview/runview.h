#ifndef WIDGET_H
#define WIDGET_H

#include <string>
#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>

#include "ginga.h"

namespace Ui {
  class RunView;
}

class RunView : public QWidget
{
  Q_OBJECT

public:
  explicit RunView (QWidget *parent = 0);
  virtual ~RunView ();

  void start (const std::string &);
  void stop ();

protected:
  void paintEvent (QPaintEvent *);
  void resizeEvent(QResizeEvent* event);

protected Q_SLOTS:
  void redrawGinga ();

private:
  Ginga *_ginga;
  GingaOptions _ginga_opts;

  cairo_surface_t *_ginga_surface;
  cairo_t *_cr;

  Ui::RunView *_ui;
  QImage _img;
  QTimer _timer;
  QElapsedTimer _elapsedTime;
};

#endif // WIDGET_H
