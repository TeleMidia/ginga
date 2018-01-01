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

#ifndef I_NCL_STRUCTURE_H_
#define I_NCL_STRUCTURE_H_

#include "ginga.h"

GINGA_DATAPROC_BEGIN

class INCLStructure
{
public:
  static const int ST_METADATA = 0x01;
  static const int ST_DATAFILE = 0x02;
  static const int ST_EVENTMAP = 0x03;
  virtual ~INCLStructure (){};
  virtual int getType () = 0;
  virtual int getId () = 0;
};

GINGA_DATAPROC_END

#endif /* I_NCL_STRUCTURE_H_ */
