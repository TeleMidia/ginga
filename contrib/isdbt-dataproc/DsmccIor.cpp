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
#include "DsmccIor.h"

GINGA_DATAPROC_BEGIN

DsmccIor::DsmccIor () {}

void
DsmccIor::setTypeId (const string &typeId)
{
  this->typeId = typeId;
}

string
DsmccIor::getTypeId ()
{
  return typeId;
}

void
DsmccIor::setCarouselId (unsigned int carouselId)
{
  this->carouselId = carouselId;
}

unsigned int
DsmccIor::getCarouselId ()
{
  return carouselId;
}

void
DsmccIor::setModuleId (unsigned int moduleId)
{
  this->moduleId = moduleId;
}

unsigned int
DsmccIor::getModuleId ()
{
  return moduleId;
}

void
DsmccIor::setObjectKey (unsigned int objectKey)
{
  this->objectKey = objectKey;
}

unsigned int
DsmccIor::getObjectKey ()
{
  return objectKey;
}

void
DsmccIor::print ()
{
  clog << "IOR" << endl;
  clog << "typeId = " << typeId.c_str () << endl;
  clog << "carouselId = " << carouselId << endl;
  clog << "moduleId = " << moduleId << endl;
}

GINGA_DATAPROC_END
