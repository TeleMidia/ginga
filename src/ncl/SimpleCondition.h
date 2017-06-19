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

#ifndef _SIMPLECONDITION_H_
#define _SIMPLECONDITION_H_

#include "EventUtil.h"
#include "TriggerExpression.h"
#include "Role.h"

GINGA_NCL_BEGIN

class SimpleCondition : public TriggerExpression, public Role
{
public:
  static const short NO_QUALIFIER = -1;
  SimpleCondition (const string &role);
  virtual ~SimpleCondition (){}

  void setLabel (const string &id);
  string getKey ();
  void setKey (const string &_key);
  EventStateTransition getTransition ();
  void setTransition (EventStateTransition transition);
  short getQualifier ();
  void setQualifier (short _qualifier);
  bool
  instanceOf (const string &type)
  {
    return TriggerExpression::instanceOf (type);
  }

private:
  string _key;
  EventStateTransition _transition;
  short _qualifier;
};

GINGA_NCL_END

#endif //_SIMPLECONDITION_H_
