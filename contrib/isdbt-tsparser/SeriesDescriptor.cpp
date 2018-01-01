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
#include "SeriesDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

SeriesDescriptor::SeriesDescriptor ()
{
  descriptorTag = 0xD5;
  descriptorLength = 0;
  episodeNumber = 0;
  seriesNameChar = NULL;
  expireDate[0] = 0;
  expireDate[1] = 0;
  expireDateValidFlag = false;
  lastEpisodeNumber = 0;
  programPattern = 0;
  repeatLabel = 0;
  seriesId = 0;
  seriesNameChar = NULL;
  seriesNameLength = 0;
}

SeriesDescriptor::~SeriesDescriptor ()
{
  if (seriesNameChar != 0)
    {
      delete seriesNameChar;
      seriesNameChar = NULL;
    }
}

unsigned char
SeriesDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

unsigned int
SeriesDescriptor::getDescriptorLength ()
{
  return (unsigned int)descriptorLength;
}

unsigned short
SeriesDescriptor::getSeriesId ()
{
  return seriesId;
}

unsigned char
SeriesDescriptor::getRepeatLabel ()
{
  return repeatLabel;
}

unsigned char
SeriesDescriptor::getProgramPattern ()
{
  return programPattern;
}

unsigned short
SeriesDescriptor::getEpisodeNumber ()
{
  return episodeNumber;
}

unsigned short
SeriesDescriptor::getLastEpisodeNumber ()
{
  return lastEpisodeNumber;
}

string
SeriesDescriptor::getSeriesNameChar ()
{
  string str;

  if (seriesNameChar == NULL)
    {
      return "";
    }
  str.append (seriesNameChar, seriesNameLength);
  return str;
}

void
SeriesDescriptor::print ()
{
  clog << "SeriesDescriptor::print printing...";
  clog << " -seriesId = " << seriesId << endl;
  clog << " -descriptorLength = " << getDescriptorLength () << endl;
  clog << " -episodeNumber = " << episodeNumber << endl;
  clog << " -lastEpisodeNumber = " << lastEpisodeNumber << endl;
  clog << " -seriesNameChar = " << getSeriesNameChar () << endl;
}
size_t
SeriesDescriptor::process (char *data, size_t pos)
{
  descriptorLength = data[pos + 1];
  pos += 2;

  seriesId = (((data[pos] << 8) & 0xFF00) | (data[pos + 1] & 0xFF));
  pos += 2; // pos = 22

  repeatLabel = ((data[pos] & 0xF0) >> 4);
  programPattern = ((data[pos] & 0x0E) >> 1);
  expireDateValidFlag = (data[pos] & 0x01);
  pos++; // pos = 23

  expireDate[0] = data[pos];
  expireDate[1] = data[pos + 1];
  pos += 2; // pos = 25

  episodeNumber
      = (((data[pos] << 4) & 0xFF00) | (data[pos + 1] & 0xF0) >> 4);

  lastEpisodeNumber
      = (((data[pos + 1] & 0x0F) << 8) | (data[pos + 2] & 0xFF) >> 8);

  pos += 3; // pos = 28
  seriesNameLength = descriptorLength - 8;
  if (seriesNameLength > 0)
    {
      seriesNameChar = new char[seriesNameLength];
      memset (seriesNameChar, 0, seriesNameLength);
      memcpy (seriesNameChar, data + pos, seriesNameLength);
    }
  return pos;
}

GINGA_TSPARSER_END
