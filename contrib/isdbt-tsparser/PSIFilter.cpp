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
#include "PSIFilter.h"

GINGA_TSPARSER_BEGIN

PSIFilter::PSIFilter (IDemuxer *dm)
{
  pid = -1;
  tid = -1;
  tidExt = -1;
  feDescriptor = -1;
  demuxer = dm;
}

PSIFilter::~PSIFilter () {}

void
PSIFilter::setPid (int pid)
{
  this->pid = pid;
}

int
PSIFilter::getPid ()
{
  return pid;
}

void
PSIFilter::setTid (int tid)
{
  this->tid = tid;
}

int
PSIFilter::getTid ()
{
  return tid;
}

void
PSIFilter::setTidExt (int tidExt)
{
  this->tidExt = tidExt;
}

int
PSIFilter::getTidExt ()
{
  return tidExt;
}

void
PSIFilter::setDescriptor (int feDescripor)
{
  this->feDescriptor = feDescripor;
}

int
PSIFilter::getDescriptor ()
{
  return feDescriptor;
}

void
PSIFilter::receivePes (unused (char *pes), unused (int pesLen))
{
}

void
PSIFilter::receiveSection (char *section, int secLen)
{
  demuxer->receiveSection (section, secLen, this);
}

GINGA_TSPARSER_END
