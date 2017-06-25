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
#include "Descriptor.h"

GINGA_NCL_BEGIN

Descriptor::Descriptor (const string &id) : GenericDescriptor (id)
{
  _explicitDuration = GINGA_TIME_NONE;
  _presentationTool = "";
  _repetitions = 0;
  _region = NULL;

  _keyNavigation = NULL;
  _focusDecoration = NULL;
}

Descriptor::~Descriptor ()
{
  map<string, Parameter *>::iterator i;

  if (_region != NULL)
    {
      // deleted through region base
      _region = NULL;
    }

  i = _parameters.begin ();
  while (i != _parameters.end ())
    {
      delete i->second;
      ++i;
    }

  if (_keyNavigation != NULL)
    {
      delete _keyNavigation;
      _keyNavigation = NULL;
    }

  if (_focusDecoration != NULL)
    {
      delete _focusDecoration;
      _focusDecoration = NULL;
    }
}

GingaTime
Descriptor::getExplicitDuration ()
{
  return _explicitDuration;
}

LayoutRegion *
Descriptor::getRegion ()
{
  return _region;
}

int
Descriptor::getRepetitions ()
{
  return _repetitions;
}

void
Descriptor::setExplicitDuration (GingaTime dur)
{
  _explicitDuration = dur;
}

void
Descriptor::setPlayerName (const string &name)
{
  _presentationTool = name;
}

void
Descriptor::setRegion (LayoutRegion *someRegion)
{
  _region = someRegion;
}

void
Descriptor::setRepetitions (int r)
{
  _repetitions = r;
}

void
Descriptor::addParameter (Parameter *parameter)
{
  string paramName;
  map<string, Parameter *>::iterator i;

  paramName = parameter->getName ();
  i = _parameters.find (paramName);
  if (i != _parameters.end () && i->second != parameter)
    {
      delete i->second;
    }

  _parameters[paramName] = parameter;
}

vector<Parameter *> *
Descriptor::getParameters ()
{
  vector<Parameter *> *ret = new vector<Parameter *>;
  map<string, Parameter *>::iterator it;
  for (it = _parameters.begin (); it != _parameters.end (); ++it)
    {
      ret->push_back (it->second);
    }
  return ret;
}

Parameter *
Descriptor::getParameter (const string &paramName)
{
  if (_parameters.count (paramName) == 0)
    {
      return NULL;
    }
  else
    {
      return _parameters[paramName];
    }
}

void
Descriptor::removeParameter (Parameter *parameter)
{
  map<string, Parameter *>::iterator it;

  it = _parameters.find (parameter->getName ());
  if (it != _parameters.end ())
    {
      _parameters.erase (it);
    }
}

KeyNavigation *
Descriptor::getKeyNavigation ()
{
  return _keyNavigation;
}

void
Descriptor::setKeyNavigation (KeyNavigation *keyNav)
{
  _keyNavigation = keyNav;
}

FocusDecoration *
Descriptor::getFocusDecoration ()
{
  return _focusDecoration;
}

void
Descriptor::setFocusDecoration (FocusDecoration *focusDec)
{
  _focusDecoration = focusDec;
}

vector<Transition *> *
Descriptor::getInputTransitions ()
{
  return &_inputTransitions;
}

bool
Descriptor::addInputTransition (Transition *transition, int somePos)
{
  unsigned int pos;
  pos = (unsigned int)somePos;
  if (pos > _inputTransitions.size () || transition == NULL)
    {
      return false;
    }

  if (pos == _inputTransitions.size ())
    {
      _inputTransitions.push_back (transition);
      return true;
    }

  vector<Transition *>::iterator i;
  i = _inputTransitions.begin () + pos;

  _inputTransitions.insert (i, transition);
  return true;
}

void
Descriptor::removeInputTransition (Transition *transition)
{
  vector<Transition *>::iterator i;
  i = _inputTransitions.begin ();
  while (i != _inputTransitions.end ())
    {
      if (*i == transition)
        {
          _inputTransitions.erase (i);
          break;
        }
      ++i;
    }
}

void
Descriptor::removeAllInputTransitions ()
{
  _inputTransitions.clear ();
}

vector<Transition *> *
Descriptor::getOutputTransitions ()
{
  return &_outputTransitions;
}

bool
Descriptor::addOutputTransition (Transition *transition, int somePos)
{
  unsigned int pos;
  pos = (unsigned int)somePos;
  if (pos > _outputTransitions.size () || transition == NULL)
    {
      return false;
    }

  if (pos == _outputTransitions.size ())
    {
      _outputTransitions.push_back (transition);
      return true;
    }

  vector<Transition *>::iterator i;
  i = _outputTransitions.begin () + pos;

  _outputTransitions.insert (i, transition);
  return true;
}

void
Descriptor::removeOutputTransition (Transition *transition)
{
  vector<Transition *>::iterator i;
  i = _outputTransitions.begin ();
  while (i != _outputTransitions.end ())
    {
      if (*i == transition)
        {
          _outputTransitions.erase (i);
          break;
        }
      ++i;
    }
}

void
Descriptor::removeAllOutputTransitions ()
{
  _outputTransitions.clear ();
}

GINGA_NCL_END
