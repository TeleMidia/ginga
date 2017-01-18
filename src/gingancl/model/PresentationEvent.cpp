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
#include "PresentationEvent.h"
#include "ncl/IntervalAnchor.h"

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_EVENT_BEGIN

// if the representation changes, update isUndefinedInstant method
const double PresentationEvent::UNDEFINED_INSTANT
    = std::numeric_limits<double>::quiet_NaN ();

PresentationEvent::PresentationEvent (string id, void *executionObject,
                                      ContentAnchor *anchor)
    : AnchorEvent (id, executionObject, anchor)
{

  typeSet.insert ("PresentationEvent");

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
      begin = PresentationEvent::UNDEFINED_INSTANT;
      end = PresentationEvent::UNDEFINED_INSTANT;
      duration = PresentationEvent::UNDEFINED_INSTANT;
    }
}

PresentationEvent::~PresentationEvent () { removeInstance (this); }

bool
PresentationEvent::stop ()
{
  if (currentState == EventUtil::ST_OCCURRING && numPresentations > 1)
    {
      numPresentations--;
    }

  return FormatterEvent::stop ();
}

double
PresentationEvent::getDuration ()
{
  return duration;
}

double
PresentationEvent::getRepetitionInterval ()
{
  return repetitionInterval;
}

long
PresentationEvent::getRepetitions ()
{
  return (numPresentations - 1);
}

void
PresentationEvent::setDuration (double dur)
{
  this->duration = dur;
}

void
PresentationEvent::setEnd (double e)
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
PresentationEvent::setRepetitionSettings (long repetitions,
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
PresentationEvent::getBegin ()
{
  return begin;
}

double
PresentationEvent::getEnd ()
{
  return end;
}

void
PresentationEvent::incrementOccurrences ()
{
  occurrences++;
}

bool
PresentationEvent::isUndefinedInstant (double value)
{
  return isnan (value);
}

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_EVENT_END
