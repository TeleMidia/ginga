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

#ifndef _ATTRIBUTEEVENT_H_
#define _ATTRIBUTEEVENT_H_

#include "ncl/PropertyAnchor.h"
using namespace ::ginga::ncl;

#include "PresentationContext.h"

#include "NclFormatterEvent.h"
#include "INclAttributeValueMaintainer.h"

GINGA_FORMATTER_BEGIN

class NclAttributionEvent : public NclFormatterEvent
{
private:
  bool settingNode;

protected:
  PropertyAnchor *anchor;
  INclAttributeValueMaintainer *valueMaintainer;
  map<string, NclFormatterEvent *> assessments;
  PresentationContext *presContext;

public:
  NclAttributionEvent (string id, void *executionObject,
                       PropertyAnchor *anchor,
                       PresentationContext *presContext);

  virtual ~NclAttributionEvent ();
  PropertyAnchor *getAnchor ();
  string getCurrentValue ();
  bool setValue (string newValue);
  void setValueMaintainer (INclAttributeValueMaintainer *valueMaintainer);
  INclAttributeValueMaintainer *getValueMaintainer ();
  void setImplicitRefAssessmentEvent (string roleId,
                                      NclFormatterEvent *event);

  NclFormatterEvent *getImplicitRefAssessmentEvent (string roleId);
};

GINGA_FORMATTER_END

#endif //_ATTRIBUTEEVENT_H_
