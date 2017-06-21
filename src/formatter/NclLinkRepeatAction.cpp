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
#include "NclLinkRepeatAction.h"

GINGA_FORMATTER_BEGIN

NclLinkRepeatAction::NclLinkRepeatAction (NclEvent *event,
                                          SimpleActionType actionType)
    : NclLinkSimpleAction (event, actionType)
{
  this->repetitions = 0;
  this->repetitionInterval = 0;

  typeSet.insert ("NclLinkRepeatAction");
}

NclLinkRepeatAction::~NclLinkRepeatAction ()
{
}

int
NclLinkRepeatAction::getRepetitions ()
{
  return repetitions;
}

GingaTime
NclLinkRepeatAction::getRepetitionInterval ()
{
  return repetitionInterval;
}

void
NclLinkRepeatAction::setRepetitions (int repetitions)
{
  this->repetitions = repetitions;
}

void
NclLinkRepeatAction::setRepetitionInterval (GingaTime delay)
{
  this->repetitionInterval = delay;
}

void
NclLinkRepeatAction::run ()
{
  if (NclLinkSimpleAction::event != NULL)
    {
      if (NclLinkSimpleAction::event->instanceOf ("PresentationEvent"))
        {
          ((PresentationEvent *)event)
              ->setRepetitionSettings (repetitions, repetitionInterval);
        }
    }
  else
    {
      clog << "NclLinkRepeatAction::run Warning! event == NULL" << endl;
    }

  NclLinkSimpleAction::run ();
}

GINGA_FORMATTER_END
