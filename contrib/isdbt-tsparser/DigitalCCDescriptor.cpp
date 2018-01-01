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
#include "DigitalCCDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_TSPARSER_BEGIN

DigitalCCDescriptor::DigitalCCDescriptor ()
{
  descriptorTag = 0xC1;
  descriptorLength = 0;
  components = NULL;
  maximumBitrateFlag = false;
  componentControlFlag = false;
  copyControlType = 0;
  digitalRecordingControlData = 0;
  maximumBitRate = 0;
}

DigitalCCDescriptor::~DigitalCCDescriptor ()
{
  if (components != NULL)
    {
      vector<Component *>::iterator i;
      for (i = components->begin (); i != components->end (); ++i)
        {
          if ((*i) != NULL)
            {
              delete (*i);
            }
        }
      delete components;
      components = NULL;
    }
}
unsigned char
DigitalCCDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}
unsigned int
DigitalCCDescriptor::getDescriptorLength ()
{
  return (unsigned int)descriptorLength;
}
void
DigitalCCDescriptor::print ()
{
  clog << "DigitalCCDescriptor::print printing..." << endl;
  if (components->size () > 0)
    {
      vector<Component *>::iterator i;
      for (i = components->begin (); i != components->end (); ++i)
        {
          clog << " -Component:";
          clog << " componentTag = " << (unsigned int)(*i)->componentTag;
          clog << " -copyControlType = ";
          clog << (unsigned int)(*i)->copyControlType;
          if ((*i)->maximumBitrateFlag == true)
            {
              clog << " -maximumBitRate = ";
              clog << (unsigned int)((*i)->maximumBitrate) << endl;
            }
          else
            {
              clog << endl;
            }
        }
    }
}
size_t
DigitalCCDescriptor::process (char *data, size_t pos)
{
  size_t remainingBytes = 0;
  struct Component *component;

  // clog << "DigitalCCDescriptor::process with pos = " << pos;
  descriptorLength = data[pos + 1];
  // clog << " and length = " << (descriptorLength & 0xFF)<< endl;
  pos += 2;

  digitalRecordingControlData = (unsigned char)((data[pos] & 0xC0) >> 6); // 2 bits
  maximumBitrateFlag = ((data[pos] & 0x20) >> 5);          // 1 bit
  componentControlFlag = ((data[pos] & 0x10) >> 4);        // 1 bit
  // clog << "DCCD componentControlFlag = " << (componentControlFlag & 0xFF)
  // <<
  // endl;
  copyControlType = (unsigned char)((data[pos] & 0x0C) >> 2); // 2 bits

  if (copyControlType != 00)
    {
      APSControlData = (data[pos] & 0x03); // 2 bits
    }
  // clog << "DigitalCCD debug third byte = " << (data[pos] & 0xFF) << endl;
  pos++;

  if (maximumBitrateFlag == 1)
    {
      maximumBitRate = data[pos];
      // clog << "DCCD maximumBitRate = " << maximumBitRate << endl;
      pos++;
    }
  if (componentControlFlag == 1)
    {
      componentControlLength = data[pos];
      remainingBytes = componentControlLength;

      components = new vector<Component *>;
      while (remainingBytes > 0)
        {
          pos++;

          component = new struct Component;
          component->componentTag = data[pos];
          pos++;

          remainingBytes -= 2; // 2 bytes read

          component->digitalRecordingControlData
              = (unsigned char)((data[pos] & 0xC0) >> 6);

          component->maximumBitrateFlag = ((data[pos] & 0x20) >> 5);
          component->copyControlType = (unsigned char)((data[pos] & 0x0C) >> 2);

          if (component->copyControlType != 00)
            {
              component->APSControlData = (data[pos] & 0x03); // 2 bits
            }

          if (maximumBitrateFlag == 1)
            {
              pos++;
              remainingBytes--;
              component->maximumBitrate = data[pos];
            }
          components->push_back (component);
        }
    }
  return pos;
}

GINGA_TSPARSER_END
