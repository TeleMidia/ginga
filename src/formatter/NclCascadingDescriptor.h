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

#ifndef _CASCADINGDESCRIPTOR_H_
#define _CASCADINGDESCRIPTOR_H_

#include "ncl/Parameter.h"
using namespace ::ginga::ncl;

#include "ncl/Descriptor.h"
#include "ncl/GenericDescriptor.h"
using namespace ::ginga::ncl;

#include "ncl/LayoutRegion.h"
using namespace ::ginga::ncl;

#include "ncl/DescriptorSwitch.h"
using namespace ::ginga::ncl;

#include "ncl/FocusDecoration.h"
#include "ncl/KeyNavigation.h"
using namespace ::ginga::ncl;

#include "ncl/Transition.h"
using namespace ::ginga::ncl;

#include "NclFormatterRegion.h"

GINGA_FORMATTER_BEGIN

class NclCascadingDescriptor
{
protected:
  set<string> typeSet;

private:
  static int dummyCount;
  string id;
  vector<GenericDescriptor *> descriptors;
  vector<GenericDescriptor *> unsolvedDescriptors;
  map<string, string> parameters;
  double explicitDuration;
  string playerName;
  int repetitions;
  bool freeze;
  LayoutRegion *region;
  NclFormatterRegion *formatterRegion;

  string focusIndex, moveUp, moveDown, moveLeft, moveRight;
  string focusSrc, selectionSrc;
  SDL_Color *focusBorderColor;
  SDL_Color *selBorderColor;
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
  double getExplicitDuration ();
  bool getFreeze ();
  string getPlayerName ();
  LayoutRegion *getRegion ();
  NclFormatterRegion *getFormatterRegion ();

public:
  void setFormatterLayout (NclFormatterLayout *formatterLayout);
  int getRepetitions ();
  vector<Parameter> getParameters ();
  string getParameterValue (const string &paramName);
  vector<GenericDescriptor *> *getNcmDescriptors ();
  SDL_Color *getFocusBorderColor ();
  double getFocusBorderTransparency ();
  int getFocusBorderWidth ();
  string getFocusIndex ();
  string getFocusSrc ();
  string getSelectionSrc ();
  string getMoveDown ();
  string getMoveLeft ();
  string getMoveRight ();
  string getMoveUp ();
  SDL_Color *getSelBorderColor ();
  int getSelBorderWidth ();
  vector<Transition *> *getInputTransitions ();
  vector<Transition *> *getOutputTransitions ();
};

GINGA_FORMATTER_END

#endif //_CASCADINGDESCRIPTOR_H_
