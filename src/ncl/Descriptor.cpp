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

Descriptor::Descriptor (const string &id) : Entity (id)
{
  _explicitDuration = GINGA_TIME_NONE;
  _focusDecoration = nullptr;
  _keyNavigation = nullptr;
  _region = nullptr;
  _repetitions = 0;
}

Descriptor::~Descriptor ()
{
  if (_focusDecoration != nullptr)
    delete _focusDecoration;

  if (_keyNavigation != nullptr)
    delete _keyNavigation;

  _parameters.clear ();
  _inputTransitions.clear ();
  _outputTransitions.clear ();

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
  g_assert_nonnull (parameter);
  _parameters.push_back (parameter);
}

const vector<Parameter *> *
Descriptor::getParameters ()
{
  return &_parameters;
}

Parameter *
Descriptor::getParameter (const string &name)
{
  for (auto param: _parameters)
    if (param->getName () == name)
      return param;
  return nullptr;
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

GINGA_NCL_END
