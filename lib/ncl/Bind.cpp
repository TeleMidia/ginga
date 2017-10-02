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
 * @param comp Component.
 * @param iface Interface.
 * @param desc Descriptor.
 * @param role Role.
 */
Bind::Bind (Role *role, Node *comp, Anchor *iface, Descriptor *desc)
{
  _role = role;
  _node = comp;
  _interface = iface;
  _descriptor = desc;
}

/**
 * @brief Destroys bind.
 */
Bind::~Bind ()
{
  _parameters.clear ();
}

/**
 * @brief Gets role.
 */
Role *
Bind::getRole ()
{
  return _role;
}

/**
 * @brief Gets component.
 */
Node *
Bind::getNode ()
{
  return _node;
}

/**
 * @brief Gets interface.
 */
Anchor *
Bind::getInterface ()
{
  return _interface;
}

/**
 * @brief Gets descriptor.
 */
Descriptor *
Bind::getDescriptor ()
{
  return _descriptor;
}

/**
 * @brief Adds parameter to bind.
 * @param parameter Parameter.
 */
void
Bind::addParameter (Parameter *parameter)
{
  g_assert_nonnull (parameter);
  _parameters.push_back (parameter);
}

/**
 * @brief Gets all parameters.
 */
const vector<Parameter *> *
Bind::getParameters ()
{
  return &_parameters;
}

/**
 * @brief Gets parameter.
 * @param name Parameter name.
 * @return Parameter if successful, or null if not found.
 */
Parameter *
Bind::getParameter (const string &name)
{
  for (auto param: _parameters)
    if (param->getName () == name)
      return param;
  return nullptr;
}

GINGA_NCL_END
