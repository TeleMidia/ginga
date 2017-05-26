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

#ifndef _ASSESSMENTSTATEMENT_H_
#define _ASSESSMENTSTATEMENT_H_

#include "Assessment.h"
#include "AttributeAssessment.h"
#include "Statement.h"

GINGA_NCL_BEGIN

class AssessmentStatement : public Statement
{
private:
  short comparator;
  AttributeAssessment *mainAssessment;
  Assessment *otherAssessment;

public:
  AssessmentStatement (short comp);
  virtual ~AssessmentStatement ();
  AttributeAssessment *getMainAssessment ();
  void setMainAssessment (AttributeAssessment *assessment);
  Assessment *getOtherAssessment ();
  void setOtherAssessment (Assessment *assessment);
  short getComparator ();
  void setComparator (short comp);
  vector<Role *> *getRoles ();
};

GINGA_NCL_END

#endif //_ASSESSMENTSTATEMENT_H_
