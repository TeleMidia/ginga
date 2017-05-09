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

#include "ncl/SwitchNode.h"
using namespace ::ginga::ncl;

#include "ncl/PropertyAnchor.h"
#include "ncl/ContentAnchor.h"
#include "ncl/LambdaAnchor.h"
#include "ncl/SwitchPort.h"
#include "ncl/Port.h"
#include "ncl/InterfacePoint.h"
using namespace ::ginga::ncl;

#include "ncl/CompositeNode.h"
#include "ncl/ContextNode.h"
#include "ncl/ContentNode.h"
#include "ncl/Node.h"
#include "ncl/NodeEntity.h"
using namespace ::ginga::ncl;

#include "ncl/EventUtil.h"
using namespace ::ginga::ncl;

#include "ncl/GenericDescriptor.h"
using namespace ::ginga::ncl;

#include "ncl/Bind.h"
#include "ncl/CausalLink.h"
#include "ncl/Link.h"
using namespace ::ginga::ncl;

#include "ncl/ReferNode.h"
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
#include "INclLinkActionListener.h"

#include "NclCascadingDescriptor.h"
#include "NclFormatterLayout.h"

#include "NclCompositeExecutionObject.h"
#include "NclApplicationExecutionObject.h"
#include "NclExecutionObject.h"
#include "NclNodeNesting.h"

#include "RuleAdapter.h"

#include "AdapterPlayerManager.h"

#include "FormatterScheduler.h"


GINGA_FORMATTER_BEGIN

class FormatterConverter : public INclEventListener
{
private:
  int depthLevel;
  static int dummyCount;
  map<string, NclExecutionObject *> executionObjects;
  set<NclFormatterEvent *> listening;
  set<NclExecutionObject *> settingObjects;
  void *linkCompiler; // FormatterLinkConverter*
  FormatterScheduler *scheduler;
  INclLinkActionListener *actionListener;
  RuleAdapter *ruleAdapter;
  pthread_mutex_t objectsMutex;
  pthread_mutex_t lMutex;
  bool handling;

public:
  FormatterConverter (RuleAdapter *ruleAdapter);
  virtual ~FormatterConverter ();

  void executionObjectReleased (const string &objectId);
  set<NclExecutionObject *> *getRunningObjects ();
  void setHandlingStatus (bool hanling);
  NclExecutionObject *getObjectFromNodeId (const string &id);

  void setScheduler (void *scheduler);
  void setLinkActionListener (INclLinkActionListener *actionListener);

  void setDepthLevel (int level);
  int getDepthLevel ();
  void checkGradSameInstance (set<ReferNode *> *gradSame,
                              NclExecutionObject *object);

  NclCompositeExecutionObject *
  addSameInstance (NclExecutionObject *executionObject,
                   ReferNode *referNode);

private:
  void addExecutionObject (NclExecutionObject *executionObject,
                           NclCompositeExecutionObject *parentObject,
                           int depthLevel);

public:
  void compileExecutionObjectLinks (NclExecutionObject *executionObject,
                                    int depthLevel);

  NclExecutionObject *getExecutionObjectFromPerspective (
      NclNodeNesting *perspective, GenericDescriptor *descriptor,
      int depthLevel);

public:
  set<NclExecutionObject *> *getSettingNodeObjects ();

private:
  NclCompositeExecutionObject *getParentExecutionObject (
      NclNodeNesting *perspective,
      int depthLevel);

public:
  NclFormatterEvent *getEvent (NclExecutionObject *executionObject,
                               InterfacePoint *interfacePoint,
                               int ncmEventType, const string &key);

private:
  void
  createMultichannelObject (NclCompositeExecutionObject *compositeObject,
                            int depthLevel);

  NclExecutionObject *
  createExecutionObject (const string &id, NclNodeNesting *perspective,
                         NclCascadingDescriptor *descriptor,
                         int depthLevel);

  static bool hasDescriptorPropName (const string &name);

  static Descriptor *createDummyDescriptor (Node *node);
  static NclCascadingDescriptor *
  createDummyCascadingDescriptor (Node *node);

  static NclCascadingDescriptor *checkCascadingDescriptor (Node *node);
  static NclCascadingDescriptor *checkContextCascadingDescriptor (
      NclNodeNesting *nodePerspective,
      NclCascadingDescriptor *cascadingDescriptor, Node *ncmNode);

public:
  static NclCascadingDescriptor *
  getCascadingDescriptor (NclNodeNesting *nodePerspective,
                          GenericDescriptor *descriptor);

private:
  void processLink (Link *ncmLink, Node *dataObject,
                    NclExecutionObject *executionObject,
                    NclCompositeExecutionObject *parentObject);

public:
  void compileExecutionObjectLinks (
      NclExecutionObject *executionObject, Node *dataObject,
      NclCompositeExecutionObject *parentObject, int depthLevel);

private:
  void setActionListener (NclLinkAction *action);

public:
  NclExecutionObject *
  processExecutionObjectSwitch (NclExecutionObjectSwitch *switchObject);

private:
  void resolveSwitchEvents (NclExecutionObjectSwitch *switchObject,
                            int depthLevel);

  NclFormatterEvent *insertNode (NclNodeNesting *perspective,
                                 InterfacePoint *interfacePoint,
                                 GenericDescriptor *descriptor);

public:
  NclFormatterEvent *insertContext (NclNodeNesting *contextPerspective,
                                    Port *port);

  bool removeExecutionObject (NclExecutionObject *executionObject,
                              ReferNode *referNode);

  bool removeExecutionObject (NclExecutionObject *executionObject);

private:
  bool ntsRemoveExecutionObject (NclExecutionObject *executionObject);

public:
  NclExecutionObject *hasExecutionObject (Node *node,
                                          GenericDescriptor *descriptor);

  NclFormatterCausalLink *addCausalLink (ContextNode *context,
                                         CausalLink *link);

  void eventStateChanged (void *someEvent, short transition,
                          short previousState);

  short getPriorityType ();
  void reset ();
};

GINGA_FORMATTER_END

#endif /*FORMATTERCONVERTER_H_*/
