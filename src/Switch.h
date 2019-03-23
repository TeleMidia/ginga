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

#ifndef GINGA_SWITCH_H
#define GINGA_SWITCH_H

#include "Composition.h"
#include "StateMachine.h"

GINGA_NAMESPACE_BEGIN

class Switch : public Composition
{
public:
  explicit Switch (Document *doc,
                   const string &id);
  ~Switch ();

  // TODO ------------------------------------------------------------------

  // Switch:
  const list<pair<Object *, Predicate *> > *getRules ();
  void addRule (Object *, Predicate *);
  const map<string, list<StateMachine *> > *getSwitchPorts ();
  void addSwitchPort (const string &, const list<StateMachine *> &);

private:
  map<string, list<StateMachine *> > _switchPorts;
  list<pair<Object *, Predicate *> > _rules;
  Object *_selected;
};

GINGA_NAMESPACE_END

#endif // GINGA_SWITCH_H
