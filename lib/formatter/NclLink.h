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

#ifndef NCL_LINK_H
#define NCL_LINK_H

#include "NclAction.h"
#include "NclCondition.h"
#include "NclEvents.h"

GINGA_FORMATTER_BEGIN

class NclLink: public INclConditionListener
{
public:
  NclLink (GingaInternal *);
  virtual ~NclLink ();

  const vector <NclCondition *> *getConditions ();
  bool addCondition (NclCondition *);

  const vector <NclAction *> *getActions ();
  bool addAction (NclAction *);

  virtual vector<NclEvent *> getEvents ();
  void disable (bool);

  // INclConditionListener
  void conditionSatisfied ();

private:
  GingaInternal *_ginga;               // ginga handle
  Scheduler *_scheduler;               // scheduler
  vector <NclCondition *> _conditions; // list of conditions
  vector <NclAction *> _actions;       // list of actions
  bool _disabled;                      // whether link is disabled
};

GINGA_FORMATTER_END

#endif // NCL_LINK_H
