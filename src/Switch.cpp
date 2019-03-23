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

#include "aux-ginga.h"
#include "Switch.h"
#include "LuaAPI.h"

#include "Document.h"

GINGA_NAMESPACE_BEGIN

// Public.

Switch::Switch (Document *doc, const string &id) : Composition (doc, id)
{
  _selected = nullptr;

  LuaAPI::Object_attachWrapper (_L, this, doc, Object::SWITCH, id);
}

Switch::~Switch ()
{
  for (auto item : _rules)
    delete item.second;

  LuaAPI::Object_detachWrapper (_L, this);
}

// Public.

const list<pair<Object *, Predicate *> > *
Switch::getRules ()
{
  return &_rules;
}

void
Switch::addRule (Object *obj, Predicate *pred)
{
  g_assert_nonnull (obj);
  g_assert_nonnull (pred);
  _rules.push_back (std::make_pair (obj, pred));
}

const map<string, list<StateMachine *> > *
Switch::getSwitchPorts ()
{
  return &_switchPorts;
}

void
Switch::addSwitchPort (const string &id,
                       const list<StateMachine *> &machines)
{
  StateMachine *sm;

  sm = this->createStateMachine (StateMachine::PRESENTATION, id);
  g_assert_nonnull (sm);
  sm->setLabel (id);
  _switchPorts[id] = machines;
}

GINGA_NAMESPACE_END
