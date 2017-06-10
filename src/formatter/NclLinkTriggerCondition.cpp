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

#include "ginga.h"
#include "NclLinkTriggerCondition.h"

#include "mb/Display.h"
using namespace ginga::mb;

GINGA_FORMATTER_BEGIN

vector<ConditionStatus *> NclLinkTriggerCondition::notes;

bool NclLinkTriggerCondition::initialized = false;
bool NclLinkTriggerCondition::running = false;

NclLinkTriggerCondition::NclLinkTriggerCondition () : NclLinkCondition ()
{
  listener = NULL;
  delay = 0.0;

  if (!initialized)
    {
      initialized = true;
    }

  typeSet.insert ("NclLinkTriggerCondition");
}

NclLinkTriggerCondition::~NclLinkTriggerCondition ()
{

  if (this->running)
    g_assert (Ginga_Display->unregisterEventListener (this));
  this->listener = NULL;
}

void
NclLinkTriggerCondition::setTriggerListener (
    NclLinkTriggerListener *listener)
{
  this->listener = listener;
}

NclLinkTriggerListener *
NclLinkTriggerCondition::getTriggerListener ()
{
  return listener;
}

GingaTime
NclLinkTriggerCondition::getDelay ()
{
  return delay;
}

void
NclLinkTriggerCondition::setDelay (GingaTime delay)
{
  this->delay = delay;
}

void
NclLinkTriggerCondition::conditionSatisfied (arg_unused (NclLinkCondition *condition))
{
  if (delay > 0)
    ERROR_NOT_IMPLEMENTED ("condition delays are not supported");
  notifyConditionObservers (NclLinkTriggerListener::CONDITION_SATISFIED);
}

void
NclLinkTriggerCondition::notifyConditionObservers (short status)
{
  ConditionStatus *data;

  if (!running)
    {
      g_assert (Ginga_Display->registerEventListener (this));
      running = true;
    }

  data = new ConditionStatus;
  data->listener = listener;
  data->status = status;
  data->condition = this;
  notes.push_back (data);
}

void
NclLinkTriggerCondition::handleTickEvent (arg_unused (GingaTime total),
                                          arg_unused (GingaTime elapsed),
                                          arg_unused (int frameno))
{
  ConditionStatus *data;
  NclLinkTriggerListener *listener;
  NclLinkCondition *cond;
  short status;

  if (!this->running)
    goto unregister;

  if (this->notes.empty ())
    goto unregister;

  data = *notes.begin ();
  notes.erase (notes.begin ());

  listener = data->listener;
  g_assert_nonnull (listener);
  cond = data->condition;
  g_assert_nonnull (cond);
  status = data->status;

  switch (status)
    {
    case NclLinkTriggerListener::CONDITION_SATISFIED:
      listener->conditionSatisfied (cond);
      break;

    case NclLinkTriggerListener::EVALUATION_STARTED:
      listener->evaluationStarted ();
      break;

    case NclLinkTriggerListener::EVALUATION_ENDED:
      listener->evaluationEnded ();
      break;

    default:
      g_assert_not_reached ();
    }

  delete data;
  if (notes.empty ())
    this->running = false;
  return;

 unregister:
  g_assert (Ginga_Display->unregisterEventListener (this));
  return;
}

GINGA_FORMATTER_END
