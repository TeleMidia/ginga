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

#ifndef ENDEVENTTRANSITION_H_
#define ENDEVENTTRANSITION_H_

#include "NclEventTransition.h"

GINGA_FORMATTER_BEGIN

class NclBeginEventTransition;

class NclEndEventTransition : public NclEventTransition
{
private:
  NclBeginEventTransition *beginTransition;

public:
  NclEndEventTransition (GingaTime time, NclPresentationEvent *event,
                         NclBeginEventTransition *transition);

  virtual ~NclEndEventTransition ();

  NclBeginEventTransition *getBeginTransition ();
};

GINGA_FORMATTER_END

#endif /*ENDEVENTTRANSITION_H_*/
