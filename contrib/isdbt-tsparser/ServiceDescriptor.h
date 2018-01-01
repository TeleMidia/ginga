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

#ifndef SERVICEDESCRIPTOR_H_
#define SERVICEDESCRIPTOR_H_

#include "IServiceDescriptor.h"

GINGA_TSPARSER_BEGIN

class ServiceDescriptor : public IServiceDescriptor
{
protected:
  unsigned char serviceType;
  unsigned char serviceProviderNameLength;
  char *serviceProviderNameChar;
  unsigned char serviceNameLength;
  char *serviceNameChar;

public:
  ServiceDescriptor ();
  virtual ~ServiceDescriptor ();
  unsigned int getDescriptorLength ();
  unsigned char getDescriptorTag ();
  unsigned int getServiceProviderNameLength ();
  unsigned int getServiceNameLength ();
  string getServiceProviderNameChar ();
  string getServiceNameChar ();
  void print ();
  size_t process (char *data, size_t pos);
};

GINGA_TSPARSER_END

#endif /* SERVICEDESCRIPTOR_H_ */
