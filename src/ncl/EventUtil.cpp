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
#include "ncl/EventUtil.h"

GINGA_NCL_BEGIN

short
EventUtil::getTypeCode (string typeName)
{
  if (typeName == "presentation")
    {
      return EventUtil::EVT_PRESENTATION;
    }
  else if (typeName == "selection")
    {
      return EventUtil::EVT_SELECTION;
    }
  else if (typeName == "attribution")
    {
      return EventUtil::EVT_ATTRIBUTION;
    }
  else if (typeName == "composition")
    {
      return EventUtil::EVT_COMPOSITION;
    }

  return EventUtil::EVT_PRESENTATION;
}

string
EventUtil::getTypeName (short type)
{
  switch (type)
    {
    case EventUtil::EVT_PRESENTATION:
      return "presentation";

    case EventUtil::EVT_SELECTION:
      return "selection";

    case EventUtil::EVT_ATTRIBUTION:
      return "attribution";

    case EventUtil::EVT_COMPOSITION:
      return "composition";

    default:
      return "";
    }
}

short
EventUtil::getStateCode (string stateName)
{
  if (stateName == "occurring")
    {
      return EventUtil::ST_OCCURRING;
    }
  else if (stateName == "paused")
    {
      return EventUtil::ST_PAUSED;
    }

  // "sleeping"
  return EventUtil::ST_SLEEPING;
}

string
EventUtil::getStateName (short state)
{
  switch (state)
    {
    case EventUtil::ST_OCCURRING:
      return "occurring";

    case EventUtil::ST_PAUSED:
      return "paused";

    case EventUtil::ST_SLEEPING:
      return "sleeping";

    default:
      return "";
    }
}

string
EventUtil::getTransitionName (short transition)
{
  switch (transition)
    {
    case EventUtil::TR_STARTS:
      return "starts";

    case EventUtil::TR_STOPS:
      return "stops";

    case EventUtil::TR_PAUSES:
      return "pauses";

    case EventUtil::TR_RESUMES:
      return "resumes";

    case EventUtil::TR_ABORTS:
      return "aborts";

    default:
      return "";
    }
}

short
EventUtil::getTransitionCode (string transition)
{
  if (transition == "starts")
    {
      return EventUtil::TR_STARTS;
    }
  else if (transition == "stops")
    {
      return EventUtil::TR_STOPS;
    }
  else if (transition == "pauses")
    {
      return EventUtil::TR_PAUSES;
    }
  else if (transition == "resumes")
    {
      return EventUtil::TR_RESUMES;
    }
  else if (transition == "aborts")
    {
      return EventUtil::TR_ABORTS;
    }

  return -1;
}

short
EventUtil::getAttributeTypeCode (string attTypeName)
{
  if (attTypeName == "occurrences")
    {
      return EventUtil::ATT_OCCURRENCES;
    }
  else if (attTypeName == "nodeProperty")
    {
      return EventUtil::ATT_NODE_PROPERTY;
    }
  else if (attTypeName == "repetitions")
    {
      return EventUtil::ATT_REPETITIONS;
    }
  else if (attTypeName == "state")
    {
      return EventUtil::ATT_STATE;
    }

  return -1;
}

string
EventUtil::getAttributeTypeName (short type)
{
  switch (type)
    {
    case EventUtil::ATT_OCCURRENCES:
      return "occurrences";

    case EventUtil::ATT_NODE_PROPERTY:
      return "nodeProperty";

    case EventUtil::ATT_REPETITIONS:
      return "repetitions";

    case EventUtil::ATT_STATE:
      return "state";

    default:
      return "nodeProperty";
    }
}

GINGA_NCL_END
