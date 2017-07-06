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

#include "Property.h"

GINGA_NCL_BEGIN

class CompositeNode;

class Node : public Entity
{
public:
  Node (const string &);
  virtual ~Node ();

  void setParentComposition (CompositeNode *);
  CompositeNode *getParentComposition ();
  vector<Node *> *getPerspective ();
  virtual bool addAnchor (Anchor *);
  virtual bool addAnchor (int, Anchor *);
  Anchor *getAnchor (const string &);
  Anchor *getAnchor (int);
  const vector<Anchor *> &getAnchors ();

  Property *getProperty (const string &);

protected:
  vector<Anchor *> _anchorList;
  vector<Property *> _originalPAnchors;

private:
  CompositeNode *_parentNode;
  bool hasProperty (const string &propName);
};

GINGA_NCL_END

#endif // NODE_H
