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

#ifndef LINK_COMPOUND_STATEMENT_H
#define LINK_COMPOUND_STATEMENT_H

#include "NclLinkCondition.h"
#include "NclLinkAssessment.h"
#include "NclEvents.h"

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class NclLinkStatement : public NclLinkCondition
{
public:
  virtual bool evaluate () = 0;
};

// class NclLinkCompoundStatement : public NclLinkStatement
// {
// public:
//   NclLinkCompoundStatement (bool, bool);
//   virtual ~NclLinkCompoundStatement ();
//   bool isConjunction ();
//   bool isNegated ();
//   void addStatement (NclLinkStatement *statement);
//   const vector<NclLinkStatement *> *getStatements ();
//   virtual vector<NclEvent *> getEvents () override;
//   virtual bool evaluate () override;
// private:
//   vector<NclLinkStatement *> _statements;
//   bool _negated;
//   bool _conjunction;
//   bool returnEvaluationResult (bool result);
// };

// class NclLinkAssessmentStatement : public NclLinkStatement
// {
// public:
//   NclLinkAssessmentStatement (const string &,
//                               NclLinkAttributeAssessment *mainAssessment,
//                               NclLinkAssessment *otherAssessment);
//   virtual ~NclLinkAssessmentStatement ();
//   vector<NclEvent *> getEvents ();
//   string getComparator ();
//   void setComparator (string comp);
//   NclLinkAssessment *getMainAssessment ();
//   void setMainAssessment (NclLinkAssessment *assessment);
//   NclLinkAssessment *getOtherAssessment ();
//   void setOtherAssessment (NclLinkAssessment *assessment);
//   virtual bool evaluate ();
// protected:
//   string _comparator;
//   NclLinkAssessment *_otherAssessment;
// private:
//   NclLinkAssessment *_mainAssessment;
// };

GINGA_FORMATTER_END

#endif // LINK_COMPOUND_STATEMENT_H
