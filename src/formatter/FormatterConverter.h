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

#include "NclCascadingDescriptor.h"

#include "NclCompositeExecutionObject.h"
#include "NclApplicationExecutionObject.h"
#include "NclExecutionObject.h"

#include "RuleAdapter.h"

GINGA_FORMATTER_BEGIN

class FormatterLinkConverter;
class FormatterScheduler;

class FormatterConverter : public INclEventListener
{
public:
  FormatterConverter (RuleAdapter *);
  virtual ~FormatterConverter ();

  void setHandlingStatus (bool hanling);
  NclExecutionObject *getObjectFromNodeId (const string &id);

  void setLinkActionListener (INclLinkActionListener *actionListener);

  NclCompositeExecutionObject *
  addSameInstance (NclExecutionObject *exeObj, ReferNode *referNode);

  void compileExecutionObjectLinks (NclExecutionObject *exeObj);

  NclExecutionObject *getExecutionObjectFromPerspective (
      NclNodeNesting *perspective, GenericDescriptor *descriptor);

  set<NclExecutionObject *> *getSettingNodeObjects ();

  NclFormatterEvent *getEvent (NclExecutionObject *exeObj,
                               InterfacePoint *interfacePoint,
                               int ncmEventType, const string &key);

  static NclCascadingDescriptor *
  getCascadingDescriptor (NclNodeNesting *nodePerspective,
                          GenericDescriptor *descriptor);

  void compileExecutionObjectLinks (
      NclExecutionObject *exeObj, Node *dataObject,
      NclCompositeExecutionObject *parentObject);

  NclExecutionObject *
  processExecutionObjectSwitch (NclExecutionObjectSwitch *switchObject);

  NclFormatterEvent *insertContext (NclNodeNesting *contextPerspective,
                                    Port *port);

  bool removeExecutionObject (NclExecutionObject *exeObj, ReferNode *referNode);

  bool removeExecutionObject (NclExecutionObject *exeObj);

private:
  static int _dummyCount;
  map<string, NclExecutionObject *> _executionObjects;
  set<NclFormatterEvent *> _listening;
  set<NclExecutionObject *> _settingObjects;
  FormatterLinkConverter *_linkCompiler;
  INclLinkActionListener *_actionListener;
  RuleAdapter *_ruleAdapter;
  bool _handling;

  void addExecutionObject (NclExecutionObject *exeObj,
                           NclCompositeExecutionObject *parentObject);

  NclCompositeExecutionObject *getParentExecutionObject (
      NclNodeNesting *perspective);

  NclExecutionObject *
  createExecutionObject (const string &id, NclNodeNesting *perspective,
                         NclCascadingDescriptor *descriptor);

  void processLink (Link *ncmLink,
                    Node *dataObject,
                    NclExecutionObject *exeObj,
                    NclCompositeExecutionObject *parentObject);

  void setActionListener (NclLinkAction *action);

  void resolveSwitchEvents (NclExecutionObjectSwitch *switchObject);

  NclFormatterEvent *insertNode (NclNodeNesting *perspective,
                                 InterfacePoint *interfacePoint,
                                 GenericDescriptor *descriptor);

  bool ntsRemoveExecutionObject (NclExecutionObject *exeObj);

  NclExecutionObject *hasExecutionObject (Node *node,
                                          GenericDescriptor *descriptor);

  NclFormatterCausalLink *addCausalLink (ContextNode *context,
                                         CausalLink *link);

  void eventStateChanged (NclFormatterEvent *someEvent, short transition,
                          short previousState) override;

  void reset ();

  static bool hasDescriptorPropName (const string &name);

  static Descriptor *createDummyDescriptor (Node *node);
  static NclCascadingDescriptor *
  createDummyCascadingDescriptor (Node *node);

  static NclCascadingDescriptor *checkCascadingDescriptor (Node *node);
  static NclCascadingDescriptor *checkContextCascadingDescriptor (
      NclNodeNesting *nodePerspective,
      NclCascadingDescriptor *cascadingDescriptor, Node *ncmNode);

  static bool isEmbeddedApp (NodeEntity *dataObject);
  static bool isEmbeddedAppMediaType (const string &mediaType);
};

GINGA_FORMATTER_END

#endif /*FORMATTERCONVERTER_H_*/
