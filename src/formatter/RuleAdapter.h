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

#ifndef _RULEADAPTER_H_
#define _RULEADAPTER_H_

#include "NclCascadingDescriptor.h"
#include "NclCompositeExecutionObject.h"
#include "NclExecutionObject.h"

#include "NclExecutionObjectSwitch.h"

#include "ncl/GenericDescriptor.h"
using namespace ::ginga::ncl;

#include "ncl/DescriptorSwitch.h"
#include "ncl/CompositeRule.h"
#include "ncl/Rule.h"
#include "ncl/SimpleRule.h"
#include "ncl/SwitchNode.h"
using namespace ::ginga::ncl;

#include "ncl/Comparator.h"
using namespace ::ginga::ncl;

#include "ncl/Node.h"
using namespace ::ginga::ncl;

#include "util/Observer.h"

#include "math.h"

#include "PresentationContext.h"

GINGA_FORMATTER_BEGIN

class RuleAdapter
{
private:
  PresentationContext *presContext;
  map<string, vector<Rule *> *> *ruleListenMap;
  map<Rule *, vector<NclExecutionObjectSwitch *> *> *entityListenMap;
  map<Rule *, vector<DescriptorSwitch *> *> *descListenMap;

public:
  RuleAdapter (PresentationContext *presContext);
  virtual ~RuleAdapter ();

  void reset ();

  PresentationContext *getPresentationContext ();

  void adapt (NclCompositeExecutionObject *compositeObject, bool force);
  void initializeAttributeRuleRelation (Rule *topRule, Rule *rule);

  void initializeRuleObjectRelation (NclExecutionObjectSwitch *object);

  void adapt (NclExecutionObjectSwitch *objectAlternatives, bool force);

  bool adaptDescriptor (NclExecutionObject *executionObject);
  Node *adaptSwitch (SwitchNode *switchNode);
  bool evaluateRule (Rule *rule);

private:
  bool evaluateCompositeRule (CompositeRule *rule);
  bool evaluateSimpleRule (SimpleRule *rule);
};

GINGA_FORMATTER_END

#endif //_RULEADAPTER_H_
