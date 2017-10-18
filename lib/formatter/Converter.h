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

#ifndef CONVERTER_H
#define CONVERTER_H

#include "ExecutionObject.h"
#include "ExecutionObjectContext.h"
#include "ExecutionObjectSettings.h"
#include "ExecutionObjectSwitch.h"
#include "GingaInternal.h"
#include "NclAction.h"
#include "NclCondition.h"
#include "NclEvents.h"
#include "NclLink.h"
#include "RuleAdapter.h"

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

class GingaInternal;

GINGA_FORMATTER_BEGIN

class Scheduler;
class Converter
{
public:
  explicit Converter (GingaInternal *, RuleAdapter *);
  virtual ~Converter ();

  ExecutionObject *
  processExecutionObjectSwitch (ExecutionObjectSwitch *);

private:
  void resolveSwitchEvents (ExecutionObjectSwitch *);
  NclEvent *createEvent (Bind *);
  bool getBindKey (Bind *, string *);

  // INSANITY ABOVE --------------------------------------------------------
public:
  ExecutionObject *obtainExecutionObject (Node *);

private:
  GingaInternal *_ginga;
  Scheduler *_scheduler;
  RuleAdapter *_ruleAdapter;

  NclLink *createLink (Link *);
  NclCondition *createCondition (Condition *, Bind *);
  NclAction *createAction (Action *, Bind *);
};

GINGA_FORMATTER_END

#endif // CONVERTER_H
