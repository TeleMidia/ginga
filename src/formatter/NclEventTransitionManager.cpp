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

GINGA_FORMATTER_BEGIN

NclEventTransitionManager::NclEventTransitionManager ()
{
}

NclEventTransitionManager::~NclEventTransitionManager ()
{
  for (NclEventTransition *trans: transTable)
    delete trans;

  transTable.clear ();
}

void
NclEventTransitionManager::addEventTransition (NclEventTransition *transition)
{
  size_t beg, end, pos;
  NclEventTransition *auxTransition;

  // binary search
  beg = 0;
  if (transTable.size () == 0)
    goto done;

  end = transTable.size () - 1;

  while (beg <= end)
    {
      pos = (beg + end) / 2;
      auxTransition = transTable[pos];
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
  transTable.insert (transTable.begin () + (int) beg, transition);
}

void
NclEventTransitionManager::removeEventTransition (NclPresentationEvent *evt)
{
  size_t i, size;
  NclEventTransition *trans;
  NclEventTransition *endTrans;

  size = transTable.size ();
  for (i = 0; i < size; i++)
    {
      trans = transTable[i];
      if (trans->getEvent () == evt)
        {
          auto beginEvtTrans = dynamic_cast<NclBeginEventTransition *> (trans);
          if (beginEvtTrans
              && (beginEvtTrans->getEndTransition () != nullptr))
            {
              endTrans = beginEvtTrans->getEndTransition ();

              for (auto j = transTable.begin (); j != transTable.end (); ++j)
                {
                  if (*j == endTrans)
                    {
                      transTable.erase (j);
                      size = transTable.size ();
                      i = 0;
                      break;
                    }
                }
            }

          for (auto j = transTable.begin (); j != transTable.end (); ++j)
            {
              if (*j == trans)
                {
                  transTable.erase (j);
                  size = transTable.size ();
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
  currentTransitionIndex = startTransitionIndex;
}

void
NclEventTransitionManager::prepare (bool wholeContent, GingaTime startTime)
{
  NclEventTransition *trans;
  size_t transIx, size;

  if (wholeContent && startTime == 0)
    {
      startTransitionIndex = 0;
    }
  else
    {
      size = transTable.size ();
      transIx = 0;
      startTransitionIndex = transIx;
      while (transIx < size)
        {
          trans = transTable[transIx];
          if (trans->getTime () >= startTime)
            {
              break;
            }

          auto beginEvtTrans = dynamic_cast<NclBeginEventTransition *> (trans);
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
          startTransitionIndex = transIx;
        }
    }

  resetTimeIndex ();
}

void
NclEventTransitionManager::start (GingaTime offsetTime)
{
  NclEventTransition *transition;
  size_t transIx, size;

  size = transTable.size ();
  transIx = currentTransitionIndex;

  while (transIx < size)
    {
      transition = transTable[transIx];
      if (transition->getTime () <= offsetTime)
        {
          auto beginEvtTrans = dynamic_cast<NclBeginEventTransition *> (transition);
          if (beginEvtTrans)
            {
              transition->getEvent ()->start ();
            }
          transIx++;
          currentTransitionIndex = transIx;
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
  vector<NclEventTransition *>::iterator i;
  NclEventTransition *transition;
  NclFormatterEvent *fev;

  i = transTable.begin ();
  while (i != transTable.end ())
    {
      transition = *i;
      if (!applicationType
          || (applicationType && GINGA_TIME_IS_VALID (transition->getTime ())))
        {
          fev = transition->getEvent ();
          if (!GINGA_TIME_IS_VALID (endTime) || transition->getTime () > endTime)
            {
              fev->setState (EventState::SLEEPING);
            }
          else if (dynamic_cast<NclEndEventTransition *> (transition))
            {
              fev->stop ();
            }
        }
      ++i;
    }
}

void
NclEventTransitionManager::abort (GingaTime endTime, bool applicationType)
{
  size_t transIx, i, size;
  NclEventTransition *trans;
  NclFormatterEvent *fev;

  transIx = currentTransitionIndex;
  size = transTable.size ();

  for (i = transIx; i < size; i++)
    {
      trans = transTable[i];
      if (!applicationType
          || (applicationType && !isinf (trans->getTime ())))
        {
          fev = trans->getEvent ();
          if (trans->getTime () > endTime)
            {
              fev->setState (EventState::SLEEPING);
            }
          else if (dynamic_cast<NclEndEventTransition *> (trans))
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
  NclBeginEventTransition *beginTrans;
  NclEndEventTransition *endTrans;
  NclEventTransition *lastTransition = NULL;
  vector<NclEventTransition *>::iterator i;
  GingaTime lTime;

  if ((evt->getAnchor ())->instanceOf ("LambdaAnchor"))
    {
      beginTrans = new NclBeginEventTransition (0, evt);
      transTable.insert (transTable.begin (), beginTrans);
      if (evt->getEnd () != GINGA_TIME_NONE)
        {
          endTrans = new NclEndEventTransition (
                evt->getEnd (), evt, beginTrans);

          i = transTable.begin ();
          while (i != transTable.end ())
            {
              lastTransition = *i;
              lTime = lastTransition->getTime ();
              if (!GINGA_TIME_IS_VALID (lTime)
                  || endTrans->getTime () < lTime)
                {
                  transTable.insert (i, endTrans);
                  break;
                }

              ++i;

              if (i == transTable.end ())
                {
                  transTable.push_back (endTrans);
                  break;
                }
            }
        }
    }
  else
    {
      begin = evt->getBegin ();
      beginTrans = new NclBeginEventTransition (begin, evt);
      addEventTransition (beginTrans);
      end = evt->getEnd ();

      endTrans = new NclEndEventTransition (end, evt, beginTrans);
      addEventTransition (endTrans);
    }
}

void
NclEventTransitionManager::updateTransitionTable (
    GingaTime value, Player *player, NclFormatterEvent *mainEvt)
{
  NclEventTransition *trans;
  NclFormatterEvent *ev;
  size_t currentIx;

  currentIx = currentTransitionIndex;

  while (currentIx < transTable.size ())
    {
      trans = transTable[currentIx];

      if (trans->getTime () <= value)
        {
          ev = trans->getEvent ();
          auto beginEvtTrans = dynamic_cast<NclBeginEventTransition *> (trans);
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
          currentTransitionIndex = currentIx;
        }
      else
        {
          break;
        }
    }
}

NclEventTransition *
NclEventTransitionManager::getNextTransition (NclFormatterEvent *mainEvt)
{
  NclEventTransition *transition;
  vector<NclEventTransition *> *transitionEvents;
  size_t currentIx;
  GingaTime transTime;
  GingaTime eventEnd;

  transitionEvents = &transTable;
  currentIx = currentTransitionIndex;

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
