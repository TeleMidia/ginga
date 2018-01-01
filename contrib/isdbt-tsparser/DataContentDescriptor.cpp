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
#include "DataContentDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_TSPARSER_BEGIN

DataContentDescriptor::DataContentDescriptor ()
{
  descriptorTag = 0xC7;
  componentRef = NULL;
  dataComponentId = 0;
  descriptorLength = 0;
  selectorByte = NULL;
  selectorLength = 0;
  textChar = NULL;
  textLength = 0;
}

DataContentDescriptor::~DataContentDescriptor ()
{
  if (selectorByte != NULL)
    {
      delete selectorByte;
      selectorByte = NULL;
    }
  if (textChar != NULL)
    {
      delete textChar;
      textChar = NULL;
    }
}
unsigned int
DataContentDescriptor::getDescriptorLength ()
{
  return (unsigned int)descriptorLength;
}
unsigned char
DataContentDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}
string
DataContentDescriptor::getTextChar ()
{
  string str;

  str.append (textChar, textLength);
  return str;
}
string
DataContentDescriptor::getLanguageCode ()
{
  string str;
  str.append (languageCode, 3);
  return str;
}
void
DataContentDescriptor::print ()
{
  clog << "DataContentDescriptor::print printing..." << endl;
  clog << " -descriptorLenght = " << getDescriptorLength () << endl;
}
size_t
DataContentDescriptor::process (char *data, size_t pos)
{
  // clog << "DataContentDescriptor process with pos = " << pos;

  descriptorLength = data[pos + 1];
  pos += 2;
  // clog << " and length = " << (descriptorLength & 0xFF) << endl;

  dataComponentId = (((data[pos] << 8) & 0x00FF) | (data[pos + 1] & 0xFF));
  // clog << "Data dataComponentId = " << (dataComponentId & 0xFF) << endl;
  pos += 2;

  entryComponent = data[pos];
  pos++;
  selectorLength = data[pos];
  // clog << "Data Contents entryComponent = " << (entryComponent & 0xFF) <<
  // "
  // and";
  // clog << " selectorLength = " << (selectorLength & 0xFF) << endl;
  if (selectorLength > 0)
    {
      selectorByte = new char[selectorLength];
      memset (selectorByte, 0, selectorLength);
      memcpy (selectorByte, data + pos + 1, selectorLength);
      /*
      clog << "Data selectorByte = " << endl;
      for(int i = 0; i < selectorLength; i++){
              clog << selectorByte[i];
      }
      clog << endl;
      */
    }
  pos += selectorLength + 1;
  numOfComponentRef = data[pos];
  if (numOfComponentRef > 0)
    {
      componentRef = new char[numOfComponentRef];
      memset (componentRef, 0, numOfComponentRef);
      memcpy (componentRef, data + pos + 1, numOfComponentRef);
      /*
      clog << "Data Comp Ref = " << endl;
      for(int i = 0; i < numOfComponentRef; i++){
              clog << componentRef[i];
      }
      clog << endl;
      */
    }
  pos += numOfComponentRef + 1;

  memcpy (languageCode, data + pos, 3);
  pos += 3;
  /*
  clog << "Data languageCode = ";
  for (int i = 0; i < 3; i++){
          clog << languageCode[i];
  }
  clog << endl;
  */
  textLength = data[pos];
  if (textLength > 0)
    {
      textChar = new char[textLength];
      memset (textChar, 0, textLength);
      memcpy (textChar, data + pos + 1, textLength);
      /*
      clog << "Data textLength = " << (textLength & 0xFF) << endl;
      clog << "Data textChar = ";
      for(int i = 0; i < textLength; i++){
              clog << textChar[i];
      }
      clog << endl;
      */
    }
  pos += textLength;
  return pos;
}

GINGA_TSPARSER_END
