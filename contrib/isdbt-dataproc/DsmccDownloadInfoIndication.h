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

#ifndef DOWNLOADINFOINDICATION_H_
#define DOWNLOADINFOINDICATION_H_

#include "DsmccModule.h"
#include "DsmccMessageHeader.h"

GINGA_DATAPROC_BEGIN

class DsmccDownloadInfoIndication
{
private:
  unsigned int downloadId;
  unsigned int blockSize;
  unsigned int numberOfModules;

  // moduleId to module
  map<unsigned int, DsmccModule *> modules;

  DsmccMessageHeader *header;

public:
  DsmccDownloadInfoIndication ();
  virtual ~DsmccDownloadInfoIndication ();

  int processMessage (DsmccMessageHeader *message);

  unsigned int getDonwloadId ();
  unsigned int getBlockSize ();
  unsigned int getNumberOfModules ();
  void getInfo (map<unsigned int, DsmccModule *> *ocInfo);

  void print ();
};

GINGA_DATAPROC_END

#endif /*DOWNLOADINFOINDICATION_H_*/
