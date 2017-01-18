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

#include "config.h"
#include "AnchorEvent.h"

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_EVENT_BEGIN

AnchorEvent::AnchorEvent (string id, void *executionObject,
                          ContentAnchor *anchor)
    : FormatterEvent (id, executionObject)
{

  this->anchor = anchor;
  typeSet.insert ("AnchorEvent");
}

AnchorEvent::~AnchorEvent () { removeInstance (this); }

ContentAnchor *
AnchorEvent::getAnchor ()
{
  return anchor;
}

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_EVENT_END
