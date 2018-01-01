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
#include "DsmccObject.h"

GINGA_DATAPROC_BEGIN

DsmccObject::DsmccObject () {}

DsmccObject::~DsmccObject ()
{
  vector<DsmccBinding *>::iterator i;

  i = bindings.begin ();
  while (i != bindings.end ())
    {
      delete (*i);

      ++i;
    }
}

void
DsmccObject::setCarouselId (unsigned int objectCarouselId)
{
  carouselId = objectCarouselId;
}

void
DsmccObject::setModuleId (unsigned int objectModuleId)
{
  moduleId = objectModuleId;
}

void
DsmccObject::setKey (unsigned int objectKey)
{
  key = objectKey;
}

void
DsmccObject::setKind (const string &objectKind)
{
  kind = objectKind;
}

void
DsmccObject::addBinding (DsmccBinding *binding)
{
  bindings.push_back (binding);
}

void
DsmccObject::setData (char *fileData)
{
  data = fileData;
}

void
DsmccObject::setDataSize (unsigned int size)
{
  dataSize = size;
}

string
DsmccObject::getObjectId ()
{
  return xstrbuild ("%u%u%u", carouselId, moduleId, key);
}

unsigned int
DsmccObject::getCarouselId ()
{
  return carouselId;
}

unsigned int
DsmccObject::getModuleId ()
{
  return moduleId;
}

unsigned int
DsmccObject::getKey ()
{
  return key;
}

string
DsmccObject::getKind ()
{
  return kind;
}

vector<DsmccBinding *> *
DsmccObject::getBindings ()
{
  return &bindings;
}

char *
DsmccObject::getData ()
{
  return data;
}

unsigned int
DsmccObject::getDataSize ()
{
  return dataSize;
}

void
DsmccObject::print ()
{
  clog << "DsmccObject" << endl;
  clog << "carouselId = " << carouselId << endl;
  clog << "moduleId = " << moduleId << endl;
  clog << "key = " << key << endl;
  clog << "kind = " << kind << endl;
  clog << "name = " << name << endl;
}

GINGA_DATAPROC_END
