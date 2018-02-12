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
 * @file ginga.h
 * @brief The libginga API.
 */

/**
 * @brief Ginga control options.
 */
struct GingaOptions
{
  /// @brief Screen width (in pixels).
  int width;

  /// @brief Screen height (in pixels).
  int height;

  /// @brief Whether to enable debug mode.
  bool debug;

  /// @brief Whether to enable experimental features.
  bool experimental;

  /// @brief Whether to use OpenGL back-end.
  /// @remark Can only when Ginga object is created.
  bool opengl;

  /// @brief Background color.
  std::string background;
};

/**
 * @brief Ginga states.
 */
typedef enum
{
 GINGA_STATE_PLAYING,           ///< Ginga is playing.
 GINGA_STATE_STOPPED,           ///< Ginga is stopped.
} GingaState;

/**
 * @brief Ginga handle.
 *
 * Opaque handle that represents an NCL formatter.
 */
class Ginga
{
public:
  explicit Ginga (const GingaOptions *opts);
  virtual ~Ginga () = 0;

  virtual GingaState getState () = 0;
  virtual bool start (const std::string &path, std::string *errmsg) = 0;
  virtual bool stop () = 0;

  virtual void resize (int width, int height) = 0;
  virtual void redraw (cairo_t *cr) = 0;

  virtual bool sendKey (const std::string &key, bool press) = 0;
  virtual bool sendTick (uint64_t total, uint64_t diff, uint64_t frame) = 0;

  virtual const GingaOptions *getOptions () = 0;
  virtual bool getOptionBool (const std::string &name) = 0;
  virtual void setOptionBool (const std::string &name, bool value) = 0;
  virtual int getOptionInt (const std::string &name) = 0;
  virtual void setOptionInt (const std::string &name, int value) = 0;
  virtual std::string getOptionString (const std::string &name) = 0;
  virtual void setOptionString (const std::string &name,
                                std::string value) = 0;

  static Ginga *create (const GingaOptions *opts);
  static std::string version ();
};

#endif // GINGA_H
