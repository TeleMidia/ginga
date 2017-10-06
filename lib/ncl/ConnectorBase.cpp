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
#include "ConnectorBase.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new connector base.
 * @param ncl Parent document.
 * @param id Base id.
 */
ConnectorBase::ConnectorBase (NclDocument *ncl, const string &id)
  : Base (ncl, id)
{
}

/**
 * @brief Destroys connector base.
 */
ConnectorBase::~ConnectorBase ()
{
}

/**
 * @brief Adds connector to base.
 * @param connector Connector.
 */
void
ConnectorBase::addConnector (Connector *connector)
{
  Base::addEntity (connector);
}

/**
 * @brief Gets connector.
 * @param id Connector id.
 * @return Connector if successful, or null if not found.
 */
Connector *
ConnectorBase::getConnector (const string &id)
{
  return cast (Connector *, Base::getEntity (id));
}

GINGA_NCL_END
