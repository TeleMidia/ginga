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
#include "player/INCLPlayer.h"
using namespace ::ginga::player;

#include "AdapterNCLPlayer.h"

#include "mb/LocalScreenManager.h"
#include "mb/ScreenManagerFactory.h"
using namespace ::ginga::mb;

#include "gingancl/FormatterMediator.h"

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_NCL_BEGIN

AdapterNCLPlayer::AdapterNCLPlayer () : AdapterApplicationPlayer ()
{
  typeSet.insert ("AdapterNCLPlayer");
}

void
AdapterNCLPlayer::createPlayer ()
{
  NclFormatterRegion *region = NULL;
  LayoutRegion *ncmRegion;
  NclCascadingDescriptor *descriptor = NULL;
  string value;
  bool isPercent;
  GingaSurfaceID s;
  NclPlayerData *childData, *playerData;
  PropertyAnchor *property;

  if (getObjectDevice () == 2)
    {
      clog << "AdapterNCLPlayer::createPlayer ";
      clog << " remote handler" << endl;
      return;
    }

  player = NULL;
  if (fileExists (mrl))
    {
      playerCompName = "Formatter";
      playerData = ((AdapterPlayerManager *)manager)->getNclPlayerData ();

      childData = new NclPlayerData;
      childData->screenId = myScreen;
      childData->x = 0;
      childData->y = 0;
      childData->w = 0;
      childData->h = 0;
      childData->devClass = playerData->devClass;
      childData->transparency = playerData->transparency;
      childData->baseId = playerData->baseId;
      childData->privateBaseManager = playerData->privateBaseManager;
      childData->playerId = object->getId ();
      childData->enableGfx = false;
      childData->parentDocId = playerData->docId;
      childData->nodeId
          = ((NodeEntity *)(object->getDataObject ()->getDataEntity ()))
                ->getId ();

      childData->docId = "";
      childData->focusManager = playerData->focusManager;
      childData->editListener = playerData->editListener;

      descriptor = object->getDescriptor ();
      if (descriptor != NULL)
        {
          region = descriptor->getFormatterRegion ();
        }

      if (region != NULL)
        {
          ncmRegion = region->getLayoutRegion ();
          childData->x = (int)(ncmRegion->getAbsoluteLeft ());
          childData->y = (int)(ncmRegion->getAbsoluteTop ());
          childData->w = (int)(ncmRegion->getWidthInPixels ());
          childData->h = (int)(ncmRegion->getHeightInPixels ());
          childData->devClass = ncmRegion->getDeviceClass ();

          property = object->getNCMProperty ("transparency");
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
              float transpValue;
              float parentOpacity = (1 - playerData->transparency);

              value = cvtPercentual (value, &isPercent);
              transpValue = ::ginga::util::stof (value);
              if (isPercent)
                {
                  transpValue = transpValue / 100;
                }

              transpValue
                  = (1 - (parentOpacity - (parentOpacity * transpValue)));

              childData->transparency = transpValue;
            }
        }

      LocalScreenManager *dm;

      dm = ScreenManagerFactory::getInstance ();
      player = (INCLPlayer *)(new FormatterMediator (childData));

      s = dm->createSurface (myScreen);
      int cap = dm->getSurfaceCap (s, "ALPHACHANNEL");
      dm->setSurfaceCaps (s, cap);

      ((INCLPlayer *)player)->setSurface (s);
      if (((INCLPlayer *)player)->setCurrentDocument (mrl) == NULL)
        {
          clog << "AdapterNCLPlayer::createPlayer Warning! ";
          clog << "can't set '" << mrl << "' as document";
          clog << endl;
        }

      if (region != NULL)
        {
          ((INCLPlayer *)player)
              ->setParentLayout (region->getLayoutManager ());
        }
    }
  else
    {
      clog << "AdapterNCLPlayer::createPlayer Warning! ";
      clog << "file not found: '" << mrl << "'" << endl;
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
      ((NclApplicationExecutionObject *)object)
          ->setCurrentEvent (currentEvent);

      if (player != NULL)
        {
          player->setCurrentScope (interfaceId);
        }
    }
  else if (event->instanceOf ("NclAttributionEvent"))
    {
      interfaceId
          = ((NclAttributionEvent *)event)->getAnchor ()->getPropertyName ();

      if (player != NULL)
        {
          player->setScope (interfaceId, IPlayer::TYPE_ATTRIBUTION);
        }

      currentEvent = event;
      ((NclApplicationExecutionObject *)object)
          ->setCurrentEvent (currentEvent);

      if (player != NULL)
        {
          player->setCurrentScope (interfaceId);
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

void
AdapterNCLPlayer::flip ()
{
  if (player != NULL)
    {
      player->flip ();
    }
}

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_NCL_END
