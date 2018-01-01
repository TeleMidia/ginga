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
#include "ParentalRatingDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_TSPARSER_BEGIN

ParentalRatingDescriptor::ParentalRatingDescriptor ()
{
  descriptorTag = 0x55;
  descriptorLength = 0;
  countryRatings = NULL;
}

ParentalRatingDescriptor::~ParentalRatingDescriptor ()
{
  vector<Parental *>::iterator i;

  if (countryRatings != NULL)
    {
      for (i = countryRatings->begin (); i != countryRatings->end (); ++i)
        {
          if ((*i) != NULL)
            {
              delete (*i);
            }
        }
      delete countryRatings;
      countryRatings = NULL;
    }
}

unsigned int
ParentalRatingDescriptor::getDescriptorLength ()
{
  return (unsigned int)descriptorLength;
}

unsigned char
ParentalRatingDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

string
ParentalRatingDescriptor::getCountryCode (Parental *parental)
{
  string str;

  str.append (parental->countryCode, 3);
  return str;
}

/*
 * Recomended age according with binary:
 * 0000: reserved
 * 0001: L
 * 0010: 10
 * 0011: 12
 * 0100: 14
 * 0101: 16
 * 0110: 18
 */
unsigned int
ParentalRatingDescriptor::getAge (Parental *parental)
{
  return (unsigned int)parental->age;
}

unsigned int
ParentalRatingDescriptor::getContentDescription (Parental *parental)
{
  return (unsigned int)parental->contentDescription;
}

vector<Parental *> *
ParentalRatingDescriptor::getCountryRatings ()
{
  return countryRatings;
}

void
ParentalRatingDescriptor::print ()
{
}

size_t
ParentalRatingDescriptor::process (char *data, size_t pos)
{
  size_t remainingBytes;
  Parental *parental;

  // clog << "ParentalRatingDescriptor::process with pos =  " << pos ;
  descriptorLength = data[pos + 1];
  // clog << " and descriptorLenght = " << (descriptorLength & 0xFF) <<endl;

  pos++;
  if (descriptorLength > 0)
    {
      countryRatings = new vector<Parental *>;
      remainingBytes = descriptorLength;
    }
  else
    {
      countryRatings = NULL;
      remainingBytes = 0;
    }

  while (remainingBytes)
    {
      pos++;

      parental = new Parental;
      // memset(parental->countryCode, 0 , 3);
      memcpy (parental->countryCode, data + pos, 3);

      pos += 3;
      parental->contentDescription = (unsigned char)((data[pos] & 0xF0) >> 4);
      parental->age = (data[pos] & 0x0F);

      // clog << "contentDescription = ";
      // clog << (parental->contentDescription & 0xFF);
      // clog << " and age = " << (parental->age & 0XFF) << endl;

      countryRatings->push_back (parental);
      remainingBytes = remainingBytes - 4;
    }

  return pos;
}

GINGA_TSPARSER_END
