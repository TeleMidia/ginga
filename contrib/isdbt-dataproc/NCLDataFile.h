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

#ifndef _DataFile_H_
#define _DataFile_H_

#include "INCLDataFile.h"

GINGA_DATAPROC_BEGIN

class NCLDataFile : public INCLDataFile
{
private:
  int structureId;
  string componentTag;
  string uri;
  double size;

public:
  NCLDataFile (int id);
  virtual ~NCLDataFile ();

  int getId ();

  void setComponentTag (const string &componentTag);
  string getCopmonentTag ();

  void setUri (const string &uri);
  string getUri ();

  void setSize (double size);
  double getSize ();
};

GINGA_DATAPROC_END

#endif //_DataFile_H_
