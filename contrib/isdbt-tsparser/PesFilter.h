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

#ifndef PesFilter_H_
#define PesFilter_H_

#include "isdbt-tuner/IFrontendFilter.h"
using namespace ::ginga::tuner;

#include "IDemuxer.h"
#include "ITSFilter.h"

GINGA_TSPARSER_BEGIN

class PesFilter : public IFrontendFilter
{
private:
  int pid;
  int tid;
  int tidExt;
  int feDescriptor;
  ITSFilter *filter;

public:
  PesFilter (ITSFilter *filter);
  virtual ~PesFilter ();

  void setPid (int pid);
  int getPid ();

  void setTid (int tid);
  int getTid ();

  void setTidExt (int tidExt);
  int getTidExt ();

  void setDescriptor (int feDescripor);
  int getDescriptor ();
  void receivePes (char *pes, int pesLen);
  void receiveSection (char *section, int secLen);
};

GINGA_TSPARSER_END

#endif /*PesFilter_H_*/
