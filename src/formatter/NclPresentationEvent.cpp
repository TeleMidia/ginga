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
#include "NclPresentationEvent.h"
#include "ncl/IntervalAnchor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wfloat-conversion)

GINGA_FORMATTER_BEGIN

NclPresentationEvent::NclPresentationEvent (const string &id,
                                            NclExecutionObject *executionObject,
                                            ContentAnchor *anchor)
    : NclAnchorEvent (id, executionObject, anchor)
{
  typeSet.insert ("NclPresentationEvent");

  numPresentations = 1;
  repetitionInterval = 0;

  if (anchor->instanceOf ("IntervalAnchor"))
    {
      begin = ((IntervalAnchor *)anchor)->getBegin ();
      end = ((IntervalAnchor *)anchor)->getEnd ();
    }
  else
    {
      begin = 0;
      end = GINGA_TIME_NONE;
    }
}

NclPresentationEvent::~NclPresentationEvent () { removeInstance (this); }

bool
NclPresentationEvent::stop ()
{
  if (currentState == EventUtil::ST_OCCURRING && numPresentations > 1)
    {
      numPresentations--;
    }

  return NclFormatterEvent::stop ();
}

GingaTime
NclPresentationEvent::getDuration ()
{
  if (!GINGA_TIME_IS_VALID (this->end))
    return GINGA_TIME_NONE;
  return this->end - this->begin;
}

GingaTime
NclPresentationEvent::getRepetitionInterval ()
{
  return repetitionInterval;
}

int
NclPresentationEvent::getRepetitions ()
{
  return (numPresentations - 1);
}

void
NclPresentationEvent::setEnd (GingaTime end)
{
  this->end = end;
}

void
NclPresentationEvent::setRepetitionSettings (int repetitions,
                                             GingaTime repetitionInterval)
{
  if (repetitions >= 0)
    {
      this->numPresentations = repetitions + 1;
    }
  else
    {
      this->numPresentations = 1;
    }

  this->repetitionInterval = repetitionInterval;
}

GingaTime
NclPresentationEvent::getBegin ()
{
  return begin;
}

GingaTime
NclPresentationEvent::getEnd ()
{
  return end;
}

void
NclPresentationEvent::incrementOccurrences ()
{
  occurrences++;
}

GINGA_FORMATTER_END
