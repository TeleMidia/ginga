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

#ifndef BASE_H
#define BASE_H

#include "ginga.h"

GINGA_NCL_BEGIN

class Base
{
private:
  static set<Base *> baseInstances;

protected:
  string id;
  vector<Base *> baseSet;
  map<string, Base *> baseAliases;
  map<string, Base *> baseLocations;
  set<string> typeSet;

public:
  Base (const string &id);
  virtual ~Base ();
  static bool hasInstance (Base *base, bool eraseFromList);
  virtual bool addBase (Base *base, const string &alias, const string &location);
  virtual void clear ();
  Base *getBase (const string &baseId);
  string getBaseAlias (Base *base);
  string getBaseLocation (Base *base);
  vector<Base *> *getBases ();
  bool removeBase (Base *base);
  void setBaseAlias (Base *base, const string &alias);
  void setBaseLocation (Base *base, const string &location);
  string getId ();
  void setId (const string &id);
  bool instanceOf (const string &s);
};

GINGA_NCL_END

#endif /* BASE_H */
