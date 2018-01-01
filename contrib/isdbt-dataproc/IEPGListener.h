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

#ifndef IEPGLISTENER_H_
#define IEPGLISTENER_H_

#include "isdbt-tsparser/IEventInfo.h"
using namespace ::ginga::tsparser;

/*
 * A Lua Node can register himself to handle EPG events (class='si',
 * type='epg').
 * Doing this, his associated Lua Player becames an IEPGListener. The
 * standard
 * defines 3 main types that a Lua Node can request EPG events.
 *
 * 1) stage='current', fields={field_1, field_2,...field_j}
 * 2) stage='next', eventId=<number>, fields={field_1, field_2,...field_j}
 * 3) stage='schedule', startTime=<date>, endTime=<date>, fields={field_1,
 *    field_2,...field_j}
 *
 * The struct Request was create to model a lua node request for EPG events.
 * That request has to be stored associated with her N IEPGListeners.
 */

struct Request
{
  string stage;
  /* 3 possible values: current, next or schedule*/

  unsigned short eventId;
  /* only if stage==next, requesting the next event of the event with this
   *  eventId. If is not specified, the request is for the next event of the
   *  current event*/

  string startTime;
  string endTime;
  /* only if stage==schedule, requesting events with startTime and endTime
  * in
  * the range specified by this startTime and this endTime*/

  vector<string> fields;
  /*requesting specified metadata fields for each event. If is not specified
   * the request is for all possible metadada fields*/
};

struct SIField
{
  string str;
  map<string, struct SIField> table;
};

GINGA_DATAPROC_BEGIN

class IEPGListener
{
public:
  static const unsigned char SI_LISTENER = 1;
  static const unsigned char EPG_LISTENER = 2;
  static const unsigned char MOSAIC_LISTENER = 3;
  static const unsigned char TIME_LISTENER = 4;

public:
  virtual ~IEPGListener (){};
  virtual void pushSIEvent (map<string, struct SIField> event,
                            unsigned char type)
      = 0;

  virtual void addAsSIListener (unsigned char type) = 0;
};

GINGA_DATAPROC_END

#endif /*IEPGLISTENER_H_*/
