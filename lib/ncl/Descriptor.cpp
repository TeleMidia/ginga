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
#include "Descriptor.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new descriptor.
 * @param ncl Parent document.
 * @param id Descriptor id.
 */
Descriptor::Descriptor (NclDocument *ncl, const string &id)
  : Entity (ncl, id)
{
  _region = nullptr;
}

/**
 * @brief Destroys descriptor.
 */
Descriptor::~Descriptor ()
{
  _parameters.clear ();
  _inputTransitions.clear ();
  _outputTransitions.clear ();
}

/**
 * @brief Gets descriptor region.
 */
Region *
Descriptor::getRegion ()
{
  return _region;
}

/**
 * @brief Initializes descriptor region.
 * @param region Region.
 */
void
Descriptor::initRegion (Region *region)
{
  g_assert_null (_region);
  g_assert_nonnull (region);
  _region = region;
}

/**
 * @brief Adds parameter to descriptor.
 * @param parameter Parameter.
 */
void
Descriptor::addParameter (Parameter *parameter)
{
  g_assert_nonnull (parameter);
  _parameters.push_back (parameter);
}

/**
 * @brief Gets all parameters.
 */
const vector<Parameter *> *
Descriptor::getParameters ()
{
  return &_parameters;
}

/**
 * @brief Gets parameter.
 * @param name Parameter name.
 * @return Parameter if successful, or null otherwise.
 */
Parameter *
Descriptor::getParameter (const string &name)
{
  for (auto param: _parameters)
    if (param->getName () == name)
      return param;
  return nullptr;
}

/**
 * @brief Adds input transition to descriptor.
 * @param transition Input transition.
 */
void
Descriptor::addInputTransition (Transition *transition)
{
  g_assert_nonnull (transition);
  _inputTransitions.push_back (transition);
}

/**
 * @brief Gets all input transitions.
 */
const vector<Transition *> *
Descriptor::getInputTransitions ()
{
  return &_inputTransitions;
}

/**
 * @brief Adds output transition to descriptor.
 * @param transition Output transition.
 */
void
Descriptor::addOutputTransition (Transition *transition)
{
  g_assert_nonnull (transition);
  _outputTransitions.push_back (transition);
}

/**
 * @brief Gets all output transitions.
 */
const vector<Transition *> *
Descriptor::getOutputTransitions ()
{
  return &_outputTransitions;
}

GINGA_NCL_END
