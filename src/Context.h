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

#ifndef GINGA_CONTEXT_H
#define GINGA_CONTEXT_H

#include "Composition.h"
#include "Event.h"

GINGA_NAMESPACE_BEGIN

/// Context in an NCL document.
class Context : public Composition
{
public:

  /// Creates a new context.
  Context (Document *doc, const string &id);

  /// Destroys context.
  ~Context ();

  /// Gets the list of ports of context.
  void getPorts (list<Event *> *ports);

  /// Adds event to the list of ports of context.
  bool addPort (Event *);

  /// Removes event from the list of ports of context.
  bool removePort (Event *);

  // TODO ------------------------------------------------------------------

  const list<pair<list<Action>, list<Action> > > *getLinks ();
  void addLink (list<Action>, list<Action>);

private:
  list<pair<list<Action>, list<Action> > > _links;
};

GINGA_NAMESPACE_END

#endif // GINGA_CONTEXT_H
