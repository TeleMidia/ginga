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
#include "AdapterBerkeliumPlayer.h"

#include "AdaptersComponentSupport.h"

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_XHTML_BEGIN

AdapterBerkeliumPlayer::AdapterBerkeliumPlayer ()
    : AdapterFormatterPlayer ()
{
}

void
AdapterBerkeliumPlayer::rebase ()
{
  clog << "AdapterBerkeliumPlayer::rebase" << endl;

  player->reset ();
  updateProperties ();
  player->rebase ();
}

void
AdapterBerkeliumPlayer::updateProperties ()
{
  LayoutRegion *ncmRegion;
  NclCascadingDescriptor *descriptor;
  int w, h, x, y;
  string value, strW, strH, strX, strY;

  x = 1;
  y = 1;
  w = 1;
  h = 1;

  descriptor = object->getDescriptor ();
  if (descriptor == NULL)
    {
      return;
    }

  ncmRegion = descriptor->getRegion ();
  if (ncmRegion != NULL)
    {
      x = (int)(ncmRegion->getAbsoluteLeft ());
      y = (int)(ncmRegion->getAbsoluteTop ());
      w = (int)(ncmRegion->getWidthInPixels ());
      h = (int)(ncmRegion->getHeightInPixels ());
    }

  strX = itos (x);
  strY = itos (y);
  strW = itos (w);
  strH = itos (h);
  value = strX + "," + strY + "," + strW + "," + strH;

  clog << "AdapterBerkeliumPlayer::updateProperties bounds = '";
  clog << value << "'" << endl;

  player->setPropertyValue ("bounds", value);
}

void
AdapterBerkeliumPlayer::createPlayer ()
{
  if (mrl != "")
    {

      if (mrl.substr (0, 1) == SystemCompat::getIUriD ())
        {
          mrl = SystemCompat::updatePath (mrl);
        }

      player = new BerkeliumPlayer (myScreen, mrl.c_str ());

      updateProperties ();
    }

  AdapterFormatterPlayer::createPlayer ();
}

bool
AdapterBerkeliumPlayer::setPropertyValue (NclAttributionEvent *event,
                                          string value)
{

  string propName;
  propName = (event->getAnchor ())->getPropertyName ();
  if (propName == "size" || propName == "bounds" || propName == "top"
      || propName == "left" || propName == "bottom" || propName == "right"
      || propName == "width" || propName == "height")
    {

      if (player != NULL)
        {
          LayoutRegion *ncmRegion;
          NclCascadingDescriptor *descriptor;
          int x, y, w, h;
          string bVal, strW, strH, strX, strY;

          descriptor = object->getDescriptor ();
          ncmRegion = descriptor->getRegion ();

          x = (int)(ncmRegion->getAbsoluteLeft ());
          y = (int)(ncmRegion->getAbsoluteTop ());
          w = (int)(ncmRegion->getWidthInPixels ());
          h = (int)(ncmRegion->getHeightInPixels ());

          strX = itos (x);
          strY = itos (y);
          strW = itos (w);
          strH = itos (h);
          bVal = strX + "," + strY + "," + strW + "," + strH;

          clog << "AdapterBerkeliumPlayer::setPropertyValue bounds = '";
          clog << bVal << "'" << endl;

          player->setPropertyValue ("bounds", bVal);
        }
    }

  return AdapterFormatterPlayer::setPropertyValue (event, value);
}

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_XHTML_END
