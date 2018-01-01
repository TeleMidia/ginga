/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef __TUNER_H__
#define __TUNER_H__

#include "system/Thread.h"
using namespace ::ginga::system;

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "Tuner.h"

#ifndef BUFFSIZE
#define BUFFSIZE 9588
#endif

#include "ITunerListener.h"
#include "NetworkInterface.h"

#include "mb/IKeyInputEventListener.h"

typedef struct
{
  char *buff;
  unsigned int size;
} Buffer;

#include "Channel.h"
#include "NetworkInterface.h"
#include "ITunerListener.h"
#include "IProviderListener.h"

GINGA_TUNER_BEGIN

class Tuner : public ITProviderListener,
              public Thread,
              IKeyInputEventListener
{
private:
  bool receiving;
  ITunerListener *listener;
  ITunerListener *loopListener;
  map<int, NetworkInterface *> interfaces;
  int currentInterface;
  bool firstTune;
  string currentSpec;

public:
  Tuner (const string &network = "", const string &protocol = "", const string &address = "");
  virtual ~Tuner ();
  void setLoopListener (ITunerListener *loopListener);
  // Input event callback.
  void keyInputCallback (SDL_EventType evtType, SDL_Keycode key);

private:
  void clearInterfaces ();
  void receiveSignal (short signalCode);
  void initializeInterface (const string &niSpec);
  void initializeInterfaces ();
  void createInterface (const string &network, const string &protocol, const string &address);

  bool listenInterface (NetworkInterface *nInterface);
  void receiveInterface (NetworkInterface *nInterface);

public:
  void setSpec (const string &ni, const string &ch);
  void tune ();
  NetworkInterface *getCurrentInterface ();
  void channelUp ();
  void channelDown ();
  void changeChannel (int factor);
  bool hasSignal ();

public:
  void setTunerListener (ITunerListener *listener);

private:
  void notifyData (char *buff, unsigned int val);
  void notifyStatus (short newStatus, Channel *channel);
  void waitForListeners ();
  virtual void run ();
};

GINGA_TUNER_END

#endif //__TUNER_H__
