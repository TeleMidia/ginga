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

GINGA_NCL_BEGIN

IntervalAnchor::IntervalAnchor (const string &id,
                                GingaTime begin,
                                GingaTime end)
  : ContentAnchor (id)
{
  typeSet.insert ("IntervalAnchor");
  this->begin = begin;
  this->end = end;
}

void
IntervalAnchor::setStrValues (const string &begin,
                              const string &end)
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

GingaTime
IntervalAnchor::getBegin ()
{
  return this->begin;
}

GingaTime
IntervalAnchor::getEnd ()
{
  return this->end;
}

void
IntervalAnchor::setBegin (GingaTime begin)
{
  this->begin = GINGA_TIME_IS_VALID (begin) ? begin : 0;
}

void
IntervalAnchor::setEnd (GingaTime end)
{
  this->end = end;
}

GINGA_NCL_END
