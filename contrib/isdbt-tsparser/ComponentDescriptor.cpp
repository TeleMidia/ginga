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
#include "ComponentDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

ComponentDescriptor::ComponentDescriptor ()
{
  descriptorTag = 0x50;
  descriptorLength = 0;
  streamContent = 0;
  componentType = 0;
  componentTag = 0;
  textChar = NULL;
}

ComponentDescriptor::~ComponentDescriptor ()
{
  if (textChar != NULL)
    {
      delete textChar;
    }
}

unsigned char
ComponentDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

unsigned int
ComponentDescriptor::getDescriptorLength ()
{
  return (unsigned int)descriptorLength;
}

string
ComponentDescriptor::getTextChar ()
{
  string str;

  if (textChar == NULL)
    {
      return "";
    }
  str.append (textChar, textLength);
  return str;
}

void
ComponentDescriptor::print ()
{
  clog << "ComponentDescriptor::print printing..." << endl;
  clog << " -descriptorLength = " << (unsigned int)descriptorLength;
  clog << endl;
  clog << " -streamContent = " << (unsigned int)streamContent;
  clog << endl;
  clog << " -componentType = " << (unsigned int)componentType;
  clog << endl;
  /*if (textLength > 0){
          clog << " and textChar = " ;
          for(int i = 0; i < textLength; ++i){
                  clog << textChar[i];
          }
          clog << endl;
  }
  */
}

size_t
ComponentDescriptor::process (char *data, size_t pos)
{
  // clog << "ComponentDescriptor::process beginning with pos =  " << pos;
  descriptorLength = data[pos + 1];

  // cout <<" and length = " << (descriptorLength & 0xFF) <<endl;

  pos += 2;
  // jumping reserved_future_use (first 4 bits of data[pos])
  streamContent = (data[pos] & 0x0F); // last 4 bits of data[pos]
  // cout <<"Componenet streamContent = " << (streamContent & 0xFF) <<endl;
  pos += 1;

  componentType = data[pos];
  // cout <<"Component componentType = " << (componentType & 0xFF) <<endl;
  pos++;

  componentTag = data[pos];
  // clog << "Componenet component tag = "<< (componentTag & 0xFF) << endl;

  pos++; // pos=23
  memcpy (languageCode, data + pos, 3);

  pos += 3;
  textLength = descriptorLength - 6;
  /*the si standard do not define textLenght for this descriptor
   *for this reason, it has to be calculated. It was kept as
   *a class atribute to maintain conformity with others classes
   */
  if (textLength > 0)
    {
      if (textChar != NULL)
        {
          delete textChar;
        }
      // setTextChar(data+pos, textLength);
      textChar = new char[textLength];
      memset (textChar, 0, textLength);
      memcpy (textChar, data + pos, textLength);

      /*
      clog << "ComponentDescriptor::process text char = ";
      for (int i = 0; i < textLength; i++){
              clog << (textChar[i]);
      }
      clog << endl;
      */
    }
  pos += textLength;
  return pos;
}

GINGA_TSPARSER_END
