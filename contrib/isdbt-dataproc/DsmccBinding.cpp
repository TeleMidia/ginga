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
#include "DsmccBinding.h"

GINGA_DATAPROC_BEGIN

DsmccBinding::DsmccBinding () { ior = NULL; }

DsmccBinding::~DsmccBinding () { releaseIor (); }

void
DsmccBinding::releaseIor ()
{
  if (ior != NULL)
    {
      delete ior;
      ior = NULL;
    }
}

void
DsmccBinding::setId (const string &id)
{
  this->id = id;
}

string
DsmccBinding::getId ()
{
  return id;
}

void
DsmccBinding::setKind (const string &kind)
{
  this->kind = kind;
}

string
DsmccBinding::getKind ()
{
  return kind;
}

void
DsmccBinding::setType (unsigned int type)
{
  this->type = type;
}

unsigned int
DsmccBinding::getType ()
{
  return type;
}

void
DsmccBinding::setIor (DsmccIor *ior)
{
  releaseIor ();

  this->ior = ior;
}

DsmccIor *
DsmccBinding::getIor ()
{
  return ior;
}

void
DsmccBinding::setObjectInfo (const string &objectInfo)
{
  this->objectInfo = objectInfo;
}

string
DsmccBinding::getObjectInfo ()
{
  return this->objectInfo;
}

void
DsmccBinding::print ()
{
  clog << "BINDING" << endl;
  clog << "id = " << id.c_str () << endl;
  clog << "kind = " << kind.c_str () << endl;
  clog << "type = " << type << endl;
  clog << " info = " << objectInfo.c_str () << endl;
}

GINGA_DATAPROC_END
