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
#include "AdapterMirrorPlayer.h"
#include "AdapterPlayerManager.h"

GINGA_FORMATTER_BEGIN

AdapterMirrorPlayer::AdapterMirrorPlayer () : AdapterFormatterPlayer () {}

AdapterMirrorPlayer::~AdapterMirrorPlayer () { player = NULL; }

void
AdapterMirrorPlayer::createPlayer ()
{
  NclFormatterRegion *fRegion;
  NclCascadingDescriptor *descriptor;
  LayoutRegion *ncmRegion = NULL;
  SDLSurface* mirrorSur;

  clog << "AdapterMirrorPlayer::createPlayer '" << mrl << "'" << endl;

  string prefix = "ncl-mirror://";
  AdapterFormatterPlayer *sourceAdapter = NULL;
  NclExecutionObject *execObjSrc;
  size_t pos;

  AdapterFormatterPlayer::createPlayer ();

  pos = mrl.find (prefix);
  assert (pos != std::string::npos);
  if (object != NULL && object->getMirrorSrc () != NULL && player != NULL)
    {
      execObjSrc = object->getMirrorSrc ();
      sourceAdapter
          = (AdapterFormatterPlayer *)((AdapterPlayerManager *)manager)
                ->getObjectPlayer (execObjSrc);
      mirrorSrc = sourceAdapter->getPlayer ();

      descriptor = object->getDescriptor ();
      if (descriptor != NULL)
        {
          fRegion = descriptor->getFormatterRegion ();
          if (fRegion != NULL)
            {
              ncmRegion = fRegion->getLayoutRegion ();
              mirrorSur = Ginga_Display->createSurface (
                  ncmRegion->getWidthInPixels (),
                  ncmRegion->getHeightInPixels ());

              player->setSurface (mirrorSur);
            }
        }
    }

  clog << "AdapterMirrorPlayer::createPlayer '";
  clog << mrl << "' ALL DONE" << endl;
}

GINGA_FORMATTER_END
