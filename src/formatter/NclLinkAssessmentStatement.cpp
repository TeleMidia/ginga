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
#include "NclLinkAssessmentStatement.h"

GINGA_FORMATTER_BEGIN

NclLinkAssessmentStatement::NclLinkAssessmentStatement (
    Comparator::Op comparator, NclLinkAttributeAssessment *mainAssessment,
    NclLinkAssessment *otherAssessment)
    : NclLinkStatement ()
{
  this->comparator = comparator;
  this->mainAssessment = mainAssessment;
  this->otherAssessment = otherAssessment;
  typeSet.insert ("NclLinkAssessmentStatement");
}

NclLinkAssessmentStatement::~NclLinkAssessmentStatement ()
{
  if (mainAssessment != NULL)
    {
      delete mainAssessment;
      mainAssessment = NULL;
    }

  if (otherAssessment != NULL)
    {
      delete otherAssessment;
      otherAssessment = NULL;
    }
}

vector<FormatterEvent *> *
NclLinkAssessmentStatement::getEvents ()
{
  vector<FormatterEvent *> *events;

  events = new vector<FormatterEvent *>;
  events->push_back (
      ((NclLinkAttributeAssessment *)mainAssessment)->getEvent ());

  if (otherAssessment->instanceOf ("NclLinkAttributeAssessment"))
    {
      events->push_back (
          ((NclLinkAttributeAssessment *)otherAssessment)->getEvent ());
    }
  return events;
}

Comparator::Op
NclLinkAssessmentStatement::getComparator ()
{
  return comparator;
}

void
NclLinkAssessmentStatement::setComparator (Comparator::Op comp)
{
  comparator = comp;
}

NclLinkAssessment *
NclLinkAssessmentStatement::getMainAssessment ()
{
  return mainAssessment;
}

void
NclLinkAssessmentStatement::setMainAssessment (
    NclLinkAssessment *assessment)
{
  mainAssessment = assessment;
}

NclLinkAssessment *
NclLinkAssessmentStatement::getOtherAssessment ()
{
  return otherAssessment;
}

void
NclLinkAssessmentStatement::setOtherAssessment (
    NclLinkAssessment *assessment)
{
  otherAssessment = assessment;
}

bool
NclLinkAssessmentStatement::evaluate ()
{
  if (mainAssessment == NULL || otherAssessment == NULL)
    return false;

  return Comparator::evaluate (mainAssessment->getValue (),
                               otherAssessment->getValue (), comparator);
}

GINGA_FORMATTER_END
