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
#include "AssessmentStatement.h"

GINGA_NCL_BEGIN

AssessmentStatement::AssessmentStatement (short comp) : Statement ()
{
  comparator = comp;
  mainAssessment = NULL;
  otherAssessment = NULL;
  typeSet.insert ("AssessmentStatement");
}

AssessmentStatement::~AssessmentStatement ()
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

AttributeAssessment *
AssessmentStatement::getMainAssessment ()
{
  return mainAssessment;
}

void
AssessmentStatement::setMainAssessment (AttributeAssessment *assessment)
{
  this->mainAssessment = assessment;
}

Assessment *
AssessmentStatement::getOtherAssessment ()
{
  return otherAssessment;
}

void
AssessmentStatement::setOtherAssessment (Assessment *assessment)
{
  this->otherAssessment = assessment;
}

short
AssessmentStatement::getComparator ()
{
  return comparator;
}

void
AssessmentStatement::setComparator (short comp)
{
  comparator = comp;
}

vector<Role *> *
AssessmentStatement::getRoles ()
{
  vector<Role *> *roles;

  roles = new vector<Role *>;
  roles->push_back (mainAssessment);
  if (otherAssessment->instanceOf ("AttributeAssessment"))
    {
      roles->push_back ((AttributeAssessment *)otherAssessment);
    }
  return roles;
}

string
AssessmentStatement::toString ()
{
  return "Role = " + getMainAssessment ()->toString () + "\n"
         + "Comparator = " + itos (getComparator ()) + "\n" + " "
         + getOtherAssessment ()->toString ();
}

GINGA_NCL_END
