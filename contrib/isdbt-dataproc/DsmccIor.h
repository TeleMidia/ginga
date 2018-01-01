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

#ifndef IOR_H
#define IOR_H

#include "ginga.h"

GINGA_DATAPROC_BEGIN

#define TAG_BIOP 0x49534F06
#define TAG_BIOP_OBJECT_LOCATION 0x49534F50

class DsmccIor
{
private:
  string typeId;
  unsigned int carouselId;
  unsigned int moduleId;
  unsigned int objectKey;

public:
  DsmccIor ();
  void setTypeId (const string &typeId);
  string getTypeId ();
  void setCarouselId (unsigned int carouselId);
  unsigned int getCarouselId ();
  void setModuleId (unsigned int moduleId);
  unsigned int getModuleId ();
  void setObjectKey (unsigned int objectKey);
  unsigned int getObjectKey ();
  void print ();
};

GINGA_DATAPROC_END

#endif /* IOR_H */
