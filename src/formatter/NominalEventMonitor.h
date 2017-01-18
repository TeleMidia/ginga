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

#ifndef NOMINALEVENTMONITOR_H_
#define NOMINALEVENTMONITOR_H_

#include "system/Thread.h"
using namespace ::ginga::system;

#include "system/ITimeBaseListener.h"
#include "system/ITimeBaseProvider.h"
using namespace ::ginga::system;

#include "player/IPlayer.h"
using namespace ::ginga::player;

#include "ncl/ContentAnchor.h"
#include "ncl/LambdaAnchor.h"
#include "ncl/IntervalAnchor.h"
using namespace ::ginga::ncl;

#include "formatter/NclPresentationEvent.h"
using namespace ::ginga::formatter;

#include "formatter/NclEventTransition.h"
using namespace ::ginga::formatter;

#include "formatter/NclExecutionObject.h"
using namespace ::ginga::formatter;

#include "IAdapterPlayer.h"

GINGA_FORMATTER_BEGIN

class NominalEventMonitor : public Thread, public ITimeBaseListener
{
private:
  double sleepTime;
  double expectedSleepTime;
  NclExecutionObject *executionObject;
  IAdapterPlayer *adapter;
  int timeBaseId;
  ITimeBaseProvider *timeBaseProvider;

  static const double DEFAULT_SLEEP_TIME;
  static const double DEFAULT_ERROR;

  bool running;
  bool paused;
  bool stopped;

public:
  NominalEventMonitor (NclExecutionObject *obj, IAdapterPlayer *player);
  virtual ~NominalEventMonitor ();

private:
  void unregisterFromTimeBase ();

public:
  void setTimeBaseProvider (ITimeBaseProvider *timeBaseProvider);

private:
  void prepareNptTransitionsEvents ();

public:
  void updateTimeBaseId (unsigned char oldContentId,
                         unsigned char newContentId);

  void valueReached (unsigned char timeBaseId, double timeValue);

  void timeBaseNaturalEnd (unsigned char contentId, double timeValue);

  void loopDetected ();

  void startMonitor ();
  void pauseMonitor ();
  void resumeMonitor ();
  void stopMonitor ();

  virtual void run ();
};

GINGA_FORMATTER_END
#endif /*NOMINALEVENTMONITOR_H_*/
