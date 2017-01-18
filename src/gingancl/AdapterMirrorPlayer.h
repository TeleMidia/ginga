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

#ifndef MIRRORPLAYERADAPTER_H_
#define MIRRORPLAYERADAPTER_H_

#include "AdapterFormatterPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include "gingancl/model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "gingancl/model/FormatterEvent.h"
#include "gingancl/model/PresentationEvent.h"
#include "gingancl/model/SelectionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_MIRROR_BEGIN

class AdapterMirrorPlayer : public AdapterFormatterPlayer
{
private:
  string mirrorSrcId;

public:
  AdapterMirrorPlayer ();
  virtual ~AdapterMirrorPlayer ();

protected:
  void createPlayer ();
};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_MIRROR_END
#endif /*MIRRORPLAYERADAPTER_H_*/
