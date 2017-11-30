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

#ifndef NCL_COMPOSITION_H
#define NCL_COMPOSITION_H

#include "NclNode.h"
#include "NclPort.h"

GINGA_NAMESPACE_BEGIN

class NclComposition: public NclNode
{
public:
  NclComposition (NclDocument *, const string &);
  ~NclComposition ();

  void addNode (NclNode *);
  const list<NclNode *> *getNodes ();
  NclNode *getNode (const string &);
  NclNode *getNestedNode (const string &);

  void addPort (NclPort *);
  const list<NclPort *> *getPorts ();
  NclPort *getPort (const string &);

  NclAnchor *getMapInterface (NclPort *);

protected:
  list<NclNode *> _nodes;
  list<NclPort *> _ports;
};

GINGA_NAMESPACE_END

#endif // NCL_COMPOSITION_H
