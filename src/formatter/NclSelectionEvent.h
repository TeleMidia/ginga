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

#ifndef _SELECTIONEVENT_H_
#define _SELECTIONEVENT_H_

#include "ncl/ContentAnchor.h"
using namespace ::ginga::ncl;

#include "ncl/EventUtil.h"
using namespace ::ginga::ncl;

#include "NclAnchorEvent.h"

GINGA_FORMATTER_BEGIN

class NclSelectionEvent : public NclAnchorEvent
{
private:
  string selectionCode;

public:
  NclSelectionEvent (const string &id, void *executionObject,
                     ContentAnchor *anchor);
  virtual ~NclSelectionEvent ();
  bool start ();
  const string getSelectionCode ();
  void setSelectionCode (const string &codeStr);
};

GINGA_FORMATTER_END

#endif //_SELECTIONEVENT_H_
