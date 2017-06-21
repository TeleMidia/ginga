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

#ifndef LINKANDCOMPOUNDTRIGGERCONDITION_H_
#define LINKANDCOMPOUNDTRIGGERCONDITION_H_

#include "NclLinkCondition.h"
#include "NclLinkStatement.h"
#include "NclLinkTriggerCondition.h"
#include "NclLinkTriggerListener.h"
#include "NclLinkCompoundTriggerCondition.h"

GINGA_FORMATTER_BEGIN

class NclLinkAndCompoundTriggerCondition
    : public NclLinkCompoundTriggerCondition
{
private:
  vector<NclLinkCondition *> unsatisfiedConditions;
  vector<NclLinkCondition *> statements;

public:
  NclLinkAndCompoundTriggerCondition ();
  virtual ~NclLinkAndCompoundTriggerCondition ();
  void addCondition (NclLinkCondition *condition);
  void conditionSatisfied (NclLinkCondition *condition);
  vector<NclEvent *> *getEvents ();
};

GINGA_FORMATTER_END

#endif /*LINKANDCOMPOUNDTRIGGERCONDITION_H_*/
