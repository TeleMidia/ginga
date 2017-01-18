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

#ifndef _EXECUTIONOBJECTSWITCH_H_
#define _EXECUTIONOBJECTSWITCH_H_

#include "ncl/Node.h"
using namespace ::ginga::ncl;

#include "ncl/LambdaAnchor.h"
using namespace ::ginga::ncl;

#include "NclCompositeExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "NclSwitchEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::switches;

#include "NclFormatterEvent.h"
#include "NclPresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_SWITCHES_BEGIN

class NclExecutionObjectSwitch : public NclCompositeExecutionObject
{
protected:
  NclExecutionObject *selectedObject;

public:
  NclExecutionObjectSwitch (string id, Node *switchNode, bool handling,
                         INclLinkActionListener *seListener);

  virtual ~NclExecutionObjectSwitch (){};
  NclExecutionObject *getSelectedObject ();
  void select (NclExecutionObject *executionObject);
  bool addEvent (NclFormatterEvent *event);
};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_SWITCHES_END
#endif //_EXECUTIONOBJECTSWITCH_H_
