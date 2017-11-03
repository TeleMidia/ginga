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

#ifndef NCL_BIND_H
#define NCL_BIND_H

#include "NclNode.h"
#include "NclRole.h"

GINGA_NAMESPACE_BEGIN

class NclBind
{
public:
  NclBind (NclRole *, NclNode *, NclAnchor *);
  ~NclBind ();

  NclRole *getRole ();
  NclNode *getNode ();
  NclAnchor *getInterface ();

  const map<string, string> *getParameters ();
  string getParameter (const string &);
  void setParameter (const string &, const string &);

private:
  NclRole *_role;
  NclNode *_node;
  NclAnchor *_interface;
  map<string, string> _params;
};

GINGA_NAMESPACE_END

#endif // NCL_BIND_H
