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

#include "ginga.h"
#include "Base.h"

GINGA_NCL_BEGIN

set<Base *> Base::baseInstances;

Base::Base (const string &id)
{
  this->id = id;

  typeSet.insert ("Base");
  baseInstances.insert (this);
}

Base::~Base ()
{
  vector<Base *>::iterator i;
  set<Base *>::iterator j;
  Base *childBase;

  j = baseInstances.find (this);
  if (j != baseInstances.end ())
    {
      baseInstances.erase (j);
    }

  i = baseSet.begin ();
  while (i != baseSet.end ())
    {
      childBase = *i;
      if (hasInstance (childBase, true))
        {
          delete childBase;
        }
      baseSet.erase (i);
      i = baseSet.begin ();
    }
}

bool
Base::hasInstance (Base *instance, bool eraseFromList)
{
  set<Base *>::iterator i;
  bool hasBase = false;

  i = baseInstances.find (instance);
  if (i != baseInstances.end ())
    {
      if (eraseFromList)
        {
          baseInstances.erase (i);
        }
      hasBase = true;
    }

  return hasBase;
}

bool
Base::addBase (Base *base, const string &alias, const string &location)
{
  if (base == NULL || location == "")
    {
      return false;
    }

  vector<Base *>::iterator i;
  for (i = baseSet.begin (); i != baseSet.end (); ++i)
    {
      if (*i == base)
        {
          return false;
        }
    }

  baseSet.push_back (base);

  if (alias != "")
    {
      baseAliases[alias] = base;
    }

  baseLocations[location] = base;
  return true;
}

void
Base::clear ()
{
  baseSet.clear ();
  baseAliases.clear ();
  baseLocations.clear ();
}

Base *
Base::getBase (const string &baseId)
{
  vector<Base *>::iterator i;
  for (i = baseSet.begin (); i != baseSet.end (); ++i)
    {
      if ((*i)->getId () == baseId)
        {
          return *i;
        }
    }
  return NULL;
}

string
Base::getBaseAlias (Base *base)
{
  map<string, Base *>::iterator i;
  for (i = baseAliases.begin (); i != baseAliases.end (); ++i)
    {
      if (i->second == base)
        {
          return i->first;
        }
    }
  return "";
}

string
Base::getBaseLocation (Base *base)
{
  map<string, Base *>::iterator i;
  for (i = baseLocations.begin (); i != baseLocations.end (); ++i)
    {
      if (i->second == base)
        {
          return i->first;
        }
    }
  return "";
}

vector<Base *> *
Base::getBases ()
{
  if (baseSet.empty ())
    {
      return NULL;
    }

  return new vector<Base *> (baseSet);
}

bool
Base::removeBase (Base *base)
{
  string alias, location;

  alias = getBaseAlias (base);
  location = getBaseLocation (base);

  vector<Base *>::iterator i;
  for (i = baseSet.begin (); i != baseSet.end (); ++i)
    {
      if (*i == base)
        {
          baseSet.erase (i);
          if (alias != "")
            {
              baseAliases.erase (alias);
            }
          baseLocations.erase (location);
          return true;
        }
    }
  return false;
}

void
Base::setBaseAlias (Base *base, const string &alias)
{
  string oldAlias;
  oldAlias = getBaseAlias (base);

  if (oldAlias != "")
    {
      baseAliases.erase (oldAlias);
    }

  if (alias != "")
    {
      baseAliases[alias] = base;
    }
}

void
Base::setBaseLocation (Base *base, const string &location)
{
  string oldLocation;

  oldLocation = getBaseLocation (base);
  if (oldLocation == "")
    return;

  baseLocations.erase (oldLocation);
  baseLocations[location] = base;
}

string
Base::getId ()
{
  return id;
}

void
Base::setId (const string &id)
{
  this->id = id;
}

bool
Base::instanceOf (const string &s)
{
  if (!typeSet.empty ())
    {
      return (typeSet.find (s) != typeSet.end ());
    }
  else
    {
      return false;
    }
}

GINGA_NCL_END
