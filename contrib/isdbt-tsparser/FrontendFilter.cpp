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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "ginga.h"
#include "FrontendFilter.h"

GINGA_TSPARSER_BEGIN

FrontendFilter::FrontendFilter (ITSFilter *f)
{
  pid = -1;
  tid = -1;
  tidExt = -1;
  feDescriptor = -1;
  filter = f;
}

FrontendFilter::~FrontendFilter () { filter = 0; }

void
FrontendFilter::setPid (int pid)
{
  this->pid = pid;
}

int
FrontendFilter::getPid ()
{
  return pid;
}

void
FrontendFilter::setTid (int tid)
{
  this->tid = tid;
}

int
FrontendFilter::getTid ()
{
  return tid;
}

void
FrontendFilter::setTidExt (int tidExt)
{
  this->tidExt = tidExt;
}

int
FrontendFilter::getTidExt ()
{
  return tidExt;
}

void
FrontendFilter::setDescriptor (int feDescripor)
{
  this->feDescriptor = feDescripor;
}

int
FrontendFilter::getDescriptor ()
{
  return feDescriptor;
}

void
FrontendFilter::receivePes (char *pes, int pesLen)
{
  if (filter != 0)
    {
      filter->receivePes (pes, pesLen, this);
    }
}

void
FrontendFilter::receiveSection (char *section, int secLen)
{
  if (filter != 0)
    {
      filter->receiveSection (section, secLen, this);
    }
}

ITSFilter *
FrontendFilter::getTSFilter ()
{
  return filter;
}

GINGA_TSPARSER_END
