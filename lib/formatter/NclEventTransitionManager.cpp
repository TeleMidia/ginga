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

#include "aux-ginga.h"
#include "NclEventTransitionManager.h"

using namespace ginga::ncl;

GINGA_FORMATTER_BEGIN

class EndEventTransition;

class BeginEventTransition : public EventTransition
{
  PROPERTY (EndEventTransition *, _endTrans, getEndTransition, setEndTransition)

public:
  BeginEventTransition (GingaTime time, PresentationEvent *evt)
  : EventTransition (time, evt) { }
};

class EndEventTransition : public EventTransition
{
  PROPERTY_READONLY (BeginEventTransition *, _beginTrans, getBeginTransition)

public:
  EndEventTransition (GingaTime t, PresentationEvent *evt,
                      BeginEventTransition *trans)
    : EventTransition (t, evt)
  {
    _beginTrans = trans;
    _beginTrans->setEndTransition (this);
  }
};

NclEventTransitionManager::~NclEventTransitionManager ()
{
  for (EventTransition *t: _transTbl)
    delete t;
}

static bool
cmp_transitions (EventTransition *t1, EventTransition *t2)
{
  if (t1->getTime() < t2->getTime()) return true;
  else if (t1->getTime() > t2->getTime()) return false;

  bool t1Begin = cast (BeginEventTransition *, t1);
  auto t2End = cast (EndEventTransition *, t2);

  return (t1Begin && t2End);
}

void
NclEventTransitionManager::addTransition (EventTransition *trans)
{
  auto it = lower_bound (_transTbl.begin(), _transTbl.end(), trans,
                         cmp_transitions);

  _transTbl.insert (it, trans);
}

void
NclEventTransitionManager::removeEventTransition (PresentationEvent *evt)
{
  vector <EventTransition *> toDel;

  for (EventTransition *trans : _transTbl)
    {
      if (trans->getEvent () == evt)
        {
          toDel.push_back (trans);

          auto beginTrans = cast (BeginEventTransition *, trans);
          if (beginTrans
              && beginTrans->getEndTransition ())
            {
              toDel.push_back (beginTrans->getEndTransition ());
            }
        }
    }

  xvectremove (_transTbl, toDel);
}

void
NclEventTransitionManager::resetTimeIndex ()
{
  _currentTransitionIdx = _startTransitionIdx;
}

void
NclEventTransitionManager::prepare (bool wholeContent, GingaTime startTime)
{
   _startTransitionIdx = 0;
  if (wholeContent && startTime == 0)
    {
      // do nothing.
    }
  else
    {
      for (EventTransition *trans : _transTbl)
        {
          if (trans->getTime () >= startTime)
            {
              break;
            }

          auto beginTrans = cast (BeginEventTransition *, trans);
          if (beginTrans)
            {
              trans->getEvent ()->setState (EventState::OCCURRING);
            }
          else
            {
              trans->getEvent ()->setState (EventState::SLEEPING);
              trans->getEvent ()->incOccurrences ();
            }
          _startTransitionIdx++;
        }
    }

  resetTimeIndex ();
}

void
NclEventTransitionManager::start (GingaTime offsetTime)
{
  for (size_t transIdx = _currentTransitionIdx; transIdx < _transTbl.size();)
    {
      EventTransition *trans = _transTbl[transIdx];
      if (trans->getTime () <= offsetTime)
        {
          auto beginTrans = cast (BeginEventTransition *, trans);
          if (beginTrans)
            {
              trans->getEvent ()->start ();
            }

          _currentTransitionIdx = ++transIdx;
        }
      else
        {
          break;
        }
    }
}

void
NclEventTransitionManager::stop (GingaTime endTime)
{
  for (EventTransition *trans : _transTbl)
    {
      NclEvent *fev = trans->getEvent ();

      if (!GINGA_TIME_IS_VALID (endTime) || trans->getTime () > endTime)
        {
          fev->setState (EventState::SLEEPING);
        }
      else if (instanceof (EndEventTransition *, trans))
        {
          fev->stop ();
        }
    }
}

void
NclEventTransitionManager::abort (GingaTime endTime)
{
  for (size_t i = _currentTransitionIdx; i < _transTbl.size (); i++)
    {
      EventTransition *trans = _transTbl[i];
      NclEvent *fev = trans->getEvent ();

      if (trans->getTime () > endTime)
        {
          fev->setState (EventState::SLEEPING);
        }
      else if (instanceof (EndEventTransition *, trans))
        {
          fev->abort ();
        }
    }
}

void
NclEventTransitionManager::addPresentationEvent (PresentationEvent *evt)
{
  BeginEventTransition *beginTrans = nullptr;
  EndEventTransition *endTrans = nullptr;

  auto lambdaAnchor = cast (AreaLambda *, evt->getAnchor());
  if (lambdaAnchor)
    {
      beginTrans = new BeginEventTransition (0, evt);

      if (evt->getEnd () != GINGA_TIME_NONE)
          endTrans = new EndEventTransition (evt->getEnd (), evt, beginTrans);
    }
  else
    {
      beginTrans = new BeginEventTransition (evt->getBegin (), evt);
      endTrans = new EndEventTransition (evt->getEnd (), evt, beginTrans);
    }

  addTransition(beginTrans);

  if (endTrans)
    addTransition(endTrans);
}

void
NclEventTransitionManager::updateTransitionTable (
    GingaTime value, Player *player, NclEvent *mainEvt)
{
  for (size_t transIdx = _currentTransitionIdx; transIdx < _transTbl.size (); )
    {
      EventTransition *trans = _transTbl[transIdx];

      if (trans->getTime () <= value)
        {
          NclEvent *ev = trans->getEvent ();
          auto beginEvtTrans = cast (BeginEventTransition *, trans);
          if (beginEvtTrans)
            {
              ev->start ();
            }
          else
            {
              if (ev == mainEvt && player != nullptr)
                {
                  player->stop ();
                }
              ev->stop ();
            }

          _currentTransitionIdx = ++transIdx;
        }
      else
        {
          break;
        }
    }
}

EventTransition *
NclEventTransitionManager::nextTransition (NclEvent *mainEvt)
{
  if (_currentTransitionIdx < _transTbl.size ())
    {
      EventTransition *trans = _transTbl.at (_currentTransitionIdx);
      PresentationEvent *presentationEvt
          = static_cast<PresentationEvent *> (mainEvt);

      GingaTime evtEnd = presentationEvt->getEnd ();
      GingaTime time = trans->getTime ();

      if (!GINGA_TIME_IS_VALID (evtEnd)
          || time <= evtEnd)
        {
          return trans;
        }
    }

  return nullptr;
}

GINGA_FORMATTER_END
