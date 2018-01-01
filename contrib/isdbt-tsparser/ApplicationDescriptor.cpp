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
#include "ApplicationDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

ApplicationDescriptor::ApplicationDescriptor ()
{
  descriptorTag = 0x00;
  descriptorLength = 0;
  applicationPriority = 0;
  applicationProfilesLength = 0;
  profiles = new vector<struct Profile *>;
  serviceBoundFlag = false;
  transportProtocolLabelsLength = 0;
  transportProtocolLabels = NULL;
}

ApplicationDescriptor::~ApplicationDescriptor ()
{
  vector<struct Profile *>::iterator i;
  if (profiles != NULL)
    {
      i = profiles->begin ();
      while (i != profiles->end ())
        {
          delete (*i);
          ++i;
        }
      delete profiles;
      profiles = NULL;
    }

  if (transportProtocolLabels != NULL)
    {
      delete transportProtocolLabels;
      transportProtocolLabels = NULL;
    }
}

unsigned char
ApplicationDescriptor::getApplicationPriority ()
{
  return applicationPriority;
}

vector<struct Profile *> *
ApplicationDescriptor::getProfiles ()
{
  return profiles;
}

unsigned char
ApplicationDescriptor::getVisibility ()
{
  return visibility;
}

unsigned char
ApplicationDescriptor::getTransportProtocolLabelsLength ()
{
  return transportProtocolLabelsLength;
}

char *
ApplicationDescriptor::getTransportProtocolLabels ()
{
  return transportProtocolLabels;
}
unsigned int
ApplicationDescriptor::getDescriptorLength ()
{
  return descriptorLength;
}

unsigned char
ApplicationDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

void
ApplicationDescriptor::print ()
{
  clog << "ApplicationDescriptor::print printing..." << endl;
  vector<Profile *>::iterator i;

  for (i = profiles->begin (); i != profiles->end (); ++i)
    {
      clog << "Profile: " << endl;
      clog << " -ApplicationProfile: " << (*i)->applicationProfile << endl;
      clog << " -VersionMajor: " << (*i)->versionMajor << endl;
      clog << " -VersionMinor: " << (*i)->versionMinor << endl;
      clog << " -VersionMicro: " << (*i)->versionMicro << endl;
    }
  clog << "ServiceBoundFlag: " << serviceBoundFlag << endl;
  clog << "Visibility: " << visibility << endl;
}

size_t
ApplicationDescriptor::process (char *data, size_t pos)
{
  struct Profile *profile;
  size_t profpos;

  descriptorLength = data[pos + 1];

  pos += 2;
  applicationProfilesLength = data[pos];

  profpos = pos;
  for (int i = 0; i < applicationProfilesLength; ++i)
    {
      pos++;
      profile = new struct Profile;
      profile->applicationProfile
          = ((((data[pos] & 0xFF) << 8) & 0xFF00) | (data[pos + 1] & 0xFF));

      pos += 2;
      profile->versionMajor = data[pos];

      pos++;
      profile->versionMinor = data[pos];

      pos++;
      profile->versionMicro = data[pos];
      profiles->push_back (profile);
    }

  pos = profpos + applicationProfilesLength;

  serviceBoundFlag = ((data[pos] & 0x80) >> 7);
  visibility = ((data[pos] & 0x60) >> 6);
  pos++;

  applicationPriority = data[pos];
  transportProtocolLabelsLength
      = descriptorLength - applicationProfilesLength - 2;

  transportProtocolLabels = new char[transportProtocolLabelsLength];
  for (int i = 0; i < transportProtocolLabelsLength; ++i)
    {
      pos++;
      transportProtocolLabels[i] = data[pos];
    }
  return pos;
}

GINGA_TSPARSER_END
