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

#ifndef _LINKCOMPOUNDSTATEMENT_H_
#define _LINKCOMPOUNDSTATEMENT_H_

#include "NclLinkCondition.h"
#include "NclLinkAssessment.h"
#include "NclLinkAttributeAssessment.h"
#include "NclEvents.h"

#include "ncl/CompoundStatement.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class NclLinkStatement : public NclLinkCondition
{
public:
  virtual bool evaluate () = 0;
};

class NclLinkCompoundStatement : public NclLinkStatement
{
protected:
  vector<NclLinkStatement *> statements;
  bool negated;
  short op;

public:
  NclLinkCompoundStatement (short op);
  virtual ~NclLinkCompoundStatement ();
  short getOperator ();
  void addStatement (NclLinkStatement *statement);
  vector<NclLinkStatement *> *getStatements ();
  bool isNegated ();
  void setNegated (bool neg);

protected:
  bool returnEvaluationResult (bool result);

public:
  virtual vector<NclEvent *> getEvents () override;
  virtual bool evaluate () override;
};

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
  vector<NclEvent *> getEvents ();
  Comparator::Op getComparator ();
  void setComparator (Comparator::Op comp);
  NclLinkAssessment *getMainAssessment ();
  void setMainAssessment (NclLinkAssessment *assessment);
  NclLinkAssessment *getOtherAssessment ();
  void setOtherAssessment (NclLinkAssessment *assessment);
  virtual bool evaluate ();
};


GINGA_FORMATTER_END

#endif //_LINKCOMPOUNDSTATEMENT_H_
