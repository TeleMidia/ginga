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
#include "SimpleCondition.h"

GINGA_NCL_BEGIN

SimpleCondition::SimpleCondition (string role)
    : TriggerExpression (), Role ()
{

  SimpleCondition::setLabel (role);
  key = "";
  qualifier = SimpleCondition::NO_QUALIFIER;
  typeSet.insert ("SimpleCondition");
  typeSet.insert ("Role");
}

string
SimpleCondition::getKey ()
{
  return key;
}

void
SimpleCondition::setKey (string key)
{
  this->key = key;
}

short
SimpleCondition::getTransition ()
{
  return transition;
}

void
SimpleCondition::setTransition (short transition)
{
  this->transition = transition;
}

short
SimpleCondition::getQualifier ()
{
  return qualifier;
}

void
SimpleCondition::setQualifier (short qualifier)
{
  this->qualifier = qualifier;
}

void
SimpleCondition::setLabel (string id)
{
  label = id;
  string upLabel = upperCase (label);

  if (upLabel == upperCase ("onBegin"))
    {
      transition = EventUtil::TR_STARTS;
      eventType = EventUtil::EVT_PRESENTATION;
    }
  else if (upLabel == upperCase ("onEnd"))
    {
      transition = EventUtil::TR_STOPS;
      eventType = EventUtil::EVT_PRESENTATION;
    }
  else if (upLabel == upperCase ("onSelection"))
    {
      transition = EventUtil::TR_STOPS;
      eventType = EventUtil::EVT_SELECTION;
    }
  else if (upLabel == upperCase ("onBeginAttribution"))
    {
      transition = EventUtil::TR_STARTS;
      eventType = EventUtil::EVT_ATTRIBUTION;
    }
  else if (upLabel == upperCase ("onEndAttribution"))
    {
      transition = EventUtil::TR_STOPS;
      eventType = EventUtil::EVT_ATTRIBUTION;
    }
  else if (upLabel == upperCase ("onAbort"))
    {
      transition = EventUtil::TR_ABORTS;
      eventType = EventUtil::EVT_PRESENTATION;
    }
  else if (upLabel == upperCase ("onPause"))
    {
      transition = EventUtil::TR_PAUSES;
      eventType = EventUtil::EVT_PRESENTATION;
    }
  else if (upLabel == upperCase ("onResume"))
    {
      transition = EventUtil::TR_RESUMES;
      eventType = EventUtil::EVT_PRESENTATION;
    }
}

GINGA_NCL_END
