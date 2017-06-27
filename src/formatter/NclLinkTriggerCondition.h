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

#include "NclLinkCondition.h"
#include "NclLinkStatement.h"

#include "ncl/Bind.h"
using namespace ::ginga::ncl;

#include "NclEvents.h"

GINGA_FORMATTER_BEGIN

class NclLinkTriggerListener
{
public:
  enum
  {
    CONDITION_SATISFIED,
    EVALUATION_STARTED,
    EVALUATION_ENDED
  };

  virtual ~NclLinkTriggerListener () {}
  virtual void conditionSatisfied (NclLinkCondition *condition) = 0;
  virtual void evaluationStarted () = 0;
  virtual void evaluationEnded () = 0;
};

class NclLinkTriggerCondition : public NclLinkCondition
{
protected:
  NclLinkTriggerListener *_listener;
  GingaTime _delay;

public:
  NclLinkTriggerCondition ();
  virtual ~NclLinkTriggerCondition () {}

protected:
  virtual void notifyConditionObservers (short satus);

public:
  void setTriggerListener (NclLinkTriggerListener *listener);
  NclLinkTriggerListener *getTriggerListener ();
  GingaTime getDelay ();
  void setDelay (GingaTime);
  virtual void conditionSatisfied (NclLinkCondition *condition);
};

class NclLinkCompoundTriggerCondition : public NclLinkTriggerCondition,
                                        public NclLinkTriggerListener
{
protected:
  vector<NclLinkCondition *> conditions;

public:
  NclLinkCompoundTriggerCondition ();
  virtual ~NclLinkCompoundTriggerCondition ();
  virtual void addCondition (NclLinkCondition *condition);
  virtual void conditionSatisfied (NclLinkCondition *condition);
  virtual vector<NclEvent *> getEvents ();

  void evaluationStarted ();
  void evaluationEnded ();
};

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
  vector<NclEvent *> getEvents ();
};

class NclLinkTransitionTriggerCondition : public NclLinkTriggerCondition,
                                          public INclEventListener
{
protected:
  NclEvent *event;
  EventStateTransition transition;
  Bind *bind;

public:
  NclLinkTransitionTriggerCondition (NclEvent *event,
                                     EventStateTransition transition,
                                     Bind *bind);

  virtual ~NclLinkTransitionTriggerCondition ();

  Bind *getBind ();

  virtual void eventStateChanged (NclEvent *event,
                                  EventStateTransition transition,
                                  EventState previousState) override;

  NclEvent *getEvent ();
  EventStateTransition getTransition ();
  virtual vector<NclEvent *> getEvents () override;
};

GINGA_FORMATTER_END

#endif // NCL_LINK_TRIGGER_CONDITION
