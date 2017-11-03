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

#ifndef NCL_MEDIA_H
#define NCL_MEDIA_H

#include "NclNode.h"

GINGA_NAMESPACE_BEGIN

class NclMediaRefer;
class NclMedia: public NclNode
{
public:
  NclMedia (NclDocument *, const string &, bool);
  ~NclMedia ();

  bool isSettings ();

  string getMimeType ();
  string getSrc ();
  void setSrc (const string &);

  void addSameInstance (NclMediaRefer *);
  const set<NclMediaRefer *> *getInstSameInstances ();

private:
  bool _isSettings;
  string _src;
  string _mimetype;
  set<NclMediaRefer *> _instances;
};

GINGA_NAMESPACE_END

#endif // NCL_MEDIA_H
