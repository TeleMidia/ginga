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

#ifndef PRIVATE_BASE_CONTEXT_H
#define PRIVATE_BASE_CONTEXT_H

#include "ncl/PrivateBase.h"
#include "ncl/NclDocument.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class PrivateBaseContext
{
private:
  map<string, NclDocument *> baseDocuments;
  map<string, NclDocument *> visibleDocuments;
  map<NclDocument *, DeviceLayout *> layouts;
  set<DeviceLayout *> layoutsGB; // layouts garbage collector (oh my god)
  PrivateBase *privateBase;

public:
  PrivateBaseContext ();
  virtual ~PrivateBaseContext ();
  void createPrivateBase (const string &id);

private:
  NclDocument *compileDocument (const string &location,
                                DeviceLayout *deviceLayout);

public:
  NclDocument *addDocument (const string &location, DeviceLayout *deviceLayout);

  void *addVisibleDocument (const string &location, DeviceLayout *deviceLayout);
  NclDocument *getVisibleDocument (const string &docId);

private:
  bool eraseVisibleDocument (const string &docLocation);

public:
  string getDocumentLocation (const string &docId);
  NclDocument *getDocument (const string &id);
  vector<NclDocument *> *getDocuments ();

private:
  void removeDocumentBase (NclDocument *document, Base *base);
  void removeDocumentBases (NclDocument *document);

public:
  NclDocument *removeDocument (const string &id);

private:
  NclDocument *getBaseDocument (const string &documentId);

public:
  PrivateBase *getPrivateBase ();
};

GINGA_FORMATTER_END

#endif // PRIVATE_BASE_CONTEXT_H
