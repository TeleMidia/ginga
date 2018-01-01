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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef RUNVIEW_H
#define RUNVIEW_H

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

#endif // RUNVIEW_H
