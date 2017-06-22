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
#include "NclEventTransitionManager.h"
#include "ncl/LambdaAnchor.h"

using namespace ginga::ncl;

GINGA_FORMATTER_BEGIN

NclEventTransitionManager::~NclEventTransitionManager ()
{
  for (EventTransition *t: _transTbl)
    delete t;
}

static bool
compare_transitions (EventTransition *t1, EventTransition *t2)
{
  if (t1->getTime() < t2->getTime()) return true;
  else if (t1->getTime() > t2->getTime()) return false;

  auto t1Begin = dynamic_cast<BeginEventTransition *> (t1);
  auto t2End = dynamic_cast<EndEventTransition *> (t2);

  return (t1Begin && t2End);
}

void
NclEventTransitionManager::addTransition (EventTransition *trans)
{
  auto it = lower_bound (_transTbl.begin(), _transTbl.end(), trans,
                         compare_transitions);

  _transTbl.insert (it, trans);
}

void
NclEventTransitionManager::removeEventTransition (PresentationEvent *evt)
{
  size_t size;
  EventTransition *trans, *endTrans;
  vector <EventTransition *> toDel;

  size = _transTbl.size ();
  for (size_t i = 0; i < size; i++)
    {
      trans = _transTbl[i];
      if (trans->getEvent () == evt)
        {
          toDel.push_back (trans);

          auto beginEvtTrans = dynamic_cast<BeginEventTransition *> (trans);
          if (beginEvtTrans
              && (beginEvtTrans->getEndTransition () != nullptr))
            {
              endTrans = beginEvtTrans->getEndTransition ();
              toDel.push_back (endTrans);
            }
        }
    }

  _transTbl.erase(
        std::remove_if (_transTbl.begin(), _transTbl.end(),
           [&](EventTransition *t) {return find(toDel.begin(), toDel.end(), t) != toDel.end();}));

}

void
NclEventTransitionManager::resetTimeIndex ()
{
  _currentTransitionIdx = _startTransitionIdx;
}

void
NclEventTransitionManager::prepare (bool wholeContent, GingaTime startTime)
{
  EventTransition *trans;
  size_t transIdx, size;

  if (wholeContent && startTime == 0)
    {
      _startTransitionIdx = 0;
    }
  else
    {
      size = _transTbl.size ();
      transIdx = 0;
      _startTransitionIdx = transIdx;
      while (transIdx < size)
        {
          trans = _transTbl[transIdx];
          if (trans->getTime () >= startTime)
            {
              break;
            }

          auto beginEvtTrans = dynamic_cast<BeginEventTransition *> (trans);
          if (beginEvtTrans)
            {
              trans->getEvent ()->setState (EventState::OCCURRING);
            }
          else
            {
              trans->getEvent ()->setState (EventState::SLEEPING);
              trans->getEvent ()->incOccurrences ();
            }
          transIdx++;
          _startTransitionIdx = transIdx;
        }
    }

  resetTimeIndex ();
}

void
NclEventTransitionManager::start (GingaTime offsetTime)
{
  EventTransition *trans;
  size_t transIdx, size;

  size = _transTbl.size ();
  transIdx = _currentTransitionIdx;

  while (transIdx < size)
    {
      trans = _transTbl[transIdx];
      if (trans->getTime () <= offsetTime)
        {
          auto beginTrans = dynamic_cast<BeginEventTransition *> (trans);
          if (beginTrans)
            {
              trans->getEvent ()->start ();
            }
          transIdx++;
          _currentTransitionIdx = transIdx;
        }
      else
        {
          break;
        }
    }
}

void
NclEventTransitionManager::stop (GingaTime endTime, bool applicationType)
{
  for (EventTransition *trans : _transTbl)
    {
      if (!applicationType
          || (applicationType && GINGA_TIME_IS_VALID (trans->getTime ())))
        {
          NclEvent *fev = trans->getEvent ();
          if (!GINGA_TIME_IS_VALID (endTime) || trans->getTime () > endTime)
            {
              fev->setState (EventState::SLEEPING);
            }
          else if (dynamic_cast<EndEventTransition *> (trans))
            {
              fev->stop ();
            }
        }
    }
}

void
NclEventTransitionManager::abort (GingaTime endTime, bool applicationType)
{
  size_t transIx, i, size;
  EventTransition *trans;
  NclEvent *fev;

  transIx = _currentTransitionIdx;
  size = _transTbl.size ();

  for (i = transIx; i < size; i++)
    {
      trans = _transTbl[i];
      if (!applicationType
          || (applicationType && !isinf (trans->getTime ())))
        {
          fev = trans->getEvent ();
          if (trans->getTime () > endTime)
            {
              fev->setState (EventState::SLEEPING);
            }
          else if (dynamic_cast<EndEventTransition *> (trans))
            {
              fev->abort ();
            }
        }
    }
}

void
NclEventTransitionManager::addPresentationEvent (PresentationEvent *evt)
{
  GingaTime begin, end;
  BeginEventTransition *beginTrans;
  EndEventTransition *endTrans;
  EventTransition *lastTransition = NULL;
  vector<EventTransition *>::iterator i;
  GingaTime lTime;

  auto lambdaAnchor = dynamic_cast <LambdaAnchor *> (evt->getAnchor());
  if (lambdaAnchor)
    {
      beginTrans = new BeginEventTransition (0, evt);
      _transTbl.insert (_transTbl.begin (), beginTrans);
      if (evt->getEnd () != GINGA_TIME_NONE)
        {
          endTrans = new EndEventTransition (evt->getEnd (), evt, beginTrans);

          i = _transTbl.begin ();
          while (i != _transTbl.end ())
            {
              lastTransition = *i;
              lTime = lastTransition->getTime ();
              if (!GINGA_TIME_IS_VALID (lTime)
                  || endTrans->getTime () < lTime)
                {
                  _transTbl.insert (i, endTrans);
                  break;
                }

              ++i;

              if (i == _transTbl.end ())
                {
                  _transTbl.push_back (endTrans);
                  break;
                }
            }
        }
    }
  else
    {
      begin = evt->getBegin ();
      beginTrans = new BeginEventTransition (begin, evt);
      addTransition (beginTrans);

      end = evt->getEnd ();
      endTrans = new EndEventTransition (end, evt, beginTrans);
      addTransition (endTrans);
    }
}

void
NclEventTransitionManager::updateTransitionTable (
    GingaTime value, Player *player, NclEvent *mainEvt)
{
  EventTransition *trans;
  NclEvent *ev;
  size_t currentIx;

  currentIx = _currentTransitionIdx;

  while (currentIx < _transTbl.size ())
    {
      trans = _transTbl[currentIx];

      if (trans->getTime () <= value)
        {
          ev = trans->getEvent ();
          auto beginEvtTrans = dynamic_cast<BeginEventTransition *> (trans);
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

          currentIx++;
          _currentTransitionIdx = currentIx;
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
  EventTransition *transition;
  vector<EventTransition *> *transitionEvents;
  size_t currentIx;
  GingaTime transTime;
  GingaTime evtEnd;

  transitionEvents = &_transTbl;
  currentIx = _currentTransitionIdx;

  if (currentIx < transitionEvents->size ())
    {
      transition = transitionEvents->at (currentIx);

      evtEnd = ((PresentationEvent *)mainEvt)->getEnd ();
      transTime = transition->getTime ();

      if (!GINGA_TIME_IS_VALID (evtEnd)
          || transTime <= evtEnd)
        {
          return transition;
        }
    }

  return nullptr;
}

GINGA_FORMATTER_END
