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

#ifndef COMPOUND_STATEMENT_H
#define COMPOUND_STATEMENT_H

#include "Statement.h"
#include "AssessmentStatement.h"


GINGA_NCL_BEGIN

class CompoundStatement : public Statement
{
public:
  CompoundStatement (bool, bool);
  virtual ~CompoundStatement ();

  bool isNegated ();
  bool isConjunction ();
  bool isDisjunction ();

  void addStatement (Statement *);
  const vector<Statement *> *getStatements ();

protected:
  vector<Statement *> _statements;
  bool _conjunction;
  bool _negated;
};

GINGA_NCL_END

#endif // COMPOUND_STATEMENT_H
