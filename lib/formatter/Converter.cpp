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

#include "aux-ginga.h"
#include "Converter.h"

#include "NclAction.h"
#include "NclLinkAssessment.h"
#include "NclLinkStatement.h"
#include "Scheduler.h"

GINGA_FORMATTER_BEGIN

Converter::Converter (GingaInternal *ginga,
                      INclActionListener *actlist,
                      RuleAdapter *ruleAdapter)
{
  g_assert_nonnull (ginga);
  _ginga = ginga;

  _scheduler = ginga->getScheduler ();
  g_assert_nonnull (_scheduler);

  _actionListener = actlist;
  _ruleAdapter = ruleAdapter;
}

Converter::~Converter ()
{
}

RuleAdapter *
Converter::getRuleAdapter ()
{
  return _ruleAdapter;
}

ExecutionObject *
Converter::getExecutionObjectFromPerspective (
    NclNodeNesting *perspective, Descriptor *descriptor)
{
  ExecutionObjectContext *parentObj;
  ExecutionObject *exeObj;

  string id = perspective->getId () + "/";

  exeObj = _scheduler->getObjectById (id);
  if (exeObj != nullptr)
    return exeObj;

  parentObj = getParentExecutionObject (perspective);
  exeObj = createExecutionObject (id, perspective, descriptor);
  g_assert_nonnull (exeObj);

  addExecutionObject (exeObj, parentObj);

  return exeObj;
}

NclEvent *
Converter::getEvent (ExecutionObject *exeObj,
                     Anchor *interfacePoint,
                     EventType ncmEventType,
                     const string &key)
{
  string id;
  string suffix;

  NclEvent *event;
  string type;

  switch (ncmEventType)
    {
    case EventType::SELECTION:
      suffix = "<sel";
      if (key != "")
        suffix += "(" + key + ")";
      suffix += ">";
      break;
    case EventType::PRESENTATION:
      suffix = "<pres>";
      break;
    case EventType::ATTRIBUTION:
      suffix = "<attr>";
      break;
    default:
      g_assert_not_reached ();
    }

  id = interfacePoint->getId () + suffix;

  event = exeObj->getEvent (id);
  if (event != nullptr)
    {
      return event;
    }

  auto switchObj = cast (ExecutionObjectSwitch *, exeObj);
  auto cObj = cast (ExecutionObjectContext *, exeObj);

  if (switchObj)
    {
      event = new SwitchEvent (_ginga,
            id, switchObj, interfacePoint, ncmEventType, key);
    }
  else if (ncmEventType == EventType::PRESENTATION)
    {
      event = new PresentationEvent (_ginga,
            id, exeObj, (Area *)interfacePoint);
    }
  else if (cObj)
    {
      if (ncmEventType == EventType::ATTRIBUTION)
        {
          auto propAnchor = cast (Property *, interfacePoint);
          g_assert_nonnull (propAnchor);
          event = new AttributionEvent (_ginga, id, exeObj, propAnchor);
        }
    }
  else
    {
      switch (ncmEventType)
        {
        case EventType::ATTRIBUTION:
          {
            auto propAnchor = cast (Property *, interfacePoint);
            g_assert_nonnull (propAnchor);
            event = new AttributionEvent (_ginga, id, exeObj, propAnchor);

            break;
          }
        case EventType::SELECTION:
          {
            event = new SelectionEvent (_ginga,
                                        id, exeObj, (Area *)interfacePoint);
            if (key != "")
              {
                ((SelectionEvent *)event)->setSelectionCode (key);
              }
            break;
          }
        default:
          g_assert_not_reached ();
        }
    }

  g_assert_nonnull (event);
  exeObj->addEvent (event);

  return event;
}

ExecutionObjectContext *
Converter::addSameInstance (ExecutionObject *exeObj,
                            Refer *referNode)
{
  vector<Node *> *ncmPerspective = referNode->getPerspective ();
  NclNodeNesting *referPerspective = new NclNodeNesting (ncmPerspective);

  ExecutionObjectContext *referParentObject
      = getParentExecutionObject (referPerspective);

  if (referParentObject != nullptr)
    {
      exeObj->addParentObject (
            referNode,
            referParentObject,
            referPerspective->getNode (referPerspective->getNumNodes () - 2));

      referParentObject->addExecutionObject (exeObj);
    }

  delete ncmPerspective;
  delete referPerspective;

  return referParentObject;
}

void
Converter::addExecutionObject (ExecutionObject *exeObj,
                               ExecutionObjectContext *parentObj)
{
  if (parentObj)
    parentObj->addExecutionObject (exeObj);

  // Hanlde settings nodes.
  Node *dataObject = exeObj->getNode ();
  NclNodeNesting *nodePerspective = exeObj->getNodePerspective ();
  Node *headNode = nodePerspective->getHeadNode ();

  auto nodeEntity = cast (Media *, dataObject);
  auto headComposition = cast (Composition *, headNode);
  if (headComposition != nullptr && nodeEntity != nullptr)
    {
      const set<Refer *> *sameInstances
        = nodeEntity->getInstSameInstances ();
      g_assert_nonnull (sameInstances);

      for (Refer *referNode: *(sameInstances))
        addSameInstance (exeObj, referNode);
    }

  delete nodePerspective;

  // Compile execution object links
  for (Node *node : exeObj->getNodes ())
    {
      auto parent = cast
        (ExecutionObjectContext*, exeObj->getParentObject (node));

      g_assert_nonnull (parent);

      compileExecutionObjectLinks (exeObj, node, parent);
    }
}

ExecutionObjectContext *
Converter::getParentExecutionObject (NclNodeNesting *perspective)
{
  NclNodeNesting *parentPerspective;

  if (perspective->getNumNodes () > 1)
    {
      parentPerspective = perspective->copy ();
      parentPerspective->removeAnchorNode ();

      auto cObj = cast (ExecutionObjectContext *,
            this->getExecutionObjectFromPerspective (
              parentPerspective, nullptr));

      g_assert_nonnull (cObj);

      delete parentPerspective;

      return cObj;
    }

  return nullptr;
}

ExecutionObject *
Converter::createExecutionObject (
    const string &id, NclNodeNesting *perspective,
    Descriptor *descriptor)
{
  Node *node;
  NclNodeNesting *nodePerspective;
  ExecutionObject *exeObj;
  PresentationEvent *compositeEvt;

  node = perspective->getAnchorNode ();

  Node *nodeEntity = cast (Node *, node->derefer ());
  g_assert_nonnull (nodeEntity);

  // solve execution object cross reference coming from refer nodes with
  // new instance = false
  auto contentNode = cast (Media *, nodeEntity);
  if (contentNode != nullptr && !contentNode->isSettings ())
    {
      auto referNode = cast (Refer *, node);
      if (referNode)
        {
          nodePerspective
            = new NclNodeNesting (nodeEntity->getPerspective ());

          // verify if both nodes are in the same base.
          if (nodePerspective->getHeadNode ()
              == perspective->getHeadNode ())
            {
              exeObj = getExecutionObjectFromPerspective (nodePerspective,
                                                          descriptor);
              if (exeObj == nullptr)
                {
                  g_assert_nonnull (nodeEntity);
                  exeObj  = new ExecutionObject
                    (_ginga, id, nodeEntity, _actionListener);
                }
            }
          else
            {
              g_assert_nonnull (nodeEntity);
              exeObj = new ExecutionObject
                (_ginga, id, nodeEntity, _actionListener);
            }

          delete nodePerspective;

          if (exeObj != nullptr)
            {
              return exeObj;
            }
        }
    }

  auto switchNode = cast (Switch *, nodeEntity);
  if (switchNode)
    {
      g_assert_nonnull (node);
      exeObj = new ExecutionObjectSwitch (_ginga, id, node, _actionListener);
      compositeEvt = new PresentationEvent (_ginga,
            nodeEntity->getLambda ()->getId () + "<pres>",
            exeObj,
            (Area *)(nodeEntity->getLambda ()));

      exeObj->addEvent (compositeEvt);
      // to monitor the switch presentation and clear the selection after
      // each execution
      compositeEvt->addListener (this);
      _listening.insert (compositeEvt);
    }
  else if (instanceof (Composition* , nodeEntity))
    {
      string s;
      g_assert_nonnull (node);
      exeObj = new ExecutionObjectContext (_ginga, id, node, _actionListener);

      xstrassign (s, "%d", (int) EventType::PRESENTATION);
      compositeEvt = new PresentationEvent (_ginga,
            nodeEntity->getLambda ()->getId () + "<pres>",
            exeObj,
            (Area *)(nodeEntity->getLambda ()));

      exeObj->addEvent (compositeEvt);

      // to monitor the presentation and remove object at stops
      // compositeEvent->addEventListener(this);
    }
  else
    {
      g_assert_nonnull (node);
      if (contentNode->isSettings ())
        {
          if ((exeObj = _ruleAdapter->getSettings ()) != nullptr)
            {
              return exeObj;
            }
          else
            {
              exeObj = new ExecutionObjectSettings
                (_ginga, id, node, _actionListener);
              _ruleAdapter->setSettings (exeObj);
              return exeObj;
            }
        }
      else
        {
          return new ExecutionObject (_ginga, id, node, _actionListener);
        }
    }

  return exeObj;
}

void
Converter::processLink (Link *ncmLink,
                        Node *dataObject,
                        ExecutionObject *executionObject,
                        ExecutionObjectContext *parentObject)
{
  Node *nodeEntity = nullptr;
  const set<Refer *> *sameInstances;
  bool contains = false;

  if (executionObject->getNode () != nullptr)
    nodeEntity = cast (Node *, executionObject->getNode ());

  if (!parentObject->containsUncompiledLink (ncmLink))
    return;

  auto causalLink = cast (Link *, ncmLink);
  g_assert_nonnull (causalLink);

  if (nodeEntity != nullptr && instanceof (Media *, nodeEntity))
    {
      sameInstances = cast (Media *, nodeEntity)
        ->getInstSameInstances ();
      for (Refer *referNode: *sameInstances)
        {
          contains = causalLink->contains (referNode, true);
          if (contains)
            break;
        }
    }

  // Checks if execution object is part of link conditions.
  if (causalLink->contains (dataObject, true) || contains)
    {
      parentObject->removeLinkUncompiled (ncmLink);
      NclFormatterLink *formatterLink
        = createLink (causalLink, parentObject);

      if (formatterLink != NULL)
        {
          // for (auto action: *formatterLink->getActions ())
          //   action->initListener (_actionListener);
          parentObject->setLinkCompiled (formatterLink);
        }
    }
}

void
Converter::compileExecutionObjectLinks (
    ExecutionObject *exeObj, Node *dataObject,
    ExecutionObjectContext *parentObj)
{
  set<Link *> *uncompiledLinks;
  ExecutionObjectContext *compObj;
  Node *execDataObject;

  exeObj->setCompiled (true);

  if (parentObj == nullptr)
    return;

  execDataObject = exeObj->getNode ();
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
            (ExecutionObjectContext *)(parentObj->getParentObject ()));
    }
  else
    {
      ExecutionObject *object;

      delete uncompiledLinks;

      while (parentObj != nullptr)
        {
          object = parentObj;
          parentObj
              = (ExecutionObjectContext *)(parentObj
                                           ->getParentObject ());

          compileExecutionObjectLinks (object, dataObject, parentObj);
        }
    }
}

ExecutionObject *
Converter::processExecutionObjectSwitch (
    ExecutionObjectSwitch *switchObject)
{

  Node *selectedNode;
  NclNodeNesting *selectedPerspective;
  string id;
  map<string, ExecutionObject *>::iterator i;
  ExecutionObject *selectedObject;

  auto switchNode = cast (Switch *, switchObject->getNode ());
  g_assert_nonnull (switchNode);

  selectedNode = _ruleAdapter->adaptSwitch (switchNode);
  g_assert_nonnull (selectedNode);

  selectedPerspective = switchObject->getNodePerspective ();
  selectedPerspective->insertAnchorNode (selectedNode);

  id = selectedPerspective->getId () + "/";

  ExecutionObject *obj = _scheduler->getObjectById (id);
  if (obj != nullptr)
    {
      switchObject->select (obj);
      resolveSwitchEvents (switchObject);
      delete selectedPerspective;
      return obj;
    }

  selectedObject = createExecutionObject (id, selectedPerspective, nullptr);

  delete selectedPerspective;

  if (selectedObject == nullptr)
    {
      // WARNING ("Cannot process '%s' because select object is NULL.",
      //          switchObject->getId ().c_str ());
      return nullptr;
    }

  addExecutionObject (selectedObject, switchObject);
  switchObject->select (selectedObject);
  resolveSwitchEvents (switchObject);

  return selectedObject;
}

void
Converter::resolveSwitchEvents (
    ExecutionObjectSwitch *switchObject)
{
  ExecutionObject *selectedObject;
  ExecutionObject *endPointObject;
  Node *selectedNode;
  vector<NclEvent *> events;
  vector<NclEvent *>::iterator i;
  SwitchEvent *switchEvent;
  Anchor *interfacePoint;
  vector<Node *> nestedSeq;
  NclNodeNesting *nodePerspective;
  NclEvent *mappedEvent;

  selectedObject = switchObject->getSelectedObject ();
  g_assert_nonnull (selectedObject);

  selectedNode = selectedObject->getNode ();
  selectedNode = cast (Node *, selectedNode);
  g_assert_nonnull (selectedNode);

  for (NclEvent *event: switchObject->getEvents ())
    {
      mappedEvent = nullptr;
      switchEvent = cast (SwitchEvent *, event);
      g_assert_nonnull (switchEvent);

      interfacePoint = switchEvent->getInterface ();
      auto lambdaAnchor = cast (AreaLambda *, interfacePoint);
      if (lambdaAnchor)
        {
          mappedEvent = getEvent (
                selectedObject, selectedNode->getLambda (),
                switchEvent->getType (), switchEvent->getKey ());
        }
      else
        {
          auto switchPort = cast (SwitchPort *, interfacePoint);
          g_assert_nonnull (switchPort);

          for (Port *mapping: *(switchPort->getPorts ()))
            {
              if (mapping->getNode () == selectedNode)
                {
                  nodePerspective
                      = switchObject->getNodePerspective ();

                  nestedSeq = mapping->getMapNodeNesting ();
                  nodePerspective->append (&nestedSeq);

                  endPointObject
                      = getExecutionObjectFromPerspective (
                        nodePerspective, nullptr);

                  if (endPointObject != nullptr)
                    {
                      mappedEvent = getEvent (
                            endPointObject,
                            mapping->getFinalInterface (),
                            switchEvent->getType (),
                            switchEvent->getKey ());
                    }

                  delete nodePerspective;

                  break;
                }
            }
        }

      if (mappedEvent != nullptr)
        {
          switchEvent->setMappedEvent (mappedEvent);
        }
    }
}

NclEvent *
Converter::insertNode (NclNodeNesting *perspective,
                       Anchor *interfacePoint,
                       Descriptor *descriptor)
{
  ExecutionObject *executionObject;
  NclEvent *event;
  EventType eventType;

  event = nullptr;
  executionObject = getExecutionObjectFromPerspective (perspective,
                                                       descriptor);

  if (executionObject != nullptr)
    {
      if (!(instanceof (Property *, interfacePoint)))
        {
          eventType = EventType::PRESENTATION;
        }
      else
        {
          eventType = EventType::ATTRIBUTION;
        }

      // get the event corresponding to the node anchor
      event = getEvent (executionObject, interfacePoint, eventType, "");
    }

  return event;

}

NclEvent *
Converter::insertContext (NclNodeNesting *persp,
                          Port *port)
{
  Anchor *anchor;
  vector<Node *> nestedSeq;
  NclNodeNesting *perspective;
  NclEvent *newEvent;

  g_assert_nonnull (persp);
  g_assert_nonnull (port);

  anchor = port->getFinalInterface ();

  if (!(instanceof (Area *, anchor)
        || instanceof (AreaLabeled *, anchor)
        || instanceof (Property *, anchor)
        || instanceof (SwitchPort *, anchor)
        || !(instanceof (Context *,
                         persp->getAnchorNode ()))))
    {
      ERROR ("invalid interface point for port");
    }

  nestedSeq = port->getMapNodeNesting ();
  perspective = new NclNodeNesting (persp);
  perspective->append (&nestedSeq);

  newEvent = insertNode (perspective,
                         port->getFinalInterface (),
                         nullptr);
  delete perspective;

  return newEvent;
}

void
Converter::eventStateChanged (NclEvent *event,
                              EventStateTransition transition,
                              unused (EventState previousState))
{
  ExecutionObject *exeObj = event->getExecutionObject ();
  auto exeSwitch = cast (ExecutionObjectSwitch *, exeObj);

  if (exeSwitch)
    {
      if (transition == EventStateTransition::START)
        {
          for (NclEvent *e: exeSwitch->getEvents())
            {
              auto switchEvt = cast (SwitchEvent *, e);
              if (switchEvt)
                {
                  NclEvent *ev = switchEvt->getMappedEvent ();

                  if (ev == nullptr)
                    {
                      // there is only one way to start a switch with
                      // NULL mapped event: a instSame refernode inside
                      // it was started
                      processExecutionObjectSwitch (exeSwitch);

                      ev = switchEvt->getMappedEvent ();
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

      if (transition == EventStateTransition::STOP
          || transition == EventStateTransition::ABORT)
        {
          exeSwitch->select (NULL);
        }
    }
}

NclEvent *
Converter::createEvent (Bind *bind, ExecutionObjectContext *parentObject)
{
  NclNodeNesting *endPointNodeSequence;
  NclNodeNesting *endPointPerspective;
  Node *parentNode;
  ExecutionObject *executionObject;
  Anchor *interfacePoint;
  string key;
  NclEvent *event = nullptr;
  vector<Node *> seq;

  endPointPerspective = parentObject->getNodePerspective ();

  parentNode = endPointPerspective->getAnchorNode ();

  Node *node = bind->getNode ();
  g_assert_nonnull (node);

  interfacePoint = bind->getInterface ();

  //seq = bind->getNodeNesting ();
  seq.push_back (node);
  if (interfacePoint != nullptr
      && instanceof (Port *, interfacePoint)
      && !(instanceof (SwitchPort *, interfacePoint)))
    {
      for (auto inner: ((Port *) interfacePoint)->getMapNodeNesting ())
        seq.push_back (inner);
    }

  endPointNodeSequence = new NclNodeNesting (&seq);
  if (endPointNodeSequence->getAnchorNode ()
      != endPointPerspective->getAnchorNode ()
      && endPointNodeSequence->getAnchorNode () != parentNode)
    {
      endPointPerspective->append (endPointNodeSequence);
    }

  delete endPointNodeSequence;

  executionObject = getExecutionObjectFromPerspective (
        endPointPerspective, nullptr);

  if (executionObject == nullptr)
    {
      delete endPointPerspective;
      return nullptr;
    }


  if (interfacePoint == nullptr)
    {
      // TODO: This is an error, the formatter then return the main event
      // WARNING ("Can't find an interface point for '%s' bind '%s'.",
      //          endPointPerspective->getId ().c_str (),
      //          bind->getRole ()->getLabel ().c_str ());
      delete endPointPerspective;

      return executionObject->getWholeContentPresentationEvent ();
    }

  if (instanceof (Composition *, node)
      && instanceof (Port *, interfacePoint))
    {
      Composition *comp = cast (Composition *, node);
      Port *port = cast (Port *, interfacePoint);
      interfacePoint = comp->getMapInterface (port);
    }

  key = getBindKey (bind);
  event = getEvent (executionObject, interfacePoint,
                    bind->getRole ()->getEventType (), key);

  delete endPointPerspective;
  return event;
}

string
Converter::getBindKey (Bind *ncmBind)
{
  Role *role;
  string keyValue;
  string key;

  role = ncmBind->getRole ();
  if (role == nullptr)
    {
      return "";
    }

  if (auto sc = cast (Condition *, role))
    {
      keyValue = sc->getKey ();
    }
  else if (auto attrAssessment = cast (AttributeAssessment *, role))
    {
      keyValue = attrAssessment->getKey ();
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
    {
      key = ncmBind->getParameter (keyValue.substr (1, keyValue.length () - 1));
    }
  else
    {
      key = keyValue;
    }

  return key;
}


// INSANITY ABOVE ----------------------------------------------------------

NclFormatterLink *
Converter::createLink (Link *docLink, ExecutionObjectContext *context)
{
  Connector *connector;
  NclFormatterLink *link;

  g_assert_nonnull (docLink);
  g_assert_nonnull (context);

  connector = cast (Connector *, docLink->getConnector ());
  g_assert_nonnull (connector);

  link = new NclFormatterLink (context);

  // Add conditions.
  for (auto connCond: *connector->getConditions ())
    {
      for (auto bind: docLink->getBinds (connCond))
        {
          NclCondition *cond;
          cond = createCondition (connCond, bind, context);
          g_assert_nonnull (cond);
          g_assert (link->addCondition (cond));
        }
    }

  // Add actions.
  for (auto connAct: *connector->getActions ())
    {
      for (auto bind: docLink->getBinds (connAct))
        {
          NclAction *action;
          action = createAction (connAct, bind, context);
          g_assert_nonnull (action);
          g_assert (link->addAction (action));
        }
    }

  return link;
}

NclCondition *
Converter::createCondition (Condition *connCondition, Bind *bind,
                            ExecutionObjectContext *context)
{
  NclEvent *event;

  g_assert_nonnull (connCondition);
  g_assert_nonnull (bind);
  g_assert_nonnull (context);

  event = createEvent (bind, context);
  g_assert_nonnull (event);
  return new NclCondition (event, connCondition->getTransition ());
}

NclAction *
Converter::createAction (Action *connAction, Bind *bind,
                         ExecutionObjectContext *context)
{
  EventType eventType;
  EventStateTransition transition;

  NclEvent *event;
  NclAction *action;

  g_assert_nonnull (connAction);
  g_assert_nonnull (bind);
  g_assert_nonnull (context);

  eventType = bind->getRole ()->getEventType ();
  transition = connAction->getTransition ();

  event = createEvent (bind, context);
  g_assert_nonnull (event);
  event->setType (eventType);

  action = new NclAction (event, transition, _actionListener);
  if (eventType == EventType::ATTRIBUTION)
    {
      string dur;
      string value;

      dur = connAction->getDuration ();
      if (dur[0] == '$')
          dur = bind->getParameter (dur.substr (1, dur.length () - 1));

      value = connAction->getValue ();
      if (value[0] == '$')
        value = bind->getParameter (value.substr (1, value.length () - 1));

      g_assert (dur[0] != '$');
      g_assert (value[0] != '$');

      action->setDuration (dur);
      action->setValue (value);
    }

  return action;
}


GINGA_FORMATTER_END
