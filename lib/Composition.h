/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef COMPOSITION_H
#define COMPOSITION_H

#include "Object.h"

GINGA_NAMESPACE_BEGIN

class Composition : public Object
{
public:
  explicit Composition (const string &);
  virtual ~Composition () = 0;

  const set<Object *> *getChildren ();
  Object *getChildById (const string &);
  Object *getChildByIdOrAlias (const string &);
  void addChild (Object *);

protected:
  set<Object *> _children;
};

GINGA_NAMESPACE_END

#endif // COMPOSITION_H
