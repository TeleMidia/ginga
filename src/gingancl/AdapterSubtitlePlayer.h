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

#ifndef SUBTITLEPLAYERADAPTER_H_
#define SUBTITLEPLAYERADAPTER_H_

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "gingancl/model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "gingancl/model/AttributionEvent.h"
#include "gingancl/model/FormatterEvent.h"
#include "gingancl/model/PresentationEvent.h"
#include "gingancl/model/SelectionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "ncl/NodeEntity.h"
#include "ncl/Content.h"
#include "ncl/ReferenceContent.h"
using namespace ::ginga::ncl;

#include "AdapterFormatterPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_TEXT_BEGIN

class AdapterSubtitlePlayer : public AdapterFormatterPlayer
{
public:
  AdapterSubtitlePlayer ();

  virtual ~AdapterSubtitlePlayer (){};

protected:
  void createPlayer ();
};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_TEXT_END
#endif /*SUBTITLEPLAYERADAPTER_H_*/
