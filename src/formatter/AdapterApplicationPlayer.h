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

#ifndef _APPLICATIONPLAYERADAPTER_H_
#define _APPLICATIONPLAYERADAPTER_H_

#include "NclApplicationExecutionObject.h"

#include "AdapterFormatterPlayer.h"

#include "player/IPlayerListener.h"
using namespace ::ginga::player;

GINGA_FORMATTER_BEGIN

class AdapterApplicationPlayer :
    public AdapterFormatterPlayer
{
public:
  explicit AdapterApplicationPlayer (AdapterPlayerManager *manager);
  virtual ~AdapterApplicationPlayer ();

  virtual bool prepare (NclExecutionObject *_object,
                        NclPresentationEvent *mainEvent) override;
  virtual bool hasPrepared () override;
  virtual bool unprepare () override;

  virtual bool stop () override;
  virtual bool abort () override;
  virtual void naturalEnd() override;

  bool setAndLockCurrentEvent (NclFormatterEvent *event);
  void unlockCurrentEvent (NclFormatterEvent *event);

protected:
  map<string, NclFormatterEvent *> _preparedEvents;
  NclFormatterEvent *_currentEvent;

  void lockEvent ();
  void unlockEvent ();

  void lockPreparedEvents ();
  void unlockPreparedEvents ();

private:
  pthread_mutex_t _eventMutex;
  pthread_mutex_t _eventsMutex;
  bool _running;

  void prepare (NclFormatterEvent *event);
};

GINGA_FORMATTER_END

#endif //_AdapterApplicationPlayer_H_
