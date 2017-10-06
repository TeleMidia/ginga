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

#include "aux-ginga.h"
#include "NclLinkAssessment.h"

GINGA_FORMATTER_BEGIN

NclLinkAttributeAssessment::NclLinkAttributeAssessment (
    NclEvent *ev, AttributeType attrType)
    : NclLinkAssessment ()
{
  _event = ev;
  _attrType = attrType;
  _offset = "";
}

NclEvent *
NclLinkAttributeAssessment::getEvent ()
{
  return _event;
}

void
NclLinkAttributeAssessment::setOffset (const string &offset)
{
  this->_offset = offset;
}

string
NclLinkAttributeAssessment::getOffset ()
{
  return _offset;
}

void
NclLinkAttributeAssessment::setEvent (NclEvent *ev)
{
  _event = ev;
}

AttributeType
NclLinkAttributeAssessment::getAttributeType ()
{
  return _attrType;
}

void
NclLinkAttributeAssessment::setAttributeType (AttributeType attrType)
{
  _attrType = attrType;
}

string
NclLinkAttributeAssessment::getValue ()
{
  string value = "";

  switch (_attrType)
    {
    case AttributeType::NODE_PROPERTY:
      if (instanceof (AttributionEvent *, _event))
        {
          value = getAssessmentWithOffset (
              ((AttributionEvent *)_event)->getCurrentValue ());
        }
      break;

    case AttributeType::STATE:
      switch (_event->getCurrentState ())
        {
        case EventState::OCCURRING:
          value = "occurring";
          break;
        case EventState::PAUSED:
          value = "paused";
          break;
        case EventState::SLEEPING:
          value = "sleeping";
          break;
        default:
          value = "unknown";
        }
      break;

    case AttributeType::OCCURRENCES:
      {
        value = getAssessmentWithOffset (
            xstrbuild ("%d", (int)_event->getOccurrences ()));
        break;
      }

    case AttributeType::REPETITIONS:
      if (instanceof (PresentationEvent *, _event))
        {
          xstrassign (
              value, "%d",
              (int)((PresentationEvent *)_event)->getRepetitions ());
        }
      break;

    default:
      g_assert_not_reached ();
    }

  return value;
}

string
NclLinkAttributeAssessment::getAssessmentWithOffset (
    const string &assessmentValue)
{
  double off, val;
  if (!_xstrtod (_offset, &off) || !_xstrtod (assessmentValue, &val))
    return assessmentValue;
  return xstrbuild ("%f", val + off);
}

NclLinkValueAssessment::NclLinkValueAssessment (const string &value)
    : NclLinkAssessment ()
{
  this->_value = value;
}

NclLinkValueAssessment::~NclLinkValueAssessment () {}

string
NclLinkValueAssessment::getComparableValue ()
{
  return _value;
}

void
NclLinkValueAssessment::setComparableValue (const string &value)
{
  this->_value = value;
}

string
NclLinkValueAssessment::getValue ()
{
  return _value;
}

GINGA_FORMATTER_END
