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

#ifndef ITSPACKET_
#define ITSPACKET_

GINGA_TSPARSER_BEGIN

class ITSPacket
{
public:
  static const unsigned char TS_PACKET_SIZE = 188;
  static const unsigned char TS_PAYLOAD_SIZE = TS_PACKET_SIZE - 4;
  static const unsigned char TS_PACKET_SYNC_BYTE = 0x47;
  virtual ~ITSPacket (){};
  virtual bool isConstructionFailed () = 0;
  virtual unsigned short getPid () = 0;
  virtual char getPacketData (char **dataStream) = 0;
  virtual void getPayload (char streamData[TS_PAYLOAD_SIZE]) = 0;
  virtual unsigned char getPayloadSize () = 0;
  virtual void getPayload2 (char streamData[TS_PAYLOAD_SIZE]) = 0;
  virtual unsigned char getPayloadSize2 () = 0;
  virtual bool getStartIndicator () = 0;
  virtual unsigned char getPointerField () = 0;
  virtual unsigned char getAdaptationFieldControl () = 0;
  virtual unsigned char getContinuityCounter () = 0;
  virtual void setContinuityCounter (unsigned int counter) = 0;
  virtual void print () = 0;
  virtual void setPacketCount (unsigned int count) = 0;
  virtual unsigned int getPacketCount () = 0;
};

GINGA_TSPARSER_END

#endif /*TSPACKET_*/
