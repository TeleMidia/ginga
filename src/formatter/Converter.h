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

#include "ExecutionObject.h"
#include "ExecutionObjectContext.h"
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

GINGA_FORMATTER_BEGIN

class Scheduler;

class Converter : public INclEventListener
{
public:
  explicit Converter (RuleAdapter *);
  virtual ~Converter ();

  void setHandlingStatus (bool handling);

  void setLinkActionListener (INclActionListener *actionListener);

  ExecutionObject *getExecutionObjectFromPerspective (
      NclNodeNesting *perspec, Descriptor *desc);

  NclEvent *getEvent (ExecutionObject *exeObj,
                      InterfacePoint *interfacePoint,
                      EventType ncmEventType,
                      const string &key);

  ExecutionObject *
  processExecutionObjectSwitch (ExecutionObjectSwitch *switchObject);

  NclEvent *insertContext (NclNodeNesting *contextPerspective,
                           Port *port);

private:
  static int _dummyCount;
  map<string, ExecutionObject *> _exeObjects;
  set<NclEvent *> _listening;
  set<ExecutionObject *> _settingsObjects;
  INclActionListener *_actionListener;
  RuleAdapter *_ruleAdapter;
  bool _handling;

  void addExecutionObject (ExecutionObject *exeObj,
                           ExecutionObjectContext *parentObj);

  bool removeExecutionObject (ExecutionObject *exeObj);

  NclFormatterLink *
  createLink (Link *ncmLink,
                    ExecutionObjectContext *parentObject);

  ExecutionObjectContext *
  addSameInstance (ExecutionObject *exeObj, ReferNode *referNode);

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
                        InterfacePoint *interfacePoint,
                        Descriptor *descriptor);

  void eventStateChanged (NclEvent *someEvent,
                          EventStateTransition transition,
                          EventState previousState) override;

  static Descriptor *createDummyDescriptor (Node *node);

  static Descriptor *
  getCascadingDescriptor (NclNodeNesting *nodePerspective,
                          Descriptor *descriptor);

  static bool hasDescriptorPropName (const string &name);

  void setImplicitRefAssessment (const string &roleId, Link *ncmLink,
                                 NclEvent *event);

  NclAction *createAction (Action *actionExpression,
                               Link *ncmLink,
                               ExecutionObjectContext *parentObject);

  NclLinkCondition *
  createCondition (ConditionExpression *ncmExpression, Link *ncmLink,
                   ExecutionObjectContext *parentObject);

  NclLinkCompoundTriggerCondition *createCompoundTriggerCondition (
      short op, GingaTime delay,
      vector<ConditionExpression *> *ncmChildConditions,
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
      AttributeAssessment *attributeAssessment, Bind *bind, Link *ncmLink,
      ExecutionObjectContext *parentObject);

  NclSimpleAction *
  createSimpleAction (SimpleAction *sae, Bind *bind, Link *ncmLink,
                      ExecutionObjectContext *parentObject);

  NclCompoundAction *createCompoundAction (
      GingaTime delay, vector<Action *> *ncmChildActions,
      Link *ncmLink, ExecutionObjectContext *parentObject);

  NclLinkTriggerCondition *createSimpleCondition (
      SimpleCondition *condition, Bind *bind, Link *ncmLink,
      ExecutionObjectContext *parentObject);

  NclEvent *createEvent (Bind *bind, Link *ncmLink,
                         ExecutionObjectContext *parentObject);

  GingaTime getDelayParameter (Link *ncmLink, Parameter *connParam,
                               Bind *ncmBind);

  string getBindKey (Link *ncmLink, Bind *ncmBind);

  GingaTime
  compileDelay (Link *ncmLink, const string &delayObject,Bind *bind);
};

GINGA_FORMATTER_END

#endif /*CONVERTER_H_*/
