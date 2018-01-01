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
#include "TSPacket.h"
#include "Demuxer.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

TSPacket::TSPacket (char *packetData)
{
  isSectionType = false;
  tsaf = NULL;
  stream = new char[TS_PACKET_SIZE];
  streamUpdated = true;
  constructionFailed = !create (packetData);
}

TSPacket::TSPacket (bool sectionType, char *payload,
                    unsigned char payloadSize)
{
  tsaf = NULL;
  stream = new char[TS_PACKET_SIZE];
  streamUpdated = false;

  if (payloadSize > 0)
    {
      adaptationFieldControl = PAYLOAD_ONLY;
    }
  else
    {
      adaptationFieldControl = FUTURE_USE;
    }

  transportErrorIndication = 0;
  transportPriority = 0;
  transportScramblingControl = 0;
  isSectionType = (sectionType > 0);
  payloadUnitStartIndicator = (isSectionType > 0);
  this->payloadSize = payloadSize;
  payloadSize2 = 0;

  memset (this->payload, 0, ITSPacket::TS_PAYLOAD_SIZE);
  memcpy (this->payload, payload, payloadSize);
}

TSPacket::~TSPacket ()
{
  releaseTSAF ();
  releaseStream ();
}

void
TSPacket::releaseTSAF ()
{
  if (tsaf != NULL)
    {
      delete tsaf;
      tsaf = NULL;
    }
}

void
TSPacket::releaseStream ()
{
  if (stream != NULL)
    {
      delete stream;
      stream = NULL;
    }
}

bool
TSPacket::isConstructionFailed ()
{
  return constructionFailed;
}

bool
TSPacket::create (char *data)
{
  unsigned char pointerFieldPos;
  unsigned char payloadOffset;
  unsigned char aflen = 0;
  unsigned int pesStartCode;

  memcpy (stream, data, TS_PACKET_SIZE);

  syncByte = (data[0] & 0xFF);
  if (syncByte != TS_PACKET_SYNC_BYTE)
    {
      cout << "TSPacket::create warning! syncByte != 0x47" << endl;
      return false;
    }

  transportErrorIndication = (data[1] & 0x80) >> 7;
  payloadUnitStartIndicator = (data[1] & 0x40) >> 6;
  transportPriority = (data[1] & 0x20) >> 5;
  pid = (((data[1] & 0x1F) << 8) | (data[2] & 0xFF));
  transportScramblingControl = (data[3] & 0xC0) >> 6;
  adaptationFieldControl = (data[3] & 0x30) >> 4;
  continuityCounter = (data[3] & 0x0F);

  payloadOffset = TS_PACKET_SIZE - TS_PAYLOAD_SIZE;

  if (adaptationFieldControl == ADAPT_PAYLOAD)
    {
      aflen = data[4] & 0xFF;
      payloadOffset += (aflen + 1);
    }

  pesStartCode = (((data[payloadOffset] & 0xFF) << 16)
                  | ((data[payloadOffset + 1] & 0xFF) << 8)
                  | (data[payloadOffset + 2] & 0xFF));

  isSectionType = !((pesStartCode == 0x01) && (pid != 0x00));

  pointerField = 0;

  if (adaptationFieldControl == PAYLOAD_ONLY)
    {
      if (isSectionType)
        {
          if (payloadUnitStartIndicator)
            {
              pointerField = (data[4] & 0xFF);
              if (pointerField > (TS_PACKET_SIZE - payloadOffset))
                {
                  cout << "TSPacket::create pointer field" << endl;
                  return false;
                }

              payloadSize = TS_PAYLOAD_SIZE - pointerField - 1;
              if (payloadSize > TS_PAYLOAD_SIZE)
                {
                  cout << "TSPacket::create TS_PAYLOAD_SIZE" << endl;
                  return false;
                }

              memcpy (payload, data + pointerField + 5, payloadSize);
              payloadSize2 = TS_PAYLOAD_SIZE - payloadSize - 1;
              if (payloadSize2 > TS_PAYLOAD_SIZE)
                {
                  cout << "TSPacket::create TS_PAYLOAD_SIZE 2" << endl;
                  return false;
                }

              memcpy (payload2, data + 5, payloadSize2);
              if ((payload[0] & 0xFF) == 0xFF)
                {
                  payloadSize = 0;
                }
            }
          else
            {
              payloadSize = TS_PAYLOAD_SIZE;
              memcpy (payload, data + 4, payloadSize);
              payloadSize2 = 0;
            }
        }
      else
        {
          payloadSize = TS_PAYLOAD_SIZE;
          memcpy (payload, data + 4, payloadSize);
          payloadSize2 = 0;
        }
    }
  else if (adaptationFieldControl == ADAPT_PAYLOAD)
    {
      releaseTSAF ();

      tsaf = new TSAdaptationField (data + 4);
      if (isSectionType)
        {
          if (payloadUnitStartIndicator)
            {
              pointerFieldPos = tsaf->getAdaptationFieldLength () + 5;
              pointerField = data[pointerFieldPos] & 0xFF;
              if (pointerField > (TS_PACKET_SIZE - payloadOffset))
                {
                  clog << "TSPacket::create pointerField ";
                  clog << "(adptationField)" << endl;

                  releaseTSAF ();
                  return false;
                }

              payloadSize
                  = (TS_PACKET_SIZE - pointerField - pointerFieldPos - 1);

              if (payloadSize > TS_PAYLOAD_SIZE)
                {
                  clog << "TSPacket::create payloadSize-pusi ";
                  clog << "(adptationField)" << endl;

                  releaseTSAF ();
                  return false;
                }

              memcpy (payload, data + pointerField + pointerFieldPos + 1,
                      payloadSize);

              payloadSize2 = pointerField;
              if (payloadSize2 > TS_PAYLOAD_SIZE)
                {
                  clog << "TSPacket::create payloadSize 2-pusi ";
                  clog << "(adptationField)" << endl;

                  releaseTSAF ();
                  return false;
                }

              memcpy (payload2, data + pointerFieldPos + 1, payloadSize2);

              if ((payload[0] & 0xFF) == 0xFF)
                {
                  payloadSize = 0;
                }
            }
          else
            {
              payloadSize = (TS_PACKET_SIZE
                             - (tsaf->getAdaptationFieldLength () + 5));

              if (payloadSize > TS_PAYLOAD_SIZE)
                {
                  clog << "TSPacket::create payloadSize ";
                  clog << "(adptationField)" << endl;

                  releaseTSAF ();
                  return false;
                }

              memcpy (payload, data + tsaf->getAdaptationFieldLength () + 5,
                      payloadSize);

              payloadSize2 = 0;
            }
        }
      else
        {
          payloadSize
              = (TS_PACKET_SIZE - (tsaf->getAdaptationFieldLength () + 5));

          if (payloadSize > TS_PAYLOAD_SIZE)
            {
              clog << "TSPacket::create payloadSize pes ";
              clog << "(adptationField)" << endl;

              releaseTSAF ();
              return false;
            }

          memcpy (payload, data + tsaf->getAdaptationFieldLength () + 5,
                  payloadSize);

          payloadSize2 = 0;
        }
    }
  else if (adaptationFieldControl == NO_PAYLOAD)
    {
      payloadSize = 0;
      payloadSize2 = 0;
      releaseTSAF ();
      tsaf = new TSAdaptationField (data + 4);
    }
  return true;
}

unsigned short
TSPacket::getPid ()
{
  return pid;
}

char
TSPacket::updateStream ()
{
  if (!streamUpdated)
    {
      char value;
      char afbuffer[MAX_ADAPTATION_FIELD_SIZE];
      unsigned int len, plen;

      if (adaptationFieldControl == 0)
        {
          clog << "TSPacket::updateStream Error: Can't create packet - ";
          clog << "adaptationFieldControl == 0" << endl;
          return -1;
        }

      memset (stream, 0xFF, TS_PACKET_SIZE);

      stream[0] = TS_PACKET_SYNC_BYTE;
      value = transportErrorIndication;
      stream[1] = stream[1] & 0x7F;
      stream[1] = stream[1] | ((value << 7) & 0x80);
      stream[1] = stream[1] & 0xBF;
      value = payloadUnitStartIndicator;
      stream[1] = stream[1] | ((value << 6) & 0x40);
      stream[1] = stream[1] & 0xDF;
      value = transportPriority;
      stream[1] = stream[1] | ((value << 5) & 0x20);
      stream[1] = stream[1] & 0xE0;
      stream[1] = stream[1] | ((pid & 0x1F00) >> 8);
      stream[2] = pid & 0x00FF;
      stream[3] = stream[3] & 0x3F;
      stream[3] = stream[3] | ((transportScramblingControl << 6) & 0xC0);
      stream[3] = stream[3] & 0xCF;
      stream[3] = stream[3] | ((adaptationFieldControl << 4) & 0x30);
      stream[3] = stream[3] & 0xF0;
      stream[3] = stream[3] | (continuityCounter & 0x0F);

      if (isSectionType && payloadUnitStartIndicator)
        {
          if (adaptationFieldControl == PAYLOAD_ONLY)
            {
              // payload only
              len = payloadSize;
              if (payloadSize > 183)
                {
                  len = 183;
                }

              stream[4] = 0;
              memcpy (stream + 5, payload, len);
              return len;
            }
          else if (adaptationFieldControl == NO_PAYLOAD)
            {
              // adaptation field only
              assert (tsaf != NULL);

              tsaf->setAdaptationFieldLength (183);
              len = tsaf->getStream (afbuffer);
              memcpy (stream + 4, afbuffer, len);
              return 0;
            }
          else if (adaptationFieldControl == ADAPT_PAYLOAD)
            {
              // adaptation field followed by payload
              assert (tsaf != NULL);

              plen = (TS_PAYLOAD_SIZE - (tsaf->getAdaptationFieldLength ()
                                         + 2)); // available

              len = tsaf->getStream (afbuffer);
              memcpy (stream + 4, afbuffer, len);
              stream[len + 4] = 0;
              if (plen > payloadSize)
                {
                  plen = payloadSize;
                }
              memcpy (stream + len + 5, payload, plen);
              return plen;
            }
        }
      else if ((!isSectionType)
               || (isSectionType && (!payloadUnitStartIndicator)))
        {
          if (adaptationFieldControl == PAYLOAD_ONLY)
            {
              // payload only
              memcpy (stream + 4, payload, payloadSize);
              return payloadSize;
            }
          else if (adaptationFieldControl == NO_PAYLOAD)
            {
              // adaptation field only
              assert (tsaf != NULL);

              tsaf->setAdaptationFieldLength (183);
              len = tsaf->getStream (afbuffer);
              memcpy (stream + 4, afbuffer, len);
              return 0;
            }
          else if (adaptationFieldControl == ADAPT_PAYLOAD)
            {
              // adaptation field followed by payload
              assert (tsaf != NULL);

              plen = (TS_PAYLOAD_SIZE
                      - (tsaf->getAdaptationFieldLength () + 1));

              len = tsaf->getStream (afbuffer);
              memcpy (stream + 4, afbuffer, len);
              if (plen > payloadSize)
                {
                  plen = payloadSize;
                }
              memcpy (stream + len + 4, payload, plen);
              return plen;
            }
        }
    }

  return -1;
}

char
TSPacket::getPacketData (char **dataStream)
{
  updateStream ();
  *dataStream = stream;
  return -1;
}

void
TSPacket::getPayload (char streamData[184])
{
  memcpy ((void *)(streamData), (void *)(payload), payloadSize);
}

void
TSPacket::getPayload2 (char streamData[184])
{
  memcpy ((void *)(streamData), (void *)(payload2), payloadSize2);
}

unsigned char
TSPacket::getPayloadSize ()
{
  return payloadSize;
}

unsigned char
TSPacket::getPayloadSize2 ()
{
  return payloadSize2;
}

bool
TSPacket::getStartIndicator ()
{
  return payloadUnitStartIndicator;
}

unsigned char
TSPacket::getPointerField ()
{
  return pointerField;
}

unsigned char
TSPacket::getAdaptationFieldControl ()
{
  return adaptationFieldControl;
}

unsigned char
TSPacket::getContinuityCounter ()
{
  return continuityCounter;
}

void
TSPacket::setPid (unsigned short pid)
{
  this->pid = pid;
  if (streamUpdated)
    {
      stream[1] = stream[1] & 0xE0;
      stream[1] = stream[1] | ((pid & 0x1F00) >> 8);
      stream[2] = pid & 0x00FF;
    }
}

void
TSPacket::setContinuityCounter (unsigned int counter)
{
  continuityCounter = counter;
  if (streamUpdated)
    {
      stream[3] = stream[3] & 0xF0;
      stream[3] = stream[3] | (continuityCounter & 0x0F);
    }
}

void
TSPacket::setPacketCount (unsigned int count)
{
  packetCount = count;
}

unsigned int
TSPacket::getPacketCount ()
{
  return packetCount;
}

void
TSPacket::print ()
{
  unsigned int i;
  clog << "TS PACK" << endl;
  clog << "sync = 0x" << hex << (syncByte & 0xFF) << endl;
  clog << "pid = 0x" << hex << pid << dec << " (" << pid << ")" << endl;

  clog << "payloadSize = " << dec << (payloadSize & 0xFF);

  if (payloadSize > TS_PAYLOAD_SIZE)
    {
      clog << " (WARNING!)" << endl;
    }
  else
    {
      clog << endl;
    }

  clog << "transportErrorIndication = " << transportErrorIndication;
  clog << endl;

  clog << "payloadUnitStartIndicator = ";
  clog << payloadUnitStartIndicator << endl;

  clog << "transportPriority = " << transportPriority << endl;

  clog << "transportScramblingControl = ";
  clog << (transportScramblingControl & 0xFF) << endl;

  clog << "adaptationFieldControl = ";
  clog << adaptationFieldControl << ": ";

  if (adaptationFieldControl == FUTURE_USE)
    {
      clog << "ISO future use; packet must be discarded";
    }
  else if (adaptationFieldControl == PAYLOAD_ONLY)
    {
      clog << "no adaptation field (payload only)";
    }
  else if (adaptationFieldControl == NO_PAYLOAD)
    {
      clog << "adaptation field only (no payload)";
    }
  else if (adaptationFieldControl == ADAPT_PAYLOAD)
    {
      clog << "adaptation field followed by payload";
    }
  clog << endl;

  clog << "continuityCounter = " << (continuityCounter & 0xFF) << endl;
  clog << "packet number is near " << packetCount << endl;

  clog << "packet content: " << endl;

  for (i = 0; i < 188; i++)
    {
      clog << "0x" << hex << (stream[i] & 0xFF) << " ";
    }

  clog << endl << endl;
}

GINGA_TSPARSER_END
