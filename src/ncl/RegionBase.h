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

#ifndef REGIONBASE_H_
#define REGIONBASE_H_

#include "Base.h"

#include "LayoutRegion.h"

GINGA_NCL_BEGIN

class RegionBase : public Base
{
private:
  map<string, LayoutRegion *> regions;
  LayoutRegion *deviceRegion;

public:
  RegionBase (const string &id);
  virtual ~RegionBase ();

private:
  void createDeviceRegion ();

public:
  bool addRegion (LayoutRegion *region);
  virtual bool addBase (Base *base, const string &alias, const string &location);

private:
  LayoutRegion *getRegionLocally (const string &regionId);

public:
  LayoutRegion *getRegion (const string &regionId);
  vector<LayoutRegion *> *getRegions ();
  bool removeRegion (LayoutRegion *region);
  string getDevice ();
  void setDevice (const string &device, const string &regionId);
  int getDeviceClass ();
  void clear ();
};

GINGA_NCL_END

#endif /*REGIONBASE_H_*/
