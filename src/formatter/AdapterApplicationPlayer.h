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

typedef struct
{
  short code;
  string param;
  short type;
  string value;
} ApplicationStatus;

class AdapterApplicationPlayer :
    public AdapterFormatterPlayer,
    public Thread
{
public:
  explicit AdapterApplicationPlayer (AdapterPlayerManager *manager);
  virtual ~AdapterApplicationPlayer ();

//  virtual bool start ();
  virtual bool stop ();
//  virtual bool pause ();
//  virtual bool resume ();
  virtual bool abort ();

  virtual void naturalEnd ();
  virtual void updateStatus (short code,
                             const string &parameter = "",
                             short type = 10,
                             const string &value = "");

  void notificationHandler (short code, const string &parameter, short type,
                            const string &value);

  void run ();

  virtual bool setAndLockCurrentEvent (NclFormatterEvent *event) = 0;
  virtual void unlockCurrentEvent (NclFormatterEvent *event);

protected:
  vector<ApplicationStatus *> _notes;
  map<string, NclFormatterEvent *> _preparedEvents;
  NclFormatterEvent *_currentEvent;

  virtual bool hasPrepared ();
  virtual bool prepare (NclExecutionObject *_object,
                        NclFormatterEvent *mainEvent);

  bool startEvent (const string &anchorId, short type, const string &value);
  bool stopEvent (const string &anchorId, short type, const string &value);
  bool abortEvent (const string &anchorId, short type);
  bool pauseEvent (const string &anchorId, short type);
  bool resumeEvent (const string &anchorId, short type);

  void lockEvent ();
  void unlockEvent ();

  void lockPreparedEvents ();
  void unlockPreparedEvents ();

private:
  pthread_mutex_t _eventMutex;
  pthread_mutex_t _eventsMutex;
  bool _running;

  void prepare (NclFormatterEvent *event);
  virtual bool unprepare ();
  bool checkEvent (NclFormatterEvent *event, short type);

};

GINGA_FORMATTER_END

#endif //_AdapterApplicationPlayer_H_
