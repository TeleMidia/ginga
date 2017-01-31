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

#include "ginga.h"
#include "NclLinkAttributeAssessment.h"

GINGA_FORMATTER_BEGIN

NclLinkAttributeAssessment::NclLinkAttributeAssessment (
    NclFormatterEvent *ev, short attrType)
    : NclLinkAssessment ()
{
  event = ev;
  attributeType = attrType;
  offset = "";
  typeSet.insert ("NclLinkAttributeAssessment");
}

NclFormatterEvent *
NclLinkAttributeAssessment::getEvent ()
{
  return event;
}

void
NclLinkAttributeAssessment::setOffset (const string &offset)
{
  this->offset = offset;
}

string
NclLinkAttributeAssessment::getOffset ()
{
  return offset;
}

void
NclLinkAttributeAssessment::setEvent (NclFormatterEvent *ev)
{
  event = ev;
}

short
NclLinkAttributeAssessment::getAttributeType ()
{
  return attributeType;
}

void
NclLinkAttributeAssessment::setAttributeType (short attrType)
{
  attributeType = attrType;
}

string
NclLinkAttributeAssessment::getValue ()
{
  string value = "";

  switch (attributeType)
    {
    case EventUtil::ATT_NODE_PROPERTY:
      if (event->instanceOf ("NclAttributionEvent"))
        {
          value = getAssessmentWithOffset (
              ((NclAttributionEvent *)event)->getCurrentValue ());
        }
      break;

    case EventUtil::ATT_STATE:
      value = NclFormatterEvent::getStateName (event->getCurrentState ());
      break;

    case EventUtil::ATT_OCCURRENCES:
      {
        value = getAssessmentWithOffset (xstrbuild ("%d", (int) event->getOccurrences ()));
        break;
      }

    case EventUtil::ATT_REPETITIONS:
      if (event->instanceOf ("NclPresentationEvent"))
        {
          xstrassign (value, "%d", (int) ((NclPresentationEvent *)event)->getRepetitions ());
        }
      break;

      default:
        g_assert_not_reached ();
    }

  return value;
}

string
NclLinkAttributeAssessment::getAssessmentWithOffset (const string &assessmentValue)
{
  double off, val;
  if (!_xstrtod (offset, &off) || !_xstrtod (assessmentValue, &val))
    return assessmentValue;
  return xstrbuild ("%f", val + off);
}

GINGA_FORMATTER_END
