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

#ifndef MEDIA_H
#define MEDIA_H

#include "Content.h"
#include "Descriptor.h"
#include "NodeEntity.h"
#include "ReferNode.h"

GINGA_NCL_BEGIN

class ReferNode;
class Media : public NodeEntity
{
public:
  Media (const string &, bool);
  virtual ~Media ();

  bool isSettings ();

  Content *getContent ();
  void setContent (Content *);

  Descriptor *getDescriptor ();
  void setDescriptor (Descriptor *);

  void addSameInstance (ReferNode *);
  const set<ReferNode *> *getInstSameInstances ();

private:
  bool _isSettings;
  Content *_content;
  Descriptor *_descriptor;
  set<ReferNode *> _instances;
};

GINGA_NCL_END

#endif // MEDIA_H
