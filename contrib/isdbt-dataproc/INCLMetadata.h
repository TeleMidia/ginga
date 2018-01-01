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

#ifndef _IMetadata_H_
#define _IMetadata_H_

#include "INCLDataFile.h"
#include "INCLStructure.h"

GINGA_DATAPROC_BEGIN

typedef struct
{
  char *stream;
  int size;
} StreamData;

class INCLMetadata : public INCLStructure
{
public:
  virtual ~INCLMetadata (){};

  int
  getType ()
  {
    return ST_METADATA;
  };
  int
  getId ()
  {
    return 0;
  };

  virtual string getRootUri () = 0;
  virtual string getName () = 0;
  virtual double getTotalLength () = 0;
  virtual void setTargetTotalLength (double targetSize) = 0;
  virtual bool isConsolidated () = 0;

  virtual void setBaseUri (const string &uri) = 0;
  virtual string getBaseUri () = 0;

  virtual void setRootObject (INCLDataFile *rootObject) = 0;
  virtual INCLDataFile *getRootObject () = 0;

  virtual void addDataFile (INCLDataFile *dataObject) = 0;
  virtual INCLDataFile *getDataFile (int structureId) = 0;
  virtual map<int, INCLDataFile *> *getDataFiles () = 0;

  virtual vector<StreamData *> *createNCLSections () = 0;
};

GINGA_DATAPROC_END

#endif //_IMetadata_H_
