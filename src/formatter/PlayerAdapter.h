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

#ifndef PLAYER_ADAPTER_H
#define PLAYER_ADAPTER_H

#include "FormatterEvents.h"
#include "ExecutionObject.h"
#include "mb/IEventListener.h"

GINGA_FORMATTER_BEGIN

class FormatterScheduler;

class PlayerAdapter :
    public IPlayerListener,
    public INclAttributeValueMaintainer,
    public IEventListener
{
public:
  explicit PlayerAdapter (FormatterScheduler *);
  virtual ~PlayerAdapter ();

  bool setCurrentEvent (NclFormatterEvent *); // app
  Player *getPlayer ();
  void setOutputWindow (SDLWindow *);

  virtual bool setProperty (NclAttributionEvent *, const string &);

  bool prepare (ExecutionObject *, NclPresentationEvent *);
  bool hasPrepared ();
  bool unprepare ();

  bool start ();
  bool stop ();
  bool pause ();
  bool resume ();
  bool abort ();
  void naturalEnd ();

  // From INclAttributeValueMaintainer.
  string getProperty (NclAttributionEvent *event) override;
  void setProperty (const string &name, const string &value) override;

  // From IPlayerListener.
  virtual void updateStatus (short code,
                             const string &parameter = "",
                             short type = 10,
                             const string &value = "") override;

  // From IEventListener.
  virtual void handleKeyEvent (SDL_EventType, SDL_Keycode) override;
  virtual void handleTickEvent (GingaTime, GingaTime, int) override;

private:
  FormatterScheduler *_scheduler;
  ExecutionObject *_object;
  Player *_player;

  void createPlayer (const string &);
  void prepareScope (GingaTime offset = 0);

  GingaTime prepareProperties (ExecutionObject *obj);

  bool checkRepeat (NclPresentationEvent *);
  void setVisible (bool);

  // Application player only.
  bool _isAppPlayer;
  map<string, NclFormatterEvent *> _preparedEvents;
  NclFormatterEvent *_currentEvent;
  void prepare (NclFormatterEvent *event);
};

GINGA_FORMATTER_END

#endif // PLAYER_ADAPTER_H
