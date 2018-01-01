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

#ifndef IAIT_H_
#define IAIT_H_

#include "IMpegDescriptor.h"

#include "IApplication.h"

GINGA_TSPARSER_BEGIN

class IAIT
{
public:
  virtual ~IAIT (){};
  virtual string getSectionName () = 0;
  virtual void setSectionName (const string &secName) = 0;
  virtual void setApplicationType (unsigned int type) = 0;
  virtual void process (void *payload, unsigned int payloadSize) = 0;
  virtual vector<IMpegDescriptor *> *copyDescriptors () = 0;
  virtual vector<IApplication *> *copyApplications () = 0;
};

GINGA_TSPARSER_END

#endif /* IAIT_H_ */
