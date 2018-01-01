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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef NCL_LINK_H
#define NCL_LINK_H

#include "NclBind.h"

GINGA_NAMESPACE_BEGIN

class NclContext;
class NclLink: public NclEntity
{
public:
  NclLink (NclDocument *, const string &);
  ~NclLink ();

  const list<NclBind *> *getBinds ();
  void addBind (NclBind *);
  bool contains (NclNode *, bool);

  const map<string, string> *getGhostBinds ();
  string getGhostBind (const string &);
  void setGhostBind (const string &, const string &);

private:
  list<NclBind *> _binds;
  map<string, string> _ghost_binds;
};

GINGA_NAMESPACE_END

#endif // NCL_LINK_H
