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
#include "RegionBase.h"

GINGA_NCL_BEGIN

RegionBase::RegionBase (const string &id) : Base (id)
{
  deviceRegion = new LayoutRegion ("");
  typeSet.insert ("RegionBase");
}

RegionBase::~RegionBase ()
{
  map<string, LayoutRegion *>::iterator i;

  regions.clear ();

  if (deviceRegion != NULL)
    {
      delete deviceRegion;
      deviceRegion = NULL;
    }
}

bool
RegionBase::addRegion (LayoutRegion *region)
{
  map<string, LayoutRegion *>::iterator i;
  string regId;

  regId = region->getId ();
  i = regions.find (regId);
  if (i != regions.end ())
    return false;               // duplicated id

  regions[regId] = region;
  return true;
}

bool
RegionBase::addBase (Base *base, const string &alias, const string &location)
{
  g_assert (base->instanceOf ("RegionBase"));
  return Base::addBase (base, alias, location);
}

LayoutRegion *
RegionBase::getRegionLocally (const string &regionId)
{
  map<string, LayoutRegion *>::iterator childRegions;
  LayoutRegion *region;

  childRegions = regions.begin ();
  while (childRegions != regions.end ())
    {
      region = childRegions->second;
      if (region->getId () == regionId)
        {
          return region;
        }
      ++childRegions;
    }
  return NULL;
}

LayoutRegion *
RegionBase::getRegion (const string &regionId)
{
  string::size_type index;
  string prefix, suffix;
  RegionBase *base;

  index = regionId.find_first_of ("#");
  if (index == string::npos)
    {
      return getRegionLocally (regionId);
    }
  prefix = regionId.substr (0, index);
  index++;
  suffix = regionId.substr (index, regionId.length () - index);
  if (baseAliases.find (prefix) != baseAliases.end ())
    {
      base = (RegionBase *)(baseAliases[prefix]);
      return base->getRegion (suffix);
    }
  else if (baseLocations.find (prefix) != baseLocations.end ())
    {
      base = (RegionBase *)(baseLocations[prefix]);
      return base->getRegion (suffix);
    }
  else
    {
      return NULL;
    }
}

vector<LayoutRegion *> *
RegionBase::getRegions ()
{
  map<string, LayoutRegion *>::iterator i;
  vector<LayoutRegion *> *childRegions;

  childRegions = new vector<LayoutRegion *>;
  for (i = regions.begin (); i != regions.end (); ++i)
    {
      childRegions->push_back (i->second);
    }

  return childRegions;
}

bool
RegionBase::removeRegion (LayoutRegion *region)
{
  map<string, LayoutRegion *>::iterator i;

  for (i = regions.begin (); i != regions.end (); ++i)
    {
      if (i->second == region)
        {
          regions.erase (i);
          return true;
        }
    }
  return false;
}

void
RegionBase::clear ()
{
  regions.clear ();
  Base::clear ();
}

GINGA_NCL_END
