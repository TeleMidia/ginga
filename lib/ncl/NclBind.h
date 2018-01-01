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

#ifndef NCL_BIND_H
#define NCL_BIND_H

#include "NclNode.h"
#include "Event.h"
#include "Predicate.h"

GINGA_NAMESPACE_BEGIN

class NclBind
{
public:
  enum RoleType
    {
     CONDITION = 0,
     ACTION
    };

  NclBind (const string &, NclBind::RoleType, Event::Type,
           Event::Transition, Predicate *,
           NclNode *, NclAnchor *);
  ~NclBind ();

  string getRole ();
  NclBind::RoleType getRoleType ();
  Event::Type getEventType ();
  Event::Transition getTransition ();
  Predicate *getPredicate ();

  NclNode *getNode ();
  NclAnchor *getInterface ();

  const map<string, string> *getParameters ();
  bool getParameter (const string &, string *);
  bool setParameter (const string &, const string &);

  static bool isReserved (const string &,
                          Event::Type *,
                          Event::Transition *);
private:
  string _role;
  NclBind::RoleType _roleType;
  Event::Type _eventType;
  Event::Transition _transition;
  Predicate *_predicate;
  NclNode *_node;
  NclAnchor *_interface;
  map<string, string> _parameters;
};

GINGA_NAMESPACE_END

#endif // NCL_BIND_H
