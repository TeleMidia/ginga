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
#include "ContentDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_TSPARSER_BEGIN

// TODO: get methods?
ContentDescriptor::ContentDescriptor ()
{
  descriptorTag = 0x54;
  // contentNibbleLevel1 = 0;
  // contentNibbleLevel2 = 0;
  // userNibble1         = 0;
  // userNibble2         = 0;
  contents = NULL;
}

ContentDescriptor::~ContentDescriptor ()
{
  if (contents != NULL)
    {
      vector<Content *>::iterator i;
      for (i = contents->begin (); i != contents->end (); ++i)
        {
          delete (*i);
        }
      delete contents;
      contents = NULL;
    }
}

unsigned char
ContentDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

unsigned int
ContentDescriptor::getDescriptorLength ()
{
  return (unsigned int)descriptorLength;
}

unsigned short
ContentDescriptor::getContentNibble1 (struct Content *content)
{
  return content->contentNibbleLevel1;
}

unsigned short
ContentDescriptor::getContentNibble2 (struct Content *content)
{
  return content->contentNibbleLevel2;
}

unsigned short
ContentDescriptor::getUserNibble1 (struct Content *content)
{
  return content->userNibble1;
}

unsigned short
ContentDescriptor::getUserNibble2 (struct Content *content)
{
  return content->userNibble2;
}
vector<Content *> *
ContentDescriptor::getContents ()
{
  return contents;
}

void
ContentDescriptor::print ()
{
  clog << "ContentDescriptor::print printing..." << endl;
  clog << " -descriptorLength = " << getDescriptorLength () << endl;
  if (contents != NULL)
    {
      vector<Content *>::iterator i;
      struct Content *content;

      for (i = contents->begin (); i != contents->end (); ++i)
        {
          content = ((struct Content *)(*i));
          clog << " -contentNibble1 = " << getContentNibble1 (content);
          clog << " -contentNibble2 = " << getContentNibble2 (content);
          clog << " -userNibble1 = " << getUserNibble1 (content);
          clog << " -userNibble2 = " << getUserNibble2 (content);
        }
    }
  clog << endl;
}

size_t
ContentDescriptor::process (char *data, size_t pos)
{
  size_t remainingBytes = 0;
  struct Content *content;

  // clog << "ContentDescriptor process with pos = " << pos;

  descriptorLength = data[pos + 1];

  // clog << " and length = " << (descriptorLength & 0xFF) << endl;
  // pos += 2;
  pos++;
  remainingBytes = descriptorLength;
  contents = new vector<Content *>;

  while (remainingBytes > 0)
    {
      pos++;
      content = new struct Content;
      content->contentNibbleLevel1 = (unsigned char)((data[pos] & 0xF0) >> 4);
      content->contentNibbleLevel2 = (data[pos] & 0x0F);

      // clog << "Content contentNibble 1 = " << ((unsigned
      // int)contentNibbleLevel1 ) ;
      // clog << " and 2 = " << ((unsigned int)contentNibbleLevel2) << endl;
      pos++;

      content->userNibble1 = (unsigned char)((data[pos] & 0xF0) >> 4);
      content->userNibble2 = (data[pos] & 0x0F);

      // clog << "userNibble 1 = " << ((unsigned int)userNibble1) ;
      // clog << " and 2 = " << ((unsigned int)userNibble2&0xFF) << endl;
      contents->push_back (content);
      remainingBytes = remainingBytes - 2;
    }
  return pos;
}

GINGA_TSPARSER_END
