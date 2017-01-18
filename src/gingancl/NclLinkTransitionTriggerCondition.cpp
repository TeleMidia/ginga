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

#include "config.h"
#include "NclLinkTransitionTriggerCondition.h"
#include "NclExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_BEGIN

NclLinkTransitionTriggerCondition::NclLinkTransitionTriggerCondition (
    NclFormatterEvent *event, short transition, Bind *bind)
    : NclLinkTriggerCondition ()
{

  typeSet.insert ("NclLinkTransitionTriggerCondition");

  this->bind = bind;
  this->event = NULL;
  this->transition = transition;

  if (NclFormatterEvent::hasInstance (event, false))
    {
      this->event = event;
      this->event->addEventListener (this);
    }
  else
    {
      clog << "NclLinkTransitionTriggerCondition::";
      clog << "NclLinkTransitionTriggerCondition Warning! ";
      clog << "creating a link with NULL event" << endl;
    }
}

NclLinkTransitionTriggerCondition::~NclLinkTransitionTriggerCondition ()
{
  isDeleting = true;
  listener = NULL;
  bind = NULL;

  if (NclFormatterEvent::hasInstance (event, false))
    {
      event->removeEventListener (this);
      event = NULL;
    }
}

Bind *
NclLinkTransitionTriggerCondition::getBind ()
{
  return bind;
}

void
NclLinkTransitionTriggerCondition::eventStateChanged (void *someEvent,
                                                   short transition,
                                                   short previousState)
{

  if (this->transition == transition)
    {
      notifyConditionObservers (NclLinkTriggerListener::EVALUATION_STARTED);

      NclLinkTriggerCondition::conditionSatisfied (this);
    }
}

short
NclLinkTransitionTriggerCondition::getPriorityType ()
{
  return INclEventListener::PT_LINK;
}

NclFormatterEvent *
NclLinkTransitionTriggerCondition::getEvent ()
{
  return event;
}

short
NclLinkTransitionTriggerCondition::getTransition ()
{
  return transition;
}

vector<NclFormatterEvent *> *
NclLinkTransitionTriggerCondition::getEvents ()
{
  if (!NclFormatterEvent::hasInstance (event, false))
    {
      return NULL;
    }

  vector<NclFormatterEvent *> *events = new vector<NclFormatterEvent *>;

  events->push_back (event);
  return events;
}

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_END
