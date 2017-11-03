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

#ifndef NCL_CONNECTOR_BASE_H
#define NCL_CONNECTOR_BASE_H

#include "NclConnectorBase.h"
#include "NclConnector.h"

GINGA_BEGIN

class NclConnectorBase: NclEntity
{
public:
  NclConnectorBase (NclDocument *, const string &);
  virtual ~NclConnectorBase ();

  void addBase (NclConnectorBase *, const string &, const string &);

  void addConnector (NclConnector *);
  NclConnector *getConnector (const string &);

private:
  vector<NclConnectorBase *> _bases;
  vector <NclEntity *> _entities;
  map<string, NclConnectorBase *> _aliases;
  map<string, NclConnectorBase *> _locations;

  void addEntity (NclEntity *);
  NclEntity *getEntity (const string &);
  NclConnectorBase *getHashBase (const string &, string *, string *);
};

GINGA_END

#endif // NCL_CONNECTOR_BASE_H
