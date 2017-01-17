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

#ifndef CONTENT_H
#define CONTENT_H

#include "ginga.h"

BR_PUCRIO_TELEMIDIA_NCL_COMPONENTS_BEGIN

class Content
{
protected:
  string type;
  long size;
  set<string> typeSet;

public:
  Content(string someType, long someSize);
  Content();
  virtual ~Content();
  bool instanceOf(string s);
  virtual long getSize(void);
  virtual string getType(void);
  virtual void setSize(long someSize);
  virtual void setType(string someType);
};

BR_PUCRIO_TELEMIDIA_NCL_COMPONENTS_END

#endif /* CONTENT_H */
