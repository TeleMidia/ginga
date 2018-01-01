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

#ifndef EXTENDEDEVENTDESCRIPTOR_H_
#define EXTENDEDEVENTDESCRIPTOR_H_

#include "IMpegDescriptor.h"

#include "IExtendedEventDescriptor.h"

/*
struct Item {
        unsigned char itemDescriptionLength;
        char* itemDescriptionChar;
        unsigned char itemLength;
        char* itemChar;
};
*/

GINGA_TSPARSER_BEGIN

class ExtendedEventDescriptor : public IExtendedEventDescriptor
{
protected:
  vector<Item *> *items;
  // unsigned char descriptorTag;
  unsigned char descriptorNumber;
  unsigned char lastDescriptorNumber;
  char languageCode[3];
  unsigned char lengthOfItems;
  unsigned char textLength;
  char *textChar; // extendedDescription field on EPG table

public:
  ExtendedEventDescriptor ();
  ~ExtendedEventDescriptor ();
  unsigned char getDescriptorTag ();
  unsigned int getDescriptorLength ();
  unsigned int getDescriptorNumber ();
  unsigned int getLastDescriptorNumber ();
  string getLanguageCode ();
  unsigned int getTextLength ();
  string getTextChar ();
  vector<Item *> *getItems ();
  string getItemDescriptionChar (struct Item *item);
  string getItemChar (struct Item *item);
  void print ();
  size_t process (char *data, size_t pos);
};

GINGA_TSPARSER_END

#endif /*EXTENDEDEVENTDESCRIPTOR_H_*/
