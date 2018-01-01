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
#include "AudioComponentDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

AudioComponentDescriptor::AudioComponentDescriptor ()
{
  descriptorTag = 0xC4;
  ESMultiLingualFlag = false;
  componentTag = 0;
  componentType = 0;
  descriptorLength = 0;
  mainComponentFlag = false;
  qualityIndicator = 0;
  samplingRate = 0;
  simulcastGroupTag = 0;
  streamContent = 0;
  streamType = 0;
  textLength = 0;
  languageCode[0] = 0;
  languageCode[1] = 0;
  languageCode2 = NULL;
  textChar = NULL;
}

AudioComponentDescriptor::~AudioComponentDescriptor ()
{
  if (textChar != NULL)
    {
      delete textChar;
      textChar = NULL;
    }

  if (languageCode2 != NULL)
    {
      delete languageCode2;
      languageCode2 = NULL;
    }
}

unsigned char
AudioComponentDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

unsigned int
AudioComponentDescriptor::getDescriptorLength ()
{
  return (unsigned int)descriptorLength;
}

unsigned char
AudioComponentDescriptor::getStreamContent ()
{
  return streamContent;
}

string
AudioComponentDescriptor::getLanguageCode ()
{
  string str;
  str.append (languageCode, 3);
  return str;
}

string
AudioComponentDescriptor::getLanguageCode2 ()
{
  string str;

  if (languageCode2 == NULL)
    {
      return "";
    }
  str.append (languageCode2, 3);
  return str;
}

string
AudioComponentDescriptor::getTextChar ()
{
  string str;

  if (textChar == NULL)
    {
      return "";
    }

  str.append (textChar, textLength);

  return str;
}

unsigned char
AudioComponentDescriptor::getComponentTag ()
{
  return componentTag;
}

unsigned char
AudioComponentDescriptor::getComponentType ()
{
  return componentType;
}

unsigned char
AudioComponentDescriptor::getStreamType ()
{
  return streamType;
}

unsigned char
AudioComponentDescriptor::getSimulcastGroupTag ()
{
  return simulcastGroupTag;
}

bool
AudioComponentDescriptor::getESMultiLingualFlag ()
{
  return ESMultiLingualFlag;
}

bool
AudioComponentDescriptor::getMainComponentFlag ()
{
  return mainComponentFlag;
}

unsigned char
AudioComponentDescriptor::getQualityIndicator ()
{
  return qualityIndicator;
}

unsigned char
AudioComponentDescriptor::getSamplingRate ()
{
  return samplingRate;
}

void
AudioComponentDescriptor::print ()
{
  // TODO: WORSE CODE EVER MADE: FIX IT
  /*		clog << "AudioComponentDescriptor::print" << endl;
                  clog << " -languageCode = " << getLanguageCode() << endl;
                  if (ESMultiLingualFlag) {
                          clog << " -languageCode2 = " << getLanguageCode2()
     <<
     endl;
                  }*/
}

size_t
AudioComponentDescriptor::process (char *data, size_t pos)
{
  // clog << "Audio Component process beginning with pos =  " << pos;

  descriptorLength = data[pos + 1];
  // cout <<" and length = " << (descriptorLength & 0xFF) <<endl;
  if (descriptorLength == 0)
    {
      pos++;
      return pos;
    }

  pos += 2; // pos = 20
  // jumping reserved_future_use (first 4 bits of data[pos]
  streamContent = (data[pos] & 0x0F); // last 4 bits of data[pos]
  pos += 1;                           // pos = 21

  componentType = data[pos];
  pos++; // pos = 22

  componentTag = data[pos];
  pos++; // pos=23

  streamType = data[pos];
  pos++; // pos=24

  simulcastGroupTag = data[pos];
  pos++; // 25

  ESMultiLingualFlag = ((data[pos] & 0x80) >> 7);
  mainComponentFlag = ((data[pos] & 0x40) >> 6);
  qualityIndicator = ((data[pos] & 0x30) >> 4);
  samplingRate = ((data[pos] & 0x0C));
  pos++; // pos= 26

  memcpy (languageCode, data + pos, 3);
  pos += 3; // pos = 29

  if (ESMultiLingualFlag)
    {
      if (languageCode2 != NULL)
        {
          delete languageCode2;
        }

      languageCode2 = new char[3];
      memcpy (languageCode2, data + pos, 3);
      pos += 3;
      textLength = descriptorLength - 12;
    }
  else
    {
      textLength = descriptorLength - 9;
      // 9 bytes since descriptorLenght until languageCode
    }

  if (textLength > 0)
    {
      if (textChar != NULL)
        {
          delete textChar;
        }
      textChar = new char[textLength];
      memset (textChar, 0, textLength);
      memcpy (textChar, data + pos, textLength);
    }

  pos += textLength;
  return pos;
}

GINGA_TSPARSER_END
