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
#include "Bind.h"

#include "Composition.h"
#include "SwitchPort.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new bind.
 * @param role Role.
 * @param comp Component.
 * @param iface Interface.

 */
Bind::Bind (Role *role, Node *comp, Anchor *iface)
{
  _role = role;
  _node = comp;
  _interface = iface;
}

/**
 * @brief Destroys bind.
 */
Bind::~Bind ()
{
}

/**
 * @brief Gets role.
 * @return Role.
 */
Role *
Bind::getRole ()
{
  return _role;
}

/**
 * @brief Gets component.
 * @return Component.
 */
Node *
Bind::getNode ()
{
  return _node;
}

/**
 * @brief Gets interface.
 * @return Interface.
 */
Anchor *
Bind::getInterface ()
{
  return _interface;
}

/**
 * @brief Gets parameters.
 * @return Parameters.
 */
const map<string, string> *
Bind::getParameters ()
{
  return &_params;
}

/**
 * @brief Gets parameter.
 * @param name Parameter name.
 * @return Parameter value.
 */
string
Bind::getParameter (const string &name)
{
  map<string, string>::iterator it;
  return ((it = _params.find (name)) != _params.end ())
    ? it->second : "";
}

/**
 * @brief Sets parameter.
 * @param name Parameter name.
 * @param value Parameter value.
 */
void
Bind::setParameter (const string &name, const string &value)
{
  _params[name] = value;
}

GINGA_NCL_END
