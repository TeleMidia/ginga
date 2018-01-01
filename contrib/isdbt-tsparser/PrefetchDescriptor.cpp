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
#include "PrefetchDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_TSPARSER_BEGIN

PrefetchDescriptor::PrefetchDescriptor ()
{
  descriptorLength = 0;
  descriptorTag = 0;
  prefetchs = new vector<struct Prefetch *>;
  transportLabel = 0;
}

PrefetchDescriptor::~PrefetchDescriptor ()
{
  if (prefetchs != NULL)
    {
      vector<struct Prefetch *>::iterator i;
      for (i = prefetchs->begin (); i != prefetchs->end (); ++i)
        {
          if ((*i)->labelChar != NULL)
            {
              delete (*i)->labelChar;
              (*i)->labelChar = NULL;
            }
          delete (*i);
          (*i) = NULL;
        }
      delete prefetchs;
      prefetchs = NULL;
    }
}

unsigned int
PrefetchDescriptor::getDescriptorLength ()
{
  return descriptorLength;
}

unsigned char
PrefetchDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

void
PrefetchDescriptor::print ()
{
}

size_t
PrefetchDescriptor::process (char *data, size_t pos)
{
  struct Prefetch *prefetch;

  descriptorTag = data[pos];
  descriptorLength = data[pos + 1];
  pos += 2;

  transportLabel = data[pos];

  for (int i = 0; i < descriptorLength - 1; i++)
    {
      pos++;

      prefetch = new struct Prefetch;
      prefetch->lambelLength = data[pos];
      memcpy (prefetch->labelChar, data + pos + 1, prefetch->lambelLength);
      pos += prefetch->lambelLength;
      prefetch->prefecthPriority = data[pos];

      prefetchs->push_back (prefetch);
    }
  return pos;
}

GINGA_TSPARSER_END
