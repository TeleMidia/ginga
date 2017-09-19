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
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef GINGA_PRIVATE_H
#define GINGA_PRIVATE_H

#include "ginga.h"

#include "formatter/Scheduler.h"
using namespace ::ginga::formatter;

#include "mb/Display.h"
using namespace ::ginga::mb;

class GingaPrivate : public Ginga
{
 public:
  GingaPrivate (int, char **, GingaOptions *);
  virtual ~GingaPrivate ();

  void resize (int, int) ;
  void start (const string &);
  void stop ();

  void redraw (cairo_t *);
  void sendKeyEvent (const string &, bool);
  void sendTickEvent (uint64_t, uint64_t, uint64_t);

  GingaOptions getOptions ();
  bool getOptionBool (const string &);
  void setOptionBool (const string &, bool);
  int getOptionInt (const string &);
  void setOptionInt (const string &, int);

  static void setOptionDebug (GingaPrivate *, const string &, bool);
  static void setOptionSize (GingaPrivate *, const string &, int);

 private:
  GingaOptions *_opts;          // current options
  bool _started;                // true if formatter was started
  Scheduler *_scheduler;
  Display *_display;
};

#endif // GINGA_PRIVATE_H
