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

#ifndef NCL_PORT_H
#define NCL_PORT_H

#include "NclAnchor.h"

GINGA_NAMESPACE_BEGIN

class NclComposition;
class NclNode;
class NclPort: public NclAnchor
{
public:
  NclPort (NclDocument *, const string &);
  ~NclPort ();

  NclComposition *getParent ();
  void initParent (NclComposition *);

  NclNode *getNode ();
  void initNode (NclNode *);

  NclAnchor *getInterface ();
  void initInterface (NclAnchor *);

  void getTarget (NclNode **, NclAnchor **);

protected:
  NclComposition *_parent;
  NclNode *_node;
  NclAnchor *_interface;

  NclNode *getFinalNode ();
  NclAnchor *getFinalInterface ();
};

GINGA_NAMESPACE_END

#endif // NCL_PORT_H
