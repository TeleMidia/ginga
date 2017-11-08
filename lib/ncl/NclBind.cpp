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

#include "aux-ginga.h"
#include "NclBind.h"

#include "NclComposition.h"

GINGA_NAMESPACE_BEGIN

NclBind::NclBind (const string &role, NclBind::RoleType roleType,
                  FormatterEvent::Type eventType,
                  FormatterEvent::Transition transition,
                  FormatterPredicate *predicate,
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

FormatterEvent::Type
NclBind::getEventType ()
{
  return _eventType;
}

FormatterEvent::Transition
NclBind::getTransition ()
{
  return _transition;
}

FormatterPredicate *
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
                     FormatterEvent::Type *type,
                     FormatterEvent::Transition *trans)
{
  static map<string, pair<int,int>> reserved =
    {
     {"onBegin",
      {(int) FormatterEvent::PRESENTATION,
       (int) FormatterEvent::START}},
     {"onEnd",
      {(int) FormatterEvent::PRESENTATION,
       (int) FormatterEvent::STOP}},
     {"onAbort",
      {(int) FormatterEvent::PRESENTATION,
       (int) FormatterEvent::ABORT}},
     {"onPause",
      {(int) FormatterEvent::PRESENTATION,
       (int) FormatterEvent::PAUSE}},
     {"onResumes",
      {(int) FormatterEvent::PRESENTATION,
       (int) FormatterEvent::RESUME}},
     {"onBeginAttribution",
      {(int) FormatterEvent::ATTRIBUTION,
       (int) FormatterEvent::START}},
     {"onEndAttribution",
      {(int) FormatterEvent::SELECTION,
       (int) FormatterEvent::STOP}},
     {"onSelection",
      {(int) FormatterEvent::SELECTION,
       (int) FormatterEvent::START}},
     {"start",
      {(int) FormatterEvent::Type::PRESENTATION,
       (int) FormatterEvent::Transition::START}},
     {"stop",
      {(int) FormatterEvent::Type::PRESENTATION,
       (int) FormatterEvent::Transition::STOP}},
     {"abort",
      {(int) FormatterEvent::Type::PRESENTATION,
       (int) FormatterEvent::Transition::ABORT}},
     {"pause",
      {(int) FormatterEvent::Type::PRESENTATION,
       (int) FormatterEvent::Transition::PAUSE}},
     {"resume",
      {(int) FormatterEvent::Type::PRESENTATION,
       (int) FormatterEvent::Transition::RESUME}},
     {"set",
      {(int) FormatterEvent::Type::ATTRIBUTION,
       (int) FormatterEvent::Transition::START}},
    };
  map<string, pair<int,int>>::iterator it;
  if ((it = reserved.find (role)) == reserved.end ())
    return false;
  tryset (type, (FormatterEvent::Type) it->second.first);
  tryset (trans, (FormatterEvent::Transition) it->second.second);
  return true;
}

GINGA_NAMESPACE_END
