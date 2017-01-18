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

#include "config.h"
#include "LinkRepeatAction.h"

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_BEGIN

LinkRepeatAction::LinkRepeatAction (FormatterEvent *event, short actionType)
    : LinkSimpleAction (event, actionType)
{

  this->repetitions = 0;
  this->repetitionInterval = 0;

  typeSet.insert ("LinkRepeatAction");
}

LinkRepeatAction::~LinkRepeatAction ()
{
  isDeleting = true;

  // clog << "LinkRepeatAction::~LinkRepeatAction" << endl;
}

long
LinkRepeatAction::getRepetitions ()
{
  return repetitions;
}

double
LinkRepeatAction::getRepetitionInterval ()
{
  return repetitionInterval;
}

void
LinkRepeatAction::setRepetitions (long repetitions)
{
  this->repetitions = repetitions;
}

void
LinkRepeatAction::setRepetitionInterval (double delay)
{
  this->repetitionInterval = delay;
}

void
LinkRepeatAction::run ()
{
  if (LinkSimpleAction::event != NULL)
    {
      if (LinkSimpleAction::event->instanceOf ("PresentationEvent"))
        {
          ((PresentationEvent *)event)
              ->setRepetitionSettings (repetitions, repetitionInterval);
        }
    }
  else
    {
      clog << "LinkRepeatAction::run Warning! event == NULL" << endl;
    }

  LinkSimpleAction::run ();
}

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_END
