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
#include "player/INCLPlayer.h"
using namespace ::ginga::player;

#include "AdapterNCLPlayer.h"
#include "FormatterMediator.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

GINGA_FORMATTER_BEGIN

AdapterNCLPlayer::AdapterNCLPlayer () : AdapterApplicationPlayer ()
{
}

void
AdapterNCLPlayer::createPlayer ()
{
  NclFormatterRegion *region = NULL;
  LayoutRegion *ncmRegion;
  NclCascadingDescriptor *descriptor = NULL;
  string value;
  bool isPercent;
  NclPlayerData *childData, *playerData;
  PropertyAnchor *property;

  _player = NULL;
  playerData = _manager->getNclPlayerData ();

  childData = new NclPlayerData;
  childData->w = 0;
  childData->h = 0;
  childData->devClass = playerData->devClass;
  childData->transparency = playerData->transparency;
  childData->baseId = playerData->baseId;
  childData->privateBaseContext = playerData->privateBaseContext;
  childData->playerId = _object->getId ();
  childData->parentDocId = playerData->docId;
  childData->nodeId
    = ((NodeEntity *)(_object->getDataObject ()->getDataEntity ()))
    ->getId ();

  childData->docId = "";
  childData->focusManager = playerData->focusManager;

  descriptor = _object->getDescriptor ();
  if (descriptor != NULL)
    {
      region = descriptor->getFormatterRegion ();
    }

  if (region != NULL)
    {
      ncmRegion = region->getLayoutRegion ();
      childData->w = (int)(ncmRegion->getWidthInPixels ());
      childData->h = (int)(ncmRegion->getHeightInPixels ());
      childData->devClass = ncmRegion->getDeviceClass ();

      property = _object->getNCMProperty ("transparency");
      if (property != NULL)
        {
          value = property->getPropertyValue ();
        }
      else
        {
          value = descriptor->getParameterValue ("transparency");
        }

      if (value != "")
        {
          double transpValue;
          double parentOpacity = (1 - playerData->transparency);

          transpValue = xstrtodorpercent (value, &isPercent);

          transpValue
            = (1 - (parentOpacity - (parentOpacity * transpValue)));

          childData->transparency = transpValue;
        }
    }

  _player = (INCLPlayer *)(new FormatterMediator ());

  if (((INCLPlayer *)_player)->setCurrentDocument (_mrl) == NULL)
    {
      clog << "AdapterNCLPlayer::createPlayer Warning! ";
      clog << "can't set '" << _mrl << "' as document";
      clog << endl;
    }

  if (region != NULL)
    {
      ((INCLPlayer *)_player)
        ->setParentLayout (region->getLayoutManager ());
    }

  AdapterApplicationPlayer::createPlayer ();
}

bool
AdapterNCLPlayer::setAndLockCurrentEvent (NclFormatterEvent *event)
{
  string interfaceId;

  lockEvent ();
  if (preparedEvents.count (event->getId ()) != 0
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

      currentEvent = event;
      ((NclApplicationExecutionObject *)_object)
          ->setCurrentEvent (currentEvent);

      if (_player != NULL)
        {
          _player->setCurrentScope (interfaceId);
        }
    }
  else if (event->instanceOf ("NclAttributionEvent"))
    {
      interfaceId = ((NclAttributionEvent *)event)
                        ->getAnchor ()
                        ->getPropertyName ();

      if (_player != NULL)
        {
          _player->setScope (interfaceId, IPlayer::TYPE_ATTRIBUTION);
        }

      currentEvent = event;
      ((NclApplicationExecutionObject *)_object)
          ->setCurrentEvent (currentEvent);

      if (_player != NULL)
        {
          _player->setCurrentScope (interfaceId);
        }
    }
  else
    {
      unlockEvent ();
      return false;
    }

  return true;
}

void
AdapterNCLPlayer::unlockCurrentEvent (NclFormatterEvent *event)
{
  if (event != currentEvent)
    {
      clog << "AdapterNCLPlayer::unlockCurrentEvent ";
      clog << "Handling events warning!" << endl;
    }
  unlockEvent ();
}

GINGA_FORMATTER_END
