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

Ginga::Ginga (GingaOptions *)
{
}

Ginga::~Ginga ()
{
}

// Class methods.

/**
 * @brief Creates a new Ginga handle.
 * @param opts Ginga options.
 * @return A new formatter handle.
 */
Ginga *
Ginga::create (GingaOptions *opts)
{
  setlocale (LC_ALL, "C");
  return new Formatter (opts);
}

/**
 * @brief Gets Ginga version string.
 * @return libginga version string.
 */
string
Ginga::version ()
{
  return PACKAGE_VERSION;
}

/**
 * @fn Ginga::start
 * @brief Starts the presentation of an NCL file.
 * @param path Path to NCL file.
 * @param[out] errmsg Error message.
 * @return \c true if successful or \c false otherwise.
 */
