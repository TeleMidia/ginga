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

#ifndef TSPACKET_
#define TSPACKET_

#include "Pat.h"

#include "ITSPacket.h"
#include "TSAdaptationField.h"

GINGA_TSPARSER_BEGIN

class TSPacket : public ITSPacket
{
protected:
  //
  // ISO/IEC 13818-1 TS PACKET HEADER
  //

  // Value must be 0x47. (1 byte)
  char syncByte;

  // Error indicator flag. (1 bit)
  //   1    Error
  //   0    OK
  bool transportErrorIndication;

  // Indicator that carries PES packets or PSI data.
  // (1 bit)
  //   1    Indicates that the payload of this TS packet
  //        will start with the first byte of a PES
  //        packet.
  //   0    Indicates no PES packet shall start in this
  //        TS packet.
  bool payloadUnitStartIndicator;

  // Indicates that the associated packet is of greater
  // priority than other packets having the same PID.
  bool transportPriority;

  // Indicates the type of the data stored in
  // the payload.  (13 bits)
  //   0x00    PAT
  //   0x01    CAT
  //   0x02    TS description table
  //   0x03    IPMP control
  unsigned short pid;

  // Indicates the scrambling mode of the TS packet.
  // (2 bits)
  //   00    Not scrambled
  //   01    User Defined
  //   10    User Defined
  //   11    User Defined
  unsigned char transportScramblingControl;

  // Indicates whether this TS packet header is followed
  // by an adaptation field and/or payload. (2 bits)
  //   00    ISO future use; packet must be discarded
  //   01    no adaptation field (payload only)
  //   10    adaptation field only (no payload)
  //   11    adaptation field followed by payload
  static const unsigned int FUTURE_USE = 0;
  static const unsigned int PAYLOAD_ONLY = 1;
  static const unsigned int NO_PAYLOAD = 2;
  static const unsigned int ADAPT_PAYLOAD = 3;
  unsigned int adaptationFieldControl;

  // Incrementing with each TS packet with the same PID.
  // This field shall not be incremented when the
  // adaptationFieldControl is set to 00 or 10. (4 bits)
  unsigned char continuityCounter;

  // TODO: AdaptationField?

  // Number of bytes immediately following this
  // pointerField until the first byte of the first
  // section that is present in the payload.  The
  // maximum value is 182.  If the
  // payloadUnitStartIndicator is set to 1, then the
  // first byte of the payload is the pointerField
  // itself.
  unsigned char pointerField;

  bool streamUpdated;
  unsigned int packetCount;

  // Packet payload data.  (184 bytes) If pointerField
  // is present, then the payload size is 183 bytes.
  char *stream;
  char payload[TS_PAYLOAD_SIZE];
  char payload2[TS_PAYLOAD_SIZE];
  TSAdaptationField *tsaf;
  unsigned char payloadSize;
  unsigned char payloadSize2;
  bool isSectionType;
  bool constructionFailed;

public:
  // Constructor to read a TS packet
  TSPacket (char *packetData);

  // Constructor to encode a TS packet
  TSPacket (bool sectionType, char *payload, unsigned char payloadSize);

  virtual ~TSPacket ();

private:
  void releaseTSAF ();
  void releaseStream ();

protected:
  bool create (char data[TS_PACKET_SIZE]);
  char updateStream ();

public:
  bool isConstructionFailed ();
  unsigned short getPid ();
  char getPacketData (char **dataStream);
  void getPayload (char streamData[TS_PAYLOAD_SIZE]);
  void getPayload2 (char streamData[TS_PAYLOAD_SIZE]);
  unsigned char getPayloadSize ();
  unsigned char getPayloadSize2 ();
  bool getStartIndicator ();
  unsigned char getPointerField ();
  unsigned char getAdaptationFieldControl ();
  unsigned char getContinuityCounter ();
  void setPid (unsigned short pid);
  void setContinuityCounter (unsigned int counter);
  void print ();
  void setPacketCount (unsigned int count);
  unsigned int getPacketCount ();
};

GINGA_TSPARSER_END

#endif /*TSPACKET_*/
