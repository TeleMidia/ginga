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

#ifndef CASCADING_DESCRIPTOR_H
#define CASCADING_DESCRIPTOR_H

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN
class NclCascadingDescriptor
{
private:
  static int dummyCount;
  string id;
  vector<GenericDescriptor *> descriptors;
  vector<GenericDescriptor *> unsolvedDescriptors;
  map<string, string> parameters;
  GingaTime explicitDuration;
  int repetitions;
  LayoutRegion *region;

  string focusIndex, moveUp, moveDown, moveLeft, moveRight;
  string focusSrc, selectionSrc;
  SDL_Color focusBorderColor;
  SDL_Color selBorderColor;
  int focusBorderWidth;
  int selBorderWidth;
  double focusBorderTransparency;
  vector<Transition *> *inputTransitions;
  vector<Transition *> *outputTransitions;

  void cascadeDescriptor (Descriptor *descriptor);

public:
  NclCascadingDescriptor (GenericDescriptor *firstDescriptor);
  NclCascadingDescriptor (NclCascadingDescriptor *descriptor);
  ~NclCascadingDescriptor ();

protected:
  void initializeCascadingDescriptor ();

public:
  string getId ();
  bool isLastDescriptor (GenericDescriptor *descriptor);
  void cascade (GenericDescriptor *preferredDescriptor);
  GenericDescriptor *getUnsolvedDescriptor (int i);
  vector<GenericDescriptor *> *getUnsolvedDescriptors ();
  void cascadeUnsolvedDescriptor ();
  GingaTime getExplicitDuration ();
  LayoutRegion *getRegion ();

public:
  void setFormatterLayout ();
  int getRepetitions ();
  vector<Parameter> getParameters ();
  string getParameterValue (const string &paramName);
  vector<GenericDescriptor *> *getNcmDescriptors ();
  SDL_Color getFocusBorderColor ();
  double getFocusBorderTransparency ();
  int getFocusBorderWidth ();
  string getFocusIndex ();
  string getFocusSrc ();
  string getSelectionSrc ();
  string getMoveDown ();
  string getMoveLeft ();
  string getMoveRight ();
  string getMoveUp ();
  SDL_Color getSelBorderColor ();
  int getSelBorderWidth ();
  vector<Transition *> *getInputTransitions ();
  vector<Transition *> *getOutputTransitions ();
};

GINGA_FORMATTER_END

#endif // CASCADING_DESCRIPTOR_H
