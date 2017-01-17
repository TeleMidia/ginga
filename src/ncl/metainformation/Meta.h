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
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef META_H
#define META_H

#include "namespaces.h"

BR_PUCRIO_TELEMIDIA_NCL_METAINFORMATION_BEGIN

class Meta
{
private:
  string name;
  void* content;

public:
  Meta(string name, void* content);
  void* getContent();
  string getName();
  void setContent(void* content);
  void setName(string name);
};

BR_PUCRIO_TELEMIDIA_NCL_METAINFORMATION_END

#endif /* META_H */
