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
#include "TOT.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

TOT::TOT ()
{
  memset (UTC3Time, 0, 5);
  descriptorsLoopLength = 0;
  descriptors = new vector<IMpegDescriptor *>;
}

TOT::~TOT ()
{
  vector<IMpegDescriptor *>::iterator i;

  if (descriptors != NULL)
    {
      for (i = descriptors->begin (); i != descriptors->end (); ++i)
        {
          delete (*i);
        }

      delete descriptors;
      descriptors = NULL;
    }
}

string
TOT::getUTC3TimeStr ()
{
  stringstream str;

  str << time.tm_mday << SystemCompat::getIUriD ();
  str << time.tm_mon + 1 << " ";
  // tm_mon goes from 0 to 11, so + 1 to print month in 1-12.
  str << time.tm_hour << ":";
  str << time.tm_min;

  return str.str ();
}

struct tm
TOT::getUTC3TimeTm ()
{
  return time;
}

struct tm
TOT::convertMJDtoUTC (unsigned int mjd)
{
  struct tm time;
  int year, month, day, k, weekDay;

  year = (int) ((mjd - 15078.2) / 365.25);
  month = (int) ((mjd - 14956.1 - (int)(year * 365.25)) / 30.6001);
  day = (int) mjd - 14956 - ((int)(year * 365.25)) - ((int)(month * 30.6001));

  if (month == 14 || month == 15)
    {
      k = 1;
    }
  else
    {
      k = 0;
    }

  year = year + k;
  month = month - 1 - k * 12;
  weekDay = (((mjd + 2) % 7) + 1);

  time.tm_year = year;
  time.tm_mon = month - 1;
  /*tm.mon goes from 0(January) to 11 (December).
   *if month is february ('month'= 2), tm.mon is 1
   */
  time.tm_mday = day;
  time.tm_wday = weekDay;
  time.tm_hour = 0;
  time.tm_min = 0;
  time.tm_sec = 0;

  return time;
}

int
TOT::convertBCDtoDecimal (int bcd)
{
  return ((bcd >> 4) * 10) + bcd % 16;
}

void
TOT::calculateTime ()
{
  unsigned int mjd;

  if (((UTC3Time[0] & 0xFF) == 0xFF) && ((UTC3Time[1] & 0xFF) == 0xFF)
      && ((UTC3Time[2] & 0xFF) == 0xFF) && ((UTC3Time[3] & 0xFF) == 0xFF)
      && ((UTC3Time[4] & 0xFF) == 0xFF))
    {
      return;
    }

  mjd = (((UTC3Time[0] << 8) & 0xFF00) | (UTC3Time[1] & 0xFF));

  time = convertMJDtoUTC (mjd);

  time.tm_hour = convertBCDtoDecimal ((int)UTC3Time[2]);
  time.tm_min = convertBCDtoDecimal ((int)UTC3Time[3]);
  time.tm_sec = convertBCDtoDecimal ((int)UTC3Time[4]);
}

vector<IMpegDescriptor *> *
TOT::getDescriptors ()
{
  return descriptors;
}

void
TOT::print ()
{
  vector<IMpegDescriptor *>::iterator i;

  clog << "TOT::print printing..." << endl;
  clog << "UTC3Time: " << getUTC3TimeStr () << endl;
  /*
                  for (i = descriptors->begin(); i != descriptors->end();
     ++i)
     {
                          (*i)->print();
                  }
  */
}

void
TOT::process (void *payloadBytes, unsigned int payloadSize)
{
  char *data;
  size_t pos, localpos;
  unsigned short remainingBytes, value;
  LocalTimeOffsetDescriptor *localDescriptor;

  clog << "TOT::process" << endl;

  pos = 0;
  data = new char[payloadSize];
  memcpy ((void *)&(data[0]), payloadBytes, payloadSize);

  memcpy (UTC3Time, data, 5);
  pos += 5;
  calculateTime ();

  descriptorsLoopLength
      = ((((data[pos] & 0x0F) << 8) & 0xFF00) | (data[pos + 1] & 0xFF));

  pos += 2;

  remainingBytes = descriptorsLoopLength;

  clog << "TOT::process going to descriptors with descriptorsLength:";
  clog << descriptorsLoopLength << endl;

  while (remainingBytes)
    {
      value = ((data[pos + 1] & 0xFF) + 2);
      remainingBytes -= value;

      if (data[pos] == DT_LOCAL_TIME_OFFSET)
        {
          clog << "TOT::process LocalTimeOffsetDescriptor" << endl;
          localDescriptor = new LocalTimeOffsetDescriptor ();
          localpos = localDescriptor->process (data, pos);
          pos += value;
          clog << "TOT::process Desc finished with pos: " << pos;
          clog << " and localpos: " << localpos << endl;

          descriptors->push_back ((IMpegDescriptor *)localDescriptor);
        }
    }
}

GINGA_TSPARSER_END
