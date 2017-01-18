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

#ifndef _FORMATTERCAUSALLINK_H_
#define _FORMATTERCAUSALLINK_H_

#include "ncl/Link.h"
using namespace ::ginga::ncl;

#include "NclFormatterEvent.h"
using namespace ::ginga::formatter;

#include "NclLinkAction.h"
#include "NclLinkActionProgressionListener.h"
#include "NclLinkTriggerCondition.h"
#include "NclLinkTriggerListener.h"
#include "NclFormatterLink.h"

GINGA_FORMATTER_BEGIN

class NclFormatterCausalLink : public NclFormatterLink,
                            public NclLinkTriggerListener,
                            public NclLinkActionProgressionListener
{

private:
  NclLinkTriggerCondition *condition;
  NclLinkAction *action;

public:
  NclFormatterCausalLink (NclLinkTriggerCondition *condition, NclLinkAction *action,
                       Link *ncmLink, void *parentObject);

  virtual ~NclFormatterCausalLink ();

  NclLinkAction *getAction ();
  NclLinkTriggerCondition *getTriggerCondition ();
  void conditionSatisfied (void *condition);
  virtual vector<NclFormatterEvent *> *getEvents ();
  void evaluationStarted ();
  void evaluationEnded ();
  void actionProcessed (bool start);
};

GINGA_FORMATTER_END
#endif //_FORMATTERCAUSALLINK_H_
