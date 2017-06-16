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
#include "DescriptorSwitch.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCL_BEGIN

set<DescriptorSwitch *> DescriptorSwitch::_objects;

void
DescriptorSwitch::addInstance (DescriptorSwitch *object)
{
  _objects.insert (object);
}

bool
DescriptorSwitch::removeInstance (DescriptorSwitch *object)
{
  set<DescriptorSwitch *>::iterator i;
  bool removed = false;

  i = _objects.find (object);
  if (i != _objects.end ())
    {
      _objects.erase (i);
      removed = true;
    }

  return removed;
}

bool
DescriptorSwitch::hasInstance (DescriptorSwitch *object, bool eraseFromList)
{
  set<DescriptorSwitch *>::iterator i;
  bool hasDSwitch = false;

  i = _objects.find (object);
  if (i != _objects.end ())
    {
      if (eraseFromList)
        {
          _objects.erase (i);
        }
      hasDSwitch = true;
    }

  return hasDSwitch;
}

DescriptorSwitch::DescriptorSwitch (const string &id) : GenericDescriptor (id)
{
  _descriptorList = new vector<GenericDescriptor *>;
  _ruleList = new vector<Rule *>;
  _defaultDescriptor = NULL;
  _selectedDescriptor = NULL;

  addInstance (this);

  _typeSet.insert ("DescriptorSwitch");
}

DescriptorSwitch::~DescriptorSwitch ()
{
  vector<GenericDescriptor *>::iterator i;
  vector<Rule *>::iterator j;

  removeInstance (this);

  if (_descriptorList != NULL)
    {
      i = _descriptorList->begin ();
      while (i != _descriptorList->end ())
        {
          delete *i;
          ++i;
        }

      delete _descriptorList;
      _descriptorList = NULL;
    }

  if (_ruleList != NULL)
    {
      delete _ruleList;
      _ruleList = NULL;
    }
}

bool
DescriptorSwitch::addDescriptor (unsigned int index,
                                 GenericDescriptor *descriptor, Rule *rule)
{
  if (index > _descriptorList->size ()
      || getDescriptor (descriptor->getId ()) != NULL)
    {
      return false;
    }

  vector<Rule *>::iterator it;
  for (it = _ruleList->begin (); it != _ruleList->end (); it++)
    {
      if (*it == rule)
        return false;
    }

  if (index == _descriptorList->size ())
    {
      _descriptorList->push_back (descriptor);
      _ruleList->push_back (rule);
    }
  else
    {
      _descriptorList->insert (_descriptorList->begin () + index, descriptor);
      _ruleList->insert (_ruleList->begin () + index, rule);
    }
  return true;
}

bool
DescriptorSwitch::addDescriptor (GenericDescriptor *descriptor, Rule *rule)
{
  return addDescriptor ((int) _descriptorList->size (), descriptor, rule);
}

bool
DescriptorSwitch::containsRule (Rule *rule)
{
  vector<Rule *>::iterator iterRule;
  for (iterRule = _ruleList->begin (); iterRule != _ruleList->end ();
       ++iterRule)
    {
      if ((*iterRule)->getId () == rule->getId ())
        {
          return true;
        }
    }
  return false;
}

void
DescriptorSwitch::exchangeDescriptorsAndRules (unsigned int index1,
                                               unsigned int index2)
{
  if (index1 >= _descriptorList->size ()
      || index2 >= _descriptorList->size ())
    {
      return;
    }

  GenericDescriptor *auxDesc;
  auxDesc = static_cast<GenericDescriptor *> ((*_descriptorList)[index1]);
  Rule *auxRule = static_cast<Rule *> ((*_ruleList)[index1]);
  (*_descriptorList)[index1] = (*_descriptorList)[index2];
  (*_descriptorList)[index2] = auxDesc;
  (*_ruleList)[index1] = (*_ruleList)[index2];
  (*_ruleList)[index2] = auxRule;
}

int
DescriptorSwitch::indexOfRule (Rule *rule)
{
  int i = 0;
  vector<Rule *>::iterator it;
  for (it = _ruleList->begin (); it != _ruleList->end (); it++)
    {
      if (*it == rule)
        return i;
      i++;
    }
  return (int) _ruleList->size () + 1;
}

GenericDescriptor *
DescriptorSwitch::getDefaultDescriptor ()
{
  return _defaultDescriptor;
}

unsigned int
DescriptorSwitch::indexOfDescriptor (GenericDescriptor *descriptor)
{
  unsigned int i = 0;
  vector<GenericDescriptor *>::iterator iterDescr;

  for (iterDescr = _descriptorList->begin ();
       iterDescr != _descriptorList->end (); ++iterDescr)
    {
      if ((*(*iterDescr)).getId () == descriptor->getId ())
        return i;
      i++;
    }
  return (int) _descriptorList->size () + 1;
}

GenericDescriptor *
DescriptorSwitch::getDescriptor (unsigned int index)
{
  if (index >= _descriptorList->size ())
    return NULL;

  return static_cast<GenericDescriptor *> ((*_descriptorList)[index]);
}

GenericDescriptor *
DescriptorSwitch::getDescriptor (const string &descriptorId)
{
  int i, size;
  GenericDescriptor *descriptor;

  if (_defaultDescriptor != NULL)
    {
      if (_defaultDescriptor->getId () == descriptorId)
        {
          return _defaultDescriptor;
        }
    }

  size = (int) _descriptorList->size ();
  for (i = 0; i < size; i++)
    {
      descriptor = (*_descriptorList)[i];
      if (descriptor->getId () == descriptorId)
        {
          return descriptor;
        }
    }

  return NULL;
}

GenericDescriptor *
DescriptorSwitch::getDescriptor (Rule *rule)
{
  unsigned int index;

  index = indexOfRule (rule);
  if (index > _ruleList->size ())
    return NULL;

  return static_cast<GenericDescriptor *> ((*_descriptorList)[index]);
}

Rule *
DescriptorSwitch::getRule (unsigned int index)
{
  if (index >= _ruleList->size ())
    return NULL;

  return static_cast<Rule *> ((*_ruleList)[index]);
}

unsigned int
DescriptorSwitch::getNumDescriptors ()
{
  return (unsigned int) _descriptorList->size ();
}

unsigned int
DescriptorSwitch::getNumRules ()
{
  return (unsigned int) _ruleList->size ();
}

bool
DescriptorSwitch::removeDescriptor (unsigned int index)
{
  if (index >= _descriptorList->size ())
    return false;

  vector<GenericDescriptor *>::iterator iterDescr;
  iterDescr = _descriptorList->begin ();
  iterDescr = iterDescr + index;
  _descriptorList->erase (iterDescr);
  return true;
}

bool
DescriptorSwitch::removeDescriptor (GenericDescriptor *descriptor)
{
  return removeDescriptor (indexOfDescriptor (descriptor));
}

bool
DescriptorSwitch::removeRule (Rule *rule)
{
  int index;

  index = indexOfRule (rule);
  if (index < 0 || index > (int)_ruleList->size ())
    return false;

  _descriptorList->erase (_descriptorList->begin () + index);
  _ruleList->erase (_ruleList->begin () + index);
  return true;
}

void
DescriptorSwitch::setDefaultDescriptor (GenericDescriptor *descriptor)
{
  _defaultDescriptor = descriptor;
}

void
DescriptorSwitch::select (GenericDescriptor *descriptor)
{
  vector<GenericDescriptor *>::iterator i;

  i = _descriptorList->begin ();
  while (i != _descriptorList->end ())
    {
      if (*i == descriptor)
        {
          _selectedDescriptor = descriptor;
          break;
        }
      ++i;
    }
}

void
DescriptorSwitch::selectDefault ()
{
  if (_defaultDescriptor != NULL)
    {
      _selectedDescriptor = _defaultDescriptor;
    }
}

GenericDescriptor *
DescriptorSwitch::getSelectedDescriptor ()
{
  return _selectedDescriptor;
}

GINGA_NCL_END
