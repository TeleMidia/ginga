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

#ifndef I_APPLICATION_H
#define I_APPLICATION_H

#include "ginga.h"

#include "IMpegDescriptor.h"

GINGA_TSPARSER_BEGIN

class IApplication
{
public:
  static const unsigned char DT_APPLICATION = 0x00;
  static const unsigned char DT_APPLICATION_NAME = 0x01;
  static const unsigned char DT_TRANSPORT_PROTOCOL = 0x02;
  static const unsigned char DT_GINGAJ_APPLICATION = 0x03;
  static const unsigned char DT_GINGAJ_APPLICATION_LOCATION = 0x04;
  static const unsigned char DT_EXTERNAL_APPLICATION = 0x05;
  static const unsigned char DT_GINGANCL_APPLICATION = 0x06;
  static const unsigned char DT_GINGANCL_APPLICATION_LOCATION = 0x07;
  static const unsigned char DT_APPLICATION_ICONS = 0x0B;
  static const unsigned char DT_PREFETCH = 0x0C;
  static const unsigned char DT_DII_LOCATION = 0x0D;
  static const unsigned char DT_IP_SIGNALLING = 0x11;
  static const unsigned char DT_PRIVATE_DATA_SPECIFIER = 0x5F;

  static const unsigned char CC_AUTOSTART = 0x01;
  static const unsigned char CC_PRESENT = 0x02;
  static const unsigned char CC_DESTROY = 0x03;
  static const unsigned char CC_KILL = 0x04;
  static const unsigned char CC_PREFETCH = 0x05;
  static const unsigned char CC_REMOTE = 0x06;
  static const unsigned char CC_UNBOUND = 0x07;
  static const unsigned char CC_STORE = 0x08;
  static const unsigned char CC_STORED_AUTOSTART = 0x09;
  static const unsigned char CC_STORED_PRESENT = 0x0A;
  static const unsigned char CC_STORED_REMOVE = 0x0B;

  virtual ~IApplication (){};
  virtual string getBaseDirectory () = 0;
  virtual string getInitialClass () = 0;
  virtual string getId () = 0;
  virtual unsigned short getControlCode () = 0;
  virtual unsigned short getLength () = 0;
  virtual unsigned short getProfile (int profileNumber = 0) = 0;
  virtual unsigned short getTransportProtocolId () = 0;
  virtual size_t process (char *data, size_t pos) = 0;
};

GINGA_TSPARSER_END

#endif /* I_APPLICATION_H */
