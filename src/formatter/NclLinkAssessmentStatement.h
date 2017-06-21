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

#ifndef _LINKASSESSMENTSTATEMENT_H_
#define _LINKASSESSMENTSTATEMENT_H_

#include "NclEvents.h"

#include "ncl/Comparator.h"
using namespace ::ginga::ncl;

#include "NclLinkStatement.h"
#include "NclLinkAssessment.h"
#include "NclLinkAttributeAssessment.h"

GINGA_FORMATTER_BEGIN

class NclLinkAssessmentStatement : public NclLinkStatement
{
protected:
  Comparator::Op comparator;
  NclLinkAssessment *otherAssessment;

private:
  NclLinkAssessment *mainAssessment;

public:
  NclLinkAssessmentStatement (Comparator::Op comparator,
                              NclLinkAttributeAssessment *mainAssessment,
                              NclLinkAssessment *otherAssessment);

  virtual ~NclLinkAssessmentStatement ();
  vector<NclEvent *> *getEvents ();
  Comparator::Op getComparator ();
  void setComparator (Comparator::Op comp);
  NclLinkAssessment *getMainAssessment ();
  void setMainAssessment (NclLinkAssessment *assessment);
  NclLinkAssessment *getOtherAssessment ();
  void setOtherAssessment (NclLinkAssessment *assessment);
  virtual bool evaluate ();
};

GINGA_FORMATTER_END

#endif //_LINKASSESSMENTSTATEMENT_H_
