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

#ifndef IFORMATTERSCHEDULERLISTENER_H_
#define IFORMATTERSCHEDULERLISTENER_H_

#include "model/FormatterEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_BEGIN

class IFormatterSchedulerListener
{
public:
  virtual ~IFormatterSchedulerListener (){};
  virtual void presentationCompleted (FormatterEvent *documentEvent) = 0;
};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_END
#endif /*IFORMATTERSCHEDULERLISTENER_H_*/
