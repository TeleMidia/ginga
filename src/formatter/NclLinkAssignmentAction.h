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

#ifndef LINKASSIGNMENTACTION_H_
#define LINKASSIGNMENTACTION_H_

#include "FormatterEvents.h"

#include "NclLinkRepeatAction.h"

GINGA_FORMATTER_BEGIN

class NclLinkAssignmentAction : public NclLinkRepeatAction
{
private:
  string value;
  Animation *animation;

public:
  NclLinkAssignmentAction (NclEvent *event,
                           SimpleActionType actionType,
                           string value);

  virtual ~NclLinkAssignmentAction ();

  string getValue ();
  void setValue (const string &value);
  Animation *getAnimation ();
  void setAnimation (Animation *animation);
};

GINGA_FORMATTER_END

#endif /*LINKASSIGNMENTACTION_H_*/
