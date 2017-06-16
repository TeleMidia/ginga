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

set<Base *> Base::_baseInstances;

Base::Base (const string &id)
{
  this->_id = id;

  _typeSet.insert ("Base");
  _baseInstances.insert (this);
}

Base::~Base ()
{
  vector<Base *>::iterator i;
  set<Base *>::iterator j;
  Base *childBase;

  j = _baseInstances.find (this);
  if (j != _baseInstances.end ())
    {
      _baseInstances.erase (j);
    }

  i = _baseSet.begin ();
  while (i != _baseSet.end ())
    {
      childBase = *i;
      if (hasInstance (childBase, true))
        {
          delete childBase;
        }
      _baseSet.erase (i);
      i = _baseSet.begin ();
    }
}

bool
Base::hasInstance (Base *instance, bool eraseFromList)
{
  set<Base *>::iterator i;
  bool hasBase = false;

  i = _baseInstances.find (instance);
  if (i != _baseInstances.end ())
    {
      if (eraseFromList)
        {
          _baseInstances.erase (i);
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
  for (i = _baseSet.begin (); i != _baseSet.end (); ++i)
    {
      if (*i == base)
        {
          return false;
        }
    }

  _baseSet.push_back (base);

  if (alias != "")
    {
      _baseAliases[alias] = base;
    }

  _baseLocations[location] = base;
  return true;
}

void
Base::clear ()
{
  _baseSet.clear ();
  _baseAliases.clear ();
  _baseLocations.clear ();
}

Base *
Base::getBase (const string &baseId)
{
  vector<Base *>::iterator i;
  for (i = _baseSet.begin (); i != _baseSet.end (); ++i)
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
  for (i = _baseAliases.begin (); i != _baseAliases.end (); ++i)
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
  for (i = _baseLocations.begin (); i != _baseLocations.end (); ++i)
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
  if (_baseSet.empty ())
    {
      return NULL;
    }

  return new vector<Base *> (_baseSet);
}

bool
Base::removeBase (Base *base)
{
  string alias, location;

  alias = getBaseAlias (base);
  location = getBaseLocation (base);

  vector<Base *>::iterator i;
  for (i = _baseSet.begin (); i != _baseSet.end (); ++i)
    {
      if (*i == base)
        {
          _baseSet.erase (i);
          if (alias != "")
            {
              _baseAliases.erase (alias);
            }
          _baseLocations.erase (location);
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
      _baseAliases.erase (oldAlias);
    }

  if (alias != "")
    {
      _baseAliases[alias] = base;
    }
}

void
Base::setBaseLocation (Base *base, const string &location)
{
  string oldLocation;

  oldLocation = getBaseLocation (base);
  if (oldLocation == "")
    return;

  _baseLocations.erase (oldLocation);
  _baseLocations[location] = base;
}

string
Base::getId ()
{
  return _id;
}

void
Base::setId (const string &id)
{
  this->_id = id;
}

bool
Base::instanceOf (const string &s)
{
  if (!_typeSet.empty ())
    {
      return (_typeSet.find (s) != _typeSet.end ());
    }
  else
    {
      return false;
    }
}

GINGA_NCL_END
