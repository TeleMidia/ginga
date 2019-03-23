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

#ifndef GINGA_COMPOSITION_H
#define GINGA_COMPOSITION_H

#include "Object.h"

GINGA_NAMESPACE_BEGIN

/// Composition object in an NCL document.
class Composition : public Object
{
public:

  /// Creates a new composition.
  Composition (Document *doc, const string &id);

  /// Destroys the composition.
  virtual ~Composition () = 0;

  /// Gets the set of objects in composition.
  void getChildren (set<Object *> *children);

  /// Gets the object with the given id in the composition.
  Object *getChild (const string &id);

  /// Adds object to composition.
  void addChild (Object *obj);

  /// Removes object from composition.
  void removeChild (Object *obj);
};

GINGA_NAMESPACE_END

#endif // GINGA_COMPOSITION_H
