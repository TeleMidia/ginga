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

#ifndef RULE_ADAPTER_H
#define RULE_ADAPTER_H

#include "ExecutionObject.h"
#include "ExecutionObjectContext.h"
#include "ExecutionObjectSwitch.h"
#include "Settings.h"

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class RuleAdapter
{
private:
  Settings *settings;
  map<string, vector<Rule *> *> *ruleListenMap;
  map<Rule *, vector<ExecutionObjectSwitch *> *> *entityListenMap;

public:
  RuleAdapter (Settings *);
  virtual ~RuleAdapter ();

  void reset ();

  Settings *getSettings ();

  void adapt (ExecutionObjectContext *compositeObject, bool force);
  void initializeAttributeRuleRelation (Rule *topRule, Rule *rule);

  void initializeRuleObjectRelation (ExecutionObjectSwitch *object);

  void adapt (ExecutionObjectSwitch *objectAlternatives, bool force);

  Node *adaptSwitch (Switch *switchNode);
  bool evaluateRule (Rule *rule);

private:
  bool evaluateCompositeRule (CompositeRule *rule);
  bool evaluateSimpleRule (SimpleRule *rule);
};

GINGA_FORMATTER_END

#endif // RULE_ADAPTER_H
