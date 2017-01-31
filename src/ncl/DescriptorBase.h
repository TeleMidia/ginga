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

#ifndef DESCRIPTORBASE_H_
#define DESCRIPTORBASE_H_

#include "Base.h"

#include "GenericDescriptor.h"

GINGA_NCL_BEGIN

class DescriptorBase : public Base
{
private:
  vector<GenericDescriptor *> *descriptorSet;

public:
  DescriptorBase (const string &id);
  virtual ~DescriptorBase ();

  bool addDescriptor (GenericDescriptor *descriptor);
  bool addBase (Base *base, const string &alias, const string &location);

  void clear ();

private:
  GenericDescriptor *getDescriptorLocally (const string &descriptorId);

public:
  GenericDescriptor *getDescriptor (const string &descriptorId);
  vector<GenericDescriptor *> *getDescriptors ();
  bool removeDescriptor (GenericDescriptor *descriptor);
};

GINGA_NCL_END

#endif /*DESCRIPTORBASE_H_*/
