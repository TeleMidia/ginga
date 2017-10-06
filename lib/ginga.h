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

#ifndef GINGA_H
#define GINGA_H

#ifdef  __cplusplus
# define GINGA_BEGIN_DECLS extern "C" {/*}*/
# define GINGA_END_DECLS            /*{*/}
#else
# define GINGA_BEGIN_DECLS
# define GINGA_END_DECLS
#endif

GINGA_BEGIN_DECLS
#include <cairo.h>
GINGA_END_DECLS

#include <cstdint>
#include <string>

/**
 * @brief Ginga options.
 */
struct GingaOptions
{
  int width;
  int height;
  bool debug;
  bool experimental;
  bool opengl;
  std::string background;
};

/**
 * @brief Ginga states.
 */
typedef enum
{
 GINGA_STATE_PLAYING,
 GINGA_STATE_STOPPED,
} GingaState;

/**
 * @brief Ginga object.
 */
class Ginga
{
public:
  Ginga (int, char **, GingaOptions *);
  virtual ~Ginga () = 0;

  virtual GingaState getState () = 0;
  virtual bool start (const std::string &, std::string *) = 0;
  virtual bool stop () = 0;

  virtual void resize (int, int) = 0;
  virtual void redraw (cairo_t *) = 0;
  virtual void redrawGL () = 0;

  virtual bool sendKeyEvent (const std::string &, bool) = 0;
  virtual bool sendTickEvent (uint64_t, uint64_t, uint64_t) = 0;

  virtual const GingaOptions *getOptions () = 0;
  virtual bool getOptionBool (const std::string &) = 0;
  virtual void setOptionBool (const std::string &, bool) = 0;
  virtual int getOptionInt (const std::string &) = 0;
  virtual void setOptionInt (const std::string &, int) = 0;
  virtual std::string getOptionString (const std::string &) = 0;
  virtual void setOptionString (const std::string &, std::string) = 0;

public:
  static Ginga *create (int, char **, GingaOptions *);
  static std::string version ();
};

#endif // GINGA_H
