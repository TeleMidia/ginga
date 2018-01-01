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
#include "ServiceDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_TSPARSER_BEGIN

ServiceDescriptor::ServiceDescriptor ()
{
  descriptorTag = 0x48;
  descriptorLength = 0;
  serviceProviderNameLength = 0;
  serviceProviderNameChar = NULL;
  serviceNameLength = 0;
  serviceNameChar = NULL;
  serviceType = 0;
}

ServiceDescriptor::~ServiceDescriptor ()
{
  if (serviceProviderNameChar != NULL)
    {
      delete serviceProviderNameChar;
      serviceProviderNameChar = NULL;
    }

  if (serviceNameChar != NULL)
    {
      delete serviceNameChar;
      serviceNameChar = NULL;
    }
}

unsigned char
ServiceDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

unsigned int
ServiceDescriptor::getDescriptorLength ()
{
  return (unsigned int)descriptorLength;
}

unsigned int
ServiceDescriptor::getServiceProviderNameLength ()
{
  return (unsigned int)serviceProviderNameLength;
}

unsigned int
ServiceDescriptor::getServiceNameLength ()
{
  return (unsigned int)serviceNameLength;
}

string
ServiceDescriptor::getServiceProviderNameChar ()
{
  string str;

  if (serviceProviderNameChar == NULL)
    {
      return "";
    }
  str.append (serviceProviderNameChar, serviceProviderNameLength);
  return str;
}

string
ServiceDescriptor::getServiceNameChar ()
{
  string str;

  if (serviceNameChar == NULL)
    {
      return "";
    }
  // using serviceNameChar[1] because [0] is usually sent with encoding
  // value, value is equal to 0x0E. So the serviceName starts in [1].
  str.assign (serviceNameChar + 1, (serviceNameLength & 0xFF) - 1);
  return str;
}

void
ServiceDescriptor::print ()
{
  clog << "ServiceDescriptor::print printing..." << endl;
  clog << " -descriptorLength = " << getDescriptorLength () << endl;

  if (serviceProviderNameLength > 0)
    {
      clog << "-serviceProviderNameChar = " << getServiceProviderNameChar ()
           << endl;
    }

  if (serviceNameLength > 0)
    {
      clog << " -serviceNameLength: " << getServiceNameLength () << endl;
      clog << " -serviceNameChar: " << getServiceNameChar () << endl;
      // clog << " -charEnconde:" << hex << (serviceNameChar[0] & 0xFF);
      // clog << dec << endl;
    }
}

size_t
ServiceDescriptor::process (char *data, size_t pos)
{
  // clog << "ServiceDescriptor:: process with pos = " << pos << endl;
  descriptorLength = data[pos + 1];
  pos += 2;

  serviceType = data[pos];
  pos++;

  // cout <<"service length = " << (unsigned int)descriptorLength;
  // clog << " and serviceType = ";
  // clog << (unsigned int)serviceType << endl;

  serviceProviderNameLength = data[pos];
  if (serviceProviderNameLength > 0)
    {
      // clog << "ServiceProviderNameLength = ";
      // clog << (unsigned int)serviceProviderNameLength;
      // clog << endl;
      serviceProviderNameChar = new char[serviceProviderNameLength];
      memset (serviceProviderNameChar, 0, serviceProviderNameLength);
      memcpy (serviceProviderNameChar, data + pos + 1,
              serviceProviderNameLength);
    }
  pos += serviceProviderNameLength + 1;
  serviceNameLength = data[pos];

  if (serviceNameLength > 0)
    {
      serviceNameChar = new char[serviceNameLength];
      memset (serviceNameChar, 0, serviceNameLength);
      memcpy (serviceNameChar, data + pos + 1, serviceNameLength);
    }
  pos += serviceNameLength;
  return pos;
}

GINGA_TSPARSER_END
