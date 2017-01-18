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
#include "ncl/NodeEntity.h"
#include "ncl/ContentNode.h"
using namespace ::ginga::ncl;

#include "NclAttributionEvent.h"

#include "NclExecutionObject.h"

GINGA_FORMATTER_BEGIN

NclAttributionEvent::NclAttributionEvent (string id, void *executionObject,
                                          PropertyAnchor *anchor,
                                          PresentationContext *presContext)
    : NclFormatterEvent (id, executionObject)
{

  Entity *entity;
  NodeEntity *dataObject;

  typeSet.insert ("NclAttributionEvent");

  this->anchor = anchor;
  this->valueMaintainer = NULL;
  this->settingNode = false;
  this->presContext = presContext;

  dataObject = (NodeEntity *)(((NclExecutionObject *)executionObject)
                                  ->getDataObject ());

  if (dataObject->instanceOf ("ContentNode")
      && ((ContentNode *)dataObject)->isSettingNode ())
    {

      settingNode = true;
    }

  if (dataObject->instanceOf ("ReferNode"))
    {
      if (((ReferNode *)dataObject)->getInstanceType () == "instSame")
        {
          entity = ((ReferNode *)dataObject)->getDataEntity ();
          if (entity->instanceOf ("ContentNode")
              && ((ContentNode *)entity)->isSettingNode ())
            {

              settingNode = true;
            }
        }
    }
}

NclAttributionEvent::~NclAttributionEvent ()
{
  removeInstance (this);

  assessments.clear ();
}

PropertyAnchor *
NclAttributionEvent::getAnchor ()
{
  return anchor;
}

string
NclAttributionEvent::getCurrentValue ()
{
  string propName;
  string maintainerValue = "";

  if (settingNode)
    {
      propName = anchor->getPropertyName ();
      if (propName != "")
        {
          maintainerValue = presContext->getPropertyValue (propName);
        }
    }
  else
    {
      if (valueMaintainer != NULL)
        {
          maintainerValue = valueMaintainer->getPropertyValue (this);
        }

      if (maintainerValue == "")
        {
          maintainerValue = anchor->getPropertyValue ();
        }
    }

  return maintainerValue;
}

bool
NclAttributionEvent::setValue (string newValue)
{
  /*		if ((value == "" && newValue != "") ||
                              (newValue == "" && value != "") ||
                              (newValue != "" && value != "" && (newValue !=
     value))) {

                          value = newValue;
                          return true;
                  }

                  return false;*/

  if (anchor->getPropertyValue () != newValue)
    {
      anchor->setPropertyValue (newValue);
      return true;
    }
  return false;
}

void
NclAttributionEvent::setValueMaintainer (
    INclAttributeValueMaintainer *valueMaintainer)
{

  this->valueMaintainer = valueMaintainer;
}

INclAttributeValueMaintainer *
NclAttributionEvent::getValueMaintainer ()
{
  return this->valueMaintainer;
}

void
NclAttributionEvent::setImplicitRefAssessmentEvent (
    string roleId, NclFormatterEvent *event)
{

  assessments[roleId] = event;
}

NclFormatterEvent *
NclAttributionEvent::getImplicitRefAssessmentEvent (string roleId)
{

  if (assessments.count (roleId) == 0)
    {
      return NULL;
    }

  return assessments[roleId];
}

GINGA_FORMATTER_END
