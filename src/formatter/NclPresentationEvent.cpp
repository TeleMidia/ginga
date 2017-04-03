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

GINGA_FORMATTER_BEGIN

// if the representation changes, update isUndefinedInstant method
const double NclPresentationEvent::UNDEFINED_INSTANT = (double)NAN;

NclPresentationEvent::NclPresentationEvent (const string &id,
                                            void *executionObject,
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
      duration = end - begin;
    }
  else
    {
      begin = NclPresentationEvent::UNDEFINED_INSTANT;
      end = NclPresentationEvent::UNDEFINED_INSTANT;
      duration = NclPresentationEvent::UNDEFINED_INSTANT;
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

double
NclPresentationEvent::getDuration ()
{
  return duration;
}

double
NclPresentationEvent::getRepetitionInterval ()
{
  return repetitionInterval;
}

long
NclPresentationEvent::getRepetitions ()
{
  return (numPresentations - 1);
}

void
NclPresentationEvent::setDuration (double dur)
{
  this->duration = dur;
}

void
NclPresentationEvent::setEnd (double e)
{
  bool isObjDur = IntervalAnchor::isObjectDuration (e);

  if (!isObjDur && isnan (begin))
    {
      begin = 0;
    }

  if (isObjDur || e >= begin)
    {
      end = e;
      if (isObjDur)
        {
          duration = end;
        }
      else
        {
          duration = end - begin;
        }
    }
}

void
NclPresentationEvent::setRepetitionSettings (long repetitions,
                                             double repetitionInterval)
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

double
NclPresentationEvent::getBegin ()
{
  return begin;
}

double
NclPresentationEvent::getEnd ()
{
  return end;
}

void
NclPresentationEvent::incrementOccurrences ()
{
  occurrences++;
}

bool
NclPresentationEvent::isUndefinedInstant (double value)
{
  return isnan (value);
}

GINGA_FORMATTER_END
