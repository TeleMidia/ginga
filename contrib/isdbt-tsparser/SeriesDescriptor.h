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

#ifndef SERIESDESCRIPTOR_H_
#define SERIESDESCRIPTOR_H_

#include "IMpegDescriptor.h"

GINGA_TSPARSER_BEGIN

class SeriesDescriptor : public IMpegDescriptor
{
protected:
  unsigned short seriesId;
  unsigned char repeatLabel;
  unsigned char programPattern;
  bool expireDateValidFlag;
  unsigned char expireDate[2];
  unsigned short episodeNumber;
  unsigned short lastEpisodeNumber;
  unsigned char seriesNameLength;
  char *seriesNameChar;

public:
  SeriesDescriptor ();
  virtual ~SeriesDescriptor ();
  unsigned char getDescriptorTag ();
  unsigned int getDescriptorLength ();
  size_t process (char *data, size_t pos);
  unsigned short getSeriesId ();
  unsigned char getRepeatLabel ();
  unsigned char getProgramPattern ();
  unsigned short getEpisodeNumber ();
  unsigned short getLastEpisodeNumber ();
  string getSeriesNameChar ();
  void print ();
};

GINGA_TSPARSER_END

#endif /* SERIESDESCRIPTOR_H_ */
