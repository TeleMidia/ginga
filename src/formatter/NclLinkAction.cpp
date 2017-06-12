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

NclLinkAction::NclLinkAction (GingaTime delay)
{
  initLinkAction (delay);
}

NclLinkAction::~NclLinkAction ()
{
  if (progressionListeners != NULL)
    {
      delete progressionListeners;
      progressionListeners = NULL;
    }
}

void
NclLinkAction::initLinkAction (GingaTime delay)
{
  satisfiedCondition = NULL;
  this->delay = delay;
  progressionListeners = new vector<NclLinkActionProgressionListener *>;
  typeSet.insert ("NclLinkAction");
}

bool
NclLinkAction::instanceOf (const string &s)
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

GingaTime
NclLinkAction::getWaitDelay ()
{
  return this->delay;
}

void
NclLinkAction::setSatisfiedCondition (NclLinkCondition *satisfiedCondition)
{
  this->satisfiedCondition = satisfiedCondition;
}

void
NclLinkAction::run (NclLinkCondition *satisfiedCondition)
{
  this->satisfiedCondition = satisfiedCondition;
  run ();
}

void
NclLinkAction::run ()
{
}

void
NclLinkAction::setWaitDelay (GingaTime delay)
{
  this->delay = delay;
}

bool
NclLinkAction::hasDelay ()
{
  return this->delay > 0;
}

void
NclLinkAction::addActionProgressionListener (
    NclLinkActionProgressionListener *listener)
{
  vector<NclLinkActionProgressionListener *>::iterator i;

  if (tryLock ())
    {
      if (progressionListeners != NULL)
        {
          i = progressionListeners->begin ();
          while (i != progressionListeners->end ())
            {
              if (listener == *i)
                {
                  clog << "NclLinkAction::addActionProgressionListener ";
                  clog << "Warning! Trying to add the same listener twice";
                  clog << endl;
                  return;
                }
              ++i;
            }
          progressionListeners->push_back (listener);
        }
    }
}

void
NclLinkAction::removeActionProgressionListener (
    NclLinkActionProgressionListener *listener)
{
  vector<NclLinkActionProgressionListener *>::iterator i;

  if (tryLock ())
    {
      if (progressionListeners != NULL)
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
    }
}

void
NclLinkAction::notifyProgressionListeners (bool start)
{
  size_t i, size;
  NclLinkActionProgressionListener *listener;
  vector<NclLinkActionProgressionListener *> *notifyList;

  if (tryLock ())
    {
      if (progressionListeners != NULL)
        {
          notifyList = new vector<NclLinkActionProgressionListener *> (
              *progressionListeners);

          size = notifyList->size ();
          for (i = 0; i < size; i++)
            {
              listener = (*notifyList)[i];
              listener->actionProcessed (start);
            }
          delete notifyList;
        }
    }
}

bool
NclLinkAction::tryLock ()
{
  return true;
}

GINGA_FORMATTER_END
