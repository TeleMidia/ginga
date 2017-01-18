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
#include "player/ImagePlayer.h"

#include "ic/InteractiveChannelManager.h"
using namespace ::ginga::ic;

GINGA_PLAYER_BEGIN

ImagePlayer::ImagePlayer (GingaScreenID screenId, string mrl)
    : Player (screenId, mrl)
{

  GingaLocatorFactory *glf = NULL;
  string path, name, clientPath, newMrl;
  bool resolved = false;

  if (mrl.substr (0, 7) == "http://" || mrl.substr (0, 8) == "https://")
    {

      InteractiveChannelManager *icm;

      icm = InteractiveChannelManager::getInstance ();
      CurlInteractiveChannel *ic = icm->createInteractiveChannel (mrl);
      newMrl = itos ((long int)this);
      ic->setSourceTarget (newMrl);
      ic->reserveUrl (mrl, NULL, "GingaNCL/0.13.6");
      ic->performUrl ();

      icm->releaseInteractiveChannel (ic);

      mrl = newMrl;
    }

  if (fileExists (mrl))
    {
      provider = dm->createImageProvider (myScreen, mrl.c_str ());
    }
  else
    {
      if (!SystemCompat::isAbsolutePath (mrl))
        {
          newMrl = SystemCompat::getUserCurrentPath () + mrl;
          if (fileExists (newMrl))
            {
              resolved = true;
              mrl = newMrl;
              newMrl = "";
            }
        }

      if (!resolved)
        {
          if (mrl.find (SystemCompat::getIUriD ()) != std::string::npos)
            {
              path = mrl.substr (
                  0, mrl.find_last_of (SystemCompat::getIUriD ()));
              name = mrl.substr (
                  mrl.find_last_of (SystemCompat::getIUriD ()) + 1,
                  mrl.length ()
                      - mrl.find_last_of (SystemCompat::getIUriD ()));
            }
          else if (mrl.find (SystemCompat::getFUriD ())
                   != std::string::npos)
            {

              path = mrl.substr (
                  0, mrl.find_last_of (SystemCompat::getFUriD ()));

              name = mrl.substr (
                  mrl.find_last_of (SystemCompat::getFUriD ()) + 1,
                  mrl.length ()
                      - mrl.find_last_of (SystemCompat::getFUriD ()));
            }

          glf = GingaLocatorFactory::getInstance ();

          if (glf != NULL)
            {
              clog << "ImagePlayer trying to find '" << mrl << "'";
              clog << endl;
              clientPath = glf->getLocation (path);
              newMrl = clientPath + name;
              clog << "ImagePlayer found newMrl = '" << newMrl;
              clog << "'" << endl;

              if (fileExists (newMrl))
                {
                  provider
                      = dm->createImageProvider (myScreen, newMrl.c_str ());
                }
              else
                {
                  provider = 0;
                  clog << "ImagePlayer::ImagePlayer Warning! File ";
                  clog << " Not Found: '" << newMrl.c_str ();
                  clog << "'" << endl;
                }
            }
          else
            {
              provider = 0;
              clog << "ImagePlayer::ImagePlayer Warning! ";
              clog << "GLF Component NOT";
              clog << " Found!" << endl;
            }
        }
    }

  if (provider != 0)
    {
      surface = prepareSurface (provider, mrl);
    }
}

ImagePlayer::~ImagePlayer ()
{
  if (provider != 0)
    {
      dm->releaseImageProvider (myScreen, provider);
    }
}

bool
ImagePlayer::play ()
{
  if (provider == 0)
    {

      return false;
    }

  dm->playProviderOver (provider, surface);
  return Player::play ();
}

void
ImagePlayer::stop ()
{
  Player::stop ();
}

void
ImagePlayer::resume ()
{
  ImagePlayer::play ();
}

void
ImagePlayer::setPropertyValue (string name, string value)
{
  // TODO: set brightness, rotate...
  // refresh changes
  GingaWindowID win;

  if (surface != 0)
    {
      win = dm->getSurfaceParentWindow (surface);
      if (win != 0)
        {
          dm->renderWindowFrom (myScreen, win, surface);
        }
    }

  Player::setPropertyValue (name, value);
}

GingaSurfaceID
ImagePlayer::prepareSurface (GingaProviderID provider, string mrl)
{

  GingaSurfaceID renderedSurface = 0;

  renderedSurface = dm->createSurfaceFrom (myScreen, 0);
  dm->playProviderOver (provider, renderedSurface);

  return renderedSurface;
}

GINGA_PLAYER_END
