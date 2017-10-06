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

#include "aux-ginga.h"
#include "Context.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new context.
 * @param ncl Parent document.
 * @param id Context id.
 */
Context::Context (NclDocument *ncl, const string &id)
  : Composition (ncl, id)
{
}

/**
 * @brief Destroys context.
 */
Context::~Context ()
{
}

/**
 * @brief Adds link to context.
 * @param link Link.
 */
void
Context::addLink (Link *link)
{
  g_assert_nonnull (link);
  _links.push_back (link);
}

/**
 * @brief Gets all context links.
 */
const vector<Link *> *
Context::getLinks ()
{
  return &_links;
}

GINGA_NCL_END
