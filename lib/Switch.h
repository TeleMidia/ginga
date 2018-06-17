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

#ifndef SWITCH_H
#define SWITCH_H

#include "Composition.h"
#include "Event.h"

GINGA_NAMESPACE_BEGIN

class Switch : public Composition
{
public:
  explicit Switch (const string &);
  ~Switch ();

  // Object:
  string getObjectTypeAsString () override;
  string toString () override;
  bool beforeTransition (Event *, Event::Transition) override;
  bool afterTransition (Event *, Event::Transition) override;

  // Switch:
  const list<pair<Object *, Predicate *> > *getRules ();
  void addRule (Object *, Predicate *);
  const map<string, list<Event *> > *getSwitchPorts ();
  void addSwitchPort (const string &, const list<Event *> &);

private:
  map<string, list<Event *> > _switchPorts; ///< List of switchPorts.
  list<pair<Object *, Predicate *> > _rules;
  Object *_selected;
};

GINGA_NAMESPACE_END

#endif // SWITCH_H
