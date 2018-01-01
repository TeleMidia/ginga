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
#include "PesFilter.h"

GINGA_TSPARSER_BEGIN

PesFilter::PesFilter (ITSFilter *f)
{
  pid = -1;
  tid = -1;
  tidExt = -1;
  feDescriptor = -1;
  filter = f;
}

PesFilter::~PesFilter () {}

void
PesFilter::setPid (int pid)
{
  this->pid = pid;
}

int
PesFilter::getPid ()
{
  return pid;
}

void
PesFilter::setTid (int tid)
{
  this->tid = tid;
}

int
PesFilter::getTid ()
{
  return tid;
}

void
PesFilter::setTidExt (int tidExt)
{
  this->tidExt = tidExt;
}

int
PesFilter::getTidExt ()
{
  return tidExt;
}

void
PesFilter::setDescriptor (int feDescripor)
{
  this->feDescriptor = feDescripor;
}

int
PesFilter::getDescriptor ()
{
  return feDescriptor;
}

void
PesFilter::receivePes (char *pes, int pesLen)
{
  filter->receivePes (pes, pesLen, this);
}

void
PesFilter::receiveSection (char *section, int secLen)
{
  receivePes (section, secLen);
}

GINGA_TSPARSER_END
