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

#ifndef NCL_NODE_H
#define NCL_NODE_H

#include "NclAnchor.h"

GINGA_NAMESPACE_BEGIN

class NclComposition;
class NclNode: public NclEntity
{
public:
  NclNode (NclDocument *, const string &);
  virtual ~NclNode ();

  NclComposition *getParent ();
  void initParent (NclComposition *);

  void addAnchor (NclAnchor *);
  const list<NclAnchor *> *getAnchors ();
  NclAnchor *getAnchor (const string &);
  NclAnchor *getLambda ();

  bool hasProperty (const string &);
  void setProperty (const string &, const string &);
  string getProperty (const string &);

  NclNode *derefer ();

private:
  NclComposition *_parent;
  list<NclAnchor *> _anchors;
  NclAnchor *_lambda;
};

GINGA_NAMESPACE_END

#endif // NCL_NODE_H
