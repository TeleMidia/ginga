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
#include "Application.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

Application::Application ()
{
  applicationControlCode = 0;
  applicationId.applicationId = 0;
  applicationId.organizationId = 0;
  appDescriptorsLoopLength = 0;
  applicationLength = 0;

  Thread::mutexInit (&stlMutex, false);
}

Application::~Application ()
{
  vector<IMpegDescriptor *>::iterator i;

  Thread::mutexLock (&stlMutex);
  i = descriptors.begin ();
  while (i != descriptors.end ())
    {
      delete (*i);
      ++i;
    }

  descriptors.clear ();
  Thread::mutexUnlock (&stlMutex);
  Thread::mutexDestroy (&stlMutex);
}

string
Application::getBaseDirectory ()
{
  vector<IMpegDescriptor *>::iterator i;
  ApplicationLocationDescriptor *location;
  unsigned char descTag;

  Thread::mutexLock (&stlMutex);
  i = descriptors.begin ();
  while (i != descriptors.end ())
    {
      descTag = (*i)->getDescriptorTag ();
      if (descTag == DT_GINGANCL_APPLICATION_LOCATION
          || descTag == DT_GINGAJ_APPLICATION_LOCATION)
        {
          location = (ApplicationLocationDescriptor *)(*i);

          Thread::mutexUnlock (&stlMutex);
          return location->getBaseDirectory ();
        }

      ++i;
    }

  Thread::mutexUnlock (&stlMutex);
  return "";
}

string
Application::getInitialClass ()
{
  vector<IMpegDescriptor *>::iterator i;
  ApplicationLocationDescriptor *location;
  unsigned char descTag;

  Thread::mutexLock (&stlMutex);
  i = descriptors.begin ();
  while (i != descriptors.end ())
    {
      descTag = (*i)->getDescriptorTag ();
      if (descTag == DT_GINGANCL_APPLICATION_LOCATION
          || descTag == DT_GINGAJ_APPLICATION_LOCATION)
        {
          location = (ApplicationLocationDescriptor *)(*i);

          Thread::mutexUnlock (&stlMutex);
          return location->getInitialClass ();
        }
      ++i;
    }

  Thread::mutexUnlock (&stlMutex);
  return "";
}

string
Application::getId ()
{
  return xstrbuild ("%d", applicationId.applicationId);
}

unsigned short
Application::getControlCode ()
{
  return applicationControlCode;
}

unsigned short
Application::getLength ()
{
  return applicationLength;
}

unsigned short
Application::getProfile (int profileNumber)
{
  vector<IMpegDescriptor *>::iterator i;
  ApplicationDescriptor *app;
  vector<struct Profile *> *profs;
  unsigned char descTag;

  Thread::mutexLock (&stlMutex);
  i = descriptors.begin ();
  while (i != descriptors.end ())
    {
      descTag = (*i)->getDescriptorTag ();
      if (descTag == DT_APPLICATION)
        {
          app = (ApplicationDescriptor *)(*i);

          Thread::mutexUnlock (&stlMutex);

          profs = app->getProfiles ();
          return profs->at (profileNumber)->applicationProfile;
        }
      ++i;
    }

  Thread::mutexUnlock (&stlMutex);
  return 0;
}

unsigned short
Application::getTransportProtocolId ()
{
  vector<IMpegDescriptor *>::iterator i;
  TransportProtocolDescriptor *tpd;
  unsigned char descTag;

  Thread::mutexLock (&stlMutex);
  i = descriptors.begin ();
  while (i != descriptors.end ())
    {
      descTag = (*i)->getDescriptorTag ();
      if (descTag == DT_TRANSPORT_PROTOCOL)
        {
          tpd = (TransportProtocolDescriptor *)(*i);

          Thread::mutexUnlock (&stlMutex);
          return tpd->getProtocolId ();
        }
      ++i;
    }

  Thread::mutexUnlock (&stlMutex);
  return 0;
}

size_t
Application::process (char *data, size_t pos)
{
  unsigned short remainingBytes;
  unsigned short descriptorTag, value;
  IMpegDescriptor *descriptor;

  Thread::mutexLock (&stlMutex);
  applicationId.organizationId
      = ((((data[pos] & 0xFF) << 24) & 0xFF000000)
         | (((data[pos + 1] & 0xFF) << 16) & 0x00FF0000)
         | (((data[pos + 2] & 0xFF) << 8) & 0x0000FF00)
         | ((data[pos + 3] & 0xFF)));

  pos += 4;
  applicationId.applicationId
      = (((data[pos] << 8) & 0xFF00) | (data[pos + 1] & 0xFF));

  pos += 2;

  applicationControlCode = data[pos];

  pos++;

  appDescriptorsLoopLength
      = ((((data[pos] & 0x0F) << 8) & 0xFF00) | (data[pos + 1] & 0xFF));

  pos += 2;
  applicationLength = appDescriptorsLoopLength + 9;
  remainingBytes = appDescriptorsLoopLength;
  descriptorTag = data[pos];

  value = ((data[pos + 1] & 0xFF) + 2);

  while (remainingBytes > 0)
    {
      descriptorTag = data[pos];

      value = ((data[pos + 1] & 0xFF) + 2);
      remainingBytes -= value;

      switch (descriptorTag)
        {
        case DT_APPLICATION:
          descriptor = new ApplicationDescriptor ();
          descriptor->process (data, pos);
          descriptors.push_back (descriptor);
          break;

        case DT_APPLICATION_NAME:
          descriptor = new ApplicationNameDescriptor ();
          descriptor->process (data, pos);
          descriptors.push_back (descriptor);
          break;

        case DT_TRANSPORT_PROTOCOL:
          descriptor = new TransportProtocolDescriptor ();
          descriptor->process (data, pos);
          descriptors.push_back (descriptor);
          break;

        case DT_GINGAJ_APPLICATION_LOCATION:
          descriptor = new ApplicationLocationDescriptor ();
          descriptor->process (data, pos);
          descriptors.push_back (descriptor);
          break;

        case DT_GINGANCL_APPLICATION_LOCATION:
          descriptor = new ApplicationLocationDescriptor ();
          descriptor->process (data, pos);
          descriptors.push_back (descriptor);
          break;

        case DT_PREFETCH:
          descriptor = new PrefetchDescriptor ();
          descriptor->process (data, pos);
          descriptors.push_back (descriptor);
          break;

        default:
          // pos += value;
          break;
        }
      pos += value;
    }

  Thread::mutexUnlock (&stlMutex);
  return pos;
}

GINGA_TSPARSER_END
