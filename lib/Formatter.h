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

/**
 * @brief Interface between libginga and the external world.
 */
class Formatter : public Ginga
{
public:
  // Ginga:
  GingaState getState ();

  bool start (const std::string &, std::string *);
  bool stop ();

  void resize (int, int);
  void redraw (cairo_t *);

  bool sendKey (const std::string &, bool);
  bool sendTick (uint64_t, uint64_t, uint64_t);

  const GingaOptions *getOptions ();
  bool getOptionBool (const std::string &);
  void setOptionBool (const std::string &, bool);
  int getOptionInt (const std::string &);
  void setOptionInt (const std::string &, int);
  string getOptionString (const std::string &);
  void setOptionString (const std::string &, std::string);

  // Formatter:
  explicit Formatter (const GingaOptions *);
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
  /// @brief Current state.
  GingaState _state;

  /// @brief Current options.
  GingaOptions _opts;

  /// @brief Current background color.
  Color _background;

  /// @brief The last total time informed via Formatter::sendTick.
  Time _lastTickTotal;

  /// @brief The last diff time informed via Formatter::sendTick.
  Time _lastTickDiff;

  /// @brief The last frame number informed via Formatter::sendTick.
  uint64_t _lastTickFrameNo;

  /// @brief The saved value of environment variable G_MESSAGES_DEBUG.
  string _saved_G_MESSAGES_DEBUG;

  /// @brief Current document tree.
  Document *_doc;

  /// @brief Path of the file that originated the current document.
  string _docPath;

  /// @brief Whether the presentation has ended naturally.
  bool _eos;
};

GINGA_NAMESPACE_END

#endif // FORMATTER_H
