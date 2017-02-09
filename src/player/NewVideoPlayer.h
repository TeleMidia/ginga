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

#ifndef NEW_VIDEO_PLAYER_H
#define NEW_VIDEO_PLAYER_H

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "system/Thread.h"
using namespace ::ginga::system;

#ifdef IPROVIDERLISTENER
#undef IPROVIDERLISTENER
#endif

#include "mb/IContinuousMediaProvider.h"
#include "mb/SDLWindow.h"
#include "mb/IProviderListener.h"
using namespace ::ginga::mb;

#include "Player.h"

GINGA_PLAYER_BEGIN

class NewVideoPlayer : public Thread, public Player, public IProviderListener
{
public:
  NewVideoPlayer (const string &mrl);
  virtual ~NewVideoPlayer ();
  SDLSurface* getSurface ();

  void finished ();
  double getEndTime ();

  static void initializeAudio (int numArgs, char *args[]);
  static void releaseAudio ();

  void getOriginalResolution (int *width, int *height);
  double getTotalMediaTime ();
  int64_t getVPts ();
  void timeShift (const string &direction);

  double getMediaTime ();
  void setMediaTime (double pos);
  void setStopTime (double pos);
  double getStopTime ();
  void setScope (const string &scope, short type = TYPE_PRESENTATION,
                 double begin = -1, double end = -1,
                 double outTransDur = -1);
  bool play ();
  void pause ();
  void stop ();
  void resume ();

  virtual string getPropertyValue (const string &name);
  virtual void setPropertyValue (const string &name, const string &value);

  void addListener (IPlayerListener *listener);
  void release ();
  string getMrl ();
  bool isPlaying ();
  bool isRunning ();

  void setAVPid (int aPid, int vPid);

  bool setOutWindow (SDLWindow* windowId);

 private:
  void run ();

};

GINGA_PLAYER_END

#endif /* NEW_VIDEO_PLAYER_H */
