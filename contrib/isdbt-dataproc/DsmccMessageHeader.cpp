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
#include "DsmccMessageHeader.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_DATAPROC_BEGIN

DsmccMessageHeader::DsmccMessageHeader () {}

int
DsmccMessageHeader::readMessageFromFile (const string &fileName, unsigned int pid)
{
  FILE *fd;
  int rval;
  char bytes[12];

  this->pid = pid;
  memset (bytes, 0, sizeof (bytes));
  fd = fopen (fileName.c_str (), "rb");
  if (fd != NULL)
    {
      this->fileName = fileName;

      rval = (int) fread ((void *)&(bytes[0]), 1, 12, fd);
      if (rval == 12)
        {
          this->protocolDiscriminator = (bytes[0] & 0xFF);
          this->dsmccType = (bytes[1] & 0xFF);
          this->messageId = ((bytes[2] & 0xFF) << 8) | (bytes[3] & 0xFF);
          this->transactionId
              = ((bytes[4] & 0xFF) << 24) | ((bytes[5] & 0xFF) << 16)
                | ((bytes[6] & 0xFF) << 8) | (bytes[7] & 0xFF);

          // reserved
          if ((bytes[8] & 0xFF) != 0xFF)
            {
              clog << "Warning: reserved field shall be equal to 0xFF";
              clog << endl;
            }

          this->adaptationLength = (bytes[9] & 0xFF);
          this->messageLength
              = ((bytes[10] & 0xFF) << 8) | (bytes[11] & 0xFF);
        }
      else
        {
          clog << "Warning: cannot read 12 bytes, " << rval;
          clog << " readed." << endl;
        }
    }
  else
    {
      clog << "DsmccMessageHeader::DsmccMessageHeader - Message header "
              "error: "
              "could not open file ";
      clog << fileName.c_str () << endl;
      return -1;
    }
  fclose (fd);
  return 0;
}

unsigned int
DsmccMessageHeader::getESId ()
{
  return this->pid;
}

string
DsmccMessageHeader::getFileName ()
{
  return fileName;
}

unsigned int
DsmccMessageHeader::getDsmccType ()
{
  return dsmccType;
}

unsigned int
DsmccMessageHeader::getMessageId ()
{
  return messageId;
}

unsigned int
DsmccMessageHeader::getTrasnsactionId ()
{
  return transactionId;
}

unsigned int
DsmccMessageHeader::getAdaptationLength ()
{
  return adaptationLength;
}

unsigned int
DsmccMessageHeader::getMessageLength ()
{
  return messageLength;
}

void
DsmccMessageHeader::print ()
{
  clog << "fileName = " << fileName.c_str () << endl;
  clog << "protocolDiscriminator = " << protocolDiscriminator << endl;
  clog << "dsmccType = " << dsmccType << endl;
  clog << "messageId = " << messageId << endl;
  clog << "transactionId = " << transactionId << endl;
  clog << "adaptationLength = " << adaptationLength << endl;
  clog << "messageLength = " << messageLength << endl;
}

GINGA_DATAPROC_END
