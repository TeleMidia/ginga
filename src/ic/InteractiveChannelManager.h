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

#ifndef INTERACTIVE_CHANNEL_MANAGER_H
#define INTERACTIVE_CHANNEL_MANAGER_H

#include "ginga.h"
#include "CurlInteractiveChannel.h"

GINGA_IC_BEGIN

class InteractiveChannelManager
{
private:
  set<CurlInteractiveChannel *> *ics;
  map<string, CurlInteractiveChannel *> *urisIcs;
  static InteractiveChannelManager *_instance;
  InteractiveChannelManager ();

public:
  ~InteractiveChannelManager ();
  bool hasInteractiveChannel ();
  static InteractiveChannelManager *getInstance ();

private:
  set<CurlInteractiveChannel *> *getInteractiveChannels ();

public:
  CurlInteractiveChannel *createInteractiveChannel (const string &remoteUri);
  void releaseInteractiveChannel (CurlInteractiveChannel *ic);
  CurlInteractiveChannel *getInteractiveChannel (const string &remoteUri);
  void clearInteractiveChannelManager ();

private:
  void releaseInteractiveChannels ();

public:
  void performPendingUrls ();

private:
  static void *asyncPerform (void *thiz);
};

GINGA_IC_END

#endif /* INTERACTIVE_CHANNEL_MANAGER_H */
