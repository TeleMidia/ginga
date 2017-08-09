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

#include "ginga-internal.h"
#include "Link.h"

#include "AttributeAssessment.h"
#include "Port.h"
#include "SimpleAction.h"
#include "SimpleCondition.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new link.
 * @param id Link id.
 * @param ctx Parent context.
 * @param conn Associated connector.
 */
Link::Link (const string &id, Context *ctx, Connector *conn)
  : Entity (id)
{
  g_assert_nonnull (conn);
  _connector = conn;
  g_assert_nonnull (ctx);
  _context = ctx;
}

/**
 * @brief Destroys link.
 */
Link::~Link ()
{
  _parameters.clear ();
  _binds.clear ();
}

/**
 * @brief Gets link connector.
 */
Connector *
Link::getConnector ()
{
  return _connector;
}

/**
 * @brief Gets link context.
 */
Context *
Link::getContext ()
{
  return _context;
}

/**
 * @brief Adds parameter to link.
 * @param parameter Parameter.
 */
void
Link::addParameter (Parameter *parameter)
{
  g_assert_nonnull (parameter);
  _parameters.push_back (parameter);
}

/**
 * @brief Gets all link parameters.
 */
const vector<Parameter *> *
Link::getParameters ()
{
  return &_parameters;
}

/**
 * @brief Gets link parameter.
 * @param name Parameter name.
 * @return Parameter if successful, or null if not found.
 */
Parameter *
Link::getParameter (const string &name)
{
  for (auto param: _parameters)
    if (param->getName () == name)
      return param;
  return nullptr;
}

/**
 * @brief Adds bind to link.
 * @param bind Bind.
 */
void
Link::addBind (Bind *bind)
{
  g_assert_nonnull (bind);
  _binds.push_back (bind);
}

/**
 * @brief Gets all link binds.
 */
const vector<Bind *> *
Link::getBinds ()
{
  return &_binds;
}

/**
 * @brief Gets link binds.
 * @param role Role.
 * @return The binds with the given role.
 */
vector<Bind *>
Link::getBinds (Role *role)
{
  vector<Bind *> result;
  for (auto bind: _binds)
    if (bind->getRole ()->getLabel () == role->getLabel ())
      result.push_back (bind);
  return result;
}

/**
 * @brief Tests whether node is referenced by link.
 * @param node Node.
 * @param condition True if only conditions are to be checked.
 * @return True if successful, or false otherwise.
 */
bool
Link::contains (Node *node, bool condition)
{
  for (auto bind: _binds)
    {
      Anchor *iface;
      Node *bound;
      Role *role;

      role = bind->getRole ();
      if ((instanceof (SimpleCondition *, role)
           || instanceof (AttributeAssessment *, role)) && !condition)
        {
          continue;             // skip
        }

      if (instanceof (SimpleAction *, role)
          && condition)
        {
          continue;             // skip
        }

      if ((iface = bind->getInterface ()) != nullptr
          && instanceof (Port *, iface))
        {
          bound = cast (Port *, iface)->getFinalNode ();
        }
      else
        {
          bound = bind->getNode ();
        }
      g_assert_nonnull (bound);

      if (bound == node)
        return true;            // found
    }
  return false;
}

GINGA_NCL_END
