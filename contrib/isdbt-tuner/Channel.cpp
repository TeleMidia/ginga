/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "ginga.h"
#include "Channel.h"

GINGA_TUNER_BEGIN

Channel::Channel ()
{
  id = 0;
  name = "";
  frequency = 0;
  isFullSegment = false;
}

Channel::~Channel () {}

short
Channel::getId ()
{
  return id;
}

void
Channel::setId (short id)
{
  this->id = id;
}

string
Channel::getName ()
{
  return name;
}

void
Channel::setName (const string &name)
{
  this->name = name;
}

unsigned int
Channel::getFrequency ()
{
  return frequency;
}

void
Channel::setFrequency (unsigned int freq)
{
  clog << "Channel::setFrequency '" << freq << "'" << endl;
  frequency = freq;
}

bool
Channel::isFullSeg ()
{
  return isFullSegment;
}

void
Channel::setSegment (bool isFullSeg)
{
  isFullSegment = isFullSeg;
}

GINGA_TUNER_END
