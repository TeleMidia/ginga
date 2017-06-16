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

#ifndef NODE_H_
#define NODE_H_

#include "Entity.h"
#include "Anchor.h"
#include "PropertyAnchor.h"

GINGA_NCL_BEGIN

class CompositeNode;

class Node : public Entity
{
public:
  Node (const string &_id);
  virtual ~Node ();

  void copyProperties (Node *node);
  void setParentComposition (CompositeNode *composition);
  CompositeNode *getParentComposition ();
  vector<Node *> *getPerspective ();
  virtual bool addAnchor (Anchor *anchor);
  virtual bool addAnchor (int index, Anchor *anchor);
  Anchor *getAnchor (const string &anchorId);
  Anchor *getAnchor (int index);

  const vector<Anchor *> &getAnchors ();
  vector<PropertyAnchor *> *getOriginalPropertyAnchors ();

  PropertyAnchor *getPropertyAnchor (const string &propertyName);
  int getNumAnchors ();
  int indexOfAnchor (Anchor *anchor);
  virtual bool removeAnchor (int index);
  virtual bool removeAnchor (Anchor *anchor);

protected:
  vector<Anchor *> _anchorList;
  vector<PropertyAnchor *> _originalPAnchors;

private:
  CompositeNode *_parentNode;

  bool hasProperty (const string &propName);
};

GINGA_NCL_END

#endif /*NODE_H_*/
