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
#include "FormatterConverter.h"
#include "FormatterLinkConverter.h"

GINGA_FORMATTER_BEGIN

int FormatterConverter::_dummyCount = 0;

static const string SEPARATOR = "/";

FormatterConverter::FormatterConverter (RuleAdapter *ruleAdapter)
{
  _linkCompiler = new FormatterLinkConverter (this);

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

      if (!ntsRemoveExecutionObject (object))
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

  if (_linkCompiler != nullptr)
    {
      delete _linkCompiler;
    }
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
  NodeEntity *dataObject;
  NclExecutionObject *expectedObject;

  for (auto &it: _executionObjects)
    {
      expectedObject = it.second;
      dataObject = (NodeEntity *)(expectedObject->getDataObject ()
                                      ->getDataEntity ());

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
  NclNodeNesting *referPerspective;
  NclCompositeExecutionObject *referParentObject;
  NclCascadingDescriptor *desc;
  string objectId;
  vector<Node *> *ncmPerspective;

  ncmPerspective = referNode->getPerspective ();
  referPerspective = new NclNodeNesting (ncmPerspective);

  delete ncmPerspective;

  referParentObject = getParentExecutionObject (referPerspective);

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
      desc = executionObject->getDescriptor ();
      if (desc != nullptr)
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

  delete referPerspective;

  return referParentObject;
}

void
FormatterConverter::addExecutionObject (
    NclExecutionObject *exeObj,
    NclCompositeExecutionObject *parentObj)
{
  _executionObjects[exeObj->getId ()] = exeObj;

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
          if (entityContentNode
              && entityContentNode->isSettingNode ())
            {
              _settingObjects.insert (exeObj);
            }
        }
    }

  if (parentObj != nullptr)
    {
      parentObj->addExecutionObject (exeObj);
    }

  NclNodeNesting *nodePerspective = exeObj->getNodePerspective ();
  Node *headNode = nodePerspective->getHeadNode ();
  delete nodePerspective;

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

  NclCascadingDescriptor *descriptor = exeObj->getDescriptor ();
  if (descriptor != nullptr)
    descriptor->setFormatterLayout ();

  compileExecutionObjectLinks (exeObj);
}

void
FormatterConverter::compileExecutionObjectLinks (
    NclExecutionObject *exeObj)
{
  vector<Node *> *nodes = exeObj->getNodes ();
  if (nodes != nullptr)
    {
      for (Node *node : *nodes)
        {
          NclCompositeExecutionObject *parent
              = dynamic_cast <NclCompositeExecutionObject*> (exeObj->getParentObject (node));

          g_assert_nonnull (parent);

          compileExecutionObjectLinks (exeObj, node, parent);
        }
      delete nodes;
    }
}

NclExecutionObject *
FormatterConverter::getExecutionObjectFromPerspective (
    NclNodeNesting *perspective, GenericDescriptor *descriptor)
{

  map<string, NclExecutionObject *>::iterator i;
  NclCascadingDescriptor *cascadingDescriptor = nullptr;
  string id;
  NclCompositeExecutionObject *parentObj;
  NclExecutionObject *exeObj;

  id = perspective->getId () + SEPARATOR;
  cascadingDescriptor = getCascadingDescriptor (perspective, descriptor);
  if (cascadingDescriptor != nullptr)
    {
      id = id + cascadingDescriptor->getId ();
    }

  i = _executionObjects.find (id);
  if (i != _executionObjects.end ())
    {
      if (cascadingDescriptor != nullptr)
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

      WARNING ("object id='%s', perspective='%s'", id.c_str (),
             perspective->getId ().c_str ());

      if (descriptor)
        WARNING (" descriptor='%s'", descriptor->getId ().c_str ());

      WARNING ("was not created.");
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
FormatterConverter::getParentExecutionObject (
    NclNodeNesting *perspective)
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
                  if (interfacePoint->instanceOf ("PropertyAnchor"))
                    {
                      event = new NclAttributionEvent (
                          id, exeObj,
                          (PropertyAnchor *)interfacePoint,
                          _ruleAdapter->getSettings ());
                    }
                  else
                    {
                      WARNING ("NCM event type is attribution,"
                               "but interface point isn't");

                      if (interfacePoint->instanceOf ("IntervalAnchor"))
                        {
                          WARNING ("it was supposed to be a"
                                   "presentation event");

                          // TODO: find the correct way to solve this
                          event = new NclPresentationEvent (
                              id, exeObj,
                              (ContentAnchor *)interfacePoint);
                        }

                      return nullptr;
                    }
                  break;

                case EventUtil::EVT_SELECTION:
                  event = new NclSelectionEvent (
                      id, exeObj, (ContentAnchor *)interfacePoint);

                  if (key != "")
                    {
                      ((NclSelectionEvent *)event)->setSelectionCode (key);
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
      WARNING ("returning a nullptr event for '%s'", id.c_str ());
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
                  try
                    {
                      exeObj = getExecutionObjectFromPerspective (
                          nodePerspective, nullptr);
                    }
                  catch (exception *exc1)
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

bool
FormatterConverter::hasDescriptorPropName (const string &name)
{
  set <string> words = {"left", "top", "width", "height", "right", "bottom",
                        "explicitDur", "size", "bounds", "location",
                        "zIndex"};

  return words.count(name);
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
  NodeEntity *nodeEntity;
  Descriptor *ncmDesc = nullptr;
  string name;

  vector<PropertyAnchor *> *anchors = node->getOriginalPropertyAnchors ();
  if (anchors != nullptr)
    {
      for (PropertyAnchor *property: *anchors)
        {
          name = property->getPropertyName ();

          // TODO: create a set with reserved words
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
    }

  if (node->instanceOf ("ReferNode")
      && ((ReferNode *)node)->getInstanceType () == "new"
      && ((ReferNode *)node)->getInstanceDescriptor () == nullptr)
    {
      nodeEntity = (NodeEntity *)node->getDataEntity ();
      ncmDesc = (Descriptor *)nodeEntity->getDescriptor ();
      ((ReferNode *)node)->setInstanceDescriptor (ncmDesc);
      return new NclCascadingDescriptor (ncmDesc);
    }

  return nullptr;
}

NclCascadingDescriptor *
FormatterConverter::checkCascadingDescriptor (Node *node)
{
  NodeEntity *nodeEntity;

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
      nodeEntity = (NodeEntity *)node->getDataEntity ();
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

  // there is a node descriptor defined in the node context?
  size = nodePerspective->getNumNodes ();
  if (size > 1 && nodePerspective->getNode (size - 2) != nullptr
      && nodePerspective->getNode (size - 2)->instanceOf ("ContextNode"))
    {
      context = (ContextNode *)(nodePerspective->getNode (size - 2)
                                    ->getDataEntity ());

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
  if (anchorNode->instanceOf ("ReferNode")
      && ((ReferNode *)anchorNode)->getInstanceType () == "new")
    {
      node = anchorNode;
      ncmDesc = (Descriptor *)((ReferNode *)node)->getInstanceDescriptor ();
    }
  else
    {
      node = (NodeEntity *)(anchorNode->getDataEntity ());

      if (node == nullptr || !node->instanceOf ("NodeEntity"))
        {
          WARNING ("failed to cascading descriptor: invalid node entity");
          return nullptr;
        }

      ncmDesc = (Descriptor *)((NodeEntity *)node)->getDescriptor ();
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

  if (executionObject->getDataObject () != nullptr
      && executionObject->getDataObject ()->instanceOf ("NodeEntity"))
    {
      nodeEntity = (NodeEntity *)(executionObject->getDataObject ());
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

          if (descriptors != NULL && !(descriptors->empty ()))
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
                          = causalLink->containsSourceNode (referNode, descriptor);

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
                  = _linkCompiler->createCausalLink (causalLink, parentObject);

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
    NclExecutionObject *executionObject, Node *dataObject,
    NclCompositeExecutionObject *parentObject)
{
  set<Link *> *uncompiledLinks;
  set<Link *>::iterator i;


  NclCompositeExecutionObject *compObj;
  Node *execDataObject;

  executionObject->setCompiled (true);

  if (parentObject == NULL)
    {
      return;
    }

  execDataObject = executionObject->getDataObject ();
  if (execDataObject != dataObject)
    {
      compObj = parentObject->getParentFromDataObject (execDataObject);
      if (compObj != NULL && compObj != parentObject)
        {
          compileExecutionObjectLinks (executionObject, execDataObject,
                                       compObj);
        }
    }

  uncompiledLinks = parentObject->getUncompiledLinks ();
  if (!uncompiledLinks->empty ())
    {
      set<Link *> *dataLinks = uncompiledLinks;

      for ( Link *ncmLink : *dataLinks)
        {
          processLink (ncmLink, dataObject, executionObject, parentObject);
        }

      delete dataLinks;

      compileExecutionObjectLinks (
          executionObject, dataObject,
          (NclCompositeExecutionObject *)(parentObject->getParentObject ()));
    }
  else
    {
      NclExecutionObject *object;

      delete uncompiledLinks;

      while (parentObject != NULL)
        {
          object = parentObject;
          parentObject
              = (NclCompositeExecutionObject *)(parentObject
                                                    ->getParentObject ());

          compileExecutionObjectLinks (object, dataObject, parentObject);
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
      = (SwitchNode *)(switchObject->getDataObject ()->getDataEntity ());

  selectedNode = _ruleAdapter->adaptSwitch (switchNode);
  if (selectedNode == NULL)
    {
      clog << "FormatterConverter::processExecutionObjectSwitch ";
      clog << "Warning! can't process '";
      clog << switchObject->getId () << "' selected NODE is NULL";
      clog << endl;
      return NULL;
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

  if (selectedObject == NULL)
    {
      if (descriptor != NULL)
        {
          delete descriptor;
          descriptor = NULL;
        }

      WARNING ("cannot process '%s' because selected object is NULL",
               switchObject->getId ().c_str ());

      return NULL;
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
  vector<Port *> *mappings;
  vector<Port *>::iterator j;
  Port *mapping;
  NclNodeNesting *nodePerspective;
  NclFormatterEvent *mappedEvent;

  selectedObject = switchObject->getSelectedObject ();
  if (selectedObject == nullptr)
    {
      WARNING ("selected object is nullptr");
      return;
    }

  selectedNode = selectedObject->getDataObject ();
  selectedNodeEntity = (NodeEntity *)(selectedNode->getDataEntity ());
  events = switchObject->getEvents ();
  if (!events.empty ())
    {
      i = events.begin ();
      while (i != events.end ())
        {
          mappedEvent = NULL;
          switchEvent = (NclSwitchEvent *)(*i);
          interfacePoint = switchEvent->getInterfacePoint ();
          if (interfacePoint->instanceOf ("LambdaAnchor"))
            {
              mappedEvent = getEvent (
                  selectedObject, selectedNodeEntity->getLambdaAnchor (),
                  switchEvent->getEventType (), switchEvent->getKey ());
            }
          else
            {
              switchPort = (SwitchPort *)interfacePoint;
              mappings = switchPort->getPorts ();
              if (mappings != NULL && !mappings->empty ())
                {
                  j = mappings->begin ();
                  while (j != mappings->end ())
                    {
                      mapping = (*j);
                      if (mapping->getNode () == selectedNode
                          || mapping->getNode ()->getDataEntity ()
                                 == selectedNode->getDataEntity ())
                        {
                          nodePerspective
                              = switchObject->getNodePerspective ();

                          nestedSeq = mapping->getMapNodeNesting ();
                          nodePerspective->append (nestedSeq);
                          delete nestedSeq;

                          try
                            {
                              endPointObject
                                  = getExecutionObjectFromPerspective (
                                      nodePerspective, NULL);

                              if (endPointObject != NULL)
                                {
                                  mappedEvent = getEvent (
                                      endPointObject,
                                      mapping->getEndInterfacePoint (),
                                      switchEvent->getEventType (),
                                      switchEvent->getKey ());
                                }
                            }
                          catch (exception *exc)
                            {
                              clog << "FormatterConverter::";
                              clog << "resolveSwitchEvents exception ";
                              clog << "for '" << nodePerspective->getId ();
                              clog << "'" << endl;
                            }

                          delete nodePerspective;
                          break;
                        }
                      ++j;
                    }
                }
              else
                {
                  clog << "FormatterConverter::resolveSwitchEvents ";
                  clog << "there is no mapped events" << endl;
                }
            }

          if (mappedEvent != NULL)
            {
              switchEvent->setMappedEvent (mappedEvent);
              clog << "FormatterConverter::resolveSwitchEvents setting '";
              clog << mappedEvent->getId () << "' as mapped event of '";
              clog << switchEvent->getId () << "'" << endl;
            }
          else
            {
              clog << "FormatterConverter::resolveSwitchEvents ";
              clog << "Warning! Can't set a mapped event for '";
              clog << switchEvent->getId () << "'" << endl;
            }

          ++i;
        }
    }
  else
    {
      WARNING ("can't find events");
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

  event = NULL;
  try
    {
      executionObject = getExecutionObjectFromPerspective (
          perspective, descriptor);

      if (executionObject != NULL)
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
  catch (exception *exc)
    {
      clog << "FormatterConverter::insertNode exception" << endl;
    }

  return NULL;
}

NclFormatterEvent *
FormatterConverter::insertContext (NclNodeNesting *contextPerspective,
                                   Port *port)
{
  vector<Node *> *nestedSeq;
  NclNodeNesting *perspective;
  NclFormatterEvent *newEvent;

  if (contextPerspective == NULL || port == NULL
      || !(port->getEndInterfacePoint ()->instanceOf ("ContentAnchor")
           || port->getEndInterfacePoint ()->instanceOf ("LabeledAnchor")
           || port->getEndInterfacePoint ()->instanceOf ("PropertyAnchor")
           || port->getEndInterfacePoint ()->instanceOf ("SwitchPort"))
      || !(contextPerspective->getAnchorNode ()
               ->getDataEntity ()
               ->instanceOf ("ContextNode")))
    {
      WARNING ("can't find a valid interface point in '%s'.",
               contextPerspective->getId ().c_str());

      return nullptr;
    }

  nestedSeq = port->getMapNodeNesting ();
  perspective = new NclNodeNesting (contextPerspective);
  perspective->append (nestedSeq);
  delete nestedSeq;

  newEvent = insertNode (perspective, port->getEndInterfacePoint (), NULL);
  delete perspective;

  return newEvent;
}

bool
FormatterConverter::removeExecutionObject (NclExecutionObject *exeObj)
{
  map<string, NclExecutionObject *>::iterator i;
  bool removed;

  removed = ntsRemoveExecutionObject (exeObj);
  return removed;
}

bool
FormatterConverter::ntsRemoveExecutionObject (NclExecutionObject *exeObj)
{
  map<string, NclExecutionObject *>::iterator i;
  bool removed = false;

  if (!NclExecutionObject::hasInstance (exeObj, false))
    {
      return removed;
    }

  i = _executionObjects.find (exeObj->getId ());
  if (i != _executionObjects.end ())
    {
      _executionObjects.erase (i);
      removed = true;
    }

  if (_settingObjects.count (exeObj) != 0)
    {
      _settingObjects.erase (_settingObjects.find (exeObj));
      removed = true;
    }

  if (removed && NclExecutionObject::hasInstance (exeObj, true))
    {
      delete exeObj;
    }

  return removed;
}

void
FormatterConverter::eventStateChanged (NclFormatterEvent *event,
                                       short transition,
                                       arg_unused (short previousState))
{
  NclExecutionObject *executionObject;
  vector<NclFormatterEvent *> evs;
  vector<NclFormatterEvent *>::iterator i;
  NclFormatterEvent *ev;

  executionObject = (NclExecutionObject *)(event->getExecutionObject ());

  if (executionObject->instanceOf ("NclExecutionObjectSwitch"))
    {
      if (transition == EventUtil::TR_STARTS)
        {
          evs = ((NclExecutionObjectSwitch *)executionObject)->getEvents ();

          i = evs.begin ();
          while (i != evs.end ())
            {
              if ((*i)->instanceOf ("NclSwitchEvent"))
                {
                  ev = ((NclSwitchEvent *)(*i))->getMappedEvent ();
                  if (ev == NULL)
                    {
                      // there is only one way to start a switch with
                      // NULL mapped event: a instSame refernode inside
                      // it was started
                      processExecutionObjectSwitch (
                          (NclExecutionObjectSwitch *)executionObject);
                      ev = ((NclSwitchEvent *)(*i))->getMappedEvent ();
                      if (ev != NULL)
                        {
                          // now we know the event is mapped, we can start
                          // the
                          // switchport
                          (*i)->start ();
                        }
                    }
                }
              ++i;
            }
        }

      if (transition == EventUtil::TR_STOPS
          || transition == EventUtil::TR_ABORTS)
        {
          ((NclExecutionObjectSwitch *)executionObject)->select (NULL);
        }
    }
  else if (executionObject->instanceOf ("NclCompositeExecutionObject"))
    {
      if (transition == EventUtil::TR_STOPS
          || transition == EventUtil::TR_ABORTS)
        {
          removeExecutionObject (executionObject);
        }
    }
}

bool
FormatterConverter::isEmbeddedApp (NodeEntity *dataObject)
{
  string mediaType = "";
  string url = "";
  string::size_type pos;
  Content *content;

  // second, media type
  if (dataObject->instanceOf ("ContentNode"))
    {
      mediaType = ((ContentNode *)dataObject)->getNodeType ();
      if (mediaType != "")
        {
          return isEmbeddedAppMediaType (mediaType);
        }
    }

  // finally, content file extension
  content = dataObject->getContent ();
  if (content != NULL)
    {
      if (content->instanceOf ("ReferenceContent"))
        {
          url = ((ReferenceContent *)(content))->getCompleteReferenceUrl ();

          if (url != "")
            {
              pos = url.find_last_of (".");
              if (pos != std::string::npos)
                {
                  string mime = "";
                  string ext = url.substr (pos, url.length () - (pos + 1));
                  ginga_mime_table_index (ext, &mime);
                  return isEmbeddedAppMediaType (mime);
                }
            }
        }
    }

  return false;
}

bool
FormatterConverter::isEmbeddedAppMediaType (const string &mediaType)
{
  string upMediaType = xstrup (mediaType);

  if (upMediaType == "APPLICATION/X-GINGA-NCLUA"
      || upMediaType == "APPLICATION/X-GINGA-NCLET"
      || upMediaType == "APPLICATION/X-GINGA-NCL"
      || upMediaType == "APPLICATION/X-NCL-NCL"
      || upMediaType == "APPLICATION/X-NCL-NCLUA")
    {
      return true;
    }

  return false;
}

GINGA_FORMATTER_END
