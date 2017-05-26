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
#include "NclSwitchEvent.h"

GINGA_FORMATTER_BEGIN

NclSwitchEvent::NclSwitchEvent (const string &id, void *executionObjectSwitch,
                                InterfacePoint *interfacePoint,
                                int eventType, const string &key)
    : NclFormatterEvent (id, executionObjectSwitch)
{
  this->interfacePoint = interfacePoint;
  this->eventType = (short) eventType;
  this->key = key;
  this->mappedEvent = NULL;

  typeSet.insert ("NclSwitchEvent");
}

NclSwitchEvent::~NclSwitchEvent ()
{
  if (NclFormatterEvent::hasInstance (mappedEvent, false))
    {
      mappedEvent->removeEventListener (this);
      mappedEvent = NULL;
    }
}

InterfacePoint *
NclSwitchEvent::getInterfacePoint ()
{
  return interfacePoint;
}

short
NclSwitchEvent::getEventType ()
{
  return eventType;
}

string
NclSwitchEvent::getKey ()
{
  return key;
}

void
NclSwitchEvent::setMappedEvent (NclFormatterEvent *event)
{
  if (mappedEvent != NULL)
    {
      mappedEvent->removeEventListener (this);
    }

  mappedEvent = event;
  if (mappedEvent != NULL)
    {
      mappedEvent->addEventListener (this);
    }
}

NclFormatterEvent *
NclSwitchEvent::getMappedEvent ()
{
  return mappedEvent;
}

void
NclSwitchEvent::eventStateChanged (arg_unused (void *someEvent), short transition,
                                   arg_unused (short previousState))
{
  changeState (getNewState (transition), transition);
}

GINGA_FORMATTER_END
