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
#include "IntervalAnchor.h"

#include "util/functions.h"
using namespace ::ginga::util;

GINGA_NCL_BEGIN

// if the representation changes, update isObjectDuration method
const double IntervalAnchor::OBJECT_DURATION = (double)INFINITY;

IntervalAnchor::IntervalAnchor (const string &id, double begin, double end)
    : ContentAnchor (id)
{
  typeSet.insert ("IntervalAnchor");
  this->begin = 0;
  setEnd (end);
  setBegin (begin);
}

void
IntervalAnchor::setStrValues (const string &begin, const string &end)
{
  this->strBegin = begin;
  this->strEnd = end;
}

string
IntervalAnchor::getStrBegin ()
{
  return this->strBegin;
}

string
IntervalAnchor::getStrEnd ()
{
  return this->strEnd;
}

double
IntervalAnchor::getBegin ()
{
  return begin;
}

double
IntervalAnchor::getEnd ()
{
  return end;
}

void
IntervalAnchor::setBegin (double b)
{
  bool isBDur = isObjectDuration (b);
  bool isEDur = isObjectDuration (end);

  if (b < 0 && !isBDur)
    {
      begin = 0;
    }
  else if ((!isBDur && !isEDur && b > end) || (isBDur && !isEDur))
    {
      begin = end;
    }
  else
    {
      begin = b;
    }
}

void
IntervalAnchor::setEnd (double e)
{
  bool isEDur = isObjectDuration (e);

  if (e < 0 && !isEDur)
    {
      end = IntervalAnchor::OBJECT_DURATION;
    }
  else if ((!isEDur && !isObjectDuration (begin) && e < begin))
    {
      end = begin;
    }
  else
    {
      end = e;
    }
}

bool
IntervalAnchor::isObjectDuration (double value)
{
  return isinf (value);
}

GINGA_NCL_END
