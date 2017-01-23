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
#include "NclLinkAction.h"

GINGA_FORMATTER_BEGIN

NclLinkAction::NclLinkAction () : Thread () { initLinkAction (0.0); }

NclLinkAction::NclLinkAction (double delay) : Thread ()
{
  initLinkAction (delay);
}

NclLinkAction::~NclLinkAction ()
{
  isDeleting = true;

  Thread::mutexLock (&plMutex);
  if (progressionListeners != NULL)
    {
      delete progressionListeners;
      progressionListeners = NULL;
    }
  Thread::mutexUnlock (&plMutex);
  Thread::mutexDestroy (&plMutex);
}

void
NclLinkAction::initLinkAction (double delay)
{
  satisfiedCondition = NULL;
  this->delay = delay;
  progressionListeners = new vector<NclLinkActionProgressionListener *>;
  Thread::mutexInit (&plMutex, false);
  typeSet.insert ("NclLinkAction");
}

bool
NclLinkAction::instanceOf (string s)
{
  if (typeSet.empty ())
    {
      return false;
    }
  else
    {
      return (typeSet.find (s) != typeSet.end ());
    }
}

double
NclLinkAction::getWaitDelay ()
{
  return this->delay;
}

void
NclLinkAction::setSatisfiedCondition (void *satisfiedCondition)
{

  this->satisfiedCondition = satisfiedCondition;
}

void
NclLinkAction::run (void *satisfiedCondition)
{
  this->satisfiedCondition = satisfiedCondition;
  run ();
}

void
NclLinkAction::run ()
{
  if (hasDelay ())
    {
      clog << "NclLinkAction::run uSleeping '" << delay * 1000 << "'"
           << endl;
      g_usleep ((long)(delay * 1000));
    }
}

void
NclLinkAction::setWaitDelay (double delay)
{
  this->delay = delay;
}

bool
NclLinkAction::hasDelay ()
{
  if (this->delay > 0)
    {
      return true;
    }
  return false;
}

void
NclLinkAction::addActionProgressionListener (
    NclLinkActionProgressionListener *listener)
{

  vector<NclLinkActionProgressionListener *>::iterator i;

  if (tryLock ())
    {
      if (progressionListeners != NULL && !isDeleting)
        {
          i = progressionListeners->begin ();
          while (i != progressionListeners->end ())
            {
              if (listener == *i)
                {
                  clog << "NclLinkAction::addActionProgressionListener ";
                  clog << "Warning! Trying to add the same listener twice";
                  clog << endl;
                  Thread::mutexUnlock (&plMutex);
                  return;
                }
              ++i;
            }
          progressionListeners->push_back (listener);
        }
      Thread::mutexUnlock (&plMutex);
    }
}

void
NclLinkAction::removeActionProgressionListener (
    NclLinkActionProgressionListener *listener)
{

  vector<NclLinkActionProgressionListener *>::iterator i;

  if (tryLock ())
    {
      if (progressionListeners != NULL && !isDeleting)
        {
          for (i = progressionListeners->begin ();
               i != progressionListeners->end (); ++i)
            {

              if (*i == listener)
                {
                  progressionListeners->erase (i);
                  break;
                }
            }
        }
      Thread::mutexUnlock (&plMutex);
    }
}

void
NclLinkAction::notifyProgressionListeners (bool start)
{
  int i, size;
  NclLinkActionProgressionListener *listener;
  vector<NclLinkActionProgressionListener *> *notifyList;

  if (tryLock ())
    {
      if (progressionListeners != NULL && !isDeleting)
        {
          notifyList = new vector<NclLinkActionProgressionListener *> (
              *progressionListeners);

          Thread::mutexUnlock (&plMutex);

          size = (int) notifyList->size ();
          for (i = 0; i < size; i++)
            {
              listener = (*notifyList)[i];
              listener->actionProcessed (start);
              if (isDeleting)
                {
                  break;
                }
            }
          delete notifyList;
        }
      else
        {
          Thread::mutexUnlock (&plMutex);
        }
    }
}

bool
NclLinkAction::tryLock ()
{
  if (isDeleting)
    {
      return false;
    }

  Thread::mutexLock (&plMutex);
  return true;
}

GINGA_FORMATTER_END
