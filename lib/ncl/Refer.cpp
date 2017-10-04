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
#include "Refer.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new refer node.
 * @param ncl Parent document.
 * @param id Refer node id.
 */
Refer::Refer (NclDocument *ncl, const string &id) : Node (ncl, id)
{
  _referred = nullptr;
}

/**
 * @brief Gets the media referenced by refer node.
 */
Media *
Refer::getReferred ()
{
  return _referred;
}

/**
 * @brief Initializes the media referenced by refer node.
 * @param media Media.
 */
void
Refer::initReferred (Media *media)
{
  g_assert_nonnull (media);
  g_assert (_referred == nullptr || _referred == media);
  _referred = media;
  media->addSameInstance (this);
}

GINGA_NCL_END
