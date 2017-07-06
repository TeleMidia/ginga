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
  _deviceRegion = new Region (id);
}

RegionBase::~RegionBase ()
{
  map<string, Region *>::iterator i;

  _regions.clear ();

  if (_deviceRegion != NULL)
    {
      delete _deviceRegion;
      _deviceRegion = NULL;
    }
}

bool
RegionBase::addRegion (Region *region)
{
  map<string, Region *>::iterator i;
  string regId;

  regId = region->getId ();
  i = _regions.find (regId);
  if (i != _regions.end ())
    return false;               // duplicated id

  _regions[regId] = region;
  return true;
}

Region *
RegionBase::getRegionLocally (const string &regionId)
{
  map<string, Region *>::iterator childRegions;
  Region *region;

  childRegions = _regions.begin ();
  while (childRegions != _regions.end ())
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

Region *
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
  if (_aliases.find (prefix) != _aliases.end ())
    {
      base = (RegionBase *)(_aliases[prefix]);
      return base->getRegion (suffix);
    }
  else if (_locations.find (prefix) != _locations.end ())
    {
      base = (RegionBase *)(_locations[prefix]);
      return base->getRegion (suffix);
    }
  else
    {
      return NULL;
    }
}

vector<Region *> *
RegionBase::getRegions ()
{
  map<string, Region *>::iterator i;
  vector<Region *> *childRegions;

  childRegions = new vector<Region *>;
  for (i = _regions.begin (); i != _regions.end (); ++i)
    {
      childRegions->push_back (i->second);
    }

  return childRegions;
}

bool
RegionBase::removeRegion (Region *region)
{
  map<string, Region *>::iterator i;

  for (i = _regions.begin (); i != _regions.end (); ++i)
    {
      if (i->second == region)
        {
          _regions.erase (i);
          return true;
        }
    }
  return false;
}

GINGA_NCL_END
