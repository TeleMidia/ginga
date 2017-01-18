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

#ifndef CHANNELPLAYERADAPTER_H_
#define CHANNELPLAYERADAPTER_H_

#include "gingancl/NclCompositeExecutionObject.h"
#include "gingancl/NclExecutionObject.h"
using namespace ::ginga::formatter;

#include "gingancl/NclAttributionEvent.h"
#include "gingancl/NclPresentationEvent.h"
using namespace ::ginga::formatter;

#include "ncl/Content.h"
#include "ncl/NodeEntity.h"
#include "ncl/ReferenceContent.h"
using namespace ::ginga::ncl;

#include "ncl/Transition.h"
using namespace ::ginga::ncl;

#include "AdapterFormatterPlayer.h"
using namespace ::ginga::formatter;

GINGA_FORMATTER_BEGIN

class AdapterChannelPlayer : public AdapterFormatterPlayer
{
public:
  AdapterChannelPlayer ();
  virtual ~AdapterChannelPlayer (){};

protected:
  void createPlayer ();
  bool setPropertyValue (NclAttributionEvent *event, string value);
};

GINGA_FORMATTER_END
#endif /*CHANNELPLAYERADAPTER_H_*/
