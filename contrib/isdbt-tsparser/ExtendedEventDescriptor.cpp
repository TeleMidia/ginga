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
#include "ExtendedEventDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

ExtendedEventDescriptor::ExtendedEventDescriptor ()
{
  descriptorTag = 0x4E;
  descriptorLength = 0;
  descriptorNumber = 0;
  lengthOfItems = 0;
  textChar = NULL;
  textLength = 0;
  items = NULL;
}

ExtendedEventDescriptor::~ExtendedEventDescriptor ()
{
  if (textChar != NULL)
    {
      delete textChar;
      textChar = NULL;
    }
  if (items != NULL)
    {
      vector<Item *>::iterator i;
      for (i = items->begin (); i != items->end (); ++i)
        {
          if ((*i) != NULL)
            {
              if ((*i)->itemChar != NULL)
                {
                  delete ((*i)->itemChar);
                }

              if ((*i)->itemDescriptionChar != NULL)
                {
                  delete ((*i)->itemDescriptionChar);
                }
              delete (*i);
            }
        }
      delete items;
      items = NULL;
    }
}
unsigned char
ExtendedEventDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

unsigned int
ExtendedEventDescriptor::getDescriptorLength ()
{
  return (unsigned int)descriptorLength;
}

unsigned int
ExtendedEventDescriptor::getDescriptorNumber ()
{
  return (unsigned int)descriptorNumber;
}

unsigned int
ExtendedEventDescriptor::getLastDescriptorNumber ()
{
  return (unsigned int)lastDescriptorNumber;
}
string
ExtendedEventDescriptor::getLanguageCode ()
{
  string str;
  str.append (languageCode, 3);
  return str;
}

string
ExtendedEventDescriptor::getTextChar ()
{
  string str;

  if (textChar == NULL)
    {
      return "";
    }
  str.append (textChar, textLength);
  return str;
}

unsigned int
ExtendedEventDescriptor::getTextLength ()
{
  return (unsigned int)textLength;
}
vector<Item *> *
ExtendedEventDescriptor::getItems ()
{
  return items;
}
string
ExtendedEventDescriptor::getItemDescriptionChar (struct Item *item)
{
  string str;

  if (item->itemDescriptionChar == NULL)
    {
      return "";
    }
  str.append (item->itemDescriptionChar, item->itemDescriptionLength);
  return str;
}
string
ExtendedEventDescriptor::getItemChar (struct Item *item)
{
  string str;
  if (item->itemChar == NULL)
    {
      return "";
    }
  str.append (item->itemChar, item->itemLength);
  return str;
}
void
ExtendedEventDescriptor::print ()
{
  clog << "ExtendedEventDescriptor::print" << endl;
  clog << " -descriptorNumber = " << getDescriptorNumber () << endl;
  clog << " -lastDescriptorNumber = " << getLastDescriptorNumber () << endl;
  // clog << " -languageCode = "         << getLanguageCode()        <<
  // endl;
  // clog << " -textChar = "             << getTextChar()            <<
  // endl;
}

size_t
ExtendedEventDescriptor::process (char *data, size_t pos)
{
  unsigned char remainingBytesItems;
  unsigned char value;
  struct Item *item;
  size_t itempos;

  // clog << "ExtendedEventDescriptor::process with pos = " << pos;

  descriptorLength = data[pos + 1]; // pos = 19
  pos += 2;                         // pos = 20
  // clog << " and  lenght = " << (descriptorLength&0xFF) << endl;

  descriptorNumber = ((data[pos] & 0xF0) >> 4); // 4 bits
  lastDescriptorNumber = ((data[pos] & 0x0F));  // 4 bits
  // clog << " Extended descriptorNumber = " << (descriptorNumber & 0xFF);
  // clog << " and lastDescriptorNumber = " << (lastDescriptorNumber & 0xFF)
  // <<
  // endl;
  pos += 1;

  memcpy (languageCode, data + pos, 3);
  // clog << "Extended languageCode = " << languageCode <<endl;
  pos += 3;

  lengthOfItems = data[pos];
  itempos = pos;
  pos++; // item_descriptor_lenght
  // clog << "Extended Lenght Of Items = " << (lengthOfItems & 0xFF);
  // clog << " and pos is = " << pos << endl;

  remainingBytesItems = lengthOfItems;
  items = new vector<Item *>;
  while (remainingBytesItems)
    {
      item = new struct Item;

      item->itemDescriptionLength = data[pos];
      // clog << " itemDescriptionLenght = "<< (item->itemDescriptionLength
      // &
      // 0xFF);
      if (item->itemDescriptionLength != 0)
        {
          item->itemDescriptionChar = new char[item->itemDescriptionLength];

          memset (item->itemDescriptionChar, 0,
                  item->itemDescriptionLength);

          memcpy (item->itemDescriptionChar, data + pos + 1,
                  item->itemDescriptionLength);
        }
      pos += data[pos] + 1;
      item->itemLength = data[pos];

      if (item->itemLength != 0)
        {
          item->itemChar = new char[item->itemLength];
          memset (item->itemChar, 0, item->itemLength);
          memcpy (item->itemChar, data + pos + 1, item->itemLength);
        }
      value = item->itemDescriptionLength + item->itemLength + 2;
      items->push_back (item);
      pos += data[pos] + 1;
      // clog << "ExtendedEvent:: insertItem finishing with pos = ";
      // clog << pos << endl;
      remainingBytesItems -= value;
    }

  itempos += lengthOfItems + 1;
  if (pos != itempos)
    {
      // clog << "Extended error in pos after all items " << endl;
      pos = itempos;
    }

  textLength = data[pos];

  if (textLength > 0)
    {
      // clog << "ExtendedEventDescriptor::process textLength = ";
      // clog << (unsigned int) textLength << endl;
      textChar = new char[textLength];
      if (textChar == NULL)
        {
          clog << "ExtendedEventDescriptor::process error allocating memory"
               << endl;
          return -1;
        }
      memset (textChar, 0, textLength);
      memcpy (textChar, data + pos + 1, textLength);
      /*
      clog << "ExtendedEventDescriptor::process textChar = ";
      for (int i = 0; i < textLength; i++){
              clog << textChar[i];
      }
      clog << endl;
      */
    }
  pos += textLength;
  return pos;
}

GINGA_TSPARSER_END
