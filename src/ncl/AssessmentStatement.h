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

#ifndef ASSESSMENT_STATEMENT_H
#define ASSESSMENT_STATEMENT_H

#include "Assessment.h"
#include "AttributeAssessment.h"
#include "Statement.h"

GINGA_NCL_BEGIN

class AssessmentStatement : public Statement
{
public:
  AssessmentStatement (const string &);
  virtual ~AssessmentStatement ();
  AttributeAssessment *getMainAssessment ();
  void setMainAssessment (AttributeAssessment *);
  Assessment *getOtherAssessment ();
  void setOtherAssessment (Assessment *);
  string getComparator ();

private:
  string _comparator;
  AttributeAssessment *_mainAssessment;
  Assessment *_otherAssessment;
};

GINGA_NCL_END

#endif // ASSESSMENT_STATEMENT_H
