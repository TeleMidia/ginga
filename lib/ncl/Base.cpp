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
#include "Base.h"

GINGA_NCL_BEGIN


// Public.

/**
 * @brief Creates a new base.
 * @param ncl Parent document.
 * @param id Base id.
 */
Base::Base (NclDocument *ncl, const string &id) : Entity (ncl, id)
{
}

/**
 * @brief Destroys base.
 */
Base::~Base ()
{
  _bases.clear ();
  _entities.clear ();
  _aliases.clear ();
  _locations.clear ();
}

/**
 * @brief Adds child base.
 * @param base Child base.
 * @param alias Base alias.
 * @param location Base location.
 */
void
Base::addBase (Base *base, const string &alias, const string &location)
{
  g_assert_nonnull (base);
  _bases.push_back (base);
  g_assert (alias != "");
  _aliases[alias] = base;
  g_assert (location != "");
  _locations[location] = base;
}


// Protected.

void
Base::addEntity (Entity *entity)
{
  g_assert_nonnull (entity);
  _entities.push_back (entity);
}

Entity *
Base::getEntity (const string &id)
{
  Base *base;
  string suffix;

  if ((base = this->getHashBase (id, nullptr, &suffix)) != nullptr)
    return base->getEntity (suffix);

  for (auto entity: _entities)
    if (entity->getId () == id)
      return entity;

  return nullptr;
}

Base *
Base::getHashBase (const string &id, string *prefixp, string *suffixp)
{
  string::size_type index;
  string prefix, suffix;
  Base *base;

  index = id.find_first_of ("#");
  if (index == string::npos)
    return nullptr;

  base = nullptr;
  prefix = id.substr (0, index++);
  suffix = id.substr (index, id.length () - index);
  if (_aliases.find (prefix) != _aliases.end ())
    {
      base = _aliases[prefix];  // found
    }
  else if (_locations.find (prefix) != _locations.end ())
    {
      base = _locations[prefix]; // found
    }

  tryset (prefixp, prefix);
  tryset (suffixp, suffix);
  return base;
}

GINGA_NCL_END
