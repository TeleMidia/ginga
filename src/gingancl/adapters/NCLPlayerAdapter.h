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

#ifndef NCLPLAYERADAPTER_H_
#define NCLPLAYERADAPTER_H_

#include "ApplicationPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_NCL_BEGIN

class NCLPlayerAdapter : public ApplicationPlayerAdapter
{
public:
  NCLPlayerAdapter ();
  virtual ~NCLPlayerAdapter (){};

protected:
  void createPlayer ();

public:
  bool setAndLockCurrentEvent (FormatterEvent *event);
  void unlockCurrentEvent (FormatterEvent *event);
  void flip ();
};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_NCL_END
#endif /* NCLPLAYERADAPTER_H_ */
