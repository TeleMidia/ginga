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

#ifndef NclDocumentConverter_H
#define NclDocumentConverter_H

#include "ncl/Node.h"
using namespace ::ginga::ncl;

#include "ModuleParser.h"
#include "NclDocumentParser.h"

#include "ncl/NclDocument.h"
using namespace ::ginga::ncl;

#include "formatter/PrivateBaseContext.h"
using namespace ::ginga::formatter;

GINGA_NCLCONV_BEGIN

class NclDocumentConverter : public NclDocumentParser
{
private:
  PrivateBaseContext *privateBaseContext;
  bool ownManager;

  void *parentObject;

public:
  NclDocumentConverter ();
  virtual ~NclDocumentConverter ();

  void setConverterInfo (PrivateBaseContext *pbc,
                         DeviceLayout *deviceLayout);

protected:
  void checkManager ();
  virtual void initialize ();
  virtual void *parseRootElement (DOMElement *rootElement);

public:
  string getAttribute (void *element, const string &attribute);
  Node *getNode (const string &id);
  bool removeNode (Node *node);
  PrivateBaseContext *getPrivateBaseContext ();
  NclDocument *importDocument (string &docLocation);

  void *
  parse (const string &uri)
  {
    return NclDocumentParser::parse (uri);
  }
};

GINGA_NCLCONV_END

#endif
