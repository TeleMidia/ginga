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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef ITUNERLISTENER_H_
#define ITUNERLISTENER_H_

#include "Channel.h"

GINGA_TUNER_BEGIN

// tuner status
static G_GNUC_UNUSED const short TS_LOOP_DETECTED = 0x01;
static G_GNUC_UNUSED const short TS_SWITCHING_CHANNEL = 0x02;
static G_GNUC_UNUSED const short TS_NEW_CHANNEL_TUNED = 0x03;
static G_GNUC_UNUSED const short TS_NEW_SERVICE_TUNED = 0x04;
static G_GNUC_UNUSED const short TS_SWITCHING_INTERFACE = 0x05;
static G_GNUC_UNUSED const short TS_NEW_INTERFACE_SELECTED = 0x06;
static G_GNUC_UNUSED const short TS_TUNER_POWEROFF = 0x07;

class ITunerListener
{
public:
  virtual ~ITunerListener (){};
  virtual void receiveData (char *buff, unsigned int size) = 0;
  virtual void updateChannelStatus (short newStatus, Channel *channel) = 0;
  virtual bool isReady () = 0;
};

GINGA_TUNER_END

#endif /*ITUNERLISTENER_H_*/
