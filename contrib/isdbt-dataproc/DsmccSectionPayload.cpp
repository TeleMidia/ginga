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
#include "DsmccSectionPayload.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_DATAPROC_BEGIN

DsmccSectionPayload::DsmccSectionPayload (char *data, unsigned int length)
{
  payload = data;
  payloadSize = length;
  privateDataByte = NULL;
  dsmccDescritorList = new vector<DsmccMpegDescriptor *>;
  privateDataLength = 0;
  checksum = 0;

  processSectionPayload ();
}

DsmccSectionPayload::~DsmccSectionPayload ()
{
  clearDsmccDescritor ();
  if (dsmccDescritorList != NULL)
    {
      delete (dsmccDescritorList);
    }
}

int
DsmccSectionPayload::processSectionPayload ()
{
  unsigned int pos;
  unsigned char descriptorTag;
  unsigned short descriptorSize;
  DsmccNPTReference *nptRef;
  DsmccNPTEndpoint *epRef;
  DsmccStreamMode *strMode;

  pos = 0;

  // dsmccDescriptorList()
  while (pos < payloadSize)
    {
      descriptorTag = payload[pos] & 0xFF;
      descriptorSize = (payload[pos + 1] & 0xFF) + 2;

      switch (descriptorTag)
        {
        case 0x01: // NPT Reference
          nptRef = new DsmccNPTReference ();
          nptRef->addData (payload + pos, descriptorSize);
          addDsmccDescriptor (nptRef);
          break;

        case 0x02: // NPT Endpoint
          epRef = new DsmccNPTEndpoint ();
          epRef->addData (payload + pos, descriptorSize);
          addDsmccDescriptor (epRef);
          break;

        case 0x03: // Stream Mode
          strMode = new DsmccStreamMode ();
          strMode->addData (payload + pos, descriptorSize);
          addDsmccDescriptor (strMode);
          break;

        case 0x04: // Stream Event
          clog << "DsmccSectionPayload::processSectionPayload";
          clog << " stream event." << endl;
          break;

        default:
          clog << "DsmccSectionPayload::processSectionPayload";
          clog << "DsmccMpegDescriptor unrecognized. ";
          clog << (descriptorTag & 0xFF) << endl;
          break;
        }
      pos = pos + descriptorSize;
    }

  return pos;
}

int
DsmccSectionPayload::updateStream ()
{
  /*int pos;

  if (sectionSyntaxIndicator) {
          privateIndicator = 0x00;
  } else {
          privateIndicator = 0x01;
  }

  pos = PrivateSection::updateStream();

  DsmccMpegDescriptor* desc;
  int streamLen;
  char* dataStream;

  if (tableId == 0x3A) {
          //LLCSNAP()
  } else if (tableId == 0x3B) {
          //userNetworkMessage()
  } else if (tableId == 0x3C) {
          //downloadDataMessage()
  } else if (tableId == 0x3D) {
          vector<DsmccMpegDescriptor*>::iterator i;
          if ((dsmccDescritorList != NULL) &&
                          (!dsmccDescritorList->empty())) {
                  i = dsmccDescritorList->begin();
                  while (i != dsmccDescritorList->end()) {
                          desc = *i;
                          streamLen = desc->getStream(&dataStream);
                          if ((pos + streamLen + 4) <= MAX_SECTION_SIZE) {
                                  memcpy(stream + pos, dataStream,
  streamLen);
                                  pos += streamLen;
                          } else {
                                  break;
                          }
                          ++i;
                  }
          }
  } else if (tableId == 0x3E) {
          //private_data_byte
  }

  if (!sectionSyntaxIndicator) {
          //TODO: checksum
          stream[pos++] = (checksum >> 24) & 0xFF;
          stream[pos++] = (checksum >> 16) & 0xFF;
          stream[pos++] = (checksum >> 8) & 0xFF;
          stream[pos++] = checksum & 0xFF;
  } else {
          //crc32
          Crc32 crc;
          unsigned int value = crc.crc(stream, pos);
          crc32 = value;
          stream[pos++] = (crc32 >> 24) & 0xFF;
          stream[pos++] = (crc32 >> 16) & 0xFF;
          stream[pos++] = (crc32 >> 8) & 0xFF;
          stream[pos++] = crc32 & 0xFF;
  }
  return pos;*/
  return 0;
}

int
DsmccSectionPayload::calculateSectionSize ()
{
  /*unsigned int pos = PrivateSection::calculateSectionSize();
  DsmccMpegDescriptor* desc;
  int streamLen;
  if (tableId == 0x3A) {
          //LLCSNAP()
  } else if (tableId == 0x3B) {
          //userNetworkMessage()
  } else if (tableId == 0x3C) {
          //downloadDataMessage()
  } else if (tableId == 0x3D) {
          vector<DsmccMpegDescriptor*>::iterator i;
          if ((dsmccDescritorList != NULL) &&
                          (!dsmccDescritorList->empty())) {
                  i = dsmccDescritorList->begin();
                  while (i != dsmccDescritorList->end()) {
                          desc = *i;
                          streamLen = desc->getStreamSize();
                          if ((pos + streamLen + 4) <= MAX_SECTION_SIZE) {
                                  pos += streamLen;
                          } else {
                                  break;
                          }
                          ++i;
                  }
          }
  } else if (tableId == 0x3E) {
          //private_data_byte
  }
  return pos + 4;*/
  return 0;
}

vector<DsmccMpegDescriptor *> *
DsmccSectionPayload::getDsmccDescritorList ()
{
  return dsmccDescritorList;
}

unsigned int
DsmccSectionPayload::getChecksum ()
{
  return checksum;
}

void
DsmccSectionPayload::setChecksum (unsigned int cs)
{
  checksum = cs;
}

int
DsmccSectionPayload::getPrivateDataByte (char **dataStream)
{
  if (privateDataByte != NULL)
    {
      *dataStream = privateDataByte;
      return privateDataLength;
    }
  else
    {
      return 0;
    }
}

int
DsmccSectionPayload::setPrivateDataByte (char *data, unsigned short length)
{
  if (privateDataByte != NULL)
    {
      delete (privateDataByte);
    }
  try
    {
      privateDataByte = new char[length];
    }
  catch (...)
    {
      return -1;
    }

  memcpy ((void *)privateDataByte, (void *)data, (::size_t)length);
  privateDataLength = length;
  return privateDataLength;
}

void
DsmccSectionPayload::addDsmccDescriptor (DsmccMpegDescriptor *d)
{
  dsmccDescritorList->push_back (d);
}

void
DsmccSectionPayload::removeDsmccDescriptor (unsigned char descriptorTag)
{
  DsmccMpegDescriptor *desc;
  vector<DsmccMpegDescriptor *>::iterator i;
  if ((dsmccDescritorList != NULL) && (!dsmccDescritorList->empty ()))
    {
      i = dsmccDescritorList->begin ();
      while (i != dsmccDescritorList->end ())
        {
          desc = *i;
          if (desc->getDescriptorTag () == descriptorTag)
            {
              delete (desc);
              dsmccDescritorList->erase (i);
              break;
            }
          ++i;
        }
    }
}

void
DsmccSectionPayload::clearDsmccDescritor ()
{
  DsmccMpegDescriptor *desc;
  vector<DsmccMpegDescriptor *>::iterator i;
  if ((dsmccDescritorList != NULL) && (!dsmccDescritorList->empty ()))
    {
      i = dsmccDescritorList->begin ();
      while (i != dsmccDescritorList->end ())
        {
          desc = *i;
          delete (desc);
          ++i;
        }
      dsmccDescritorList->clear ();
    }
}

GINGA_DATAPROC_END
