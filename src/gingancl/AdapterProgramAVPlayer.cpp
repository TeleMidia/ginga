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
#include "AdapterProgramAVPlayer.h"
#include "player/ProgramAV.h"

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_AV_TV_BEGIN

AdapterProgramAVPlayer::AdapterProgramAVPlayer () : AdapterFormatterPlayer ()
{
  typeSet.insert ("AdapterProgramAVPlayer");
  lastValue = "";
}

AdapterProgramAVPlayer *AdapterProgramAVPlayer::_instance = 0;

AdapterProgramAVPlayer *
AdapterProgramAVPlayer::getInstance ()
{
  if (AdapterProgramAVPlayer::_instance == NULL)
    {
      AdapterProgramAVPlayer::_instance = new AdapterProgramAVPlayer ();
    }

  return AdapterProgramAVPlayer::_instance;
}

bool
AdapterProgramAVPlayer::hasPrepared ()
{
  return object != NULL;
}

bool
AdapterProgramAVPlayer::start ()
{
  return AdapterFormatterPlayer::start ();
}

bool
AdapterProgramAVPlayer::stop ()
{
  player->setPropertyValue ("bounds", ""); // full screen

  // TODO: create a map to handle each default value (before AIT app start cmd)
  player->setPropertyValue ("soundLevel", "1.0"); // full sound

  return AdapterFormatterPlayer::stop ();
}

bool
AdapterProgramAVPlayer::resume ()
{
  updateAVBounds ();
  return AdapterFormatterPlayer::resume ();
}

void
AdapterProgramAVPlayer::createPlayer ()
{
  CascadingDescriptor *descriptor;
  string soundLevel;

  player = ProgramAV::getInstance (myScreen);

  AdapterFormatterPlayer::createPlayer ();
  updateAVBounds ();
}

bool
AdapterProgramAVPlayer::setPropertyValue (AttributionEvent *event,
                                          string value)
{

  string propName;
  bool attribution;

  attribution = AdapterFormatterPlayer::setPropertyValue (event, value);

  propName = event->getAnchor ()->getPropertyName ();
  clog << "AdapterProgramAVPlayer::setPropertyValue '";
  clog << propName << "' ";
  clog << value << "'" << endl;

  if (propName == "size" || propName == "location" || propName == "bounds"
      || propName == "top" || propName == "left" || propName == "bottom"
      || propName == "right" || propName == "width" || propName == "height")
    {

      updateAVBounds ();
    }
  else if (propName == "standby")
    {
      if (value == "true")
        {
          player->setPropertyValue ("bounds", ""); // fullscreen
          lastValue = player->getPropertyValue ("soundLevel");
          player->setPropertyValue ("soundLevel", ""); // audio
        }
      else
        {
          updateAVBounds ();
          player->setPropertyValue ("soundLevel", lastValue); // audio
        }
    }
  else
    {
      player->setPropertyValue (propName, value);
    }

  return attribution;
}

void
AdapterProgramAVPlayer::updateAVBounds ()
{
  CascadingDescriptor *descriptor;
  FormatterRegion *region;
  LayoutRegion *ncmRegion;

  if (object != NULL && object->getDescriptor () != NULL)
    {
      descriptor = object->getDescriptor ();
      region = descriptor->getFormatterRegion ();
      ncmRegion = region->getLayoutRegion ();

      player->setPropertyValue (
          "bounds", itos (ncmRegion->getAbsoluteLeft ()) + ","
                        + itos (ncmRegion->getAbsoluteTop ()) + ","
                        + itos (ncmRegion->getWidthInPixels ()) + ","
                        + itos (ncmRegion->getHeightInPixels ()));
    }
  else
    {
      clog << "AdapterProgramAVPlayer::updateAVBounds Warning!";
      clog << "NULL object or descriptor" << endl;
    }
}

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_AV_TV_END
