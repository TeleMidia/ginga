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

#ifndef EXECUTION_OBJECT_SWITCH_H
#define EXECUTION_OBJECT_SWITCH_H

#include "ExecutionObjectContext.h"
#include "NclEvent.h"

GINGA_FORMATTER_BEGIN

class ExecutionObjectSwitch: public ExecutionObjectContext
{
public:
  ExecutionObjectSwitch (GingaInternal *, const string &, Node *);
  virtual ~ExecutionObjectSwitch ();

  bool exec (NclEvent *, EventState, EventState,
             EventStateTransition) override;
private:
  Switch *_switch;
  ExecutionObject *_selected;
};

GINGA_FORMATTER_END

#endif // EXECUTION_OBJECT_SWITCH_H
