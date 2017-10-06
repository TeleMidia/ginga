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

#include "aux-ginga.h"
#include "DescriptorBase.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new descriptor base.
 * @param ncl Parent document.
 * @param id Base id.
 */
DescriptorBase::DescriptorBase (NclDocument *ncl, const string &id)
  : Base (ncl, id)
{
}

/**
 * @brief Destroys descriptor base.
 */
DescriptorBase::~DescriptorBase ()
{
}

/**
 * @brief Adds descriptor to base.
 * @param descriptor Descriptor.
 */
void
DescriptorBase::addDescriptor (Descriptor *descriptor)
{
  Base::addEntity (descriptor);
}

/**
 * @brief Gets descriptor.
 * @param id Descriptor id.
 * @return Descriptor if successful, or null if not found.
 */
Descriptor *
DescriptorBase::getDescriptor (const string &id)
{
  return cast (Descriptor *, Base::getEntity (id));
}

GINGA_NCL_END
