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
#include "NclCascadingDescriptor.h"

#include "ExecutionObject.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

GINGA_PRAGMA_DIAG_IGNORE (-Wfloat-conversion)
GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_FORMATTER_BEGIN

int NclCascadingDescriptor::dummyCount = 0;

NclCascadingDescriptor::NclCascadingDescriptor (
    Descriptor *firstDescriptor)
{
  initializeCascadingDescriptor ();

  if (firstDescriptor != NULL)
    {
      cascade (firstDescriptor);
    }
}

NclCascadingDescriptor::NclCascadingDescriptor (
    NclCascadingDescriptor *descriptor)
{
  int i, size;

  initializeCascadingDescriptor ();
  if (descriptor != NULL)
    {
      size = (int)((NclCascadingDescriptor *)descriptor)->descriptors.size ();
      for (i = 0; i < size; i++)
        {
          cascade (
              (Descriptor *)(((NclCascadingDescriptor *)descriptor)
                                        ->descriptors[i]));
        }

      size = (int)((NclCascadingDescriptor *)descriptor)
                 ->unsolvedDescriptors.size ();

      for (i = 0; i < size; i++)
        {
          cascade (
              (Descriptor *)(((NclCascadingDescriptor *)descriptor)
                                        ->unsolvedDescriptors[i]));
        }
    }
}

NclCascadingDescriptor::~NclCascadingDescriptor ()
{
  vector<Descriptor *>::iterator i;

  if (inputTransitions != NULL)
    {
      delete inputTransitions;
      inputTransitions = NULL;
    }

  if (outputTransitions != NULL)
    {
      delete outputTransitions;
      outputTransitions = NULL;
    }
}

void
NclCascadingDescriptor::initializeCascadingDescriptor ()
{
  id = "";
  explicitDuration = GINGA_TIME_NONE;
  repetitions = 0;
  region = NULL;

  focusIndex = "";
  moveUp = "";
  moveDown = "";
  moveLeft = "";
  moveRight = "";

  focusSrc = "";
  selectionSrc = "";
  focusBorderColor = {0, 0, 255, 255};
  selBorderColor = {0, 255, 0, 255};
  focusBorderWidth = 0;
  selBorderWidth = 0;
  focusBorderTransparency = (double) NAN;
  inputTransitions = new vector<Transition *>;
  outputTransitions = new vector<Transition *>;
}

string
NclCascadingDescriptor::getId ()
{
  return id;
}

void
NclCascadingDescriptor::cascadeDescriptor (Descriptor *descriptor)
{
  vector<Transition *> *transitions = NULL;

  region = descriptor->getRegion ();
  if (!isnan (descriptor->getExplicitDuration ()))
    {
      explicitDuration = descriptor->getExplicitDuration ();
    }

  if (descriptor->getKeyNavigation () != NULL)
    {
      KeyNavigation *keyNavigation;
      keyNavigation = (KeyNavigation *)(descriptor->getKeyNavigation ());

      if (keyNavigation->getFocusIndex () != "")
        {
          focusIndex = keyNavigation->getFocusIndex ();
        }

      if (keyNavigation->getMoveUp () != "")
        {
          moveUp = keyNavigation->getMoveUp ();
        }

      if (keyNavigation->getMoveDown () != "")
        {
          moveDown = keyNavigation->getMoveDown ();
        }

      if (keyNavigation->getMoveLeft () != "")
        {
          moveLeft = keyNavigation->getMoveLeft ();
        }

      if (keyNavigation->getMoveRight () != "")
        {
          moveRight = keyNavigation->getMoveRight ();
        }
    }

  if (descriptor->getFocusDecoration () != NULL)
    {
      FocusDecoration *focusDecoration;
      focusDecoration
          = (FocusDecoration *)(descriptor->getFocusDecoration ());

      this->focusBorderColor = focusDecoration->getFocusBorderColor ();
      this->selBorderColor = focusDecoration->getSelBorderColor ();
      focusBorderWidth = focusDecoration->getFocusBorderWidth ();

      if (focusDecoration->getFocusSrc () != "")
        {
          focusSrc = focusDecoration->getFocusSrc ();
        }

      if (focusDecoration->getFocusSelSrc () != "")
        {
          selectionSrc = focusDecoration->getFocusSelSrc ();
        }
    }

  transitions = descriptor->getInputTransitions ();
  if (transitions != NULL)
    {
      if (this->inputTransitions == NULL)
        {
          this->inputTransitions = new vector<Transition *>;
        }

      inputTransitions->insert (inputTransitions->end (),
                                transitions->begin (), transitions->end ());
    }

  transitions = descriptor->getOutputTransitions ();
  if (transitions != NULL)
    {
      if (this->outputTransitions == NULL)
        {
          this->outputTransitions = new vector<Transition *>;
        }

      outputTransitions->insert (outputTransitions->end (),
                                 transitions->begin (),
                                 transitions->end ());
    }
}

bool
NclCascadingDescriptor::isLastDescriptor (Descriptor *descriptor)
{
  if ((descriptors.size () > 0)
      && (descriptor->getId ()
          == ((Descriptor *)descriptors[descriptors.size () - 1])
                 ->getId ()))

    return true;
  else
    return false;
}

void
NclCascadingDescriptor::cascade (Descriptor *descriptor)
{
  Descriptor *preferredDescriptor;

  preferredDescriptor = descriptor;

  if ((preferredDescriptor == NULL)
      || instanceof (NclCascadingDescriptor *, preferredDescriptor))

    return;

  if (isLastDescriptor (preferredDescriptor))
    return;

  descriptors.push_back (preferredDescriptor);
  if (id == "")
    id = preferredDescriptor->getId ();
  else
    id = id + "+" + preferredDescriptor->getId ();

  if (instanceof (Descriptor *, preferredDescriptor)
      && unsolvedDescriptors.empty ())
    {
      cascadeDescriptor ((Descriptor *)preferredDescriptor);
    }
  else
    {
      unsolvedDescriptors.push_back (preferredDescriptor);
    }
}

Descriptor *
NclCascadingDescriptor::getUnsolvedDescriptor (int i)
{
  if ((size_t) i >= unsolvedDescriptors.size ())
    {
      return NULL;
    }

  return unsolvedDescriptors.at (i);
}

vector<Descriptor *> *
NclCascadingDescriptor::getUnsolvedDescriptors ()
{
  return &unsolvedDescriptors;
}

void
NclCascadingDescriptor::cascadeUnsolvedDescriptor ()
{
  if (unsolvedDescriptors.empty ())
    return;

  Descriptor *genericDescriptor, *descriptor;

  genericDescriptor = (Descriptor *)(unsolvedDescriptors[0]);

  descriptor = (Descriptor *)genericDescriptor;
  unsolvedDescriptors.erase (unsolvedDescriptors.begin ());
  if (isLastDescriptor (descriptor))
    {
      return;
    }

  cascadeDescriptor ((Descriptor *)descriptor);
}

GingaTime
NclCascadingDescriptor::getExplicitDuration ()
{
  return this->explicitDuration;
}

LayoutRegion *
NclCascadingDescriptor::getRegion ()
{
  return region;
}

int
NclCascadingDescriptor::getRepetitions ()
{
  return repetitions;
}

vector<Parameter>
NclCascadingDescriptor::getParameters ()
{
  vector<Parameter> params;

  for (auto p: parameters)
    {
      params.emplace_back (p.first, p.second);
    }
  return params;
}

string
NclCascadingDescriptor::getParameterValue (const string &paramName)
{
  string::size_type pos;
  string paramValue;

  if (parameters.count (paramName) == 0)
    {
      return "";
    }

  paramValue = parameters[paramName];

  if (paramValue == "")
    {
      return paramValue;
    }

  pos = paramValue.find_last_of ("%");
  if (pos != std::string::npos && pos == paramValue.length () - 1)
    {
      double d;
      paramValue = paramValue.substr (0, paramValue.length () - 1);
      if (_xstrtod (paramValue, &d))
        xstrassign (paramValue, "%f", d / 100);
    }

  return paramValue;
}

vector<Descriptor *> *
NclCascadingDescriptor::getNcmDescriptors ()
{
  return &descriptors;
}

SDL_Color
NclCascadingDescriptor::getFocusBorderColor ()
{
  return focusBorderColor;
}

double
NclCascadingDescriptor::getFocusBorderTransparency ()
{
  return focusBorderTransparency;
}

int
NclCascadingDescriptor::getFocusBorderWidth ()
{
  return focusBorderWidth;
}

string
NclCascadingDescriptor::getFocusIndex ()
{
  return focusIndex;
}

string
NclCascadingDescriptor::getFocusSrc ()
{
  return focusSrc;
}

string
NclCascadingDescriptor::getSelectionSrc ()
{
  return selectionSrc;
}

string
NclCascadingDescriptor::getMoveDown ()
{
  return moveDown;
}

string
NclCascadingDescriptor::getMoveLeft ()
{
  return moveLeft;
}

string
NclCascadingDescriptor::getMoveRight ()
{
  return moveRight;
}

string
NclCascadingDescriptor::getMoveUp ()
{
  return moveUp;
}

SDL_Color
NclCascadingDescriptor::getSelBorderColor ()
{
  return selBorderColor;
}

int
NclCascadingDescriptor::getSelBorderWidth ()
{
  return selBorderWidth;
}

vector<Transition *> *
NclCascadingDescriptor::getInputTransitions ()
{
  return this->inputTransitions;
}

vector<Transition *> *
NclCascadingDescriptor::getOutputTransitions ()
{
  return this->outputTransitions;
}

GINGA_FORMATTER_END
