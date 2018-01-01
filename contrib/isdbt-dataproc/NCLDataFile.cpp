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
#include "NCLDataFile.h"

GINGA_DATAPROC_BEGIN

NCLDataFile::NCLDataFile (int id)
{
  this->structureId = id;
  this->componentTag = "";
  this->size = 0;
  this->uri = "";
}

NCLDataFile::~NCLDataFile () {}

int
NCLDataFile::getId ()
{
  return this->structureId;
}

void
NCLDataFile::setComponentTag (const string &componentTag)
{
  this->componentTag = componentTag;
}

string
NCLDataFile::getCopmonentTag ()
{
  return this->componentTag;
}

void
NCLDataFile::setUri (const string &uri)
{
  this->uri = uri;
}

string
NCLDataFile::getUri ()
{
  return this->uri;
}

void
NCLDataFile::setSize (double size)
{
  this->size = size;
}

double
NCLDataFile::getSize ()
{
  return this->size;
}

GINGA_DATAPROC_END
