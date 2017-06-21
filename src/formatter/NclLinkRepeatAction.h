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

#ifndef _LINKREPEATACTION_H_
#define _LINKREPEATACTION_H_

#include "NclLinkSimpleAction.h"

#include "NclEvents.h"

GINGA_FORMATTER_BEGIN

class NclLinkRepeatAction : public NclLinkSimpleAction
{
public:
  NclLinkRepeatAction (NclEvent *, SimpleActionType);
  virtual ~NclLinkRepeatAction ();

  virtual void run () override;

  int getRepetitions ();
  GingaTime getRepetitionInterval ();
  void setRepetitions (int repetitions);
  void setRepetitionInterval (GingaTime delay);

private:
  int repetitions;
  GingaTime repetitionInterval;
};

GINGA_FORMATTER_END

#endif
