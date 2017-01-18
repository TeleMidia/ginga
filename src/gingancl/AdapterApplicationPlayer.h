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

#include "gingancl/NclApplicationExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "AdapterFormatterPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include "player/IPlayerListener.h"
using namespace ::ginga::player;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_BEGIN

typedef struct
{
  short code;
  string param;
  short type;
  string value;
} ApplicationStatus;

class AdapterApplicationPlayer : public AdapterFormatterPlayer,
                                 public Thread
{

private:
  IPlayerListener *editingCommandListener;
  pthread_mutex_t eventMutex;
  pthread_mutex_t eventsMutex;
  bool running;

protected:
  vector<ApplicationStatus *> notes;
  map<string, NclFormatterEvent *> preparedEvents;
  NclFormatterEvent *currentEvent;

public:
  AdapterApplicationPlayer ();
  virtual ~AdapterApplicationPlayer ();

  void setNclEditListener (IPlayerListener *listener);

protected:
  void checkPlayerSurface (NclExecutionObject *obj);
  virtual void createPlayer ();

  virtual bool hasPrepared ();
  virtual bool prepare (NclExecutionObject *object, NclFormatterEvent *mainEvent);

private:
  void prepare (NclFormatterEvent *event);

public:
  virtual bool start ();
  virtual bool stop ();
  virtual bool pause ();
  virtual bool resume ();
  virtual bool abort ();

private:
  virtual bool unprepare ();

public:
  virtual void naturalEnd ();
  virtual void updateStatus (short code, string parameter = "",
                             short type = 10, string value = "");

private:
  void notificationHandler (short code, string parameter, short type,
                            string value);

  void run ();

public:
  virtual bool setAndLockCurrentEvent (NclFormatterEvent *event) = 0;
  virtual void unlockCurrentEvent (NclFormatterEvent *event) = 0;

private:
  bool checkEvent (NclFormatterEvent *event, short type);

protected:
  bool startEvent (string anchorId, short type, string value);
  bool stopEvent (string anchorId, short type, string value);
  bool abortEvent (string anchorId, short type);
  bool pauseEvent (string anchorId, short type);
  bool resumeEvent (string anchorId, short type);

  void lockEvent ();
  void unlockEvent ();

  void lockPreparedEvents ();
  void unlockPreparedEvents ();

public:
  virtual void flip (){};
};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_END
#endif //_AdapterApplicationPlayer_H_
