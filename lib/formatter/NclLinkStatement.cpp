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
#include "NclLinkStatement.h"

GINGA_FORMATTER_BEGIN

// NclLinkCompoundStatement::NclLinkCompoundStatement (bool conj, bool neg)
//     : NclLinkStatement ()
// {
//   _conjunction = conj;
//   _negated = neg;
// }

// NclLinkCompoundStatement::~NclLinkCompoundStatement ()
// {
//   _statements.clear ();
// }

// bool
// NclLinkCompoundStatement::isConjunction ()
// {
//   return _conjunction;
// }

// bool
// NclLinkCompoundStatement::isNegated ()
// {
//   return _negated;
// }

// void
// NclLinkCompoundStatement::addStatement (NclLinkStatement *statement)
// {
//   g_assert_nonnull (statement);
//   _statements.push_back (statement);
// }

// const vector<NclLinkStatement *> *
// NclLinkCompoundStatement::getStatements ()
// {
//   return &_statements;
// }

// bool
// NclLinkCompoundStatement::returnEvaluationResult (bool result)
// {
//   return (_negated ^ result);
// }

// vector<NclEvent *>
// NclLinkCompoundStatement::getEvents ()
// {
//   vector<NclEvent *> events;

//   for (NclLinkStatement *statement : _statements)
//     {
//       for (NclEvent *evt : statement->getEvents ())
//         {
//           events.push_back (evt);
//         }
//     }

//   return events;
// }

// bool
// NclLinkCompoundStatement::evaluate ()
// {
//   size_t i, size;
//   NclLinkStatement *childStatement;

//   size = _statements.size ();
//   if (!_conjunction)
//     {
//       for (i = 0; i < size; i++)
//         {
//           childStatement = _statements[i];
//           if (childStatement->evaluate ())
//             return returnEvaluationResult (true);
//         }

//       return returnEvaluationResult (false);
//     }
//   else
//     {
//       for (i = 0; i < size; i++)
//         {
//           childStatement = _statements[i];
//           if (!childStatement->evaluate ())
//             return returnEvaluationResult (false);
//         }

//       return returnEvaluationResult (true);
//     }
// }

// NclLinkAssessmentStatement::NclLinkAssessmentStatement (
//     const string &comparator, NclLinkAttributeAssessment *mainAssessment,
//     NclLinkAssessment *otherAssessment)
//     : NclLinkStatement ()
// {
//   this->_comparator = comparator;
//   this->_mainAssessment = mainAssessment;
//   this->_otherAssessment = otherAssessment;
// }

// NclLinkAssessmentStatement::~NclLinkAssessmentStatement ()
// {
//   if (_mainAssessment)
//     delete _mainAssessment;

//   if (_otherAssessment)
//     delete _otherAssessment;
// }

// vector<NclEvent *>
// NclLinkAssessmentStatement::getEvents ()
// {
//   vector<NclEvent *> events;

//   events.push_back (
//       ((NclLinkAttributeAssessment *)_mainAssessment)->getEvent ());

//   auto attrAssessment
//       = cast (NclLinkAttributeAssessment *, _otherAssessment);
//   if (attrAssessment)
//     {
//       events.push_back (attrAssessment->getEvent ());
//     }
//   return events;
// }

// string
// NclLinkAssessmentStatement::getComparator ()
// {
//   return _comparator;
// }

// NclLinkAssessment *
// NclLinkAssessmentStatement::getMainAssessment ()
// {
//   return _mainAssessment;
// }

// void
// NclLinkAssessmentStatement::setMainAssessment (
//     NclLinkAssessment *assessment)
// {
//   _mainAssessment = assessment;
// }

// NclLinkAssessment *
// NclLinkAssessmentStatement::getOtherAssessment ()
// {
//   return _otherAssessment;
// }

// void
// NclLinkAssessmentStatement::setOtherAssessment (
//     NclLinkAssessment *assessment)
// {
//   _otherAssessment = assessment;
// }

// bool
// NclLinkAssessmentStatement::evaluate ()
// {
//   if (_mainAssessment == NULL || _otherAssessment == NULL)
//     return false;
//   return ginga_eval_comparator (_comparator, _mainAssessment->getValue (),
//                                 _otherAssessment->getValue ());
// }

GINGA_FORMATTER_END
