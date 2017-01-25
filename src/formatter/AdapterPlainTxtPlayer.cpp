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
#include "AdapterPlainTxtPlayer.h"
#include "player/PlainTxtPlayer.h"

GINGA_FORMATTER_BEGIN

AdapterPlainTxtPlayer::AdapterPlainTxtPlayer () : AdapterFormatterPlayer ()
{
}

void
AdapterPlainTxtPlayer::createPlayer ()
{
  string paramValue;
  NclCascadingDescriptor *descriptor;

  if (fileExists (mrl))
    {
      player = new PlainTxtPlayer (mrl.c_str ());
    }
  else
    {
      return;
    }

  player->setPropertyValue ("x-setFile", mrl);
  descriptor = object->getDescriptor ();
  if (descriptor != NULL)
    {
      paramValue = xstrchomp (descriptor->getParameterValue ("fontUri"));
      if (paramValue == "")
        {
          paramValue = string (GINGA_FONT_DATADIR) + "/" + "vera.ttf";
        }
      player->setPropertyValue ("fontUri", paramValue);

      paramValue = xstrchomp (descriptor->getParameterValue ("fontSize"));
      if (paramValue == "")
        {
          paramValue = "10";
        }
      player->setPropertyValue ("fontSize", paramValue);

      paramValue = xstrchomp (descriptor->getParameterValue ("x-rgbBgColor"));
      if (paramValue != "")
        {
          player->setPropertyValue ("x-rgbBgColor", paramValue);
        }

      paramValue = xstrchomp (descriptor->getParameterValue ("fontColor"));
      if (paramValue == "")
        {
          paramValue
              = xstrchomp (descriptor->getParameterValue ("x-rgbFontColor"));

          if (paramValue == "")
            {
              paramValue = "255,255,255";
            }
          player->setPropertyValue ("x-rgbFontColor", paramValue);
        }
      else
        {
          player->setPropertyValue ("fontColor", paramValue);
        }
    }

  AdapterFormatterPlayer::createPlayer ();
}

GINGA_FORMATTER_END
