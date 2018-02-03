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

#ifndef FORMATTER_H
#define FORMATTER_H

#include "ginga.h"
#include "aux-ginga.h"

#include "Document.h"

GINGA_NAMESPACE_BEGIN

class Context;
class Event;
class Media;
class MediaSettings;
class Object;

class Formatter: public Ginga
{
 public:
  // External API.
  GingaState getState ();

  bool start (const string &, string *);
  bool start (const string &, size_t, string *);
  bool stop ();

  void resize (int, int);
  void redraw (cairo_t *);

  bool sendKey (const string &, bool);
  bool sendTick (uint64_t, uint64_t, uint64_t);

  const GingaOptions *getOptions ();
  bool getOptionBool (const string &);
  void setOptionBool (const string &, bool);
  int getOptionInt (const string &);
  void setOptionInt (const string &, int);
  string getOptionString (const string &);
  void setOptionString (const string &, string);

  // Internal API.
  Formatter (int, char **, GingaOptions *);
  ~Formatter ();

  Document *getDocument ();
  bool getEOS ();
  void setEOS (bool);

  static void setOptionBackground (Formatter *, const string &, string);
  static void setOptionDebug (Formatter *, const string &, bool);
  static void setOptionExperimental (Formatter *, const string &, bool);
  static void setOptionOpenGL (Formatter *, const string &, bool);
  static void setOptionSize (Formatter *, const string &, int);

 private:
  GingaState _state;            // current state
  GingaOptions _opts;           // current options
  Color _background;            // current background color

  uint64_t _last_tick_total;      // last total informed via sendTick
  uint64_t _last_tick_diff;       // last diff informed via sendTick
  uint64_t _last_tick_frameno;    // last frameno informed via sendTick
  string _saved_G_MESSAGES_DEBUG; // saved G_MESSAGES_DEBUG value

  Document *_doc;               // current document
  string _docPath;              // path to current document
  bool _eos;                    // true if EOS was reached
};

GINGA_NAMESPACE_END

#endif // FORMATTER_H
