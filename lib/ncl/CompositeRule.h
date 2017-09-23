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

#ifndef COMPOSITE_RULE_H
#define COMPOSITE_RULE_H

#include "Rule.h"

GINGA_NCL_BEGIN

class CompositeRule : public Rule
{
public:
  CompositeRule (NclDocument *, const string &, bool);
  virtual ~CompositeRule ();
  bool isConjunction ();
  void addRule (Rule *);
  const vector<Rule *> *getRules ();

private:
  vector<Rule *> _rules;
  bool _conjunction;
};

GINGA_NCL_END

#endif // COMPOSITE_RULE_H
