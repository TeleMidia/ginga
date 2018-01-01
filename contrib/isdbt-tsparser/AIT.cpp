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
#include "AIT.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

AIT::AIT ()
{
  secName = "";
  applicationType = 0;
  commonDescriptorsLength = 0;
}

AIT::~AIT ()
{
  vector<IMpegDescriptor *>::iterator i;
  vector<IApplication *>::iterator j;

  i = descriptors.begin ();
  while (i != descriptors.end ())
    {
      delete (*i);
      ++i;
    }
  descriptors.clear ();

  j = applications.begin ();
  while (j != applications.end ())
    {
      delete (*j);
      ++j;
    }
  applications.clear ();
}

string
AIT::getSectionName ()
{
  return secName;
}

void
AIT::setSectionName (const string &secName)
{
  this->secName = secName;
}

void
AIT::setApplicationType (unsigned int type)
{
  applicationType = type;
}

vector<IMpegDescriptor *> *
AIT::copyDescriptors ()
{
  vector<IMpegDescriptor *> *descs;

  descs = new vector<IMpegDescriptor *> (descriptors);

  return descs;
}

vector<IApplication *> *
AIT::copyApplications ()
{
  vector<IApplication *> *apps;

  apps = new vector<IApplication *> (applications);

  return apps;
}

void
AIT::process (void *payloadBytes, unsigned int payloadSize)
{
  char *data;
  size_t pos, descpos;
  unsigned char descriptorTag;
  unsigned short remainingBytes;
  Application *application;
  IMpegDescriptor *descriptor;
  unsigned char value;

  data = new char[payloadSize];
  memcpy ((void *)&(data[0]), payloadBytes, payloadSize);

  pos = 0;
  commonDescriptorsLength
      = ((((data[pos] & 0x0F) << 8) & 0xFF00) | (data[pos + 1] & 0xFF));

  // handling MPEG descriptors
  pos += 2;
  descpos = pos;
  remainingBytes = commonDescriptorsLength;
  while (remainingBytes)
    {
      descriptorTag = data[descpos];
      value = ((data[pos + 1] & 0xFF) + 2);
      remainingBytes -= value;

      switch (descriptorTag)
        {
        case IApplication::DT_APPLICATION:
          descriptor = new ApplicationDescriptor ();
          descriptor->process (data, pos);
          descriptors.push_back (descriptor);
          break;

        case IApplication::DT_APPLICATION_NAME:
          descriptor = new ApplicationNameDescriptor ();
          descriptor->process (data, pos);
          descriptors.push_back (descriptor);
          break;

        case IApplication::DT_TRANSPORT_PROTOCOL:
          descriptor = new TransportProtocolDescriptor ();
          descriptor->process (data, pos);
          descriptors.push_back (descriptor);
          break;

        case IApplication::DT_GINGAJ_APPLICATION_LOCATION:
          descriptor = new ApplicationLocationDescriptor ();
          descriptor->process (data, pos);
          descriptors.push_back (descriptor);
          break;

        case IApplication::DT_GINGANCL_APPLICATION_LOCATION:
          descriptor = new ApplicationLocationDescriptor ();
          descriptor->process (data, pos);
          descriptors.push_back (descriptor);
          break;

        case IApplication::DT_PREFETCH:
          descriptor = new PrefetchDescriptor ();
          descriptor->process (data, pos);
          descriptors.push_back (descriptor);
          break;

        default:
          break;
        }
      pos += value;
    }

  pos = descpos + commonDescriptorsLength;
  applicationLoopLength
      = ((((data[pos] & 0x0F) << 8) & 0xFF00) | (data[pos + 1] & 0xFF));

  pos += 2;
  remainingBytes = applicationLoopLength;
  while (pos < payloadSize)
    {
      application = new Application ();
      application->process (data, pos);
      pos += application->getLength ();
      applications.push_back (application);
    }

  delete[] data;
}

GINGA_TSPARSER_END
