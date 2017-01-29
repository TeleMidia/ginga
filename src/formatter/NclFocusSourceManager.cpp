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
#include "NclFocusSourceManager.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

#include "player/IPlayer.h"
using namespace ::ginga::player;

#include "util/functions.h"
using namespace ::ginga::util;

GINGA_FORMATTER_BEGIN

SDLSurface*
NclFocusSourceManager::getFocusSourceComponent (string src)
{
  string::size_type index;
  string extension;
  SDLSurface* s;

  if (src != "")
    {
      index = src.find_last_of ('.');
      if (index != std::string::npos)
        {
          index++;
          extension = src.substr (index, src.length () - index);

          if (extension == "png" || extension == "gif" || extension == "jpg"
              || extension == "jpeg" || extension == "bmp"
              || extension == "bpg" || extension == "svg"
              || extension == "svgz" || extension == "webp")
            {
              s = Ginga_Display->createRenderedSurfaceFromImageFile
                (src.c_str ());

              return s;
            }
        }
    }
  return 0;
}

GINGA_FORMATTER_END
