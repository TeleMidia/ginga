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

const map<string, list<Event *> > *
Switch::getSwitchPorts ()
{
  return &_switchPorts;
}

void
Switch::addSwitchPort (const string &id, const list<Event *> &evts)
{
  Event *evt;

  evt = this->createEvent (Event::PRESENTATION, id);
  g_assert_nonnull (evt);
  evt->setLabel (id);

  TRACE ("Adding switchPort %s to %s mapping %u evts.", id.c_str (),
         getId ().c_str (), (guint) evts.size ());

  _switchPorts[id] = evts;
}

GINGA_NAMESPACE_END
