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

#include "ginga.h"

#include "ncl/SwitchNode.h"
#include "ncl/PropertyAnchor.h"
#include "ncl/ContentAnchor.h"
#include "ncl/LambdaAnchor.h"
#include "ncl/SwitchPort.h"
#include "ncl/Port.h"
#include "ncl/InterfacePoint.h"
#include "ncl/CompositeNode.h"
#include "ncl/ContextNode.h"
#include "ncl/ContentNode.h"
#include "ncl/Node.h"
#include "ncl/NodeEntity.h"
#include "ncl/EventUtil.h"
#include "ncl/GenericDescriptor.h"
#include "ncl/Bind.h"
#include "ncl/CausalLink.h"
#include "ncl/Link.h"
#include "ncl/ReferNode.h"
using namespace ::ginga::ncl;

#include "NclLinkCompoundStatement.h"
#include "NclLinkRepeatAction.h"
#include "NclLinkTransitionTriggerCondition.h"
#include "NclLinkValueAssessment.h"
#include "NclLinkCompoundTriggerCondition.h"
#include "NclLinkAssignmentAction.h"
#include "NclLinkCompoundTriggerCondition.h"
#include "NclLinkAndCompoundTriggerCondition.h"

#include "FormatterConverter.h"
GINGA_FORMATTER_BEGIN

int FormatterConverter::_dummyCount = 0;

static const string SEPARATOR = "/";

FormatterConverter::FormatterConverter (RuleAdapter *ruleAdapter)
{
  this->_actionListener = nullptr;
  this->_ruleAdapter = ruleAdapter;
  this->_handling = false;
}

FormatterConverter::~FormatterConverter ()
{
  map<string, NclExecutionObject *>::iterator i;
  NclExecutionObject *object;

  for (NclFormatterEvent *evt: _listening)
    {
      if (NclFormatterEvent::hasInstance (evt, false))
        {
          evt->removeEventListener (this);
        }
    }
  this->_ruleAdapter = nullptr;

  for (i = _executionObjects.begin (); i != _executionObjects.end (); )
    {
      object = i->second;

      if (!removeExecutionObject (object))
        {
          i = _executionObjects.erase (i);
        }
      else
        {
          ++i;
        }
    }

  _executionObjects.clear ();
  _settingObjects.clear ();
}

void
FormatterConverter::setHandlingStatus (bool handling)
{
  NclExecutionObject *object;
  this->_handling = handling;

  for (auto &i : _executionObjects)
    {
      object = i.second;
      object->setHandling (handling);
    }
}

NclExecutionObject *
FormatterConverter::getObjectFromNodeId (const string &id)
{
  for (auto &it: _executionObjects)
    {
      NclExecutionObject *expectedObject = it.second;
      NodeEntity *dataObject
          = dynamic_cast<NodeEntity *>(expectedObject->getDataObject ()
                                       ->getDataEntity ());

      g_assert_nonnull (dataObject);

      if (dataObject->getId () == id)
        return expectedObject;
    }

  ERROR ("cannot find object '%s'", id.c_str());
  g_assert_not_reached ();

  return nullptr;
}

void
FormatterConverter::setLinkActionListener (INclLinkActionListener *actListener)
{
  this->_actionListener = actListener;
}

NclCompositeExecutionObject *
FormatterConverter::addSameInstance (NclExecutionObject *executionObject,
                                     ReferNode *referNode)
{
  vector<Node *> *ncmPerspective = referNode->getPerspective ();
  NclNodeNesting *referPerspective = new NclNodeNesting (ncmPerspective);

  NclCompositeExecutionObject *referParentObject
      = getParentExecutionObject (referPerspective);

  if (referParentObject != nullptr)
    {
      TRACE ("'%s' with head node '%s' refer to '%s', which has as "
             "execution object '%s' and parent object '%s'",
             referNode->getId ().c_str (),
             referPerspective->getHeadNode ()->getId ().c_str (),
             referNode->getReferredEntity ()->getId ().c_str (),
             executionObject->getId ().c_str (),
             referParentObject->getId ().c_str ());

      executionObject->addParentObject (
            referNode,
            referParentObject,
            referPerspective->getNode (referPerspective->getNumNodes () - 2));

      referParentObject->addExecutionObject (executionObject);

      // A new entry for the execution object is inserted using
      // the refer node id.  As a consequence, links referring to the
      // refer node will generate events in the execution object.
      NclCascadingDescriptor *desc = executionObject->getDescriptor ();

      string objectId;
      if (desc)
        {
          objectId = (referPerspective->getId () + SEPARATOR
                      + executionObject->getDescriptor ()->getId ());
        }
      else
        {
          objectId = referPerspective->getId ();
        }
      _executionObjects[objectId] = executionObject;
    }

  delete ncmPerspective;
  delete referPerspective;

  return referParentObject;
}

void
FormatterConverter::addExecutionObject (NclExecutionObject *exeObj,
                                        NclCompositeExecutionObject *parentObj)
{
  _executionObjects[exeObj->getId ()] = exeObj;

  if (parentObj)
    {
      parentObj->addExecutionObject (exeObj);
    }

  // Hanlde settings nodes.
  Node *dataObject = exeObj->getDataObject ();
  ContentNode *contentNode = dynamic_cast <ContentNode *> (dataObject);

  if (contentNode && contentNode->isSettingNode ())
    {
      _settingObjects.insert (exeObj);
    }

  ReferNode *referNode = dynamic_cast <ReferNode *> (dataObject);
  if (referNode)
    {
      if (referNode->getInstanceType () == "instSame")
        {
          Entity *entity = referNode->getDataEntity ();
          ContentNode *entityContentNode
              = dynamic_cast <ContentNode *> (entity);

          if (entityContentNode && entityContentNode->isSettingNode ())
            {
              _settingObjects.insert (exeObj);
            }
        }
    }

  NclNodeNesting *nodePerspective = exeObj->getNodePerspective ();
  Node *headNode = nodePerspective->getHeadNode ();

  NodeEntity *nodeEntity = dynamic_cast <NodeEntity *> (dataObject);
  CompositeNode *headCompositeNode = dynamic_cast <CompositeNode *> (headNode);
  if (headCompositeNode && nodeEntity)
    {
      set<ReferNode *> *sameInstances = nodeEntity->getInstSameInstances ();
      g_assert_nonnull (sameInstances);

      for (ReferNode *referNode: *(sameInstances))
        {
          TRACE ("'%s' instSame of '%s'",
                 exeObj->getId ().c_str(),
                 referNode->getId ().c_str());

          if (headCompositeNode->recursivelyContainsNode (referNode))
            {
              addSameInstance (exeObj, referNode);
            }
          else
            {
              WARNING ("cannot find '%s' inside '%s'",
                       referNode->getId ().c_str(),
                       headNode->getId ().c_str());
            }
        }
    }

  delete nodePerspective;

  NclCascadingDescriptor *descriptor = exeObj->getDescriptor ();
  if (descriptor)
    descriptor->setFormatterLayout ();

  // Compile execution object links
  for (Node *node : exeObj->getNodes ())
    {
      NclCompositeExecutionObject *parent
          = dynamic_cast <NclCompositeExecutionObject*> (
              exeObj->getParentObject (node));

      g_assert_nonnull (parent);

      compileExecutionObjectLinks (exeObj, node, parent);
    }
}

bool
FormatterConverter::removeExecutionObject (NclExecutionObject *exeObj)
{
  bool removed = false;

  if (!NclExecutionObject::hasInstance (exeObj, false))
    {
      return removed;
    }

   map<string, NclExecutionObject *>::iterator i
       = _executionObjects.find (exeObj->getId ());

  if (i != _executionObjects.end ())
    {
      _executionObjects.erase (i);
      removed = true;
    }

  if (_settingObjects.count (exeObj))
    {
      _settingObjects.erase (_settingObjects.find (exeObj));
      removed = true;
    }

  if (removed
      && NclExecutionObject::hasInstance (exeObj, true))
    {
      delete exeObj;
    }

  return removed;
}

NclExecutionObject *
FormatterConverter::getExecutionObjectFromPerspective (
    NclNodeNesting *perspective, GenericDescriptor *descriptor)
{
  NclCompositeExecutionObject *parentObj;
  NclExecutionObject *exeObj;

  string id = perspective->getId () + SEPARATOR;
  NclCascadingDescriptor *cascadingDescriptor
      = getCascadingDescriptor (perspective, descriptor);
  if (cascadingDescriptor)
    {
      id = id + cascadingDescriptor->getId ();
    }

  auto i = _executionObjects.find (id);
  if (i != _executionObjects.end ())
    {
      if (cascadingDescriptor)
        {
          delete cascadingDescriptor;
          cascadingDescriptor = nullptr;
        }
      exeObj = i->second;
      return exeObj;
    }

  parentObj = getParentExecutionObject (perspective);
  exeObj = createExecutionObject (id, perspective, cascadingDescriptor);

  if (exeObj == nullptr)
    {
      if (cascadingDescriptor != nullptr)
        {
          delete cascadingDescriptor;
          cascadingDescriptor = nullptr;
        }

      string descId = "nullptr";
      if (descriptor)
        {
          descId = descriptor->getId();
        }

      WARNING ("Object id = '%s', perspective = '%s' descriptor = '%s' "
               "was not created.",
               id.c_str(),
               perspective->getId().c_str(),
               descId.c_str());

      return nullptr;
    }

  string parentId = "nullptr";
  if (parentObj != nullptr)
    {
      parentId = parentObj->getId ();
    }

  TRACE ("adding_object with id='%s', perspective='%s', and parent='%s'",
         id.c_str (),
         perspective->getId ().c_str (),
         parentId.c_str ());

  addExecutionObject (exeObj, parentObj);

  return exeObj;
}

set<NclExecutionObject *> *
FormatterConverter::getSettingNodeObjects ()
{
  return new set<NclExecutionObject *> (_settingObjects);
}

NclCompositeExecutionObject *
FormatterConverter::getParentExecutionObject (NclNodeNesting *perspective)
{
  NclNodeNesting *parentPerspective;

  if (perspective->getNumNodes () > 1)
    {
      parentPerspective = perspective->copy ();
      parentPerspective->removeAnchorNode ();

      NclCompositeExecutionObject *cObj
          = dynamic_cast <NclCompositeExecutionObject*> (
              this->getExecutionObjectFromPerspective (
                parentPerspective, nullptr));

      g_assert_nonnull (cObj);

      delete parentPerspective;
      return cObj;
    }

  return nullptr;
}

NclFormatterEvent *
FormatterConverter::getEvent (NclExecutionObject *exeObj,
                              InterfacePoint *interfacePoint,
                              int ncmEventType, const string &key)
{
  string id;
  NclFormatterEvent *event;
  string type;

  xstrassign (type, "%d", ncmEventType);
  if (key == "")
    {
      id = interfacePoint->getId () + "_" + type;
    }
  else
    {
      id = interfacePoint->getId () + "_" + type + "_" + key;
    }

  event = exeObj->getEvent (id);
  if (event != nullptr)
    {
      return event;
    }

  NclExecutionObjectSwitch * switchObj
      = dynamic_cast <NclExecutionObjectSwitch *> (exeObj);
  if (switchObj)
    {
      event = new NclSwitchEvent (
          id, switchObj, interfacePoint, ncmEventType, key);
    }
  else
    {
      if (ncmEventType == EventUtil::EVT_PRESENTATION)
        {
          event = new NclPresentationEvent (
              id, exeObj, (ContentAnchor *)interfacePoint);
        }
      else
        {
          NclCompositeExecutionObject *cObj
              = dynamic_cast <NclCompositeExecutionObject *> (exeObj);
          if (cObj)
            {
              // TODO: eventos internos da composicao.
              // Estao sendo tratados nos elos.
              if (ncmEventType == EventUtil::EVT_ATTRIBUTION)
                {
                  if (interfacePoint->instanceOf ("PropertyAnchor"))
                    {
                      event = new NclAttributionEvent (
                          id, exeObj,
                          (PropertyAnchor *)interfacePoint,
                          _ruleAdapter->getSettings ());
                    }
                  else
                    {
                      WARNING ("NCM event type is attribution, "
                               "but interface point isn't");

                      event = new NclAttributionEvent (
                          id, exeObj, nullptr,
                          _ruleAdapter->getSettings ());
                    }
                }
            }
          else
            {
              switch (ncmEventType)
                {
                case EventUtil::EVT_ATTRIBUTION:
                  {
                    PropertyAnchor *propAnchor
                        = dynamic_cast <PropertyAnchor *> (interfacePoint);
                    if (propAnchor)
                      {
                        event = new NclAttributionEvent (
                            id, exeObj, propAnchor,
                            _ruleAdapter->getSettings ());
                      }
                    else
                      {
                        WARNING ("NCM event type is attribution, but "
                                 "interface point isn't.");

                        IntervalAnchor *intervalAnchor
                            = dynamic_cast <IntervalAnchor *> (interfacePoint);
                        if (intervalAnchor)
                          {
                            WARNING ("it was supposed to be a PRESENTATION "
                                     "EVENT");

                            // TODO: find the correct way to solve this
                            event = new NclPresentationEvent (
                                id, exeObj,
                                intervalAnchor);
                          }

                        return nullptr;
                      }
                  }
                  break;

                case EventUtil::EVT_SELECTION:
                  {
                    event = new NclSelectionEvent (
                        id, exeObj, (ContentAnchor *)interfacePoint);

                    if (key != "")
                      {
                        ((NclSelectionEvent *)event)->setSelectionCode (key);
                      }
                  }
                  break;

                default:
                  WARNING ("unknown event type '%d'", ncmEventType);
                  break;
                }
            }
        }
    }

  if (event != nullptr)
    {
      exeObj->addEvent (event);
    }
  else
    {
      WARNING ("Returning a nullptr event for '%s'",
               id.c_str ());
    }

  return event;
}

NclExecutionObject *
FormatterConverter::createExecutionObject (
    const string &id, NclNodeNesting *perspective,
    NclCascadingDescriptor *descriptor)
{
  NodeEntity *nodeEntity;
  Node *node;
  NclNodeNesting *nodePerspective;
  NclExecutionObject *exeObj;
  NclPresentationEvent *compositeEvt;

  nodeEntity = dynamic_cast <NodeEntity *> (
                  perspective->getAnchorNode ()->getDataEntity ());

  g_assert_nonnull (nodeEntity);

  node = perspective->getAnchorNode ();

  // solve execution object cross reference coming from refer nodes with
  // new instance = false
  ContentNode *contentNode = dynamic_cast <ContentNode *> (nodeEntity);
  if (contentNode
      && contentNode->getNodeType () != ""
      && !contentNode->isSettingNode ())
    {
      ReferNode *referNode = dynamic_cast <ReferNode *> (node);
      if (referNode)
        {
          if (referNode->getInstanceType () != "new")
            {
              nodePerspective
                  = new NclNodeNesting (nodeEntity->getPerspective ());

              // verify if both nodes are in the same base.
              if (nodePerspective->getHeadNode ()
                  == perspective->getHeadNode ())
                {
                  exeObj = getExecutionObjectFromPerspective (nodePerspective,
                                                              nullptr);
                  if (exeObj == nullptr)
                    {
                      if (isEmbeddedApp (nodeEntity))
                        {
                          exeObj
                              = new NclApplicationExecutionObject (
                                  id, nodeEntity, descriptor, _handling,
                                  _actionListener);
                        }
                      else
                        {
                          exeObj  = new NclExecutionObject (
                                id, nodeEntity, descriptor, _handling,
                              _actionListener);
                        }

                      // TODO informa a substituicao
                    }
                }
              else
                {
                  // not in the same base => create a new version
                  if (isEmbeddedApp (nodeEntity))
                    {
                      exeObj = new NclApplicationExecutionObject (
                          id, nodeEntity, descriptor, _handling,
                          _actionListener);
                    }
                  else
                    {
                      exeObj = new NclExecutionObject (
                          id, nodeEntity, descriptor, _handling,
                          _actionListener);
                    }

                  // TODO informa a substituicao
                }

              delete nodePerspective;

              if (exeObj != nullptr)
                {
                  return exeObj;
                }
            }
        }
    }

  SwitchNode *switchNode = dynamic_cast <SwitchNode *> (nodeEntity);
  if (switchNode)
    {
      string s;
      exeObj = new NclExecutionObjectSwitch (id, node, _handling,
                                                      _actionListener);
      xstrassign (s, "%d", EventUtil::EVT_PRESENTATION);
      compositeEvt = new NclPresentationEvent (
          nodeEntity->getLambdaAnchor ()->getId () + "_" + s,
          exeObj,
          (ContentAnchor *)(nodeEntity->getLambdaAnchor ()));

      exeObj->addEvent (compositeEvt);
      // to monitor the switch presentation and clear the selection after
      // each execution
      compositeEvt->addEventListener (this);
      _listening.insert (compositeEvt);
    }
  else if (nodeEntity->instanceOf ("CompositeNode"))
    {
      string s;
      exeObj = new NclCompositeExecutionObject (
          id, node, descriptor, _handling, _actionListener);

      xstrassign (s, "%d", EventUtil::EVT_PRESENTATION);
      compositeEvt = new NclPresentationEvent (
          nodeEntity->getLambdaAnchor ()->getId () + "_" + s,
          exeObj,
          (ContentAnchor *)(nodeEntity->getLambdaAnchor ()));

      exeObj->addEvent (compositeEvt);

      // to monitor the presentation and remove object at stops
      // compositeEvent->addEventListener(this);
    }
  else if (isEmbeddedApp (nodeEntity))
    {
      exeObj = new NclApplicationExecutionObject (
          id, node, descriptor, _handling, _actionListener);
    }
  else
    {
      exeObj = new NclExecutionObject (id, node, descriptor,
                                       _handling, _actionListener);
    }

  return exeObj;
}



Descriptor *
FormatterConverter::createDummyDescriptor (arg_unused (Node *node))
{
  Descriptor *ncmDesc
      = new Descriptor ("dummyDescriptor" + xstrbuild ("%d", _dummyCount));

  ncmDesc->setFocusDecoration (new FocusDecoration ());

  _dummyCount++;

  return ncmDesc;
}

NclCascadingDescriptor *
FormatterConverter::createDummyCascadingDescriptor (Node *node)
{
  Descriptor *ncmDesc = nullptr;
  string name;

  vector<PropertyAnchor *> *anchors = node->getOriginalPropertyAnchors ();
  g_assert_nonnull (anchors);

  for (PropertyAnchor *property: *anchors)
    {
      name = property->getPropertyName ();

      if (hasDescriptorPropName (name))
        {
          NodeEntity *nodeEntity = dynamic_cast <NodeEntity *> (node);
          ReferNode *referNode = dynamic_cast <ReferNode *> (node);
          if (nodeEntity)
            {
              ncmDesc = createDummyDescriptor (nodeEntity);
              nodeEntity->setDescriptor (ncmDesc);
            }
          else if (referNode
                   && referNode->getInstanceType () == "new")
            {
              if (referNode->getInstanceDescriptor () == nullptr)
                {
                  nodeEntity = (NodeEntity *)node->getDataEntity ();
                  ncmDesc = (Descriptor *)nodeEntity->getDescriptor ();

                  if (ncmDesc == nullptr)
                    {
                      ncmDesc = createDummyDescriptor (node);
                    }
                  referNode->setInstanceDescriptor (ncmDesc);
                }
              else
                {
                  ncmDesc = (Descriptor *) referNode->getInstanceDescriptor ();
                }
            }
          else
            {
              ncmDesc = createDummyDescriptor (node);
            }

          return new NclCascadingDescriptor (ncmDesc);
        }
    }

  ReferNode *referNode = dynamic_cast <ReferNode *> (node);
  if (referNode
      && referNode->getInstanceType () == "new"
      && referNode->getInstanceDescriptor () == nullptr)
    {
      NodeEntity *nodeEntity
          = dynamic_cast<NodeEntity *> (node->getDataEntity ());
      g_assert_nonnull (nodeEntity);

      ncmDesc = dynamic_cast<Descriptor *> (nodeEntity->getDescriptor ());
      g_assert_nonnull (ncmDesc);

      referNode->setInstanceDescriptor (ncmDesc);

      return new NclCascadingDescriptor (ncmDesc);
    }

  return nullptr;
}

NclCascadingDescriptor *
FormatterConverter::checkCascadingDescriptor (Node *node)
{
  NclCascadingDescriptor *cascadingDescriptor = nullptr;

  ContentNode *contentNode = dynamic_cast <ContentNode *> (node);
  ReferNode *referNode = dynamic_cast <ReferNode *> (node);

  if (contentNode)
    {
      cascadingDescriptor = createDummyCascadingDescriptor (node);
    }
  else if (referNode
           && referNode->getInstanceType () == "new")
    {
      NodeEntity *nodeEntity
          = dynamic_cast<NodeEntity *> (node->getDataEntity ());
      g_assert_nonnull (nodeEntity);

      node->copyProperties (nodeEntity);
      cascadingDescriptor = createDummyCascadingDescriptor (node);
    }

  return cascadingDescriptor;
}

NclCascadingDescriptor *
FormatterConverter::checkContextCascadingDescriptor (
    NclNodeNesting *nodePerspective,
    NclCascadingDescriptor *cascadingDescriptor, Node *ncmNode)
{
  ContextNode *context;
  int size;
  NclCascadingDescriptor *resDesc = cascadingDescriptor;

  // Is there a node descriptor defined in the context node?
  size = nodePerspective->getNumNodes ();
  if (size > 1 && nodePerspective->getNode (size - 2) != nullptr
      && nodePerspective->getNode (size - 2)->instanceOf ("ContextNode"))
    {
      context
          = dynamic_cast<ContextNode *> (nodePerspective->getNode (size - 2)
                                          ->getDataEntity ());
      g_assert_nonnull (context);

      if (context->getNodeDescriptor (ncmNode) != nullptr)
        {
          if (resDesc == nullptr)
            {
              resDesc = new NclCascadingDescriptor (
                  context->getNodeDescriptor (ncmNode));
            }
          else
            {
              resDesc->cascade (context->getNodeDescriptor (ncmNode));
            }
        }
    }

  return resDesc;
}

NclCascadingDescriptor *
FormatterConverter::getCascadingDescriptor (NclNodeNesting *nodePerspective,
                                            GenericDescriptor *descriptor)
{
  NclCascadingDescriptor *cascadingDescriptor = nullptr;
  Descriptor *ncmDesc;
  Node *anchorNode;
  Node *node;

  anchorNode = nodePerspective->getAnchorNode ();

  ReferNode *referNode = dynamic_cast <ReferNode *> (anchorNode);

  if (referNode
      && referNode->getInstanceType () == "new")
    {
      node = anchorNode;
      ncmDesc
          = dynamic_cast<Descriptor *> (referNode->getInstanceDescriptor ());
    }
  else
    {
      node = dynamic_cast<Node *>(anchorNode->getDataEntity ());
      NodeEntity *nodeEntity = dynamic_cast <NodeEntity *> (node);
      if (node == nullptr || nodeEntity == nullptr)
        {
          WARNING ("failed to cascading descriptor: invalid node entity");
          return nullptr;
        }

      ncmDesc = dynamic_cast <Descriptor *>(nodeEntity->getDescriptor ());
    }

  if (ncmDesc != nullptr)
    {
      cascadingDescriptor = new NclCascadingDescriptor (ncmDesc);
    }

  cascadingDescriptor = checkContextCascadingDescriptor (
      nodePerspective, cascadingDescriptor, node);

  // there is an explicit descriptor (user descriptor)?
  if (descriptor != nullptr)
    {
      if (cascadingDescriptor == nullptr)
        {
          cascadingDescriptor = new NclCascadingDescriptor (descriptor);
        }
      else
        {
          cascadingDescriptor->cascade (descriptor);
        }
    }

  if (cascadingDescriptor == nullptr)
    {
      cascadingDescriptor = checkCascadingDescriptor (node);
    }

  return cascadingDescriptor;
}

void
FormatterConverter::processLink (Link *ncmLink,
                                 Node *dataObject,
                                 NclExecutionObject *executionObject,
                                 NclCompositeExecutionObject *parentObject)
{
  vector<GenericDescriptor *> *descriptors;
  GenericDescriptor *descriptor;
  NodeEntity *nodeEntity = nullptr;
  set<ReferNode *> *sameInstances;
  bool contains = false;

  if (executionObject->getDataObject () != nullptr)
    {
      nodeEntity
          = dynamic_cast <NodeEntity *>(executionObject->getDataObject ());
    }

  // Since the link may be removed in a deepest compilation it is necessary to
  // check that the link was not compiled.
  if (parentObject->containsUncompiledLink (ncmLink))
    {
      descriptor = nullptr;
      if (executionObject->getDescriptor () != nullptr)
        {
          descriptors
              = executionObject->getDescriptor ()->getNcmDescriptors ();

          if (descriptors != nullptr && !(descriptors->empty ()))
            {
              descriptor = (*descriptors)[descriptors->size () - 1];
            }
        }

      CausalLink *causalLink = dynamic_cast <CausalLink *> (ncmLink);
      if (causalLink)
        {
          if (nodeEntity != NULL)
            {
              sameInstances = nodeEntity->getInstSameInstances ();
              if (sameInstances != NULL)
                {
                  for (ReferNode *referNode: *sameInstances)
                    {
                      contains
                          = causalLink->containsSourceNode (referNode,
                                                            descriptor);

                      if (contains)
                        {
                          break;
                        }
                    }
                }
            }

          // Checks if execution object is part of link conditions.
          if (causalLink->containsSourceNode (dataObject, descriptor)
              || contains)
            {
              // Compile causal link.
              parentObject->removeLinkUncompiled (ncmLink);
              NclFormatterLink *formatterLink
                  = createCausalLink (causalLink, parentObject);

              if (formatterLink != NULL)
                {
                  setActionListener (
                      ((NclFormatterCausalLink *)formatterLink)
                          ->getAction ());

                  parentObject->setLinkCompiled (formatterLink);
                  TRACE ("link compiled '%s'",
                         ncmLink->getId ().c_str());
                }
            }
          else
            {
              WARNING ("cannot process ncmLink '%s' inside '%s' "
                       "because '%s' does not contain '%s' src",
                       ncmLink->getId ().c_str(),
                       parentObject->getId ().c_str(),
                       ncmLink->getId ().c_str(),
                       dataObject->getId ().c_str());
            }
        }
      else
        {
          WARNING ("cannot process ncmLink '%s' inside '%s' "
                   "because it isn't a causal link",
                   ncmLink->getId ().c_str (),
                   parentObject->getId ().c_str ());
        }
    }
  else
    {
      WARNING ("cannot process ncmLink '%s' inside '%s' "
               "because link may be removed in a deepest compilation.",
               ncmLink->getId ().c_str (),
               parentObject->getId ().c_str());
    }
}

void
FormatterConverter::compileExecutionObjectLinks (
    NclExecutionObject *exeObj, Node *dataObject,
    NclCompositeExecutionObject *parentObj)
{
  set<Link *> *uncompiledLinks;
  NclCompositeExecutionObject *compObj;
  Node *execDataObject;

  exeObj->setCompiled (true);

  if (parentObj == nullptr)
    {
      return;
    }

  execDataObject = exeObj->getDataObject ();
  if (execDataObject != dataObject)
    {
      compObj = parentObj->getParentFromDataObject (execDataObject);
      if (compObj != nullptr && compObj != parentObj)
        {
          compileExecutionObjectLinks (exeObj, execDataObject,
                                       compObj);
        }
    }

  uncompiledLinks = parentObj->getUncompiledLinks ();
  if (!uncompiledLinks->empty ())
    {
      set<Link *> *dataLinks = uncompiledLinks;

      for ( Link *ncmLink : *dataLinks)
        {
          processLink (ncmLink, dataObject, exeObj, parentObj);
        }

      delete dataLinks;

      compileExecutionObjectLinks (
          exeObj, dataObject,
          (NclCompositeExecutionObject *)(parentObj->getParentObject ()));
    }
  else
    {
      NclExecutionObject *object;

      delete uncompiledLinks;

      while (parentObj != nullptr)
        {
          object = parentObj;
          parentObj
              = (NclCompositeExecutionObject *)(parentObj
                                                    ->getParentObject ());

          compileExecutionObjectLinks (object, dataObject, parentObj);
        }
    }
}

void
FormatterConverter::setActionListener (NclLinkAction *action)
{
  NclLinkSimpleAction *simpleAction
      = dynamic_cast <NclLinkSimpleAction *> (action);
  NclLinkCompoundAction *compoundAction
      = dynamic_cast <NclLinkCompoundAction *> (action);

  if (simpleAction)
    {
      simpleAction->setSimpleActionListener (_actionListener);
    }
  else if (compoundAction)
    {
      vector<NclLinkSimpleAction *> actions;

      compoundAction->setCompoundActionListener (_actionListener);
      compoundAction->getSimpleActions (&actions);

      for (NclLinkSimpleAction *a: actions)
        {
          setActionListener (a);
        }
    }
  else
    {
      g_assert_not_reached();
    }
}

NclExecutionObject *
FormatterConverter::processExecutionObjectSwitch (
    NclExecutionObjectSwitch *switchObject)
{
  SwitchNode *switchNode;
  Node *selectedNode;
  NclNodeNesting *selectedPerspective;
  string id;
  NclCascadingDescriptor *descriptor;
  map<string, NclExecutionObject *>::iterator i;
  NclExecutionObject *selectedObject;

  switchNode
      = dynamic_cast<SwitchNode *>(switchObject->getDataObject ()->getDataEntity ());
  g_assert_nonnull (switchNode);

  selectedNode = _ruleAdapter->adaptSwitch (switchNode);
  if (selectedNode == NULL)
    {
      WARNING ("Cannot process '%s'. Selected NODE is nullptr.",
                switchObject->getId ().c_str());

      return nullptr;
    }

  selectedPerspective = switchObject->getNodePerspective ();
  selectedPerspective->insertAnchorNode (selectedNode);

  id = selectedPerspective->getId () + SEPARATOR;

  descriptor = FormatterConverter::getCascadingDescriptor (
      selectedPerspective, NULL);

  if (descriptor != NULL)
    {
      id += descriptor->getId ();
    }

  i = _executionObjects.find (id);
  if (i != _executionObjects.end ())
    {
      selectedObject = i->second;
      switchObject->select (selectedObject);
      resolveSwitchEvents (switchObject);
      if (descriptor != NULL)
        {
          delete descriptor;
          descriptor = NULL;
        }

      delete selectedPerspective;

      return selectedObject;
    }

  selectedObject = createExecutionObject (id, selectedPerspective,
                                          descriptor);

  delete selectedPerspective;

  if (selectedObject == nullptr)
    {
      if (descriptor != nullptr)
        {
          delete descriptor;
          descriptor = nullptr;
        }

      WARNING ("Cannot process '%s' because select object is NULL.",
                switchObject->getId ().c_str ());
      return nullptr;
    }

  addExecutionObject (selectedObject, switchObject);
  switchObject->select (selectedObject);
  resolveSwitchEvents (switchObject);

  return selectedObject;
}

void
FormatterConverter::resolveSwitchEvents (
    NclExecutionObjectSwitch *switchObject)
{
  NclExecutionObject *selectedObject;
  NclExecutionObject *endPointObject;
  Node *selectedNode;
  NodeEntity *selectedNodeEntity;
  vector<NclFormatterEvent *> events;
  vector<NclFormatterEvent *>::iterator i;
  NclSwitchEvent *switchEvent;
  InterfacePoint *interfacePoint;
  SwitchPort *switchPort;
  vector<Node *> *nestedSeq;
  NclNodeNesting *nodePerspective;
  NclFormatterEvent *mappedEvent;

  selectedObject = switchObject->getSelectedObject ();
  if (selectedObject == nullptr)
    {
      WARNING ("Selected object is nullptr");
      return;
    }

  selectedNode = selectedObject->getDataObject ();
  selectedNodeEntity = (NodeEntity *)(selectedNode->getDataEntity ());

  if (events.empty ())
    {
      WARNING ("Can't find events.");
    }

  for (NclFormatterEvent *event: switchObject->getEvents ())
    {
      mappedEvent = nullptr;
      switchEvent = dynamic_cast<NclSwitchEvent *> (event);
      g_assert_nonnull (switchEvent);

      interfacePoint = switchEvent->getInterfacePoint ();
      if (interfacePoint->instanceOf ("LambdaAnchor"))
        {
          mappedEvent = getEvent (
                selectedObject, selectedNodeEntity->getLambdaAnchor (),
                switchEvent->getEventType (), switchEvent->getKey ());
        }
      else
        {
          switchPort = dynamic_cast<SwitchPort *> (interfacePoint);
          g_assert_nonnull (switchPort);

          for (Port *mapping: *(switchPort->getPorts ()))
            {
              if (mapping->getNode () == selectedNode
                  || mapping->getNode ()->getDataEntity ()
                  == selectedNode->getDataEntity ())
                {
                  nodePerspective
                      = switchObject->getNodePerspective ();

                  nestedSeq = mapping->getMapNodeNesting ();
                  nodePerspective->append (nestedSeq);

                  endPointObject
                      = getExecutionObjectFromPerspective (
                        nodePerspective, nullptr);

                  if (endPointObject != nullptr)
                    {
                      mappedEvent = getEvent (
                            endPointObject,
                            mapping->getEndInterfacePoint (),
                            switchEvent->getEventType (),
                            switchEvent->getKey ());
                    }

                  delete nestedSeq;
                  delete nodePerspective;

                  break;
                }
            }
        }

      if (mappedEvent != nullptr)
        {
          switchEvent->setMappedEvent (mappedEvent);
          TRACE ("Setting '%s' as mapped event of '%s'.",
                 mappedEvent->getId ().c_str(),
                 switchEvent->getId ().c_str());
        }
      else
        {
          WARNING ("Can't set a mapped event for '%s'.",
                   switchEvent->getId ().c_str());
        }
    }
}

NclFormatterEvent *
FormatterConverter::insertNode (NclNodeNesting *perspective,
                                InterfacePoint *interfacePoint,
                                GenericDescriptor *descriptor)
{
  NclExecutionObject *executionObject;
  NclFormatterEvent *event;
  short eventType;

  event = nullptr;
  executionObject = getExecutionObjectFromPerspective (perspective,
                                                       descriptor);

  if (executionObject != nullptr)
    {
      if (!interfacePoint->instanceOf ("PropertyAnchor"))
        {
          eventType = EventUtil::EVT_PRESENTATION;
        }
      else
        {
          eventType = EventUtil::EVT_ATTRIBUTION;
        }

      // get the event corresponding to the node anchor
      event = getEvent (executionObject, interfacePoint, eventType, "");
    }

  return event;

}

NclFormatterEvent *
FormatterConverter::insertContext (NclNodeNesting *contextPerspective,
                                   Port *port)
{
  vector<Node *> *nestedSeq;
  NclNodeNesting *perspective;
  NclFormatterEvent *newEvent;
  bool error = false;

  if (contextPerspective == nullptr || port == nullptr)
    {
      error = true;
    }

  if (!(port->getEndInterfacePoint ()->instanceOf ("ContentAnchor")
        || port->getEndInterfacePoint ()->instanceOf ("LabeledAnchor")
        || port->getEndInterfacePoint ()->instanceOf ("PropertyAnchor")
        || port->getEndInterfacePoint ()->instanceOf ("SwitchPort"))
      || !(contextPerspective->getAnchorNode ()
               ->getDataEntity ()
               ->instanceOf ("ContextNode")))
    {
      error = true;

    }

  if (error)
    {
      WARNING ("Can't find a valid interface point in '%s'.",
               contextPerspective->getId ().c_str());

      return nullptr;
    }
  else
    {
      nestedSeq = port->getMapNodeNesting ();
      perspective = new NclNodeNesting (contextPerspective);
      perspective->append (nestedSeq);
      delete nestedSeq;

      newEvent = insertNode (perspective, port->getEndInterfacePoint (), NULL);
      delete perspective;

      return newEvent;
    }
}

void
FormatterConverter::eventStateChanged (NclFormatterEvent *event,
                                       short transition,
                                       arg_unused (short previousState))
{
  NclExecutionObject *exeObj
      = (NclExecutionObject *)(event->getExecutionObject ());
  NclExecutionObjectSwitch *exeSwitch
      = dynamic_cast <NclExecutionObjectSwitch *> (exeObj);
  if (exeSwitch)
    {
      if (transition == EventUtil::TR_STARTS)
        {
          for (NclFormatterEvent *e: exeSwitch->getEvents())
            {
              if (e->instanceOf ("NclSwitchEvent"))
                {
                  NclFormatterEvent *ev
                      = ((NclSwitchEvent *)(e))->getMappedEvent ();

                  if (ev == nullptr)
                    {
                      // there is only one way to start a switch with
                      // NULL mapped event: a instSame refernode inside
                      // it was started
                      processExecutionObjectSwitch (exeSwitch);

                      ev = ((NclSwitchEvent *)(e))->getMappedEvent ();
                      if (ev != nullptr)
                        {
                          // now we know the event is mapped, we can start
                          // the
                          // switchport
                          e->start ();
                        }
                    }
                }
            }
        }

      if (transition == EventUtil::TR_STOPS
          || transition == EventUtil::TR_ABORTS)
        {
          exeSwitch->select (NULL);
        }
    }
  else if (exeObj->instanceOf ("NclCompositeExecutionObject"))
    {
      if (transition == EventUtil::TR_STOPS
          || transition == EventUtil::TR_ABORTS)
        {
          removeExecutionObject (exeObj);
        }
    }
}

bool
FormatterConverter::isEmbeddedApp (NodeEntity *dataObject)
{
  string mime = "";

  // second, media type
  ContentNode *contentNode = dynamic_cast <ContentNode *> (dataObject);
  if (contentNode)
    {
      mime = contentNode->getNodeType ();
    }

  // finally, content file extension
  Content *content = dataObject->getContent ();
  if (content)
    {
      ReferenceContent *referenceContent
          = dynamic_cast <ReferenceContent *> (content);
      if (referenceContent)
        {
          string url = referenceContent->getCompleteReferenceUrl ();

          if (url != "")
            {
              string::size_type pos = url.find_last_of (".");
              if (pos != std::string::npos)
                {
                  string mime = "";
                  string ext = url.substr (pos, url.length () - (pos + 1));
                  ginga_mime_table_index (ext, &mime);
                }
            }
        }
    }

  return isEmbeddedAppMediaType (mime);
}

bool
FormatterConverter::hasDescriptorPropName (const string &name)
{
  static const set <string> words = { "left", "top", "width", "height",
                                      "right", "bottom", "explicitDur",
                                      "size", "bounds", "location",
                                      "zIndex" };

  return words.count(name);
}

bool
FormatterConverter::isEmbeddedAppMediaType (const string &mediaType)
{
  static const set <string> appMediaTypes = { "APPLICATION/X-GINGA-NCLUA",
                                              "APPLICATION/X-GINGA-NCLET",
                                              "APPLICATION/X-GINGA-NCL",
                                              "APPLICATION/X-NCL-NCL",
                                              "APPLICATION/X-NCL-NCLUA" };

  return appMediaTypes.count (xstrup (mediaType));
}

NclFormatterCausalLink *
FormatterConverter::createCausalLink (
    CausalLink *ncmLink, NclCompositeExecutionObject *parentObject)
{
  CausalConnector *connector;
  ConditionExpression *conditionExpression;
  Action *actionExpression;
  NclLinkCondition *formatterCondition;
  NclLinkAction *formatterAction;
  NclFormatterCausalLink *formatterLink;
  vector<NclLinkAction *> *acts;
  vector<NclLinkAction *>::iterator i;
  NclLinkAssignmentAction *action;
  string value;
  NclFormatterEvent *event;
  Animation *anim;

  clog << "FormatterConverter::createCausalLink inside '";
  clog << parentObject->getId () << "' from ncmlinkId '";
  clog << ncmLink->getId () << "'" << endl;

  if (ncmLink == NULL)
    {
      clog << "FormatterConverter::createCausalLink Warning!";
      clog << " cannot create formatter link inside '";
      clog << parentObject->getId () << "' NCM link is NULL" << endl;
      return NULL;
    }

  if (!ncmLink->isConsistent ())
    {
      clog << "FormatterConverter::createCausalLink Warning!";
      clog << " cannot create formatter link inside '";
      clog << parentObject->getId () << "' from ncmlinkId '";
      clog << ncmLink->getId () << "': inconsistent link (check min and ";
      clog << "max attributes!)" << endl;
      return NULL;
    }

  // compile link condition and verify if it is a trigger condition
  connector = (CausalConnector *)(ncmLink->getConnector ());
  conditionExpression = connector->getConditionExpression ();
  formatterCondition
      = createCondition ((TriggerExpression *)conditionExpression, ncmLink,
                         parentObject);

  if (formatterCondition == NULL
      || !(formatterCondition->instanceOf ("NclLinkTriggerCondition")))
    {
      clog << "FormatterConverter::createCausalLink Warning!";
      clog << " cannot create formatter link inside '";
      clog << parentObject->getId () << "' from ncmlinkId '";
      clog << ncmLink->getId () << "' with a unknown condition (";
      clog << formatterCondition << ")" << endl;
      if (formatterCondition != NULL)
        {
          delete formatterCondition;
        }
      return NULL;
    }

  // compile link action
  actionExpression = connector->getAction ();
  formatterAction
      = createAction (actionExpression, ncmLink, parentObject);

  if (formatterAction == NULL)
    {
      clog << "FormatterConverter::createCausalLink Warning!";
      clog << " cannot create formatter link inside '";
      clog << parentObject->getId () << "' from ncmlinkId '";
      clog << ncmLink->getId () << "' with a NULL action (";
      clog << endl;
      return NULL;
    }

  // create formatter causal link
  formatterLink = new NclFormatterCausalLink (
      (NclLinkTriggerCondition *)formatterCondition, formatterAction,
      ncmLink, (NclCompositeExecutionObject *)parentObject);

  //&got
  if (formatterCondition->instanceOf ("NclLinkCompoundTriggerCondition"))
    {
      acts = formatterAction->getImplicitRefRoleActions ();
      if (acts != NULL)
        {
          i = acts->begin ();
          while (i != acts->end ())
            {
              action = (NclLinkAssignmentAction *)(*i);
              value = action->getValue ();
              if (value != "" && value.substr (0, 1) == "$")
                {
                  event = ((NclLinkRepeatAction *)(*i))->getEvent ();
                  setImplicitRefAssessment (
                      value.substr (1, value.length ()), ncmLink, event);
                }

              anim = action->getAnimation ();
              if (anim != NULL)
                {
                  value = anim->getDuration ();
                  if (value != "" && value.substr (0, 1) == "$")
                    {
                      event = ((NclLinkRepeatAction *)(*i))->getEvent ();
                      setImplicitRefAssessment (
                          value.substr (1, value.length ()), ncmLink,
                          event);
                    }

                  value = anim->getBy ();
                  if (value != "" && value.substr (0, 1) == "$")
                    {
                      event = ((NclLinkRepeatAction *)(*i))->getEvent ();
                      setImplicitRefAssessment (
                          value.substr (1, value.length ()), ncmLink,
                          event);
                    }
                }
              ++i;
            }

          delete acts;
        }
    }

  return formatterLink;
}

void
FormatterConverter::setImplicitRefAssessment (const string &roleId,
                                                  CausalLink *ncmLink,
                                                  NclFormatterEvent *event)
{
  NclFormatterEvent *refEvent;
  vector<Node *> *ncmPerspective;
  vector<Bind *> *binds;
  vector<Bind *>::iterator i;
  NclNodeNesting *refPerspective;
  NclExecutionObject *refObject;
  InterfacePoint *refInterface;
  string value;

  if (event->instanceOf ("NclAttributionEvent"))
    {
      binds = ncmLink->getBinds ();
      i = binds->begin ();
      while (i != binds->end ())
        {
          value = (*i)->getRole ()->getLabel ();
          if (roleId == value)
            {
              refInterface = (*i)->getInterfacePoint ();
              if (refInterface != NULL
                  && refInterface->instanceOf ("PropertyAnchor"))
                {
                  ncmPerspective = (*i)->getNode ()->getPerspective ();
                  refPerspective = new NclNodeNesting (ncmPerspective);

                  delete ncmPerspective;

                  /*clog << "FormatterConverter::";
                  clog << "createCausalLink '";
                  clog << " perspective = '";
                  clog << refPerspective->getId() << "'";
                  if ((*j)->getDescriptor() != NULL) {
                          clog << " descriptor = '";
                          clog << (*j)->getDescriptor()->getId();
                          clog << "'";
                  }
                  clog << endl;*/

                  refObject = this->getExecutionObjectFromPerspective (
                      refPerspective, (*i)->getDescriptor ());

                  delete refPerspective;

                  refEvent
                      = this->getEvent (refObject, refInterface,
                                            EventUtil::EVT_ATTRIBUTION, "");

                  ((NclAttributionEvent *)event)
                      ->setImplicitRefAssessmentEvent (roleId, refEvent);

                  break;
                }
            }
          ++i;
        }
    }
}

NclLinkAction *
FormatterConverter::createAction (
    Action *actionExpression, CausalLink *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  GingaTime delay;
  SimpleAction *sae;
  CompoundAction *cae;
  vector<Bind *> *binds;
  int i, size;
  string delayObject;
  NclLinkSimpleAction *simpleAction;
  NclLinkCompoundAction *compoundAction;

  if (actionExpression == NULL)
    {
      clog << "FormatterConverter::createAction ";
      clog << "Warning! ActionExpression is NULL" << endl;
      return NULL;
    }

  if (actionExpression->instanceOf ("SimpleAction"))
    {
      sae = (SimpleAction *)actionExpression;
      binds = ncmLink->getRoleBinds (sae);
      if (binds != NULL)
        {
          size = (int) binds->size ();
          if (size == 1)
            {
              return createSimpleAction (sae, (*binds)[0], ncmLink,
                                         parentObject);
            }
          else if (size > 1)
            {
              compoundAction
                  = new NclLinkCompoundAction (sae->getQualifier ());

              for (i = 0; i < size; i++)
                {
                  simpleAction = createSimpleAction (
                      sae, (*binds)[i], ncmLink, parentObject);

                  if (simpleAction == NULL)
                    {
                      clog << "FormatterConverter::createAction ";
                      clog << "Warning! cannot create compound action: ";
                      clog << "found invalid action(s)" << endl;
                      delete compoundAction;
                      return NULL;
                    }
                  compoundAction->addAction (simpleAction);
                }

              return compoundAction;
            }
          else
            {
              clog << "FormatterConverter::createAction ";
              clog << "Warning! cannot create action of link '";
              clog << ncmLink->getId () << "' because ";
              clog << "number of binds is = " << size << endl;
              return NULL;
            }
        }
    }
  else
    { // CompoundAction)
      delayObject = actionExpression->getDelay ();
      delay = compileDelay (ncmLink, delayObject, NULL);
      cae = (CompoundAction *)actionExpression;
      return createCompoundAction (cae->getOperator (), delay,
                                   cae->getActions (), ncmLink,
                                   parentObject);
    }

  clog << "FormatterConverter::createAction ";
  clog << "Warning! cannot create action of link '";
  clog << ncmLink->getId () << "' returning NULL" << endl;
  return NULL;
}

NclLinkCondition *
FormatterConverter::createCondition (
    ConditionExpression *ncmExpression, CausalLink *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  if (ncmExpression->instanceOf ("TriggerExpression"))
    {
      return createCondition ((TriggerExpression *)ncmExpression, ncmLink,
                              parentObject);
    }
  else
    { // IStatement
      return createStatement ((Statement *)ncmExpression, ncmLink,
                              parentObject);
    }
}

NclLinkCompoundTriggerCondition *
FormatterConverter::createCompoundTriggerCondition (
    short op, GingaTime delay,
    vector<ConditionExpression *> *ncmChildConditions, CausalLink *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  NclLinkCompoundTriggerCondition *condition;
  ConditionExpression *ncmChildCondition;
  NclLinkCondition *childCondition;

  if (op == CompoundCondition::OP_AND)
    {
      condition = new NclLinkAndCompoundTriggerCondition ();
    }
  else
    {
      condition = new NclLinkCompoundTriggerCondition ();
    }

  if (delay > 0)
    {
      condition->setDelay (delay);
    }

  if (ncmChildConditions != NULL)
    {
      vector<ConditionExpression *>::iterator i;
      i = ncmChildConditions->begin ();
      while (i != ncmChildConditions->end ())
        {
          ncmChildCondition = (*i);
          childCondition = createCondition (ncmChildCondition, ncmLink,
                                            parentObject);

          condition->addCondition (childCondition);
          ++i;
        }
    }

  return condition;
}

NclLinkCondition *
FormatterConverter::createCondition (
    TriggerExpression *condition, CausalLink *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  GingaTime delay;
  SimpleCondition *ste;
  CompoundCondition *cte;
  vector<Bind *> *binds;
  int i, size;
  string delayObject;
  NclLinkCompoundTriggerCondition *compoundCondition;
  NclLinkTriggerCondition *simpleCondition;

  if (condition->instanceOf ("SimpleCondition"))
    {
      ste = (SimpleCondition *)condition;
      binds = ncmLink->getRoleBinds (ste);
      if (binds != NULL)
        {
          size = (int) binds->size ();
          if (size == 1)
            {
              return createSimpleCondition (ste, (*binds)[0], ncmLink,
                                            parentObject);
            }
          else if (size > 1)
            {
              if (ste->getQualifier () == CompoundCondition::OP_AND)
                {
                  compoundCondition
                      = new NclLinkAndCompoundTriggerCondition ();
                }
              else
                {
                  compoundCondition
                      = new NclLinkCompoundTriggerCondition ();
                }

              for (i = 0; i < size; i++)
                {
                  simpleCondition = createSimpleCondition (
                      ste, (*binds)[i], ncmLink, parentObject);

                  compoundCondition->addCondition (simpleCondition);
                }
              return compoundCondition;
            }
          else
            {
              clog << "FormatterConverter::createCondition ";
              clog << "Warning! cannot create condition of link '";
              clog << ncmLink->getId () << "' because ";
              clog << "number of binds is = " << size << endl;
              return NULL;
            }
        }
    }
  else
    { // CompoundCondition
      delayObject = condition->getDelay ();
      delay = compileDelay (ncmLink, delayObject, NULL);
      cte = (CompoundCondition *)condition;
      return createCompoundTriggerCondition (cte->getOperator (), delay,
                                             cte->getConditions (), ncmLink,
                                             parentObject);
    }

  clog << "FormatterConverter::createCondition ";
  clog << "Warning! cannot create condition of link '";
  clog << ncmLink->getId () << "' returning NULL" << endl;
  return NULL;
}

NclLinkAssessmentStatement *
FormatterConverter::createAssessmentStatement (
    AssessmentStatement *assessmentStatement, Bind *bind, Link *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  NclLinkAttributeAssessment *mainAssessment;
  NclLinkAssessment *otherAssessment;
  AttributeAssessment *aa;
  NclLinkAssessmentStatement *statement;
  ValueAssessment *valueAssessment;
  string paramValue;
  Parameter *connParam, *param;
  vector<Bind *> *otherBinds;

  mainAssessment = createAttributeAssessment (
      assessmentStatement->getMainAssessment (), bind, ncmLink,
      parentObject);

  if (assessmentStatement->getOtherAssessment ()->instanceOf (
          "ValueAssessment"))
    {
      valueAssessment
          = (ValueAssessment *)(assessmentStatement->getOtherAssessment ());

      paramValue = valueAssessment->getValue ();
      if (paramValue[0] == '$')
        { // instanceOf("Parameter")
          connParam = new Parameter (
              paramValue.substr (1, paramValue.length () - 1), "");

          param = bind->getParameter (connParam->getName ());
          if (param == NULL)
            {
              param = ncmLink->getParameter (connParam->getName ());
            }

          if (param != NULL)
            {
              paramValue = param->getValue ();
            }
        }

      otherAssessment = new NclLinkValueAssessment (paramValue);
    }
  else
    {
      aa = (AttributeAssessment *)(assessmentStatement
                                       ->getOtherAssessment ());

      otherBinds = ncmLink->getRoleBinds (aa);
      if (otherBinds != NULL && !otherBinds->empty ())
        {
          otherAssessment = createAttributeAssessment (
              aa, (*otherBinds)[0], ncmLink, parentObject);
        }
      else
        {
          otherAssessment = createAttributeAssessment (
              aa, NULL, ncmLink, parentObject);
        }
    }
  statement = new NclLinkAssessmentStatement (
      assessmentStatement->getComparator (), mainAssessment,
      otherAssessment);

  return statement;
}

NclLinkStatement *
FormatterConverter::createStatement (
    Statement *statementExpression, Link *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  AssessmentStatement *as;
  CompoundStatement *cs;
  vector<Bind *> *binds;
  int size;
  NclLinkStatement *statement;
  NclLinkStatement *childStatement;
  vector<Statement *> *statements;
  vector<Statement *>::iterator i;
  Statement *ncmChildStatement;

  if (statementExpression->instanceOf ("AssessmentStatement"))
    {
      as = (AssessmentStatement *)statementExpression;
      binds = ncmLink->getRoleBinds (as->getMainAssessment ());
      if (binds != NULL)
        {
          size = (int) binds->size ();
          if (size == 1)
            {
              statement = createAssessmentStatement (
                  as, (*binds)[0], ncmLink, parentObject);
            }
          else
            {
              clog << "FormatterConverter::createStatement ";
              clog << "Warning! cannot create statement of link '";
              clog << ncmLink->getId () << "' because ";
              clog << "binds size = '" << size << "'" << endl;
              return NULL;
            }
        }
      else
        {
          clog << "FormatterConverter::createStatement ";
          clog << "Warning! cannot create statement of link '";
          clog << ncmLink->getId () << "' because ";
          clog << "binds == NULL" << endl;
          return NULL;
        }
    }
  else
    { // CompoundStatement
      cs = (CompoundStatement *)statementExpression;
      statement = new NclLinkCompoundStatement (cs->getOperator ());
      ((NclLinkCompoundStatement *)statement)
          ->setNegated (cs->isNegated ());
      statements = cs->getStatements ();
      if (statements != NULL)
        {
          i = statements->begin ();
          while (i != statements->end ())
            {
              ncmChildStatement = (*i);
              childStatement = createStatement (ncmChildStatement, ncmLink,
                                                parentObject);

              ((NclLinkCompoundStatement *)statement)
                  ->addStatement (childStatement);

              ++i;
            }
        }
    }

  return statement;
}

NclLinkAttributeAssessment *
FormatterConverter::createAttributeAssessment (
    AttributeAssessment *attributeAssessment, Bind *bind, Link *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  NclFormatterEvent *event;

  event = createEvent (bind, ncmLink, parentObject);
  return new NclLinkAttributeAssessment (
      event, attributeAssessment->getAttributeType ());
}

NclLinkSimpleAction *
FormatterConverter::createSimpleAction (
    SimpleAction *sae, Bind *bind, Link *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  NclFormatterEvent *event;
  SimpleActionType actionType;
  short eventType = -1;
  NclLinkSimpleAction *action;
  Parameter *connParam;
  Parameter *param;
  string paramValue;
  Animation *animation;
  int repeat;
  GingaTime delay;
  Animation *newAnimation;
  bool isUsing;

  newAnimation = new Animation ();
  isUsing = false;
  action = NULL;
  event = createEvent (bind, ncmLink, parentObject);

  actionType = sae->getActionType ();
  if (event != NULL)
    {
      eventType = bind->getRole ()->getEventType ();
      event->setEventType (eventType);
    }
  else
    {
      clog << "FormatterConverter::createSimpleAction Warning! ";
      clog << "Trying to create a simple action with a NULL event";
      clog << endl;
    }

  switch (actionType)
    {
    case ACT_START:
    case ACT_SET:
      if (eventType == EventUtil::EVT_PRESENTATION)
        {
          action = new NclLinkRepeatAction (event, actionType);

          // repeat
          paramValue = sae->getRepeat ();
          if (paramValue == "")
            {
              repeat = 0;
            }
          else if (paramValue[0] == '$')
            {
              connParam = new Parameter (
                  paramValue.substr (1, paramValue.length () - 1), "");

              param = bind->getParameter (connParam->getName ());
              if (param == NULL)
                {
                  param = ncmLink->getParameter (connParam->getName ());
                }

              if (param == NULL)
                {
                  repeat = 0;
                }
              else
                {
                  repeat = xstrtoint (param->getValue (), 10);
                }
            }
          else
            {
              repeat = xstrtoint (paramValue, 10);
            }

          ((NclLinkRepeatAction *)action)->setRepetitions (repeat);

          // repeatDelay
          paramValue = sae->getRepeatDelay ();
          delay = compileDelay (ncmLink, paramValue, bind);
          ((NclLinkRepeatAction *)action)->setRepetitionInterval (delay);
        }
      else if (eventType == EventUtil::EVT_ATTRIBUTION)
        {
          paramValue = sae->getValue ();
          if (paramValue != "" && paramValue[0] == '$')
            {
              connParam = new Parameter (
                  paramValue.substr (1, paramValue.length () - 1), "");

              param = bind->getParameter (connParam->getName ());
              if (param == NULL)
                {
                  param = ncmLink->getParameter (connParam->getName ());
                }

              delete connParam;
              connParam = NULL;

              if (param != NULL)
                {
                  paramValue = param->getValue ();
                }
              else
                {
                  paramValue = "";
                }
            }

          action
              = new NclLinkAssignmentAction (event, actionType, paramValue);

          // animation
          animation = sae->getAnimation ();

          if (animation != NULL)
            {
              string durVal = "0";
              string byVal = "0";

              paramValue = animation->getDuration ();
              if (paramValue[0] == '$')
                {
                  connParam = new Parameter (
                      paramValue.substr (1, paramValue.length () - 1), "");

                  param = bind->getParameter (connParam->getName ());
                  if (param == NULL)
                    {
                      param = ncmLink->getParameter (connParam->getName ());
                    }

                  delete connParam;
                  connParam = NULL;

                  if (param != NULL)
                    {
                      durVal = param->getValue ();
                    }

                  newAnimation->setDuration (durVal);
                }
              else
                {
                  durVal = paramValue;
                  newAnimation->setDuration (durVal);
                }

              paramValue = animation->getBy ();
              if (paramValue[0] == '$')
                {
                  connParam = new Parameter (
                      paramValue.substr (1, paramValue.length () - 1), "");

                  param = bind->getParameter (connParam->getName ());
                  if (param == NULL)
                    {
                      param = ncmLink->getParameter (connParam->getName ());
                    }

                  delete connParam;
                  connParam = NULL;

                  if (param != NULL)
                    {
                      byVal = param->getValue ();
                    }

                  newAnimation->setBy (byVal);
                }
              else
                {
                  byVal = paramValue;
                  newAnimation->setBy (byVal);
                }

              if (durVal != "0")
                {
                  isUsing = true;
                  ((NclLinkAssignmentAction *)action)
                      ->setAnimation (newAnimation);
                }
            }
        }
      else
        {
          clog << "FormatterConverter::createSimpleAction ";
          clog << "Warning! Unknown event type '" << eventType;
          clog << "' for action type '";
          clog << actionType << "'" << endl;
        }
      break;

    case ACT_STOP:
    case ACT_PAUSE:
    case ACT_RESUME:
    case ACT_ABORT:
      action = new NclLinkSimpleAction (event, actionType);
      break;

    default:
      action = NULL;
      clog << "FormatterConverter::createSimpleAction ";
      clog << "Warning! Unknown action type '";
      clog << actionType << "'" << endl;
      break;
    }

  if (action != NULL)
    {
      paramValue = sae->getDelay ();
      delay = compileDelay (ncmLink, paramValue, bind);
      action->setWaitDelay (delay);
    }

  if (!isUsing)
    {
      delete newAnimation;
    }

  return action;
}

NclLinkCompoundAction *
FormatterConverter::createCompoundAction (
    short op, GingaTime delay, vector<Action *> *ncmChildActions,
    CausalLink *ncmLink, NclCompositeExecutionObject *parentObject)
{
  NclLinkCompoundAction *action;
  Action *ncmChildAction;
  NclLinkAction *childAction;

  action = new NclLinkCompoundAction (op);
  if (delay > 0)
    {
      action->setWaitDelay (delay);
    }

  if (ncmChildActions != NULL)
    {
      vector<Action *>::iterator i;
      i = ncmChildActions->begin ();
      while (i != ncmChildActions->end ())
        {
          ncmChildAction = (*i);
          childAction = createAction (ncmChildAction, ncmLink, parentObject);

          if (childAction != NULL)
            {
              action->addAction (childAction);
            }
          else
            {
              clog << "FormatterConverter::createCompoundAction ";
              clog << "creating link '" << ncmLink->getId () << "' ";
              clog << "Warning! Can't create ";
              if (ncmChildAction->instanceOf ("SimpleAction"))
                {
                  clog << "simple action type '";
                  clog
                      << ((SimpleAction *)ncmChildAction)->getActionType ();
                  clog << "'" << endl;
                }
              else if (ncmChildAction->instanceOf ("CompoundAction"))
                {
                  clog << "inner compound action " << endl;
                }
            }
          ++i;
        }
    }

  return action;
}

NclLinkTriggerCondition *
FormatterConverter::createSimpleCondition (
    SimpleCondition *simpleCondition, Bind *bind, Link *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  NclFormatterEvent *event;
  GingaTime delay;
  string delayObject;
  NclLinkTriggerCondition *condition;

  event = createEvent (bind, ncmLink, parentObject);
  condition = new NclLinkTransitionTriggerCondition (
      event, simpleCondition->getTransition (), bind);

  delayObject = simpleCondition->getDelay ();
  delay = compileDelay (ncmLink, delayObject, bind);
  if (delay > 0)
    {
      condition->setDelay (delay);
    }
  return condition;
}

NclFormatterEvent *
FormatterConverter::createEvent (
    Bind *bind, Link *ncmLink, NclCompositeExecutionObject *parentObject)
{
  NclNodeNesting *endPointNodeSequence;
  NclNodeNesting *endPointPerspective;
  Node *parentNode;
  NclExecutionObject *executionObject;
  InterfacePoint *interfacePoint;
  string key;
  NclFormatterEvent *event = NULL;
  vector<Node *> *seq;

  endPointPerspective = parentObject->getNodePerspective ();

  // parent object may be a refer
  parentNode = endPointPerspective->getAnchorNode ();

  // teste para verificar se ponto terminal eh o proprio contexto ou
  // refer para o proprio contexto
  seq = bind->getNodeNesting ();
  endPointNodeSequence = new NclNodeNesting (seq);
  if (endPointNodeSequence->getAnchorNode ()
          != endPointPerspective->getAnchorNode ()
      && endPointNodeSequence->getAnchorNode ()
             != parentNode->getDataEntity ())
    {
      endPointPerspective->append (endPointNodeSequence);
    }

  delete seq;
  delete endPointNodeSequence;

  try
    {
      executionObject = getExecutionObjectFromPerspective (
          endPointPerspective, bind->getDescriptor ());

      if (executionObject == NULL)
        {
          clog << "FormatterConverter::createEvent Warning! ";
          clog << "can't find execution object for perspective '";
          clog << endPointPerspective->getId () << "'" << endl;

          delete endPointPerspective;
          return NULL;
        }
    }
  catch (exception *exc)
    {
      clog << "FormatterConverter::createEvent Warning! ";
      clog << "can't execution object exception for perspective '";
      clog << endPointPerspective->getId () << "'" << endl;

      delete endPointPerspective;
      return NULL;
    }

  interfacePoint = bind->getEndPointInterface ();
  if (interfacePoint == NULL)
    {
      // TODO: This is an error, the formatter then return the main event
      clog << "FormatterConverter::createEvent Warning! ";
      clog << "can't find an interface point for '";
      clog << endPointPerspective->getId () << "' bind '";
      clog << bind->getRole ()->getLabel () << "'" << endl;
      delete endPointPerspective;
      return executionObject->getWholeContentPresentationEvent ();
    }

  key = getBindKey (ncmLink, bind);
  event = getEvent (executionObject, interfacePoint,
                    bind->getRole ()->getEventType (), key);

  delete endPointPerspective;
  return event;
}

GingaTime
FormatterConverter::getDelayParameter (Link *ncmLink,
                                           Parameter *connParam,
                                           Bind *ncmBind)
{
  Parameter *parameter;
  string param;

  parameter = NULL;
  if (ncmBind != NULL)
    {
      parameter = ncmBind->getParameter (connParam->getName ());
    }

  if (parameter == NULL)
    {
      parameter = ncmLink->getParameter (connParam->getName ());
    }

  if (parameter == NULL)
    {
      return 0;
    }
  else
    {
      param = parameter->getValue ();
      if (param == "")
        {
          return 0;
        }
      else
        {
          return (GingaTime)(xstrtod (param) * GINGA_NSECOND);
        }
    }
}

string
FormatterConverter::getBindKey (Link *ncmLink, Bind *ncmBind)
{
  Role *role;
  string keyValue;
  Parameter *param, *auxParam;
  string key;

  role = ncmBind->getRole ();
  if (role == NULL)
    {
      return "";
    }

  if (role->instanceOf ("SimpleCondition"))
    {
      keyValue = ((SimpleCondition *)role)->getKey ();
    }
  else if (role->instanceOf ("AttributeAssessment"))
    {
      keyValue = ((AttributeAssessment *)role)->getKey ();
    }
  else
    {
      return "";
    }

  if (keyValue == "")
    {
      key = "";
    }
  else if (keyValue[0] == '$')
    { // instanceof Parameter
      param
          = new Parameter (keyValue.substr (1, keyValue.length () - 1), "");

      auxParam = ncmBind->getParameter (param->getName ());
      if (auxParam == NULL)
        {
          auxParam = ncmLink->getParameter (param->getName ());
        }

      if (auxParam != NULL)
        {
          key = auxParam->getValue ();
        }
      else
        {
          key = "";
        }

      delete param;
    }
  else
    {
      key = keyValue;
    }

  return key;
}

GingaTime
FormatterConverter::compileDelay (Link *ncmLink,
                                      const string &delayObject,
                                      Bind *bind)
{
  GingaTime delay;
  string::size_type pos;
  Parameter *param;
  string delayValue;

  if (delayObject == "")
    {
      delay = 0;
    }
  else
    {
      pos = delayObject.find ("$");
      if (pos != std::string::npos && pos == 0)
        { // instanceof Parameter
          delayValue = delayObject.substr (1, delayObject.length () - 1);
          param = new Parameter (delayValue, "");
          delay = getDelayParameter (ncmLink, param, bind);
          delete param;
        }
      else
        {
          delay = (GingaTime)(xstrtod (delayObject) * GINGA_NSECOND);
        }
    }

  return delay;
}

GINGA_FORMATTER_END
