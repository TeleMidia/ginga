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

#include "config.h"
#include "FormatterConverter.h"
#include "FormatterLinkConverter.h"

GINGA_FORMATTER_BEGIN

int FormatterConverter::dummyCount = 0;

FormatterConverter::FormatterConverter (RuleAdapter *ruleAdapter)
{
  linkCompiler = (void *)(new FormatterLinkConverter (this));

  this->scheduler = NULL;
  this->actionListener = NULL;
  this->ruleAdapter = ruleAdapter;
  this->depthLevel = 1; // Formatter::DEEPEST_LEVEL;
  this->handling = false;

  Thread::mutexInit (&objectsMutex, false);
  Thread::mutexInit (&lMutex, false);
}

FormatterConverter::~FormatterConverter ()
{
  map<string, NclExecutionObject *>::iterator i;
  NclExecutionObject *object;
  set<NclFormatterEvent *>::iterator j;

  Thread::mutexLock (&lMutex);
  j = listening.begin ();
  while (j != listening.end ())
    {
      if (NclFormatterEvent::hasInstance ((*j), false))
        {
          (*j)->removeEventListener (this);
        }
      ++j;
    }
  Thread::mutexUnlock (&lMutex);
  Thread::mutexDestroy (&lMutex);

  ruleAdapter = NULL;
  scheduler = NULL;

  Thread::mutexLock (&objectsMutex);
  i = executionObjects.begin ();
  while (i != executionObjects.end ())
    {
      object = i->second;
      if (ntsRemoveExecutionObject (object))
        {
        }
      else
        {
          executionObjects.erase (i);
        }

      i = executionObjects.begin ();
    }
  executionObjects.clear ();
  settingObjects.clear ();
  Thread::mutexUnlock (&objectsMutex);

  if (linkCompiler != NULL)
    {
      delete (FormatterLinkConverter *)linkCompiler;
      linkCompiler = NULL;
    }

  Thread::mutexDestroy (&objectsMutex);
}

void
FormatterConverter::executionObjectReleased (string objectId)
{
  map<string, NclExecutionObject *>::iterator i;
  set<NclExecutionObject *>::iterator j;
  NclExecutionObject *object = NULL;

  Thread::mutexLock (&objectsMutex);
  i = executionObjects.find (objectId);
  if (i != executionObjects.end ())
    {
      object = i->second;
      executionObjects.erase (i);
    }

  if (object != NULL)
    {
      j = settingObjects.find (object);
      if (j != settingObjects.end ())
        {
          settingObjects.erase (j);
        }
    }
  Thread::mutexUnlock (&objectsMutex);
}

set<NclExecutionObject *> *
FormatterConverter::getRunningObjects ()
{
  map<string, NclExecutionObject *>::iterator i;
  set<NclExecutionObject *> *objects;
  NclExecutionObject *object;
  NclFormatterEvent *ev;

  objects = new set<NclExecutionObject *>;
  Thread::mutexLock (&objectsMutex);
  i = executionObjects.begin ();
  while (i != executionObjects.end ())
    {
      object = i->second;
      ev = object->getMainEvent ();
      if (ev != NULL && ev->getCurrentState () != EventUtil::ST_SLEEPING)
        {
          objects->insert (object);
        }
      ++i;
    }
  Thread::mutexUnlock (&objectsMutex);
  return objects;
}

void
FormatterConverter::setHandlingStatus (bool handling)
{
  NclExecutionObject *object;
  map<string, NclExecutionObject *>::iterator i;

  this->handling = handling;

  Thread::mutexLock (&objectsMutex);
  i = executionObjects.begin ();
  while (i != executionObjects.end ())
    {
      object = i->second;
      object->setHandling (handling);
      ++i;
    }
  Thread::mutexUnlock (&objectsMutex);
}

NclExecutionObject *
FormatterConverter::getObjectFromNodeId (string id)
{
  NodeEntity *dataObject;
  NclExecutionObject *expectedObject;
  map<string, NclExecutionObject *>::iterator i;

  Thread::mutexLock (&objectsMutex);
  i = executionObjects.begin ();
  while (i != executionObjects.end ())
    {
      expectedObject = i->second;
      dataObject = (NodeEntity *)(expectedObject->getDataObject ()
                                      ->getDataEntity ());

      if (dataObject->getId () == id)
        {
          Thread::mutexUnlock (&objectsMutex);
          return expectedObject;
        }
      ++i;
    }

  Thread::mutexUnlock (&objectsMutex);

  clog << "FormatterConverter::getObjectFromNodeId ";
  clog << "Warning! cannot find object '" << id << "'";
  clog << endl;
  return NULL;
}

void
FormatterConverter::setScheduler (void *scheduler)
{
  this->scheduler = (FormatterScheduler *)scheduler;
}

void
FormatterConverter::setLinkActionListener (
    INclLinkActionListener *actionListener)
{

  this->actionListener = actionListener;
}

void
FormatterConverter::setDepthLevel (int level)
{
  depthLevel = level;
}

int
FormatterConverter::getDepthLevel ()
{
  return depthLevel;
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

  try
    {
      referParentObject
          = getParentExecutionObject (referPerspective, depthLevel);

      if (referParentObject != NULL)
        {

          clog << "FormatterConverter::addSameInstance '";
          clog << referNode->getId () << "' with head node '";
          clog << referPerspective->getHeadNode ()->getId ();
          clog << "' refer to '";
          clog << referNode->getReferredEntity ()->getId ();
          clog << "' which has as execution object '";
          clog << executionObject->getId () << "' and parent object '";
          clog << referParentObject->getId () << "'";
          clog << endl;

          executionObject->addParentObject (
              referNode, referParentObject,
              referPerspective->getNode (referPerspective->getNumNodes ()
                                         - 2));

          referParentObject->addExecutionObject (executionObject);

          // A new entry for the execution object is inserted using
          // the refer node id. As a consequence, links referring to the
          // refer node will generate events in the execution object.
          desc = executionObject->getDescriptor ();
          if (desc != NULL)
            {
              objectId
                  = (referPerspective->getId () + SystemCompat::getIUriD ()
                     + executionObject->getDescriptor ()->getId ());
            }
          else
            {
              objectId = referPerspective->getId ();
            }

          Thread::mutexLock (&objectsMutex);
          executionObjects[objectId] = executionObject;
          Thread::mutexUnlock (&objectsMutex);
        }

      delete referPerspective;
      return referParentObject;
    }
  catch (ObjectCreationForbiddenException *exc)
    {
      // nothing to be done
      delete referPerspective;
    }

  clog << "FormatterConverter::addSameInstace Exception ";
  clog << "Warning! returning NULL";
  clog << endl;
  return NULL;
}

void
FormatterConverter::checkGradSameInstance (set<ReferNode *> *gradSame,
                                           NclExecutionObject *object)
{

  clog << "FormatterConverter::checkGradSameInstance obj '";
  clog << object->getId () << "'";

  NclNodeNesting *entityPerspective;
  NclNodeNesting *perspective;
  NclNodeNesting *referPerspective;
  set<ReferNode *>::iterator i;
  NodeEntity *dataObject;

  dataObject = (NodeEntity *)(object->getDataObject ());
  perspective = object->getNodePerspective ();
  clog << " objectPerspective = '" << perspective->getId () << "'";
  clog << " with anchorNode = '";
  clog << perspective->getAnchorNode ()->getId ();
  clog << "' with entity = '" << dataObject->getId () << "' with";
  entityPerspective = new NclNodeNesting (dataObject->getPerspective ());
  clog << " perspective '" << entityPerspective->getId ();
  i = gradSame->begin ();
  while (i != gradSame->end ())
    {
      referPerspective = new NclNodeNesting ((*i)->getPerspective ());
      clog << "', gradNode = '";
      clog << (*i)->getId () << "' has perspective '";
      clog << referPerspective->getId () << "' with anchorNode '";
      clog << referPerspective->getAnchorNode ()->getId ();
      ++i;
    }
  clog << "'" << endl;
}

void
FormatterConverter::addExecutionObject (
    NclExecutionObject *executionObject,
    NclCompositeExecutionObject *parentObject, int depthLevel)
{

  NodeEntity *dataObject;
  set<ReferNode *> *sameInstances;
  set<ReferNode *>::iterator i;
  ReferNode *referNode;
  NclNodeNesting *nodePerspective;
  Node *headNode;
  NclCascadingDescriptor *descriptor;
  Entity *entity;

  vector<Node *>::iterator j;

  Thread::mutexLock (&objectsMutex);
  executionObjects[executionObject->getId ()] = executionObject;
  Thread::mutexUnlock (&objectsMutex);

  dataObject = (NodeEntity *)(executionObject->getDataObject ());

  /*clog << endl;
  clog << "FormatterConverter::addExecutionObject '";
  clog << executionObject->getId() << "' in parent '";
  if (parentObject != NULL) {
          clog << parentObject->getId() << "'";
  } else {
          clog << "NULL'";
  }
  clog << " and dataObject '" << dataObject->getId() << "':";
  dataObject->printHierarchy();
  clog << endl;*/

  if (dataObject->instanceOf ("ContentNode")
      && ((ContentNode *)dataObject)->isSettingNode ())
    {

      settingObjects.insert (executionObject);
    }

  if (dataObject->instanceOf ("ReferNode"))
    {
      if (((ReferNode *)dataObject)->getInstanceType () == "instSame")
        {
          entity = ((ReferNode *)dataObject)->getDataEntity ();
          if (entity->instanceOf ("ContentNode")
              && ((ContentNode *)entity)->isSettingNode ())
            {

              settingObjects.insert (executionObject);
            }
        }
    }

  if (parentObject != NULL)
    {
      parentObject->addExecutionObject (executionObject);
    }

  nodePerspective = executionObject->getNodePerspective ();
  headNode = nodePerspective->getHeadNode ();
  delete nodePerspective;

  if (headNode->instanceOf ("CompositeNode")
      && dataObject->instanceOf ("NodeEntity"))
    {

      sameInstances = dataObject->getInstSameInstances ();
      if (sameInstances != NULL)
        {
          i = sameInstances->begin ();
          while (i != sameInstances->end ())
            {
              referNode = (ReferNode *)(*i);
              clog << "FormatterConverter::addExecutionObject '";
              clog << executionObject->getId () << "' instSame '";
              clog << referNode->getId () << "' " << endl;

              if (((CompositeNode *)headNode)
                      ->recursivelyContainsNode (referNode))
                {

                  addSameInstance (executionObject, referNode);
                }
              else
                {
                  clog << "FormatterConverter::addExecutionObject Can't ";
                  clog << "find '" << referNode->getId () << "' inside '";
                  clog << headNode->getId () << "' ";
                  clog << endl;
                }
              ++i;
            }
        }
    }

  descriptor = executionObject->getDescriptor ();
  if (descriptor != NULL)
    {
      descriptor->setFormatterLayout (
          (NclFormatterLayout *)(scheduler->getFormatterLayout (
              descriptor, executionObject)));
    }

  if (descriptor != NULL && descriptor->getPlayerName () != ""
      && ((descriptor->getPlayerName () == "VideoChannelPlayerAdapter")
          || (descriptor->getPlayerName () == "AudioChannelPlayerAdapter")
          || (descriptor->getPlayerName ()
              == "JmfVideoChannelPlayerAdapter")
          || (descriptor->getPlayerName ()
              == "JmfAudioChannelPlayerAdapter")
          || (descriptor->getPlayerName () == "QtVideoChannelPlayerAdapter")
          || (descriptor->getPlayerName ()
              == "QtAudioChannelPlayerAdapter")))
    {

      createMultichannelObject (
          (NclCompositeExecutionObject *)executionObject, depthLevel);
    }

  if (depthLevel != 0)
    {
      if (depthLevel > 0)
        {
          depthLevel--;
        }
      compileExecutionObjectLinks (executionObject, depthLevel);
    }
  else if (executionObject->instanceOf ("NclCompositeExecutionObject"))
    {
      compileExecutionObjectLinks (executionObject, depthLevel);
    }
}

void
FormatterConverter::compileExecutionObjectLinks (
    NclExecutionObject *executionObject, int depthLevel)
{

  vector<Node *> *nodes;
  vector<Node *>::iterator i;
  Node *node;

  /*clog << "FormatterConverter::compileExecutionObjectLinks";
  clog << " EO " << executionObject->getId().c_str();
  clog << " depth " << depthLevel << endl;*/

  nodes = executionObject->getNodes ();
  if (nodes != NULL)
    {
      i = nodes->begin ();
      while (i != nodes->end ())
        {
          node = (Node *)(*i);
          compileExecutionObjectLinks (
              executionObject, node,
              (NclCompositeExecutionObject *)(executionObject
                                                  ->getParentObject (node)),
              depthLevel);

          ++i;
        }
      delete nodes;
      nodes = NULL;
    }
}

NclExecutionObject *
FormatterConverter::getExecutionObjectFromPerspective (
    NclNodeNesting *perspective, GenericDescriptor *descriptor,
    int depthLevel) throw (ObjectCreationForbiddenException *)
{

  map<string, NclExecutionObject *>::iterator i;
  NclCascadingDescriptor *cascadingDescriptor = NULL;
  string id;
  NclCompositeExecutionObject *parentObject;
  // NclExecutionObjectSwitch* parentSwitch = NULL;
  NclExecutionObject *executionObject;
  // Node* selectedNode;

  id = perspective->getId () + SystemCompat::getIUriD ();
  cascadingDescriptor = getCascadingDescriptor (perspective, descriptor);
  if (cascadingDescriptor != NULL)
    {
      id = id + cascadingDescriptor->getId ();
    }

  Thread::mutexLock (&objectsMutex);
  i = executionObjects.find (id);
  if (i != executionObjects.end ())
    {
      if (cascadingDescriptor != NULL)
        {
          delete cascadingDescriptor;
          cascadingDescriptor = NULL;
        }
      executionObject = i->second;
      Thread::mutexUnlock (&objectsMutex);
      return executionObject;
    }
  Thread::mutexUnlock (&objectsMutex);

  parentObject = getParentExecutionObject (perspective, depthLevel);

  executionObject = createExecutionObject (id, perspective,
                                           cascadingDescriptor, depthLevel);

  if (executionObject == NULL)
    {
      if (cascadingDescriptor != NULL)
        {
          delete cascadingDescriptor;
          cascadingDescriptor = NULL;
        }

      clog << "FormatterConverter::getExecutionObjectFromPerspective ";
      clog << "Warning! Object id = '" << id << "'";
      clog << " perspective = '" << perspective->getId () << "'";
      if (descriptor != NULL)
        {
          clog << " descriptor = '" << descriptor->getId () << "'";
        }
      clog << " was not created" << endl;
      return NULL;
    }

  clog << "FormatterConverter::getExecutionObjectFromPerspective ";
  clog << "Adding_object with id = '" << id << "', perspective = '";
  clog << perspective->getId () << "' and parent = '";
  if (parentObject != NULL)
    {
      clog << parentObject->getId () << "'";
    }
  else
    {
      clog << "NULL'";
    }
  clog << endl;

  addExecutionObject (executionObject, parentObject, depthLevel);

  checkMirror (executionObject, depthLevel);
  return executionObject;
}

void
FormatterConverter::checkMirror (NclExecutionObject *object, int depthLevel)
{

  CompositeNode *parentNode;
  Node *mirrorNode;
  Node *dataObject = object->getDataObject ();
  NodeEntity *dataEntity = (NodeEntity *)(dataObject->getDataEntity ());
  Content *content = dataEntity->getContent ();
  string url, mirrorSrc;
  NclExecutionObject *mExecObj;
  NclNodeNesting *mPerspective;

  if (content != NULL)
    {
      if (content->instanceOf ("ReferenceContent"))
        {
          url = ((ReferenceContent *)(content))->getCompleteReferenceUrl ();

          if (url.substr (0, 13) == "ncl-mirror://")
            {
              mirrorSrc = url.substr (13, url.length () - 13);
              parentNode
                  = (CompositeNode *)dataObject->getParentComposition ();
              while (parentNode->getParentComposition () != NULL)
                {
                  parentNode = (CompositeNode *)
                                   parentNode->getParentComposition ();
                }

              mirrorNode = parentNode->recursivelyGetNode (mirrorSrc);
              if (mirrorNode != NULL)
                {
                  mPerspective
                      = new NclNodeNesting (mirrorNode->getPerspective ());
                  mExecObj = getExecutionObjectFromPerspective (
                      mPerspective, NULL, depthLevel);

                  delete mPerspective;
                  object->setMirrorSrc (mExecObj);
                }
            }
        }
    }
}

set<NclExecutionObject *> *
FormatterConverter::getSettingNodeObjects ()
{
  return new set<NclExecutionObject *> (settingObjects);
}

NclCompositeExecutionObject *
FormatterConverter::getParentExecutionObject (
    NclNodeNesting *perspective,
    int depthLevel) throw (ObjectCreationForbiddenException *)
{

  NclNodeNesting *parentPerspective;
  NclCompositeExecutionObject *cObj;

  if (perspective->getNumNodes () > 1)
    {
      parentPerspective = perspective->copy ();
      parentPerspective->removeAnchorNode ();

      /*clog << "FormatterConverter::getParentExecutionObject '";
      clog << " perspective = '" << parentPerspective->getId() << "'";
      clog << endl;*/

      cObj = (NclCompositeExecutionObject
                  *)(this->getExecutionObjectFromPerspective (
          parentPerspective, NULL, depthLevel));

      delete parentPerspective;
      return cObj;
    }

  return NULL;
}

NclFormatterEvent *
FormatterConverter::getEvent (NclExecutionObject *executionObject,
                              InterfacePoint *interfacePoint,
                              int ncmEventType, string key)
{

  string id;
  NclFormatterEvent *event;

  if (key == "")
    {
      id = interfacePoint->getId () + "_" + itos (ncmEventType);
    }
  else
    {
      id = interfacePoint->getId () + "_" + itos (ncmEventType) + "_" + key;
    }

  event = executionObject->getEvent (id);
  if (event != NULL)
    {
      return event;
    }

  if (executionObject->instanceOf ("NclExecutionObjectSwitch"))
    {
      event = new NclSwitchEvent (
          id, (NclExecutionObjectSwitch *)executionObject, interfacePoint,
          ncmEventType, key);
    }
  else
    {
      if (ncmEventType == EventUtil::EVT_PRESENTATION)
        {
          event = new NclPresentationEvent (
              id, executionObject, (ContentAnchor *)interfacePoint);
        }
      else
        {
          if (executionObject->instanceOf ("NclCompositeExecutionObject"))
            {
              // TODO: eventos internos da composicao.
              // Estao sendo tratados nos elos.
              if (ncmEventType == EventUtil::EVT_ATTRIBUTION)
                {
                  if (interfacePoint->instanceOf ("PropertyAnchor"))
                    {
                      event = new NclAttributionEvent (
                          id, executionObject,
                          (PropertyAnchor *)interfacePoint,
                          ruleAdapter->getPresentationContext ());
                    }
                  else
                    {
                      clog << "FormatterConverter::getEvent Warning!";
                      clog << "NCM event type is attribution, but ";
                      clog << "interface point isn't" << endl;

                      event = new NclAttributionEvent (
                          id, executionObject, NULL,
                          ruleAdapter->getPresentationContext ());
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
                          id, executionObject,
                          (PropertyAnchor *)interfacePoint,
                          ruleAdapter->getPresentationContext ());
                    }
                  else
                    {
                      clog << "FormatterConverter::getEvent Warning!";
                      clog << "NCM event type is attribution, but ";
                      clog << "interface point isn't" << endl;

                      if (interfacePoint->instanceOf ("IntervalAnchor"))
                        {

                          clog << "FormatterConverter::getEvent ";
                          clog << "it was supposed to be ";
                          clog << "a PRESENTATION EVENT ";
                          clog << endl;
                          // TODO: find the correct way to solve this
                          event = new NclPresentationEvent (
                              id, executionObject,
                              (ContentAnchor *)interfacePoint);
                        }

                      return NULL;
                    }
                  break;

                case EventUtil::EVT_SELECTION:
                  event = new NclSelectionEvent (
                      id, executionObject, (ContentAnchor *)interfacePoint);

                  if (key != "")
                    {
                      ((NclSelectionEvent *)event)->setSelectionCode (key);
                    }

                  break;

                default:
                  clog << "FormatterConverter::getEvent Warning!";
                  clog << "Unknown event type '" << ncmEventType;
                  clog << "'" << endl;
                  break;
                }
            }
        }
    }

  if (event != NULL)
    {
      executionObject->addEvent (event);
    }
  else
    {
      clog << "FormatterConverter::getEvent Warning!";
      clog << " returning a NULL event for '" << id << "'" << endl;
    }

  return event;
}

void
FormatterConverter::createMultichannelObject (
    NclCompositeExecutionObject *compositeObject, int depthLevel)
{

  CompositeNode *compositeNode;
  vector<Node *> *nodes;
  Node *node;
  NclNodeNesting *perspective;
  string id;
  NclCascadingDescriptor *cascadingDescriptor;
  NclExecutionObject *childObject;

  compositeNode = (CompositeNode *)compositeObject->getDataObject ();
  nodes = compositeNode->getNodes ();
  if (nodes != NULL)
    {
      vector<Node *>::iterator i;
      i = nodes->begin ();
      while (i != nodes->end ())
        {
          node = *i;
          perspective
              = new NclNodeNesting (compositeObject->getNodePerspective ());

          perspective->insertAnchorNode (node);

          id = perspective->getId () + SystemCompat::getIUriD ();
          cascadingDescriptor = getCascadingDescriptor (perspective, NULL);

          if (cascadingDescriptor != NULL)
            {
              id += cascadingDescriptor->getId ();
            }

          childObject = createExecutionObject (
              id, perspective, cascadingDescriptor, depthLevel);

          if (childObject != NULL)
            {
              getEvent (childObject, ((NodeEntity *)node->getDataEntity ())
                                         ->getLambdaAnchor (),
                        EventUtil::EVT_PRESENTATION, "");

              addExecutionObject (childObject, compositeObject, depthLevel);
            }

          ++i;
        }
    }
}

NclExecutionObject *
FormatterConverter::createExecutionObject (
    string id, NclNodeNesting *perspective,
    NclCascadingDescriptor *descriptor, int depthLevel)
{

  NodeEntity *nodeEntity;
  Node *node;
  NclNodeNesting *nodePerspective;
  NclExecutionObject *executionObject;
  NclPresentationEvent *compositeEvent;
  vector<PropertyAnchor *> *properties;

  /*clog << "FormatterConverter::createExecutionObject ";
  clog << " id = '" << id << "'";
  clog << " perspective = '" << perspective->getId() << "'";
  if (descriptor != NULL) {
          clog << " descriptor = '" << descriptor->getId() << "'";
  }
  clog << endl;*/

  nodeEntity
      = (NodeEntity *)(perspective->getAnchorNode ()->getDataEntity ());

  if (nodeEntity == NULL || !nodeEntity->instanceOf ("NodeEntity"))
    {
      clog << "FormatterConverter::createExecutionObject Warning! ";
      clog << "Can't create object id = '" << id << "' ";
      clog << "perspective = '" << perspective->getId () << "'";
      if (descriptor != NULL)
        {
          clog << "descriptor = '" << descriptor->getId () << "'";
        }
      clog << ": invalid node entity" << endl;
      return NULL;
    }

  node = perspective->getAnchorNode ();

  // solve execution object cross reference coming from refer nodes with
  // new instance = false
  if (nodeEntity->instanceOf ("ContentNode")
      && ((ContentNode *)nodeEntity)->getNodeType () != ""
      && !((ContentNode *)nodeEntity)->isSettingNode ())
    {

      if (node->instanceOf ("ReferNode"))
        {
          if (((ReferNode *)node)->getInstanceType () != "new")
            {

              nodePerspective
                  = new NclNodeNesting (nodeEntity->getPerspective ());

              // verify if both nodes are in the same base.
              if (nodePerspective->getHeadNode ()
                  == perspective->getHeadNode ())
                {

                  try
                    {
                      executionObject = getExecutionObjectFromPerspective (
                          nodePerspective, NULL, depthLevel);
                    }
                  catch (ObjectCreationForbiddenException *exc1)
                    {
                      if (AdapterPlayerManager::isEmbeddedApp (nodeEntity))
                        {
                          executionObject
                              = new NclApplicationExecutionObject (
                                  id, nodeEntity, descriptor, handling,
                                  actionListener);
                        }
                      else
                        {
                          executionObject = new NclExecutionObject (
                              id, nodeEntity, descriptor, handling,
                              actionListener);
                        }

                      // TODO informa a substituicao
                    }
                }
              else
                {
                  // not in the same base => create a new version
                  if (AdapterPlayerManager::isEmbeddedApp (nodeEntity))
                    {
                      executionObject = new NclApplicationExecutionObject (
                          id, nodeEntity, descriptor, handling,
                          actionListener);
                    }
                  else
                    {
                      executionObject = new NclExecutionObject (
                          id, nodeEntity, descriptor, handling,
                          actionListener);
                    }

                  // TODO informa a substituicao
                }

              delete nodePerspective;

              if (executionObject != NULL)
                {
                  return executionObject;
                }
            }
        }
    }

  if (nodeEntity->instanceOf ("SwitchNode"))
    {
      executionObject = new NclExecutionObjectSwitch (id, node, handling,
                                                      actionListener);

      compositeEvent = new NclPresentationEvent (
          nodeEntity->getLambdaAnchor ()->getId () + "_"
              + itos (EventUtil::EVT_PRESENTATION),
          executionObject,
          (ContentAnchor *)(nodeEntity->getLambdaAnchor ()));

      executionObject->addEvent (compositeEvent);
      // to monitor the switch presentation and clear the selection after
      // each execution
      compositeEvent->addEventListener (this);
      Thread::mutexLock (&lMutex);
      listening.insert (compositeEvent);
      Thread::mutexUnlock (&lMutex);
    }
  else if (nodeEntity->instanceOf ("CompositeNode"))
    {
      executionObject = new NclCompositeExecutionObject (
          id, node, descriptor, handling, actionListener);

      compositeEvent = new NclPresentationEvent (
          nodeEntity->getLambdaAnchor ()->getId () + "_"
              + itos (EventUtil::EVT_PRESENTATION),
          executionObject,
          (ContentAnchor *)(nodeEntity->getLambdaAnchor ()));

      executionObject->addEvent (compositeEvent);

      // to monitor the presentation and remove object at stops
      // compositeEvent->addEventListener(this);
    }
  else if (AdapterPlayerManager::isEmbeddedApp (nodeEntity))
    {
      executionObject = new NclApplicationExecutionObject (
          id, node, descriptor, handling, actionListener);
    }
  else
    {
      executionObject = new NclExecutionObject (id, node, descriptor,
                                                handling, actionListener);
    }

  return executionObject;
}

bool
FormatterConverter::hasDescriptorPropName (string name)
{
  // TODO: create a set with reserved words
  if (name == "left" || name == "top" || name == "width" || name == "height"
      || name == "right" || name == "bottom" || name == "explicitDur"
      || name == "size" || name == "bounds" || name == "location"
      || name == "zIndex")
    {

      return true;
    }

  return false;
}

Descriptor *
FormatterConverter::createDummyDescriptor (Node *node)
{
  Descriptor *ncmDesc = NULL;
  FocusDecoration *focusDecoration;

  ncmDesc = new Descriptor ("dummyDescriptor" + itos (dummyCount));
  dummyCount++;
  focusDecoration = new FocusDecoration ();
  ncmDesc->setFocusDecoration (focusDecoration);

  return ncmDesc;
}

NclCascadingDescriptor *
FormatterConverter::createDummyCascadingDescriptor (Node *node)
{

  NodeEntity *nodeEntity;
  Descriptor *ncmDesc = NULL;
  string name;

  PropertyAnchor *property;
  vector<PropertyAnchor *> *anchors;
  vector<PropertyAnchor *>::iterator i;

  anchors = node->getOriginalPropertyAnchors ();
  if (anchors != NULL)
    {
      i = anchors->begin ();
      while (i != anchors->end ())
        {
          property = ((PropertyAnchor *)(*i));
          name = property->getPropertyName ();

          // TODO: create a set with reserved words
          if (hasDescriptorPropName (name))
            {
              if (node->instanceOf ("NodeEntity"))
                {
                  ncmDesc = createDummyDescriptor (node);
                  ((NodeEntity *)node)->setDescriptor (ncmDesc);
                }
              else if (node->instanceOf ("ReferNode")
                       && ((ReferNode *)node)->getInstanceType () == "new")
                {

                  if (((ReferNode *)node)->getInstanceDescriptor () == NULL)
                    {
                      nodeEntity = (NodeEntity *)node->getDataEntity ();
                      ncmDesc = (Descriptor *)nodeEntity->getDescriptor ();

                      if (ncmDesc == NULL)
                        {
                          ncmDesc = createDummyDescriptor (node);
                        }
                      ((ReferNode *)node)->setInstanceDescriptor (ncmDesc);
                    }
                  else
                    {
                      ncmDesc = (Descriptor *)((ReferNode *)node)
                                    ->getInstanceDescriptor ();
                    }
                }
              else
                {
                  ncmDesc = createDummyDescriptor (node);
                }

              return new NclCascadingDescriptor (ncmDesc);
            }

          ++i;
        }
    }

  if (node->instanceOf ("ReferNode")
      && ((ReferNode *)node)->getInstanceType () == "new"
      && ((ReferNode *)node)->getInstanceDescriptor () == NULL)
    {

      nodeEntity = (NodeEntity *)node->getDataEntity ();
      ncmDesc = (Descriptor *)nodeEntity->getDescriptor ();
      ((ReferNode *)node)->setInstanceDescriptor (ncmDesc);
      return new NclCascadingDescriptor (ncmDesc);
    }

  return NULL;
}

NclCascadingDescriptor *
FormatterConverter::checkCascadingDescriptor (Node *node)
{

  NodeEntity *nodeEntity;

  NclCascadingDescriptor *cascadingDescriptor = NULL;

  if (node->instanceOf ("ContentNode"))
    {
      cascadingDescriptor = createDummyCascadingDescriptor (node);
    }
  else if (node->instanceOf ("ReferNode")
           && ((ReferNode *)node)->getInstanceType () == "new")
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
  if (size > 1 && nodePerspective->getNode (size - 2) != NULL
      && nodePerspective->getNode (size - 2)->instanceOf ("ContextNode"))
    {

      context = (ContextNode *)(nodePerspective->getNode (size - 2)
                                    ->getDataEntity ());

      if (context->getNodeDescriptor (ncmNode) != NULL)
        {
          if (resDesc == NULL)
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

  NclCascadingDescriptor *cascadingDescriptor = NULL;
  Descriptor *ncmDesc;
  Node *anchorNode;
  Node *node;
  ContextNode *context;
  int size;

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

      if (node == NULL || !node->instanceOf ("NodeEntity"))
        {
          clog << "FormatterConverter::getCascadingDescriptor Warning! ";
          clog << "Can't create cascading descriptor: ";
          clog << "invalid node entity" << endl;
          return NULL;
        }

      ncmDesc = (Descriptor *)((NodeEntity *)node)->getDescriptor ();
    }

  if (ncmDesc != NULL)
    {
      cascadingDescriptor = new NclCascadingDescriptor (ncmDesc);
    }

  cascadingDescriptor = checkContextCascadingDescriptor (
      nodePerspective, cascadingDescriptor, node);

  // there is an explicit descriptor (user descriptor)?
  if (descriptor != NULL)
    {
      if (cascadingDescriptor == NULL)
        {
          cascadingDescriptor = new NclCascadingDescriptor (descriptor);
        }
      else
        {
          cascadingDescriptor->cascade (descriptor);
        }
    }

  if (cascadingDescriptor == NULL)
    {
      cascadingDescriptor = checkCascadingDescriptor (node);
    }

  return cascadingDescriptor;
}

void
FormatterConverter::processLink (Link *ncmLink, Node *dataObject,
                                 NclExecutionObject *executionObject,
                                 NclCompositeExecutionObject *parentObject)
{

  vector<GenericDescriptor *> *descriptors;
  GenericDescriptor *descriptor;
  NodeEntity *nodeEntity = NULL;
  set<ReferNode *> *sameInstances;
  bool contains = false;
  set<ReferNode *>::iterator i;
  NclFormatterLink *formatterLink;

  if (executionObject->getDataObject () != NULL
      && executionObject->getDataObject ()->instanceOf ("NodeEntity"))
    {

      nodeEntity = (NodeEntity *)(executionObject->getDataObject ());
    }

  // since the link may be removed in a deepest compilation its
  // necessary to certify that the link was not compiled
  if (parentObject->containsUncompiledLink (ncmLink))
    {
      descriptor = NULL;
      if (executionObject->getDescriptor () != NULL)
        {
          descriptors
              = executionObject->getDescriptor ()->getNcmDescriptors ();

          if (descriptors != NULL && !(descriptors->empty ()))
            {
              descriptor = (*descriptors)[descriptors->size () - 1];
            }
        }

      if (ncmLink->instanceOf ("CausalLink"))
        {
          if (nodeEntity != NULL)
            {
              sameInstances = nodeEntity->getInstSameInstances ();
              if (sameInstances != NULL)
                {
                  i = sameInstances->begin ();
                  while (i != sameInstances->end ())
                    {
                      contains = ((CausalLink *)ncmLink)
                                     ->containsSourceNode (*i, descriptor);

                      if (contains)
                        {
                          break;
                        }
                      ++i;
                    }
                }

              if (!contains)
                {
                  sameInstances = nodeEntity->getGradSameInstances ();

                  if (sameInstances != NULL)
                    {
                      i = sameInstances->begin ();
                      while (i != sameInstances->end ())
                        {
                          contains
                              = ((CausalLink *)ncmLink)
                                    ->containsSourceNode (*i, descriptor);

                          if (contains)
                            {
                              break;
                            }
                          ++i;
                        }
                    }
                }
            }

          // verify if execution object is part of
          // link conditions
          if (((CausalLink *)ncmLink)
                  ->containsSourceNode (dataObject, descriptor)
              || contains)
            {

              // compile causal link
              parentObject->removeLinkUncompiled (ncmLink);
              formatterLink
                  = ((FormatterLinkConverter *)linkCompiler)
                        ->createCausalLink ((CausalLink *)ncmLink,
                                            parentObject, depthLevel);

              if (formatterLink != NULL)
                {
                  setActionListener (
                      ((NclFormatterCausalLink *)formatterLink)
                          ->getAction ());

                  parentObject->setLinkCompiled (formatterLink);
                  clog << "FormatterConverter::processLink ";
                  clog << "LINKCOMPILED '" << ncmLink->getId ();
                  clog << "'" << endl;
                }
            }
          else
            {
              clog << "FormatterConverter::processLink ";
              clog << "can't process ncmLink '";
              clog << ncmLink->getId () << "'";
              clog << " inside '" << parentObject->getId () << "'";
              clog << "  because '";
              clog << ncmLink->getId () << "'";
              clog << " does not contain '";
              clog << dataObject->getId () << "' src";
              clog << endl;
            }
        }
      else
        {
          clog << "FormatterConverter::processLink ";
          clog << "can't process ncmLink '" << ncmLink->getId ();
          clog << "' inside '" << parentObject->getId () << "'";
          clog << "  because isn't a causal link";
          clog << endl;
        }
    }
  else
    {
      clog << "FormatterConverter::processLink ";
      clog << "can't process ncmLink '" << ncmLink->getId ();
      clog << "' inside '" << parentObject->getId () << "'";
      clog << " link may be removed in a deepest compilation";
      clog << endl;
    }
}

void
FormatterConverter::compileExecutionObjectLinks (
    NclExecutionObject *executionObject, Node *dataObject,
    NclCompositeExecutionObject *parentObject, int depthLevel)
{

  set<Link *> *dataLinks;
  set<Link *> *uncompiledLinks;
  set<Link *>::iterator i;
  Link *ncmLink;

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
                                       compObj, depthLevel);
        }
    }

  uncompiledLinks = parentObject->getUncompiledLinks ();
  if (!uncompiledLinks->empty ())
    {
      dataLinks = uncompiledLinks;

      i = dataLinks->begin ();
      while (i != dataLinks->end ())
        {
          ncmLink = *i;

          clog << "FormatterConverter::compileExecutionObjectLinks ";
          clog << "processing ncmLink '" << ncmLink->getId () << "'";
          clog << " inside '" << parentObject->getId () << "'" << endl;

          processLink (ncmLink, dataObject, executionObject, parentObject);

          ++i;
        }

      delete dataLinks;
      dataLinks = NULL;

      compileExecutionObjectLinks (
          executionObject, dataObject,
          (NclCompositeExecutionObject *)(parentObject->getParentObject ()),
          depthLevel);
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

          compileExecutionObjectLinks (object, dataObject, parentObject,
                                       depthLevel);
        }
    }
}

void
FormatterConverter::setActionListener (NclLinkAction *action)
{
  if (action->instanceOf ("NclLinkSimpleAction"))
    {
      ((NclLinkSimpleAction *)action)
          ->setSimpleActionListener (actionListener);
    }
  else
    {
      vector<NclLinkSimpleAction *> actions;
      vector<NclLinkSimpleAction *>::iterator i;

      ((NclLinkCompoundAction *)action)
          ->setCompoundActionListener (actionListener);

      ((NclLinkCompoundAction *)action)->getSimpleActions (&actions);
      i = actions.begin ();
      while (i != actions.end ())
        {
          setActionListener ((NclLinkAction *)(*i));
          ++i;
        }
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

  selectedNode = ruleAdapter->adaptSwitch (switchNode);
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

  id = selectedPerspective->getId () + SystemCompat::getIUriD ();

  descriptor = FormatterConverter::getCascadingDescriptor (
      selectedPerspective, NULL);

  if (descriptor != NULL)
    {
      id += descriptor->getId ();
    }

  i = executionObjects.find (id);
  if (i != executionObjects.end ())
    {
      selectedObject = i->second;
      switchObject->select (selectedObject);
      resolveSwitchEvents (switchObject, depthLevel);
      if (descriptor != NULL)
        {
          delete descriptor;
          descriptor = NULL;
        }

      delete selectedPerspective;

      return selectedObject;
    }

  selectedObject = createExecutionObject (id, selectedPerspective,
                                          descriptor, depthLevel);

  delete selectedPerspective;

  if (selectedObject == NULL)
    {
      if (descriptor != NULL)
        {
          delete descriptor;
          descriptor = NULL;
        }

      clog << "FormatterConverter::processExecutionObjectSwitch ";
      clog << "Warning! can't process '";
      clog << switchObject->getId () << "' selected OBJECT is NULL";
      clog << endl;
      return NULL;
    }

  addExecutionObject (selectedObject, switchObject, depthLevel);
  switchObject->select (selectedObject);
  resolveSwitchEvents (switchObject, depthLevel);
  return selectedObject;
}

void
FormatterConverter::resolveSwitchEvents (
    NclExecutionObjectSwitch *switchObject, int depthLevel)
{

  NclExecutionObject *selectedObject;
  NclExecutionObject *endPointObject;
  Node *selectedNode, *mappedNode;
  NodeEntity *selectedNodeEntity;
  vector<NclFormatterEvent *> *events;
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

  clog << "FormatterConverter::resolveSwitchEvents for '";
  clog << switchObject->getId () << "'" << endl;

  selectedObject = switchObject->getSelectedObject ();
  if (selectedObject == NULL)
    {
      clog << "FormatterConverter::resolveSwitchEvents Warning!";
      clog << "selected object is NULL" << endl;
      return;
    }

  selectedNode = selectedObject->getDataObject ();
  selectedNodeEntity = (NodeEntity *)(selectedNode->getDataEntity ());
  events = switchObject->getEvents ();
  if (events != NULL && !events->empty ())
    {
      i = events->begin ();
      while (i != events->end ())
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
                                      nodePerspective, NULL, depthLevel);

                              if (endPointObject != NULL)
                                {
                                  mappedEvent = getEvent (
                                      endPointObject,
                                      mapping->getEndInterfacePoint (),
                                      switchEvent->getEventType (),
                                      switchEvent->getKey ());
                                }
                            }
                          catch (ObjectCreationForbiddenException *exc)
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
      delete events;
      events = NULL;
    }
  else
    {
      clog << "FormatterConverter::resolveSwitchEvents Warning! ";
      clog << "can't find events" << endl;
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
          perspective, descriptor, depthLevel);

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
  catch (ObjectCreationForbiddenException *exc)
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

      clog << "FormatterConverter::insertContext Warning! ";
      clog << "can't find a valid interface point in '";
      clog << contextPerspective->getId () << "'";
      clog << endl;
      return NULL;
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
FormatterConverter::removeExecutionObject (
    NclExecutionObject *executionObject, ReferNode *referNode)
{

  NclNodeNesting *referPerspective;
  map<string, NclExecutionObject *>::iterator i;
  set<NclExecutionObject *>::iterator j;

  if (executionObject == NULL || referNode == NULL)
    {
      return false;
    }

  executionObject->removeNode (referNode);
  referPerspective = new NclNodeNesting (referNode->getPerspective ());
  string objectId;
  objectId = referPerspective->getId () + SystemCompat::getIUriD ()
             + executionObject->getDescriptor ()->getId ();

  delete referPerspective;

  Thread::mutexLock (&objectsMutex);
  i = executionObjects.find (objectId);
  if (i != executionObjects.end ())
    {
      executionObjects.erase (i);
    }
  Thread::mutexUnlock (&objectsMutex);

  j = settingObjects.find (executionObject);
  if (j != settingObjects.end ())
    {
      settingObjects.erase (j);
    }
  // TODO: problema se esse era a base para outros objetos
  return true;
}

bool
FormatterConverter::removeExecutionObject (
    NclExecutionObject *executionObject)
{

  map<string, NclExecutionObject *>::iterator i;
  bool removed;

  Thread::mutexLock (&objectsMutex);
  removed = ntsRemoveExecutionObject (executionObject);
  Thread::mutexUnlock (&objectsMutex);

  return removed;
}

bool
FormatterConverter::ntsRemoveExecutionObject (
    NclExecutionObject *executionObject)
{

  map<string, NclExecutionObject *>::iterator i;
  bool removed = false;

  if (!NclExecutionObject::hasInstance (executionObject, false))
    {
      return removed;
    }

  i = executionObjects.find (executionObject->getId ());
  if (i != executionObjects.end ())
    {
      executionObjects.erase (i);
      removed = true;
    }

  if (settingObjects.count (executionObject) != 0)
    {
      settingObjects.erase (settingObjects.find (executionObject));
      removed = true;
    }

  if (removed && NclExecutionObject::hasInstance (executionObject, true))
    {
      delete executionObject;
      executionObject = NULL;
    }

  return removed;
}

NclExecutionObject *
FormatterConverter::hasExecutionObject (Node *node,
                                        GenericDescriptor *descriptor)
{

  NclExecutionObject *expectedObject;
  map<string, NclExecutionObject *>::iterator i;
  NclNodeNesting *perspective;
  string id;
  NclCascadingDescriptor *cascadingDescriptor;
  vector<Node *> *nodes;

  // TODO procurar por potenciais substitutos no caso de REFER

  nodes = node->getPerspective ();
  perspective = new NclNodeNesting (nodes);
  delete nodes;
  id = perspective->getId () + SystemCompat::getIUriD ();
  cascadingDescriptor = getCascadingDescriptor (perspective, descriptor);
  if (cascadingDescriptor != NULL)
    {
      id += cascadingDescriptor->getId ();
      delete cascadingDescriptor;
      cascadingDescriptor = NULL;
    }

  Thread::mutexLock (&objectsMutex);
  i = executionObjects.find (id);
  if (i != executionObjects.end ())
    {
      expectedObject = i->second;
      Thread::mutexUnlock (&objectsMutex);
      return expectedObject;
    }

  Thread::mutexUnlock (&objectsMutex);
  return NULL;
}

NclFormatterCausalLink *
FormatterConverter::addCausalLink (ContextNode *context, CausalLink *link)
{

  NclExecutionObject *object;

  object = hasExecutionObject (context, NULL);
  if (object == NULL)
    {
      clog << "FormatterConverter::addCausalLink Warning! Can't ";
      clog << " add link '" << link->getId () << "' inside '";
      clog << context->getId () << "': execution object not found!";
      clog << endl;
      return NULL;
    }

  NclCompositeExecutionObject *contextObject;
  vector<Bind *> *binds;

  contextObject = (NclCompositeExecutionObject *)object;
  contextObject->addNcmLink (link);

  binds = link->getConditionBinds ();
  if (binds != NULL)
    {
      NclExecutionObject *childObject;
      vector<Bind *>::iterator i;
      Bind *bind;
      NclFormatterCausalLink *formatterLink;

      i = binds->begin ();
      while (i != binds->end ())
        {
          bind = (*i);
          childObject = hasExecutionObject (bind->getNode (),
                                            bind->getDescriptor ());

          if (childObject != NULL)
            {
              // compile causal link
              contextObject->removeLinkUncompiled (link);
              formatterLink = ((FormatterLinkConverter *)linkCompiler)
                                  ->createCausalLink (link, contextObject,
                                                      depthLevel);

              if (formatterLink != NULL)
                {
                  setActionListener (formatterLink->getAction ());
                  contextObject->setLinkCompiled (formatterLink);
                }

              delete binds;
              return formatterLink;
            }
          ++i;
        }

      delete binds;
    }

  clog << "FormatterConverter::addCausalLink Warning! Can't ";
  clog << " add link '" << link->getId () << "' inside '";
  clog << context->getId () << "' returning NULL";
  clog << endl;
  return NULL;
}

void
FormatterConverter::eventStateChanged (void *someEvent, short transition,
                                       short previousState)
{

  NclExecutionObject *executionObject;
  NclFormatterEvent *event;
  vector<NclFormatterEvent *> *evs;
  vector<NclFormatterEvent *>::iterator i;
  NclFormatterEvent *ev;
  NclExecutionObject *obj;

  event = (NclFormatterEvent *)someEvent;
  executionObject = (NclExecutionObject *)(event->getExecutionObject ());

  if (executionObject->instanceOf ("NclExecutionObjectSwitch"))
    {
      if (transition == EventUtil::TR_STARTS)
        {
          evs = ((NclExecutionObjectSwitch *)executionObject)->getEvents ();

          i = evs->begin ();
          while (i != evs->end ())
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

          delete evs;
        }

      if (transition == EventUtil::TR_STOPS
          || transition == EventUtil::TR_ABORTS)
        {

          /*
          removeExecutionObject(
                          ((NclExecutionObjectSwitch*)executionObject)->
                          getSelectedObject());
          */
          ((NclExecutionObjectSwitch *)executionObject)->select (NULL);
          // removeExecutionObject(executionObject);
        }
    }
  else if (executionObject->instanceOf ("NclCompositeExecutionObject"))
    {
      /*clog << "FormatterConverter::eventStateChanged CEO = '";
      clog << executionObject->getId() << "' transition '";
      clog << transition << "' starts=>'" << EventUtil::TR_STARTS;
      clog << "'" << endl;*/

      if (transition == EventUtil::TR_STOPS
          || transition == EventUtil::TR_ABORTS)
        {

          removeExecutionObject (executionObject);
        }
    }
}

short
FormatterConverter::getPriorityType ()
{
  return INclEventListener::PT_CORE;
}

void
FormatterConverter::reset ()
{
  Thread::mutexLock (&objectsMutex);
  executionObjects.clear ();
  Thread::mutexUnlock (&objectsMutex);
}

GINGA_FORMATTER_END
