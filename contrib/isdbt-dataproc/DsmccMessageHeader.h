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

#ifndef DSMCC_MESSAGE_HEADER_H
#define DSMCC_MESSAGE_HEADER_H

#include "ginga.h"

GINGA_DATAPROC_BEGIN

class DsmccMessageHeader
{
private:
  // origin es is
  unsigned int pid;
  string fileName;

  //
  // ISO/IEC 13818-6 DSM-CC MESSAGE HEADER
  //

  // Must be 0x11 for DSMCC message. (1 byte)
  unsigned int protocolDiscriminator;

  // Defines message type. (1 byte)
  //   0x01    Configuration message
  //   0x02    Session message
  //   0x03    Download message
  //   0x04    Channel change
  //   0x05    Passthru message
  unsigned int dsmccType;

  // Indicates the message type depending on
  // DSMCC type. (2 bytes)
  //   0x1001    Dowload info request
  //   0x1002    Download info response or DII
  //   0x1003    Download data block (actual data)
  //   0x1004    Download data request
  //   0x1005    Download cancel
  //   0x1006    Download server initiate (DSI)
  unsigned int messageId;

  // Used for session integrity and
  // error. (4 bytes)
  unsigned int transactionId;

  // RESERVED BYTE, must be 0xFF.

  // Indicates the lenght in bytes of the
  // adaptation header. (1 byte)
  unsigned int adaptationLength;

  // The total lenght in bytes of this message
  // following this field.  This lenght includes
  // any adaptation headers. (2 bytes)
  unsigned int messageLength;

public:
  DsmccMessageHeader ();
  int readMessageFromFile (const string &fileName, unsigned int pid);
  unsigned int getESId ();
  string getFileName ();
  unsigned int getDsmccType ();
  unsigned int getMessageId ();
  unsigned int getTrasnsactionId ();
  unsigned int getAdaptationLength ();
  unsigned int getMessageLength ();
  void print ();
};

GINGA_DATAPROC_END

#endif /* DSMCC_MESSAGE_HEADER_H */
