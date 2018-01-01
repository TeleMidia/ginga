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
#include "ShortEventDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_TSPARSER_BEGIN

ShortEventDescriptor::ShortEventDescriptor ()
{
  descriptorTag = 0x4D;
  descriptorLength = 0;
  eventNameLength = 0;
  eventNameChar = NULL;
  textLength = 0;
  textChar = NULL;
}

ShortEventDescriptor::~ShortEventDescriptor ()
{
  if (eventNameChar != NULL)
    {
      delete eventNameChar;
      eventNameChar = NULL;
    }
  if (textChar != NULL)
    {
      delete textChar;
      textChar = NULL;
    }
}

unsigned char
ShortEventDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

unsigned int
ShortEventDescriptor::getDescriptorLength ()
{
  return (unsigned int)descriptorLength;
}

unsigned int
ShortEventDescriptor::getEventNameLength ()
{
  return (unsigned int)eventNameLength;
}

unsigned int
ShortEventDescriptor::getTextLength ()
{
  return (unsigned int)textLength;
}

string
ShortEventDescriptor::getLanguageCode ()
{
  string str;

  str.append (languageCode, 3);
  return str;
}

string
ShortEventDescriptor::getEventName ()
{
  string str;

  if (eventNameChar == NULL)
    {
      return "";
    }
  str.append (eventNameChar, eventNameLength);
  return str;
}

string
ShortEventDescriptor::getTextChar ()
{
  string str;

  if (textChar == NULL)
    {
      return "";
    }
  str.append (textChar, textLength);
  return textChar;
}

void
ShortEventDescriptor::print ()
{
  clog << "ShortEventDescriptor::print printing...." << endl;
  clog << " -languageCode: " << getLanguageCode () << endl;
  clog << " -eventNameChar: " << getEventName () << endl;
  clog << " -textChar: " << getTextChar () << endl;
}

size_t
ShortEventDescriptor::process (char *data, size_t pos)
{
  // clog << "ShortEventDescriptor::process with pos = " << pos << endl;

  descriptorLength = data[pos + 1];
  pos += 2;

  memcpy (languageCode, data + pos, 3);
  pos += 3;

  eventNameLength = data[pos];

  if (eventNameLength > 0)
    {
      eventNameChar = new char[eventNameLength];

      if (eventNameChar == NULL)
        {
          clog << "ShortEvent::process error allocating memory" << endl;
          return -1;
        }
      memset (eventNameChar, 0, eventNameLength);
      memcpy (eventNameChar, data + pos + 1, eventNameLength);
    }
  pos += eventNameLength + 1;

  textLength = data[pos];
  if (textLength)
    {
      textChar = new char[textLength];
      if (textChar == NULL)
        {
          // clog << "ShortEvent::process error allocating memory" << endl;
          return -1;
        }
      memset (textChar, 0, textLength);
      memcpy (textChar, data + pos + 1, textLength);
    }
  pos += textLength;

  return pos;
}

GINGA_TSPARSER_END
