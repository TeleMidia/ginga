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

#ifndef PROGRAMAVPLAYERADAPTER_H_
#define PROGRAMAVPLAYERADAPTER_H_

#include "NclExecutionObject.h"

#include "NclAttributionEvent.h"
#include "NclFormatterEvent.h"
#include "NclPresentationEvent.h"
#include "NclSelectionEvent.h"

#include "ncl/Content.h"
#include "ncl/ReferenceContent.h"
using namespace ::ginga::ncl;

#include "player/IProgramAV.h"
using namespace ::ginga::player;

#include "AdapterFormatterPlayer.h"

GINGA_FORMATTER_BEGIN

class AdapterProgramAVPlayer : public AdapterFormatterPlayer
{
private:
  string lastValue;

protected:
  AdapterProgramAVPlayer ();
  virtual ~AdapterProgramAVPlayer (){};
  static AdapterProgramAVPlayer *_instance;

public:
  static AdapterProgramAVPlayer *getInstance ();

  virtual bool hasPrepared ();
  virtual bool start ();
  virtual bool stop ();
  virtual bool resume ();

protected:
  void createPlayer ();
  bool setPropertyValue (NclAttributionEvent *event, string value);

private:
  void updateAVBounds ();
};

GINGA_FORMATTER_END

#endif /*PROGRAMAVPLAYERADAPTER_H_*/
