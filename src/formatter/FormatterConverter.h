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

#ifndef FORMATTERCONVERTER_H_
#define FORMATTERCONVERTER_H_

#include "ncl/Action.h"
#include "ncl/AssessmentStatement.h"
#include "ncl/CausalConnector.h"
#include "ncl/ValueAssessment.h"
#include "ncl/CompoundAction.h"
#include "ncl/CompoundStatement.h"
#include "ncl/CompoundCondition.h"
#include "ncl/ConditionExpression.h"
#include "ncl/AttributeAssessment.h"
#include "ncl/Role.h"
#include "ncl/SimpleAction.h"
#include "ncl/SimpleCondition.h"
#include "ncl/Statement.h"
#include "ncl/TriggerExpression.h"
#include "ncl/EventUtil.h"
#include "ncl/Bind.h"
#include "ncl/CausalLink.h"
#include "ncl/Link.h"
#include "ncl/Node.h"
#include "ncl/InterfacePoint.h"
#include "ncl/Parameter.h"
using namespace ::ginga::ncl;

#include "NclExecutionObjectSwitch.h"
#include "NclSwitchEvent.h"

#include "NclAttributionEvent.h"
#include "NclPresentationEvent.h"
#include "NclSelectionEvent.h"
#include "INclEventListener.h"
#include "NclFormatterEvent.h"
#include "NclPresentationEvent.h"

#include "NclFormatterCausalLink.h"
#include "NclFormatterLink.h"
#include "NclLinkAction.h"
#include "NclLinkCompoundAction.h"
#include "NclLinkSimpleAction.h"
#include "NclLinkAssessmentStatement.h"
#include "NclLinkCompoundTriggerCondition.h"

#include "NclCascadingDescriptor.h"

#include "NclCompositeExecutionObject.h"
#include "NclApplicationExecutionObject.h"
#include "NclExecutionObject.h"

#include "RuleAdapter.h"

GINGA_FORMATTER_BEGIN

class FormatterScheduler;

class FormatterConverter : public INclEventListener
{
public:
  explicit FormatterConverter (RuleAdapter *);
  virtual ~FormatterConverter ();

  void setHandlingStatus (bool handling);
  NclExecutionObject *getObjectFromNodeId (const string &id);

  void setLinkActionListener (INclLinkActionListener *actionListener);

  NclExecutionObject *getExecutionObjectFromPerspective (
      NclNodeNesting *perspec, GenericDescriptor *desc);

  set<NclExecutionObject *> *getSettingNodeObjects ();

  NclFormatterEvent *getEvent (NclExecutionObject *exeObj,
                               InterfacePoint *interfacePoint,
                               int ncmEventType,
                               const string &key);

  NclExecutionObject *
  processExecutionObjectSwitch (NclExecutionObjectSwitch *switchObject);

  NclFormatterEvent *insertContext (NclNodeNesting *contextPerspective,
                                    Port *port);

private:
  static int _dummyCount;
  map<string, NclExecutionObject *> _executionObjects;
  set<NclFormatterEvent *> _listening;
  set<NclExecutionObject *> _settingObjects;
  INclLinkActionListener *_actionListener;
  RuleAdapter *_ruleAdapter;
  bool _handling;

  void addExecutionObject (NclExecutionObject *exeObj,
                           NclCompositeExecutionObject *parentObj);

  bool removeExecutionObject (NclExecutionObject *exeObj);

  NclFormatterCausalLink *
  createCausalLink (CausalLink *ncmLink,
                    NclCompositeExecutionObject *parentObject);

  NclCompositeExecutionObject *
  addSameInstance (NclExecutionObject *exeObj, ReferNode *referNode);

  NclCompositeExecutionObject *getParentExecutionObject (
      NclNodeNesting *perspective);

  NclExecutionObject *
  createExecutionObject (const string &id, NclNodeNesting *perspective,
                         NclCascadingDescriptor *descriptor);

  void compileExecutionObjectLinks (NclExecutionObject *exeObj, Node *dataObj,
                                    NclCompositeExecutionObject *parentObj);

  void processLink (Link *ncmLink,
                    Node *dataObject,
                    NclExecutionObject *exeObj,
                    NclCompositeExecutionObject *parentObj);

  void setActionListener (NclLinkAction *action);

  void resolveSwitchEvents (NclExecutionObjectSwitch *switchObject);

  NclFormatterEvent *insertNode (NclNodeNesting *perspective,
                                 InterfacePoint *interfacePoint,
                                 GenericDescriptor *descriptor);

  void eventStateChanged (NclFormatterEvent *someEvent, short transition,
                          short previousState) override;

  static Descriptor *createDummyDescriptor (Node *node);
  static NclCascadingDescriptor *createDummyCascadingDescriptor (Node *node);

  static NclCascadingDescriptor *
  getCascadingDescriptor (NclNodeNesting *nodePerspective,
                          GenericDescriptor *descriptor);

  static NclCascadingDescriptor *checkCascadingDescriptor (Node *node);
  static NclCascadingDescriptor *checkContextCascadingDescriptor (
      NclNodeNesting *nodePerspective,
      NclCascadingDescriptor *cascadingDescriptor, Node *ncmNode);

  static bool hasDescriptorPropName (const string &name);
  static bool isEmbeddedApp (NodeEntity *dataObject);
  static bool isEmbeddedAppMediaType (const string &mediaType);

  void setImplicitRefAssessment (const string &roleId, CausalLink *ncmLink,
                                 NclFormatterEvent *event);

  NclLinkAction *createAction (Action *actionExpression,
                               CausalLink *ncmLink,
                               NclCompositeExecutionObject *parentObject);

  NclLinkCondition *
  createCondition (ConditionExpression *ncmExpression, CausalLink *ncmLink,
                   NclCompositeExecutionObject *parentObject);

  NclLinkCompoundTriggerCondition *createCompoundTriggerCondition (
      short op, GingaTime delay,
      vector<ConditionExpression *> *ncmChildConditions,
      CausalLink *ncmLink, NclCompositeExecutionObject *parentObject);

  NclLinkCondition *createCondition (
      TriggerExpression *triggerExpression, CausalLink *ncmLink,
      NclCompositeExecutionObject *parentObject);

  NclLinkAssessmentStatement *createAssessmentStatement (
      AssessmentStatement *assessmentStatement, Bind *bind, Link *ncmLink,
      NclCompositeExecutionObject *parentObject);

  NclLinkStatement *
  createStatement (Statement *statementExpression, Link *ncmLink,
                   NclCompositeExecutionObject *parentObject);

  NclLinkAttributeAssessment *createAttributeAssessment (
      AttributeAssessment *attributeAssessment, Bind *bind, Link *ncmLink,
      NclCompositeExecutionObject *parentObject);

  NclLinkSimpleAction *
  createSimpleAction (SimpleAction *sae, Bind *bind, Link *ncmLink,
                      NclCompositeExecutionObject *parentObject);

  NclLinkCompoundAction *createCompoundAction (
      short op, GingaTime delay, vector<Action *> *ncmChildActions,
      CausalLink *ncmLink, NclCompositeExecutionObject *parentObject);

  NclLinkTriggerCondition *createSimpleCondition (
      SimpleCondition *condition, Bind *bind, Link *ncmLink,
      NclCompositeExecutionObject *parentObject);

  NclFormatterEvent *createEvent (Bind *bind, Link *ncmLink,
                                  NclCompositeExecutionObject *parentObject);

  GingaTime getDelayParameter (Link *ncmLink, Parameter *connParam,
                               Bind *ncmBind);

  string getBindKey (Link *ncmLink, Bind *ncmBind);
  GingaTime compileDelay (Link *ncmLink, const string &delayObject, Bind *bind);
  //end FormatterLinkConverter
};

GINGA_FORMATTER_END

#endif /*FORMATTERCONVERTER_H_*/
