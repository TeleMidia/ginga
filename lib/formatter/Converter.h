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

#ifndef CONVERTER_H
#define CONVERTER_H

#include "GingaInternal.h"
#include "ExecutionObject.h"
#include "ExecutionObjectContext.h"
#include "ExecutionObjectSettings.h"
#include "ExecutionObjectSwitch.h"
#include "NclActions.h"
#include "NclEvents.h"
#include "NclFormatterLink.h"
#include "NclFormatterLink.h"
#include "NclLinkCondition.h"
#include "NclLinkStatement.h"
#include "RuleAdapter.h"

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

class GingaInternal;

GINGA_FORMATTER_BEGIN

class Scheduler;
class Converter : public INclEventListener
{
public:
  explicit Converter (GingaInternal *, INclActionListener *, RuleAdapter *);
  virtual ~Converter ();

  void setHandlingStatus (bool handling);

  ExecutionObject *getExecutionObjectFromPerspective
  (NclNodeNesting *, Descriptor *);

  NclEvent *getEvent (ExecutionObject *exeObj,
                      Anchor *interfacePoint,
                      EventType ncmEventType,
                      const string &key);

  ExecutionObject *
  processExecutionObjectSwitch (ExecutionObjectSwitch *switchObject);

  NclEvent *insertContext (NclNodeNesting *contextPerspective,
                           Port *port);

  RuleAdapter *getRuleAdapter ();


private:
  GingaInternal *_ginga;
  Scheduler *_scheduler;

  set<NclEvent *> _listening;
  INclActionListener *_actionListener;
  RuleAdapter *_ruleAdapter;
  bool _handling;

  void addExecutionObject (ExecutionObject *exeObj,
                           ExecutionObjectContext *parentObj);


  NclFormatterLink *
  createLink (Link *ncmLink,
                    ExecutionObjectContext *parentObject);

  ExecutionObjectContext *
  addSameInstance (ExecutionObject *exeObj, Refer *referNode);

  ExecutionObjectContext *getParentExecutionObject (
      NclNodeNesting *perspective);

  ExecutionObject *
  createExecutionObject (const string &id, NclNodeNesting *perspective,
                         Descriptor *descriptor);

  void compileExecutionObjectLinks (ExecutionObject *exeObj, Node *dataObj,
                                    ExecutionObjectContext *parentObj);

  void processLink (Link *ncmLink,
                    Node *dataObject,
                    ExecutionObject *exeObj,
                    ExecutionObjectContext *parentObj);

  void setActionListener (NclAction *action);

  void resolveSwitchEvents (ExecutionObjectSwitch *switchObject);

  NclEvent *insertNode (NclNodeNesting *perspective,
                        Anchor *interfacePoint,
                        Descriptor *descriptor);

  void eventStateChanged (NclEvent *someEvent,
                          EventStateTransition transition,
                          EventState previousState) override;

  static Descriptor *
  getCascadingDescriptor (NclNodeNesting *nodePerspective,
                          Descriptor *descriptor);

  static bool hasDescriptorPropName (const string &name);

  void setImplicitRefAssessment (const string &roleId, Link *ncmLink,
                                 NclEvent *event);

  NclAction *createAction (Connector *, Link *, ExecutionObjectContext *);

  NclLinkCondition *
  createCondition (Condition *ncmExpression, Link *ncmLink,
                   ExecutionObjectContext *parentObject);

  NclLinkCompoundTriggerCondition *createCompoundTriggerCondition (
      GingaTime delay,
      const vector<Condition *> *ncmChildConditions,
      Link *ncmLink, ExecutionObjectContext *parentObject);

  NclLinkCondition *createCondition (
      TriggerExpression *triggerExpression, Link *ncmLink,
      ExecutionObjectContext *parentObject);

  NclLinkAssessmentStatement *createAssessmentStatement (
      AssessmentStatement *assessmentStatement, Bind *bind, Link *ncmLink,
      ExecutionObjectContext *parentObject);

  NclLinkStatement *
  createStatement (Statement *statementExpression, Link *ncmLink,
                   ExecutionObjectContext *parentObject);

  NclLinkAttributeAssessment *createAttributeAssessment (
      AttributeAssessment *attributeAssessment, Bind *bind,
      ExecutionObjectContext *parentObject);

  NclSimpleAction *
  createSimpleAction (Action *, Bind *,
                      ExecutionObjectContext *);

  NclLinkTriggerCondition *createSimpleCondition (
      SimpleCondition *condition, Bind *bind,
      ExecutionObjectContext *parentObject);

  NclEvent *createEvent (Bind *bind,
                         ExecutionObjectContext *parentObject);

  string getBindKey (Bind *ncmBind);
};

GINGA_FORMATTER_END

#endif /*CONVERTER_H_*/
