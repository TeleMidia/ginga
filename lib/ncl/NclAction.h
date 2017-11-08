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

#ifndef NCL_ACTION_H
#define NCL_ACTION_H

#include "NclAction.h"
#include "NclRole.h"

GINGA_NAMESPACE_BEGIN

class NclAction: public NclRole
{
public:
  NclAction (FormatterEvent::Type, FormatterEvent::Transition, const string &,
             const string &, const string &, const string &);
  ~NclAction ();

  FormatterEvent::Transition getTransition ();
  string getDelay ();
  string getValue ();
  string getDuration ();

  static bool isReserved (const string &, FormatterEvent::Type *,
                          FormatterEvent::Transition *);
private:
  FormatterEvent::Transition _transition;
  string _delay;
  string _value;
  string _duration;
};

GINGA_NAMESPACE_END

#endif // NCL_ACTION_H
