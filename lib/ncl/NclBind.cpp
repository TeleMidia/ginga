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

GINGA_NAMESPACE_END
