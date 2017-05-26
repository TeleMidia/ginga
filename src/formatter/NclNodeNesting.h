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

#ifndef NODENESTING_H_
#define NODENESTING_H_

#include "system/Thread.h"
using namespace ::ginga::system;

#include "ncl/Node.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class NclNodeNesting
{
private:
  string id;
  vector<Node *> *nodes;
  set<string> typeSet;
  pthread_mutex_t mutexNodes;

public:
  NclNodeNesting ();
  NclNodeNesting (Node *node);
  NclNodeNesting (NclNodeNesting *seq);
  NclNodeNesting (vector<Node *> *seq);
  virtual ~NclNodeNesting ();

private:
  void initialize ();

public:
  bool instanceOf (const string &s);
  void append (NclNodeNesting *otherSeq);
  void append (vector<Node *> *otherSeq);
  Node *getAnchorNode ();
  Node *getHeadNode ();
  Node *getNode (int index);
  int getNumNodes ();
  void insertAnchorNode (Node *node);
  void insertHeadNode (Node *node);
  bool removeAnchorNode ();
  bool removeHeadNode ();
  string getId ();
  NclNodeNesting *copy ();
};

GINGA_FORMATTER_END

#endif /*NODENESTING_H_*/
