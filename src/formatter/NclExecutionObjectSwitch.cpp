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
    const string &id, Node *switchNode, bool handling,
    INclLinkActionListener *seListener)
    : NclCompositeExecutionObject (id, switchNode, nullptr, handling, seListener)
{
  _selectedObject = nullptr;
  _typeSet.insert ("NclExecutionObjectSwitch");
}

NclExecutionObject *
NclExecutionObjectSwitch::getSelectedObject ()
{
  return _selectedObject;
}

void
NclExecutionObjectSwitch::select (NclExecutionObject *executionObject)
{
  NclSwitchEvent *switchEvent;

  if (executionObject != nullptr
      && containsExecutionObject (executionObject->getId ()))
    {
      _selectedObject = executionObject;
    }
  else
    {
      _selectedObject = NULL;
      for (NclFormatterEvent *evt: getEvents())
        {
          switchEvent = dynamic_cast<NclSwitchEvent *> (evt);
          g_assert_nonnull (switchEvent);
          switchEvent->setMappedEvent (NULL);
        }
    }
}

bool
NclExecutionObjectSwitch::addEvent (NclFormatterEvent *evt)
{
 auto presentationEvt = dynamic_cast <NclPresentationEvent *> (evt);

  if (presentationEvt
      && dynamic_cast<LambdaAnchor *> (presentationEvt->getAnchor ()))
    {
      NclExecutionObject::_wholeContent = presentationEvt;
      return true;
    }
  else
    {
      return NclExecutionObject::addEvent (evt);
    }
}

GINGA_FORMATTER_END
