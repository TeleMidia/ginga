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

GINGA_FORMATTER_BEGIN

pthread_mutex_t NclLinkTriggerCondition::sMutex;
vector<ConditionStatus *> NclLinkTriggerCondition::notes;

bool NclLinkTriggerCondition::initialized = false;
bool NclLinkTriggerCondition::running = false;

NclLinkTriggerCondition::NclLinkTriggerCondition ()
    : NclLinkCondition (), Thread ()
{
  listener = NULL;
  delay = 0.0;

  if (!initialized)
    {
      initialized = true;
      Thread::mutexInit (&sMutex, false);
    }

  typeSet.insert ("NclLinkTriggerCondition");
}

NclLinkTriggerCondition::~NclLinkTriggerCondition ()
{
  isDeleting = true;

  Thread::mutexLock (&sMutex);
  listener = NULL;
  Thread::mutexUnlock (&sMutex);
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

double
NclLinkTriggerCondition::getDelay ()
{
  return delay;
}

void
NclLinkTriggerCondition::setDelay (double delay)
{
  if (delay < 0)
    {
      this->delay = 0;
    }
  else
    {
      this->delay = delay;
    }
}

void
NclLinkTriggerCondition::conditionSatisfied (arg_unused (void *condition))
{
  if (delay > 0)
    {
      // HELL: Thread::startThread ();
      this->run ();
    }
  else
    {
      notifyConditionObservers (
          NclLinkTriggerListener::CONDITION_SATISFIED);
    }
}

void
NclLinkTriggerCondition::notifyConditionObservers (short status)
{
  pthread_attr_t t_attr;
  pthread_t t_id;
  ConditionStatus *data;

  Thread::mutexLock (&sMutex);

  if (!running)
    {
      running = true;
      pthread_attr_init (&t_attr);
      pthread_attr_setdetachstate (&t_attr, PTHREAD_CREATE_DETACHED);
      pthread_attr_setscope (&t_attr, PTHREAD_SCOPE_SYSTEM);

      if (isDeleting)
        {
          Thread::mutexUnlock (&sMutex);
          return;
        }

      pthread_create (&t_id, &t_attr, notificationThread, this);
      pthread_detach (t_id);
    }

  data = new ConditionStatus;
  data->listener = listener;
  data->status = status;
  data->condition = this;

  notes.push_back (data);

  Thread::mutexUnlock (&sMutex);
}

void *
NclLinkTriggerCondition::notificationThread (arg_unused (void *ptr))
{
  ConditionStatus *data;
  NclLinkTriggerListener *listener;
  NclLinkCondition *condition;
  short status;

  while (running)
    {
      listener = NULL;

      Thread::mutexLock (&sMutex);
      if (!notes.empty ())
        {
          data = *notes.begin ();
          notes.erase (notes.begin ());

          listener = data->listener;
          status = data->status;
          condition = data->condition;

          if (((NclLinkTriggerCondition *)condition)->isDeleting)
            {
              delete data;
              Thread::mutexUnlock (&sMutex);
              continue;
            }
        }
      Thread::mutexUnlock (&sMutex);

      if (listener != NULL)
        {
          switch (status)
            {
            case NclLinkTriggerListener::CONDITION_SATISFIED:
              listener->conditionSatisfied ((void *)condition);
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
        }

      Thread::mutexLock (&sMutex);
      if (notes.empty ())
        {
          running = false;
        }
      Thread::mutexUnlock (&sMutex);
    }

  return NULL;
}

void
NclLinkTriggerCondition::run ()
{
  if (delay > 0)
    {
      g_usleep ((gulong) delay * 1000);
    }
  notifyConditionObservers (NclLinkTriggerListener::CONDITION_SATISFIED);
}

GINGA_FORMATTER_END
