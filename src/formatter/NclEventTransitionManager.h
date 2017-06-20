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

#include "FormatterEvents.h"
#include "NclEventTransition.h"

#include "ncl/EventUtil.h"
using namespace ::ginga::ncl;

#include "player/Player.h"
using namespace ::ginga::player;

GINGA_FORMATTER_BEGIN

class NclEventTransitionManager
{
public:
  NclEventTransitionManager ();
  virtual ~NclEventTransitionManager ();

  void addPresentationEvent (NclPresentationEvent *evt);

  void removeEventTransition (NclPresentationEvent *evt);

  void resetTimeIndex ();
  void prepare (bool isWholeContent, GingaTime startTime);
  void start (GingaTime offsetTime);
  void stop (GingaTime endTime, bool applicationType = false);
  void abort (GingaTime endTime, bool applicationType = false);

  void updateTransitionTable (GingaTime timeValue, Player *player,
                              NclFormatterEvent *mainEvent);

  set<GingaTime> getTransitionsValues ();
  NclEventTransition *getNextTransition (NclFormatterEvent *mainEvent);

private:
  size_t currentTransitionIndex;
  size_t startTransitionIndex;
  vector<NclEventTransition *> transTable;

  void addEventTransition (NclEventTransition *transition);
};

GINGA_FORMATTER_END

#endif /*EventTransitionManager_H_*/
