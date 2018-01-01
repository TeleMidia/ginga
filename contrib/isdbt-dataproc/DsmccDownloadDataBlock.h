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

#ifndef DOWNLOADDATABLOCK_H_
#define DOWNLOADDATABLOCK_H_

#include "system/Thread.h"
using namespace ::ginga::system;

#include "DsmccModule.h"
#include "DsmccMessageHeader.h"

GINGA_DATAPROC_BEGIN

class DsmccDownloadDataBlock
{
private:
  unsigned int moduleId;
  unsigned int moduleVersion;
  DsmccMessageHeader *header;

public:
  DsmccDownloadDataBlock (DsmccMessageHeader *message);
  virtual ~DsmccDownloadDataBlock ();

  int processDataBlock (map<unsigned int, DsmccModule *> *mods);
  unsigned int getModuleId ();
  unsigned int getModuleVersion ();
  void print ();
};

GINGA_DATAPROC_END

#endif /*DOWNLOADDATABLOCK_H_*/
