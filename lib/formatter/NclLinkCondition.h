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

#ifndef NCL_LINK_TRIGGER_CONDITION
#define NCL_LINK_TRIGGER_CONDITION

#include "NclEvents.h"

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class NclLinkCondition;
class NclLinkTriggerListener
{
public:
  virtual void conditionSatisfied () = 0;
};

class NclLinkCondition
{
public:
  NclLinkCondition () {}
  virtual ~NclLinkCondition () {}
  virtual vector<NclEvent *> getEvents () = 0;
};

class NclLinkTriggerCondition : public NclLinkCondition
{
  PROPERTY (NclLinkTriggerListener *, _listener, getTriggerListener,
            setTriggerListener);
  PROPERTY (GingaTime, _delay, getDelay, setDelay);

public:
  NclLinkTriggerCondition ();
  virtual ~NclLinkTriggerCondition () {}
  void conditionSatisfied ();
};

class NclLinkCompoundTriggerCondition : public NclLinkTriggerCondition,
                                        public NclLinkTriggerListener
{
public:
  NclLinkCompoundTriggerCondition ();
  virtual ~NclLinkCompoundTriggerCondition ();

  virtual void conditionSatisfied ();
  virtual void addCondition (NclLinkCondition *condition);
  virtual vector<NclEvent *> getEvents ();

protected:
  vector<NclLinkCondition *> _conditions;
};

class NclLinkTransitionTriggerCondition : public NclLinkTriggerCondition,
                                          public INclEventListener
{
public:
  NclLinkTransitionTriggerCondition (NclEvent *ev,
                                     EventStateTransition trans);

  virtual ~NclLinkTransitionTriggerCondition ();

  virtual void eventStateChanged (NclEvent *_event,
                                  EventStateTransition _transition,
                                  EventState previousState) override;

  virtual vector<NclEvent *> getEvents () override;

protected:
  NclEvent *_event;
  EventStateTransition _transition;
  Bind *_bind;
};

GINGA_FORMATTER_END

#endif // NCL_LINK_TRIGGER_CONDITION
