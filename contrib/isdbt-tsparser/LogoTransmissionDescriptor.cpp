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
#include "LogoTransmissionDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

// TODO: test this class - no use of this descriptor on TS files.
LogoTransmissionDescriptor::LogoTransmissionDescriptor ()
{
  descriptorTag = 0xCF;
  descriptorLength = 0;
  downloadDataId = 0;
  logoId = 0;
  logoName = "";
  logoType = 0;
  logoVersion = 0;
}

LogoTransmissionDescriptor::~LogoTransmissionDescriptor () {}

unsigned int
LogoTransmissionDescriptor::getDescriptorLength ()
{
  return (unsigned int)descriptorLength;
}

void
LogoTransmissionDescriptor::setType (unsigned char type)
{
  logoType = type;
  switch (logoType)
    {
    case 0x01:
      descriptorLength = 7;
      break;
    case 0x02:
      descriptorLength = 3;
      break;
    default:
      break;
    }
}

unsigned char
LogoTransmissionDescriptor::getType ()
{
  return logoType;
}

void
LogoTransmissionDescriptor::setLogoId (unsigned short id)
{
  logoId = id;
}

unsigned short
LogoTransmissionDescriptor::getLogoId ()
{
  return logoId;
}

void
LogoTransmissionDescriptor::setLogoVersion (unsigned short version)
{
  logoVersion = version;
}

unsigned short
LogoTransmissionDescriptor::getLogoVersion ()
{
  return logoVersion;
}

void
LogoTransmissionDescriptor::setDownloadDataId (unsigned short id)
{
  downloadDataId = id;
}

unsigned short
LogoTransmissionDescriptor::getDownloadDataId ()
{
  return downloadDataId;
}

void
LogoTransmissionDescriptor::setName (const string &name)
{
  logoName = name;
  descriptorLength = (logoName.length () + 1);
  logoType = 0x03;
}

string
LogoTransmissionDescriptor::getName ()
{
  return logoName;
}

unsigned int
LogoTransmissionDescriptor::getNameLength ()
{
  return logoName.length ();
}

unsigned char
LogoTransmissionDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

void
LogoTransmissionDescriptor::print ()
{
}

size_t
LogoTransmissionDescriptor::process (char *data, size_t pos)
{
  descriptorLength = data[pos + 1];
  // clog << "Descriptor length: ";
  // clog << (ltd->getDescriptorLength() & 0xFF) << endl;
  pos += 2;

  setType (data[pos]);
  // clog << "Type: " << (ltd->getType() & 0xFF) << endl;
  pos++;
  if (logoType == 0x01)
    { // scheme 1
      logoId = (((data[pos] << 8) & 0x0100) | (data[pos + 1] & 0xFF));

      pos += 2;
      logoVersion = (((data[pos] << 8) & 0x0100) | (data[pos + 1] & 0xFF));
      pos += 2;
      downloadDataId
          = (((data[pos] << 8) & 0xFF00) | (data[pos + 1] & 0xFF));
      pos += 2;
    }
  else if (logoType == 0x02)
    { // scheme 2
      logoId = (((data[pos] << 8) & 0x0100) | (data[pos + 1] & 0xFF));
      pos += 2;
    }
  else if (logoType == 0x03)
    { // simple logo system

      logoName = "";
      logoName.append (data + pos, descriptorLength - 1);
      pos += (descriptorLength - 1);
      // clog << "Simple logo system: " << ltd->getName() << endl;
    }
  else
    {
      pos += (descriptorLength - 1);
      // clog << "Unrecognized Logo Transmission Type: "
      //	<< logoType << endl;
    }
  return pos;
}

GINGA_TSPARSER_END
