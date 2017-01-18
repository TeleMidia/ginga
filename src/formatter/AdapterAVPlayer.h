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

#ifndef AVPLAYERADAPTER_H_
#define AVPLAYERADAPTER_H_

#include "formatter/NclExecutionObject.h"
using namespace ::ginga::formatter;

#include "formatter/NclAttributionEvent.h"
#include "formatter/NclFormatterEvent.h"
#include "formatter/NclPresentationEvent.h"
#include "formatter/NclSelectionEvent.h"
using namespace ::ginga::formatter;

#include "ncl/NodeEntity.h"
#include "ncl/ReferenceContent.h"
using namespace ::ginga::ncl;

#include "AdapterFormatterPlayer.h"
using namespace ::ginga::formatter;

GINGA_FORMATTER_BEGIN

class AdapterAVPlayer : public AdapterFormatterPlayer
{
public:
  AdapterAVPlayer ();

  virtual ~AdapterAVPlayer (){};

protected:
  void createPlayer ();
  bool setPropertyValue (NclAttributionEvent *event, string value);
};

GINGA_FORMATTER_END
#endif /*AVPLAYERADAPTER_H_*/
