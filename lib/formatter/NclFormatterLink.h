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

#ifndef FORMATTER_LINK_H
#define FORMATTER_LINK_H

#include "NclAction.h"
#include "NclEvents.h"
#include "NclFormatterLink.h"
#include "NclLinkCondition.h"

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class ExecutionObjectContext;

class NclFormatterLink: public NclLinkTriggerListener
{
public:
  NclFormatterLink (Link *, ExecutionObjectContext *);
  virtual ~NclFormatterLink ();

  void suspendLinkEvaluation (bool suspended);
  Link *getNcmLink ();

  const vector <NclAction *> *getActions ();
  void addAction (NclAction *);

  const vector <NclLinkTriggerCondition *> *getConditions ();
  void addCondition (NclLinkTriggerCondition *);

  void conditionSatisfied ();

  virtual vector<NclEvent *> getEvents ();
  void evaluationStarted ();
  void evaluationEnded ();

protected:
  Link *_ncmLink;
  bool _suspended;
  ExecutionObjectContext *_parentObj;

private:
  vector <NclLinkTriggerCondition *> _conditions;
  vector <NclAction *> _actions;
};

GINGA_FORMATTER_END

#endif //_FORMATTERLINK_H_
