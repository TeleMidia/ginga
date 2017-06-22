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

NclLinkAction::~NclLinkAction () {}

void
NclLinkAction::initLinkAction (GingaTime delay)
{
  satisfiedCondition = nullptr;
  this->delay = delay;
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
    NclLinkActionProgressListener *listener)
{
  auto i = progressListeners.begin ();
  while (i != progressListeners.end ())
    {
      if (listener == *i)
        {
          WARNING ("Trying to add the same listener twice.");
          return;
        }
      ++i;
    }
  progressListeners.push_back (listener);
}

void
NclLinkAction::removeActionProgressionListener (
    NclLinkActionProgressListener *listener)
{
  for (auto i = progressListeners.begin ();
       i != progressListeners.end (); ++i)
    {
      if (*i == listener)
        {
          progressListeners.erase (i);
          break;
        }
    }
}

void
NclLinkAction::notifyProgressionListeners (bool start)
{
  NclLinkActionProgressListener *listener;
  vector<NclLinkActionProgressListener *> notifyList (progressListeners);

  for (size_t i = 0; i < notifyList.size (); i++)
    {
      listener = notifyList[i];
      listener->actionProcessed (start);
    }
}

GINGA_FORMATTER_END
