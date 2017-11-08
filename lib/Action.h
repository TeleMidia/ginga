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

#ifndef ACTION_H
#define ACTION_H

#include "Event.h"

GINGA_NAMESPACE_BEGIN

class Action
{
public:
  Action (Event *, Event::Transition);
  ~Action ();

  Event *getEvent ();
  Event::Type getEventType ();
  Event::Transition getTransition ();

  bool getParameter (const string &, string *);
  bool setParameter (const string &, const string &);

private:
  Event *_event;
  Event::Transition _transition;
  map<string, string> _parameters;
};

GINGA_NAMESPACE_END

#endif // ACTION_H
