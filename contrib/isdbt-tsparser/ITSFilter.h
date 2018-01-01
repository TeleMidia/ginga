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

#ifndef ITSFILTER_H_
#define ITSFILTER_H_

#include "isdbt-tuner/IFrontendFilter.h"
using namespace ::ginga::tuner;

#include "ITSPacket.h"

GINGA_TSPARSER_BEGIN

class ITSFilter
{
public:
  virtual ~ITSFilter (){};

  virtual void addPid (int pid) = 0;
  virtual void receiveTSPacket (ITSPacket *pack) = 0;

  virtual void receiveSection (char *buf, int len, IFrontendFilter *filter)
      = 0;

  virtual void receivePes (char *buf, int len, IFrontendFilter *filter) = 0;

  virtual string setDestinationUri (const string &dstUri) = 0;
  virtual void setSourceUri (const string &srcUri, bool isPipe) = 0;
};

GINGA_TSPARSER_END

#endif /*ITSFILTER_H_*/
