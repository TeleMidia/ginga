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

#ifndef ENTITY_H
#define ENTITY_H

#include "ginga.h"

GINGA_NCL_BEGIN

class Entity
{
protected:
  set<string> typeSet; // type information

private:
  static set<Entity *> instances;
  static pthread_mutex_t iMutex;
  static bool initMutex;

  string id; // id=comparable unique entity Id

public:
  Entity (const string &someId);
  virtual ~Entity ();

  static bool hasInstance (Entity *instance, bool eraseFromList);
  void printHierarchy ();
  bool instanceOf (const string &s);
  int compareTo (Entity *otherEntity);
  string getId ();
  int hashCode ();

  virtual void setId (const string &someId);

  virtual string toString ();
  virtual Entity *getDataEntity ();
};

GINGA_NCL_END

#endif /* ENTITY_H */
