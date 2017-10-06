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

#ifndef NODE_H
#define NODE_H

#include "Anchor.h"

GINGA_NCL_BEGIN

class Composition;
class Node : public Entity
{
public:
  Node (NclDocument *, const string &);
  virtual ~Node ();

  Composition *getParent ();
  void initParent (Composition *);

  void addAnchor (Anchor *);
  const vector<Anchor *> *getAnchors ();
  Anchor *getAnchor (const string &);
  Anchor *getLambda ();

  bool hasProperty (const string &);
  void setProperty (const string &, const string &);
  string getProperty (const string &);

  vector<Node *> *getPerspective (); // fixme
  Node *derefer ();                  // fixme

private:
  Composition *_parent;
  vector<Anchor *> _anchors;
  Anchor *_lambda;
};

GINGA_NCL_END

#endif // NODE_H
