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

#ifndef EVENT_UTIL_H
#define EVENT_UTIL_H

#include "ginga.h"

GINGA_NCL_BEGIN

class EventUtil
{
public:
  // Event types.
  static const short EVT_SELECTION = 0;
  static const short EVT_PRESENTATION = 1;
  static const short EVT_ATTRIBUTION = 2;
  static const short EVT_COMPOSITION = 4;

  // Event state transitions.
  static const short TR_STARTS = 0;
  static const short TR_STOPS = 1;
  static const short TR_PAUSES = 2;
  static const short TR_RESUMES = 3;
  static const short TR_ABORTS = 4;

  // Event states.
  static const short ST_SLEEPING = 0;
  static const short ST_OCCURRING = 1;
  static const short ST_PAUSED = 2;

  // Attribute types.
  static const short ATT_OCCURRENCES = 0;
  static const short ATT_REPETITIONS = 1;
  static const short ATT_STATE = 2;
  static const short ATT_NODE_PROPERTY = 3;

  static short getTypeCode (const string &typeName);
  static string getTypeName (short type);
  static short getStateCode (const string &stateName);
  static string getStateName (short state);
  static string getTransitionName (short transition);
  static short getTransitionCode (const string &transition);
  static short getAttributeTypeCode (const string &attTypeName);
  static string getAttributeTypeName (short type);
};

GINGA_NCL_END

#endif /* EVENT_UTIL_H */
