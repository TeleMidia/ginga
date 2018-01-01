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

#ifndef _INCLSectionProcessor_H_
#define _INCLSectionProcessor_H_

#include "INCLMetadata.h"

GINGA_DATAPROC_BEGIN

class INCLSectionProcessor
{
public:
  virtual ~INCLSectionProcessor (){};

  virtual string getRootUri () = 0;

  // receiving stream
  virtual void process (char *stream, int streamSize) = 0;
  virtual void mount () = 0;
  virtual bool isConsolidated () = 0;
  virtual INCLMetadata *getMetadata () = 0;

  // creating stream
  virtual vector<StreamData *> *
  createNCLSections (const string &componentTag, const string &name, const string &baseUri,
                     vector<string> *files, map<int, string> *eventMap)
      = 0;
};

GINGA_DATAPROC_END

#endif //_INCLSectionProcessor_H_
