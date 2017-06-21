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

NclEventTransitionManager::NclEventTransitionManager ()
{
}

NclEventTransitionManager::~NclEventTransitionManager ()
{
  for (EventTransition *trans: _transTable)
    delete trans;

  _transTable.clear ();
}

void
NclEventTransitionManager::addEventTransition (EventTransition *transition)
{
  size_t beg, end, pos;
  EventTransition *auxTransition;

  // binary search
  beg = 0;
  if (_transTable.size () == 0)
    goto done;

  end = _transTable.size () - 1;

  while (beg <= end)
    {
      pos = (beg + end) / 2;
      auxTransition = _transTable[pos];
      switch (transition->compareTo (auxTransition))
        {
        case 0:
          return;
        case -1:
          end = pos - 1;
          break;
        case 1:
          beg = pos + 1;
          break;
        default:
          g_assert_not_reached ();
        }
    }

done:
  _transTable.insert (_transTable.begin () + (int) beg, transition);
}

void
NclEventTransitionManager::removeEventTransition (NclPresentationEvent *evt)
{
  size_t i, size;
  EventTransition *trans;
  EventTransition *endTrans;

  size = _transTable.size ();
  for (i = 0; i < size; i++)
    {
      trans = _transTable[i];
      if (trans->getEvent () == evt)
        {
          auto beginEvtTrans = dynamic_cast<BeginEventTransition *> (trans);
          if (beginEvtTrans
              && (beginEvtTrans->getEndTransition () != nullptr))
            {
              endTrans = beginEvtTrans->getEndTransition ();

              for (auto j = _transTable.begin (); j != _transTable.end (); ++j)
                {
                  if (*j == endTrans)
                    {
                      _transTable.erase (j);
                      size = _transTable.size ();
                      i = 0;
                      break;
                    }
                }
            }

          for (auto j = _transTable.begin (); j != _transTable.end (); ++j)
            {
              if (*j == trans)
                {
                  _transTable.erase (j);
                  size = _transTable.size ();
                  i = 0;
                  break;
                }
            }
        }
    }
}

void
NclEventTransitionManager::resetTimeIndex ()
{
  _currentTransitionIndex = _startTransitionIndex;
}

void
NclEventTransitionManager::prepare (bool wholeContent, GingaTime startTime)
{
  EventTransition *trans;
  size_t transIx, size;

  if (wholeContent && startTime == 0)
    {
      _startTransitionIndex = 0;
    }
  else
    {
      size = _transTable.size ();
      transIx = 0;
      _startTransitionIndex = transIx;
      while (transIx < size)
        {
          trans = _transTable[transIx];
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
              clog << "NclEventTransitionManager::prepare set '";
              clog << trans->getEvent ()->getId ();
              clog << "' to SLEEP" << endl;

              trans->getEvent ()->setState (EventState::SLEEPING);

              trans->getEvent ()->incOccurrences ();
            }
          transIx++;
          _startTransitionIndex = transIx;
        }
    }

  resetTimeIndex ();
}

void
NclEventTransitionManager::start (GingaTime offsetTime)
{
  EventTransition *transition;
  size_t transIx, size;

  size = _transTable.size ();
  transIx = _currentTransitionIndex;

  while (transIx < size)
    {
      transition = _transTable[transIx];
      if (transition->getTime () <= offsetTime)
        {
          auto beginEvtTrans = dynamic_cast<BeginEventTransition *> (transition);
          if (beginEvtTrans)
            {
              transition->getEvent ()->start ();
            }
          transIx++;
          _currentTransitionIndex = transIx;
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
  for (EventTransition *trans : _transTable)
    {
      if (!applicationType
          || (applicationType && GINGA_TIME_IS_VALID (trans->getTime ())))
        {
          NclFormatterEvent *fev = trans->getEvent ();
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
  NclFormatterEvent *fev;

  transIx = _currentTransitionIndex;
  size = _transTable.size ();

  for (i = transIx; i < size; i++)
    {
      trans = _transTable[i];
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
NclEventTransitionManager::addPresentationEvent (NclPresentationEvent *evt)
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
      _transTable.insert (_transTable.begin (), beginTrans);
      if (evt->getEnd () != GINGA_TIME_NONE)
        {
          endTrans = new EndEventTransition (evt->getEnd (), evt, beginTrans);

          i = _transTable.begin ();
          while (i != _transTable.end ())
            {
              lastTransition = *i;
              lTime = lastTransition->getTime ();
              if (!GINGA_TIME_IS_VALID (lTime)
                  || endTrans->getTime () < lTime)
                {
                  _transTable.insert (i, endTrans);
                  break;
                }

              ++i;

              if (i == _transTable.end ())
                {
                  _transTable.push_back (endTrans);
                  break;
                }
            }
        }
    }
  else
    {
      begin = evt->getBegin ();
      beginTrans = new BeginEventTransition (begin, evt);
      addEventTransition (beginTrans);

      end = evt->getEnd ();
      endTrans = new EndEventTransition (end, evt, beginTrans);
      addEventTransition (endTrans);
    }
}

void
NclEventTransitionManager::updateTransitionTable (
    GingaTime value, Player *player, NclFormatterEvent *mainEvt)
{
  EventTransition *trans;
  NclFormatterEvent *ev;
  size_t currentIx;

  currentIx = _currentTransitionIndex;

  while (currentIx < _transTable.size ())
    {
      trans = _transTable[currentIx];

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
          _currentTransitionIndex = currentIx;
        }
      else
        {
          break;
        }
    }
}

EventTransition *
NclEventTransitionManager::getNextTransition (NclFormatterEvent *mainEvt)
{
  EventTransition *transition;
  vector<EventTransition *> *transitionEvents;
  size_t currentIx;
  GingaTime transTime;
  GingaTime eventEnd;

  transitionEvents = &_transTable;
  currentIx = _currentTransitionIndex;

  if (currentIx < transitionEvents->size ())
    {
      transition = transitionEvents->at (currentIx);

      eventEnd = ((NclPresentationEvent *)mainEvt)->getEnd ();
      transTime = transition->getTime ();

      if (!GINGA_TIME_IS_VALID (eventEnd)
          || transTime <= eventEnd)
        {
          return transition;
        }
    }

  return nullptr;
}

GINGA_FORMATTER_END
