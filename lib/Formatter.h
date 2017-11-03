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

#ifndef FORMATTER_H
#define FORMATTER_H

#include "ginga.h"
#include "aux-ginga.h"

GINGA_NAMESPACE_BEGIN

class FormatterScheduler;
class Formatter: public Ginga
{
 public:
  // External API.
  GingaState getState ();

  bool start (const string &, string *);
  bool stop ();

  void resize (int, int);
  void redraw (cairo_t *);

  bool sendKeyEvent (const string &, bool);
  bool sendTickEvent (uint64_t, uint64_t, uint64_t);

  const GingaOptions *getOptions ();
  bool getOptionBool (const string &);
  void setOptionBool (const string &, bool);
  int getOptionInt (const string &);
  void setOptionInt (const string &, int);
  string getOptionString (const string &);
  void setOptionString (const string &, string);

  // Internal API.
  Formatter (int, char **, GingaOptions *);
  virtual ~Formatter ();
  FormatterScheduler *getScheduler ();

  bool getEOS ();
  void setEOS (bool);

  static void setOptionDebug (Formatter *, const string &, bool);
  static void setOptionExperimental (Formatter *, const string &, bool);
  static void setOptionOpenGL (Formatter *, const string &, bool);
  static void setOptionSize (Formatter *, const string &, int);
  static void setOptionBackground (Formatter *, const string &, string);

 private:
  GingaState _state;              // current state
  GingaOptions _opts;             // current options
  FormatterScheduler *_scheduler; // formatter core

  string _ncl_file;               // path to current NCL file
  bool _eos;                      // true if EOS was reached
  GingaColor _background;         // current background color
  uint64_t _last_tick_total;      // last total informed via sendTickEvent
  uint64_t _last_tick_diff;       // last diff informed via sendTickEvent
  uint64_t _last_tick_frameno;    // last frameno informed via sendTickEvent
  string _saved_G_MESSAGES_DEBUG; // saved G_MESSAGES_DEBUG value
};

GINGA_NAMESPACE_END

#endif // FORMATTER_H
