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

#ifndef NCL_DOCUMENT_H
#define NCL_DOCUMENT_H

#include "NclContext.h"

GINGA_NAMESPACE_BEGIN

class NclDocument
{
public:
  NclDocument (const string &, const string &);
  ~NclDocument ();

  string getId ();
  string getURI ();
  NclContext *getRoot ();

  NclEntity *getEntityById (const string &);
  bool registerEntity (NclEntity *);
  bool unregisterEntity (NclEntity *);

  list<NclNode *> *getSettingsNodes ();

private:
  string _id;
  string _uri;
  NclContext *_root;
  map<string, NclEntity *> _entities;
};

GINGA_NAMESPACE_END

#endif // NCL_DOCUMENT_H
