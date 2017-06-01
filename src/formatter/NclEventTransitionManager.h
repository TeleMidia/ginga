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

#ifndef EventTransitionManager_H_
#define EventTransitionManager_H_

#include "NclPresentationEvent.h"
#include "NclBeginEventTransition.h"
#include "NclEndEventTransition.h"

#include "ncl/RelativeTimeIntervalAnchor.h"
using namespace ::ginga::ncl;

#include "ncl/EventUtil.h"
using namespace ::ginga::ncl;

#include "player/Player.h"
using namespace ::ginga::player;

GINGA_FORMATTER_BEGIN

class NclEventTransitionManager
{
private:
  map<short int, int> currentTransitionIndex;
  map<short int, int> startTransitionIndex;
  map<short int, vector<NclEventTransition *> *> transTable;

public:
  NclEventTransitionManager ();
  virtual ~NclEventTransitionManager ();

private:
  short int getType (NclPresentationEvent *event);
  vector<NclEventTransition *> *getTransitionEvents (short int type);

public:
  void addPresentationEvent (NclPresentationEvent *event);

private:
  void addEventTransition (NclEventTransition *transition, short int type);

public:
  void removeEventTransition (NclPresentationEvent *event);

  void resetTimeIndex ();
  void resetTimeIndexByType (short int type);
  void prepare (bool wholeContent, double startTime, short int type);
  void start (double offsetTime);
  void stop (double endTime, bool applicationType = false);
  void abort (double endTime, bool applicationType = false);

  void timeBaseNaturalEnd (int64_t timeValue, NclFormatterEvent *mainEvent,
                           short int transType);

  void updateTransitionTable (double timeValue, Player *player,
                              NclFormatterEvent *mainEvent,
                              short int transType);

  set<double> *getTransitionsValues (short int transType);
  NclEventTransition *getNextTransition (NclFormatterEvent *mainEvent);
};

GINGA_FORMATTER_END

#endif /*EventTransitionManager_H_*/
