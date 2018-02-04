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

#include <locale.h>
#include "ginga.h"
#include "aux-ginga.h"

#include "Formatter.h"

/**
 * @file Ginga.cpp
 * @brief The libginga API.
 */

/**
 * @brief Creates a new Ginga object.
 * @param opts Options to initialize the object with.
 * @return New #Ginga.
 */
Ginga::Ginga (unused (const GingaOptions *opts))
{
}

/**
 * @brief Destroys Ginga object.
 */
Ginga::~Ginga ()
{
}

/**
 * @brief Creates a new Ginga object.
 * @param opts Options to initialize the object with.
 * @return New #Ginga.
 */
Ginga *
Ginga::create (const GingaOptions *opts)
{
  setlocale (LC_ALL, "C");
  return new Formatter (opts);
}

/**
 * @brief Gets libginga version string.
 * @return libginga version string.
 */
string
Ginga::version ()
{
  return PACKAGE_VERSION;
}

/**
 * @fn Ginga::getState
 * @brief Gets Ginga object state.
 * @return Ginga object state.
 */

/**
 * @fn Ginga::start
 * @brief Starts the presentation of an NCL file.
 * @param path Path to NCL file.
 * @param[out] errmsg Variable to store the error message (if any).
 * @return \c true if successful or \c false otherwise.
 */

/**
 * @fn Ginga::stop
 * @brief Stops the presentation.
 * @return \c true if successful or \c false otherwise.
 */

/**
 * @fn Ginga::resize
 * @brief Resizes the presentation screen.
 * @param width Screen width (in pixels).
 * @param height Screen height (in pixels).
 */

/**
 * @fn Ginga::redraw
 * @brief Draws the latest frame of the presentation on Cairo context.
 * @param cr Cairo context.
 */

/**
 * @fn Ginga::sendKey
 * @brief Sends key event to presentation.
 * @param key Key name.
 * @param press Whether the key was pressed (or released).
 * @return \c true if successful, or \c false otherwise.
 */

/**
 * @fn Ginga::sendTick
 * @brief Sends tick event to presentation.
 * @param total Time since the presentation started.
 * @param diff Time since last tick.
 * @param frame Tick number.
 * @return \c true if successful, or \c false otherwise.
 */

/**
 * @fn Ginga::getOptions
 * @brief Gets current options.
 * @return Current options.
 */

/**
 * @fn Ginga::getOptionBool
 * @brief Gets boolean option value.
 * @param name Option name.
 * @return Option value.
 */

/**
 * @fn Ginga::setOptionBool
 * @brief Sets boolean option.
 * @param name Option name.
 * @param value Option value.
 */

/**
 * @fn Ginga::getOptionInt
 * @brief Gets integer option value.
 * @param name Option name.
 * @return Option value.
 */

/**
 * @fn Ginga::setOptionInt
 * @brief Sets integer option.
 * @param name Option name.
 * @param value Option value.
 */

/**
 * @fn Ginga::getOptionString
 * @brief Gets string option value.
 * @param name Option name.
 * @return Option value.
 */

/**
 * @fn Ginga::setOptionString
 * @brief Sets string option.
 * @param name Option name.
 * @param value Option value.
 */
