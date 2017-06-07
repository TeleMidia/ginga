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

#ifndef _FORMATTERPLAYER_H_
#define _FORMATTERPLAYER_H_

#include "ncl/Content.h"
#include "ncl/ContentNode.h"
#include "ncl/NodeEntity.h"
#include "ncl/ReferenceContent.h"
#include "ncl/LambdaAnchor.h"
#include "ncl/IntervalAnchor.h"
#include "ncl/EventUtil.h"
using namespace ::ginga::ncl;

#include "NclCompositeExecutionObject.h"
#include "NclExecutionObject.h"

#include "NclAttributionEvent.h"
#include "NclPresentationEvent.h"

GINGA_FORMATTER_BEGIN

class FormatterScheduler;

class PlayerAdapter :
    public IPlayerListener,
    public INclAttributeValueMaintainer,
    public IEventListener
{
public:
  explicit PlayerAdapter (FormatterScheduler *scheduler);
  virtual ~PlayerAdapter ();

  bool prepare (NclExecutionObject *, NclPresentationEvent *);
  bool hasPrepared ();
  bool unprepare ();

  bool start ();
  bool stop ();
  bool pause ();
  bool resume ();
  bool abort ();
  void naturalEnd ();

  // From INclAttributeValueMaintainer
  string getProperty (NclAttributionEvent *event) override;
  void setProperty (const string &name, const string &value) override;

  virtual bool setProperty (NclAttributionEvent *evt, const string &value);
  virtual string getProperty (const string &name);

  // From IPlayerListener
  virtual void updateStatus (short code,
                             const string &parameter = "",
                             short type = 10,
                             const string &value = "");

  virtual void handleTickEvent (GingaTime, GingaTime, int) {}
  virtual void handleKeyEvent (SDL_EventType, SDL_Keycode);

  double getMediaTime ();
  Player *getPlayer ();
  void setOutputWindow (SDLWindow* windowId);

  bool setCurrentEvent (NclFormatterEvent *event); // app

protected:
  FormatterScheduler *_scheduler;
  NclExecutionObject *_object;
  Player *_player;

  virtual void createPlayer (const string &mrl);
  virtual void prepareScope (double offset = -1);

  double prepareProperties (NclExecutionObject *obj);
  void prepare (void);
  void updateObjectExpectedDuration ();

  bool _isAppPlayer;
  map<string, NclFormatterEvent *> _preparedEvents;  // app
  NclFormatterEvent *_currentEvent; // app
  void prepare (NclFormatterEvent *event); // app

private:
  bool checkRepeat (NclPresentationEvent *mainEvent);
  void setVisible (bool visible);

};

GINGA_FORMATTER_END

#endif //_FORMATTERPLAYER_H_
