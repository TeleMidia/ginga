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

#ifndef DSMCCSectionPayload_H_
#define DSMCCSectionPayload_H_

#include "DsmccCrc.h"
#include "DsmccMpegDescriptor.h"

#include "DsmccNPTReference.h"
#include "DsmccNPTEndpoint.h"
#include "DsmccStreamMode.h"

GINGA_DATAPROC_BEGIN

class DsmccSectionPayload
{
protected:
  unsigned short privateDataLength;
  char *privateDataByte;

  vector<DsmccMpegDescriptor *> *dsmccDescritorList;
  unsigned int checksum;

  char *payload;
  unsigned int payloadSize;

  int processSectionPayload ();
  int updateStream ();
  int calculateSectionSize ();

  void clearDsmccDescritor ();
  void deleteDescriptor (DsmccMpegDescriptor *desc);

  /*
   * if (tableId == 3D) {
   *     tableIdExtension = dataEventId & eventMsgGroupId
   * }
   */
public:
  DsmccSectionPayload (char *data, unsigned int length);
  virtual ~DsmccSectionPayload ();

  vector<DsmccMpegDescriptor *> *getDsmccDescritorList ();
  unsigned int getChecksum ();
  void setChecksum (unsigned int cs);
  int getPrivateDataByte (char **dataStream);
  int setPrivateDataByte (char *data, unsigned short length);

  void addDsmccDescriptor (DsmccMpegDescriptor *d);
  void removeDsmccDescriptor (unsigned char descriptorTag);
};

GINGA_DATAPROC_END

#endif /* DSMCCSectionPayload_H_ */
