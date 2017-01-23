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
#include "NclExecutionObjectSwitch.h"

GINGA_FORMATTER_BEGIN

NclExecutionObjectSwitch::NclExecutionObjectSwitch (
    string id, Node *switchNode, bool handling,
    INclLinkActionListener *seListener)
    : NclCompositeExecutionObject (id, switchNode, handling, seListener)
{
  selectedObject = NULL;
  typeSet.insert ("NclExecutionObjectSwitch");
}

NclExecutionObject *
NclExecutionObjectSwitch::getSelectedObject ()
{
  return selectedObject;
}

void
NclExecutionObjectSwitch::select (NclExecutionObject *executionObject)
{
  vector<NclFormatterEvent *> *eventsVector;
  vector<NclFormatterEvent *>::iterator i;
  NclSwitchEvent *switchEvent;

  if (executionObject != NULL
      && containsExecutionObject (executionObject->getId ()))
    {
      clog << "NclExecutionObjectSwitch::select '";
      clog << executionObject->getId () << "'" << endl;

      selectedObject = executionObject;
    }
  else
    {
      selectedObject = NULL;
      eventsVector = getEvents ();
      if (eventsVector != NULL)
        {
          i = eventsVector->begin ();
          while (i != eventsVector->end ())
            {
              switchEvent = (NclSwitchEvent *)(*i);
              switchEvent->setMappedEvent (NULL);
              ++i;
            }
          delete eventsVector;
          eventsVector = NULL;
        }
    }
}

bool
NclExecutionObjectSwitch::addEvent (NclFormatterEvent *event)
{
  if (event->instanceOf ("NclPresentationEvent")
      && (((NclPresentationEvent *)event)->getAnchor ())
             ->instanceOf ("LambdaAnchor"))
    {
      NclExecutionObject::wholeContent = (NclPresentationEvent *)event;
      return true;
    }
  else
    {
      return NclExecutionObject::addEvent (event);
    }
}

GINGA_FORMATTER_END
