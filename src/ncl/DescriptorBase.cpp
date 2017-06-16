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

#include "ginga.h"
#include "DescriptorBase.h"

GINGA_NCL_BEGIN

DescriptorBase::DescriptorBase (const string &id) : Base (id)
{
  _descriptorSet = new vector<GenericDescriptor *>;
  _typeSet.insert ("DescriptorBase");
}

DescriptorBase::~DescriptorBase ()
{
  vector<GenericDescriptor *>::iterator i;

  if (_descriptorSet != NULL)
    {
      i = _descriptorSet->begin ();
      while (i != _descriptorSet->end ())
        {
          delete *i;
          ++i;
        }
      delete _descriptorSet;
      _descriptorSet = NULL;
    }
}

bool
DescriptorBase::addDescriptor (GenericDescriptor *descriptor)
{
  if (descriptor == NULL)
    return false;

  vector<GenericDescriptor *>::iterator i;
  for (i = _descriptorSet->begin (); i != _descriptorSet->end (); ++i)
    {
      if (*i == descriptor)
        {
          return false;
        }
    }
  _descriptorSet->push_back (descriptor);
  return true;
}

bool
DescriptorBase::addBase (Base *base, const string &alias, const string &location)
{
  if (base->instanceOf ("DescriptorBase"))
    {
      return Base::addBase (base, alias, location);
    }
  return false;
}

void
DescriptorBase::clear ()
{
  _descriptorSet->clear ();
  Base::clear ();
}

GenericDescriptor *
DescriptorBase::getDescriptorLocally (const string &descriptorId)
{
  vector<GenericDescriptor *>::iterator descriptors;

  descriptors = _descriptorSet->begin ();
  while (descriptors != _descriptorSet->end ())
    {
      if ((*descriptors)->getId () == descriptorId)
        {
          return (*descriptors);
        }
      ++descriptors;
    }
  return NULL;
}

GenericDescriptor *
DescriptorBase::getDescriptor (const string &descriptorId)
{
  string::size_type index;
  string prefix, suffix;
  DescriptorBase *base;

  index = descriptorId.find_first_of ("#");
  if (index == string::npos)
    {
      return getDescriptorLocally (descriptorId);
    }
  prefix = descriptorId.substr (0, index);
  index++;
  suffix = descriptorId.substr (index, descriptorId.length () - index);
  if (_baseAliases.find (prefix) != _baseAliases.end ())
    {
      base = (DescriptorBase *)(_baseAliases[prefix]);
      return base->getDescriptor (suffix);
    }
  else if (_baseLocations.find (prefix) != _baseLocations.end ())
    {
      base = (DescriptorBase *)(_baseLocations[prefix]);
      return base->getDescriptor (suffix);
    }
  else
    {
      return NULL;
    }
}

vector<GenericDescriptor *> *
DescriptorBase::getDescriptors ()
{
  return _descriptorSet;
}

bool
DescriptorBase::removeDescriptor (GenericDescriptor *descriptor)
{
  vector<GenericDescriptor *>::iterator i;
  for (i = _descriptorSet->begin (); i != _descriptorSet->end (); ++i)
    {
      if (*i == descriptor)
        {
          _descriptorSet->erase (i);
          return true;
        }
    }
  return false;
}

GINGA_NCL_END
