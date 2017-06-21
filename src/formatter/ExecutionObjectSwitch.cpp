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
#include "ExecutionObjectSwitch.h"

GINGA_FORMATTER_BEGIN

ExecutionObjectSwitch::ExecutionObjectSwitch (
    const string &id, Node *switchNode, bool handling,
    INclLinkActionListener *seListener)
    : ExecutionObjectContext (id, switchNode, nullptr, handling,
                                   seListener)
{
  _selectedObj = nullptr;
  _typeSet.insert ("ExecutionObjectSwitch");
}

void
ExecutionObjectSwitch::select (ExecutionObject *exeObj)
{
  if (exeObj != nullptr
      && containsExecutionObject (exeObj->getId ()))
    {
      _selectedObj = exeObj;
    }
  else
    {
      _selectedObj = nullptr;
      for (NclEvent *evt: getEvents())
        {
          auto switchEvent = dynamic_cast<SwitchEvent *> (evt);
          g_assert_nonnull (switchEvent);
          switchEvent->setMappedEvent (nullptr);
        }
    }
}

bool
ExecutionObjectSwitch::addEvent (NclEvent *evt)
{
 auto presentationEvt = dynamic_cast<PresentationEvent *> (evt);

  if (presentationEvt
      && dynamic_cast<LambdaAnchor *> (presentationEvt->getAnchor ()))
    {
      ExecutionObject::_wholeContent = presentationEvt;
      return true;
    }
  else
    {
      return ExecutionObject::addEvent (evt);
    }
}

GINGA_FORMATTER_END
