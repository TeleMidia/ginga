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
#include "AdapterLuaPlayer.h"
#include "player/LuaPlayer.h"

GINGA_FORMATTER_BEGIN

AdapterLuaPlayer::AdapterLuaPlayer () : AdapterApplicationPlayer () {}

void
AdapterLuaPlayer::createPlayer ()
{
  _player = new LuaPlayer (_mrl.c_str ());
  AdapterApplicationPlayer::createPlayer ();
}

bool
AdapterLuaPlayer::setAndLockCurrentEvent (NclFormatterEvent *event)
{
  string interfaceId;

  lockEvent ();
  if (_preparedEvents.count (event->getId ()) != 0
      && !event->instanceOf ("NclSelectionEvent")
      && event->instanceOf ("NclAnchorEvent"))
    {
      interfaceId = ((NclAnchorEvent *)event)->getAnchor ()->getId ();

      if ((((NclAnchorEvent *)event)->getAnchor ())
              ->instanceOf ("LabeledAnchor"))
        {
          interfaceId
              = ((LabeledAnchor *)((NclAnchorEvent *)event)->getAnchor ())
                    ->getLabel ();
        }
      else if ((((NclAnchorEvent *)event)->getAnchor ())
                   ->instanceOf ("LambdaAnchor"))
        {
          interfaceId = "";
        }

      _currentEvent = event;
      ((NclApplicationExecutionObject *)_object)
          ->setCurrentEvent (_currentEvent);

      _player->setCurrentScope (interfaceId);
    }
  else if (event->instanceOf ("NclAttributionEvent"))
    {
      interfaceId = ((NclAttributionEvent *)event)
                        ->getAnchor ()
                        ->getPropertyName ();

      _player->setScope (interfaceId, IPlayer::TYPE_ATTRIBUTION);

      _currentEvent = event;
      ((NclApplicationExecutionObject *)_object)
          ->setCurrentEvent (_currentEvent);

      _player->setCurrentScope (interfaceId);
    }
  else
    {
      clog << "AdapterLuaPlayer::setAndLockCurrentEvent Warning! ";
      clog << "event '" << event->getId () << "' isn't prepared" << endl;

      unlockEvent ();
      return false;
    }

  return true;
}

void
AdapterLuaPlayer::unlockCurrentEvent (NclFormatterEvent *event)
{
  if (event != _currentEvent)
    {
      clog << "AdapterLuaPlayer::unlockCurrentEvent ";
      clog << "Handling events Warning: currentEvent = '";
      if (_currentEvent != NULL)
        {
          clog << _currentEvent->getId ();
        }
      clog << "' event requested to unlock = '" << event->getId ();
      clog << "'" << endl;
    }
  unlockEvent ();
}

GINGA_FORMATTER_END
