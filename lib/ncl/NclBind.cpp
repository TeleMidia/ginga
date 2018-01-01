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
#include "NclBind.h"

#include "NclComposition.h"

GINGA_NAMESPACE_BEGIN

NclBind::NclBind (const string &role, NclBind::RoleType roleType,
                  Event::Type eventType,
                  Event::Transition transition,
                  Predicate *predicate,
                  NclNode *node, NclAnchor *iface)
{
  _role = role;
  _roleType = roleType;
  _eventType = eventType;
  _transition = transition;
  _predicate = predicate;
  _node = node;
  _interface = iface;
}

NclBind::~NclBind ()
{
}

string
NclBind::getRole ()
{
  return _role;
}

NclBind::RoleType
NclBind::getRoleType ()
{
  return _roleType;
}

Event::Type
NclBind::getEventType ()
{
  return _eventType;
}

Event::Transition
NclBind::getTransition ()
{
  return _transition;
}

Predicate *
NclBind::getPredicate ()
{
  return _predicate;
}

NclNode *
NclBind::getNode ()
{
  return _node;
}

NclAnchor *
NclBind::getInterface ()
{
  return _interface;
}

const map<string, string> *
NclBind::getParameters ()
{
  return &_parameters;
}

bool
NclBind::getParameter (const string &name, string *value)
{
  MAP_GET_IMPL (_parameters, name, value);
}

bool
NclBind::setParameter (const string &name, const string &value)
{
  MAP_SET_IMPL (_parameters, name, value);
}

bool
NclBind::isReserved (const string &role,
                     Event::Type *type,
                     Event::Transition *trans)
{
  static map<string, pair<int,int>> reserved =
    {
     {"onBegin",
      {(int) Event::PRESENTATION,
       (int) Event::START}},
     {"onEnd",
      {(int) Event::PRESENTATION,
       (int) Event::STOP}},
     {"onAbort",
      {(int) Event::PRESENTATION,
       (int) Event::ABORT}},
     {"onPause",
      {(int) Event::PRESENTATION,
       (int) Event::PAUSE}},
     {"onResumes",
      {(int) Event::PRESENTATION,
       (int) Event::RESUME}},
     {"onBeginAttribution",
      {(int) Event::ATTRIBUTION,
       (int) Event::START}},
     {"onEndAttribution",
      {(int) Event::SELECTION,
       (int) Event::STOP}},
     {"onSelection",
      {(int) Event::SELECTION,
       (int) Event::START}},
     {"start",
      {(int) Event::Type::PRESENTATION,
       (int) Event::Transition::START}},
     {"stop",
      {(int) Event::Type::PRESENTATION,
       (int) Event::Transition::STOP}},
     {"abort",
      {(int) Event::Type::PRESENTATION,
       (int) Event::Transition::ABORT}},
     {"pause",
      {(int) Event::Type::PRESENTATION,
       (int) Event::Transition::PAUSE}},
     {"resume",
      {(int) Event::Type::PRESENTATION,
       (int) Event::Transition::RESUME}},
     {"set",
      {(int) Event::Type::ATTRIBUTION,
       (int) Event::Transition::START}},
    };
  map<string, pair<int,int>>::iterator it;
  if ((it = reserved.find (role)) == reserved.end ())
    return false;
  tryset (type, (Event::Type) it->second.first);
  tryset (trans, (Event::Transition) it->second.second);
  return true;
}

GINGA_NAMESPACE_END
