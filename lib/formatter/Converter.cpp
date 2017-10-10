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
#include "Scheduler.h"

GINGA_FORMATTER_BEGIN

Converter::Converter (GingaInternal *ginga, RuleAdapter *ruleAdapter)
{
  g_assert_nonnull (ginga);
  _ginga = ginga;
  _scheduler = ginga->getScheduler ();
  g_assert_nonnull (_scheduler);
  _ruleAdapter = ruleAdapter;
}

Converter::~Converter ()
{
}

NclEvent *
Converter::obtainEvent (ExecutionObject *object,
                        Anchor *iface,
                        EventType eventType,
                        const string &key)
{
  string id;
  NclEvent *event;
  string type;

  id = iface->getId ();
  switch (eventType)
    {
    case EventType::SELECTION:
      id += "<sel";
      if (key != "")
        id += "(" + key + ")";
      id += ">";
      break;
    case EventType::PRESENTATION:
      id += "<pres>";
      break;
    case EventType::ATTRIBUTION:
      id += "<attr>";
      break;
    default:
      g_assert_not_reached ();
    }

  event = object->getEventById (id);
  if (event != nullptr)
    return event;

  if (instanceof (ExecutionObjectSwitch *, object))
    {
      event = new SwitchEvent
        (_ginga, id, object, iface, eventType, key);
    }
  else if (instanceof (ExecutionObjectContext *, object))
    {
      g_assert (eventType == EventType::PRESENTATION);
      event = new PresentationEvent (_ginga, id, object, (Area *) iface);
    }
  else
    {
      switch (eventType)
        {
        case EventType::PRESENTATION:
          event = new PresentationEvent (_ginga, id, object, (Area *) iface);
          break;
        case EventType::ATTRIBUTION:
          {
            auto propAnchor = cast (Property *, iface);
            g_assert_nonnull (propAnchor);
            event = new AttributionEvent (_ginga, id, object, propAnchor);
            break;
          }
        case EventType::SELECTION:
          {
            event = new SelectionEvent (_ginga, id, object, (Area *) iface);
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
  object->addEvent (event);
  return event;
}


void
Converter::processLink (Link *docLink,
                        Node *node,
                        ExecutionObject *object,
                        ExecutionObjectContext *parentObject)
{
  Node *nodeEntity = nullptr;
  const set<Refer *> *sameInstances;
  bool contains = false;

  if (object->getNode () != nullptr)
    nodeEntity = cast (Node *, object->getNode ());

  if (!parentObject->containsUncompiledLink (docLink))
    return;

  auto causalLink = cast (Link *, docLink);
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
  if (causalLink->contains (node, true) || contains)
    {
      parentObject->removeLinkUncompiled (docLink);
      NclLink *formatterLink = createLink (causalLink);

      if (formatterLink != NULL)
        parentObject->setLinkCompiled (formatterLink);
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
      compObj = parentObj->getParent ();
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

      for ( Link *docLink : *dataLinks)
        {
          processLink (docLink, dataObject, exeObj, parentObj);
        }

      delete dataLinks;

      compileExecutionObjectLinks (
            exeObj, dataObject,
            parentObj->getParent ());
    }
  else
    {
      ExecutionObject *object;

      delete uncompiledLinks;

      while (parentObj != nullptr)
        {
          object = parentObj;
          parentObj = parentObj->getParent ();
          compileExecutionObjectLinks (object, dataObject, parentObj);
        }
    }
}

ExecutionObject *
Converter::processExecutionObjectSwitch (
    ExecutionObjectSwitch *switchObject)
{

  Node *selectedNode;
  string id;
  map<string, ExecutionObject *>::iterator i;
  ExecutionObject *selectedObject;

  auto switchNode = cast (Switch *, switchObject->getNode ());
  g_assert_nonnull (switchNode);

  selectedNode = _ruleAdapter->adaptSwitch (switchNode);
  g_assert_nonnull (selectedNode);

  ExecutionObject *obj = _scheduler->getObjectByIdOrAlias (selectedNode->getId ());
  if (obj != nullptr)
    {
      switchObject->select (obj);
      resolveSwitchEvents (switchObject);
      return obj;
    }

  selectedObject = obtainExecutionObject (selectedNode);
  g_assert_nonnull (selectedNode);

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
  NclEvent *mappedEvent;

  selectedObject = switchObject->getSelectedObject ();
  g_assert_nonnull (selectedObject);

  selectedNode = selectedObject->getNode ();
  selectedNode = cast (Node *, selectedNode);
  g_assert_nonnull (selectedNode);

  for (auto event: *(switchObject->getEvents ()))
    {
      mappedEvent = nullptr;
      switchEvent = cast (SwitchEvent *, event);
      g_assert_nonnull (switchEvent);

      interfacePoint = switchEvent->getInterface ();
      auto lambdaAnchor = cast (AreaLambda *, interfacePoint);
      if (lambdaAnchor)
        {
          mappedEvent = obtainEvent (
                selectedObject, selectedNode->getLambda (),
                switchEvent->getType (), switchEvent->getKey ());
        }
      else
        {
          auto switchPort = cast (SwitchPort *, interfacePoint);
          g_assert_nonnull (switchPort);

          for (Port *mapping: *(switchPort->getPorts ()))
            {
              mapping->getTarget (&selectedNode, &interfacePoint);
              endPointObject = obtainExecutionObject (selectedNode);
              g_assert_nonnull (endPointObject);
              mappedEvent = obtainEvent (endPointObject,
                                      interfacePoint,
                                      switchEvent->getType (),
                                      switchEvent->getKey ());
              break;
            }
        }

      if (mappedEvent != nullptr)
        {
          switchEvent->setMappedEvent (mappedEvent);
        }
    }
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
          for (auto e: *(exeSwitch->getEvents()))
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
Converter::createEvent (Bind *bind)
{
  ExecutionObject *executionObject;
  Anchor *interfacePoint;
  string key;
  NclEvent *event = nullptr;

  Node *node = bind->getNode ();
  g_assert_nonnull (node);

  interfacePoint = bind->getInterface ();

  if (interfacePoint != nullptr
      && instanceof (Port *, interfacePoint)
      && !(instanceof (SwitchPort *, interfacePoint)))
    {
      cast (Port *, interfacePoint)->getTarget (&node, nullptr);
    }

  executionObject = obtainExecutionObject (node);
  g_assert_nonnull (executionObject);

  if (interfacePoint == nullptr)
    {
      return executionObject->getLambda ();
    }

  if (instanceof (Composition *, node)
      && instanceof (Port *, interfacePoint))
    {
      Composition *comp = cast (Composition *, node);
      Port *port = cast (Port *, interfacePoint);
      interfacePoint = comp->getMapInterface (port);
    }

  if (!getBindKey (bind, &key))
    key = "";
  event = obtainEvent (executionObject, interfacePoint,
                       bind->getRole ()->getEventType (), key);

  return event;
}

bool
Converter::getBindKey (Bind *bind, string *result)
{
  Role *role;
  Condition *docCond;
  string key;

  role = bind->getRole ();
  g_assert_nonnull (role);

  docCond = cast (Condition *, role);
  if (docCond == nullptr)
    return false;

  key = docCond->getKey ();
  if (key[0] == '$')
    key = bind->getParameter (key.substr (1, key.length () - 1));

  tryset (result, key);
  return true;
}


// INSANITY ABOVE ----------------------------------------------------------

ExecutionObject *
Converter::obtainExecutionObject (Node *node)
{
  string id;
  Node *parentNode;
  ExecutionObjectContext *parent;
  ExecutionObject *object;
  PresentationEvent *event;

  id = node->getId ();
  g_assert (id != "");

  // Already created.
  if ((object = _scheduler->getObjectByIdOrAlias (id)) != nullptr)
    return object;

  // Get parent.
  parentNode = node->getParent ();
  if (parentNode == nullptr)
    {
      parent = nullptr;
    }
  else
    {
      parent = cast (ExecutionObjectContext *,
                     obtainExecutionObject (parentNode));
      g_assert_nonnull (parent);
    }

  if (instanceof (Refer *, node))
    {
      Node *target;

      TRACE ("solving refer %s", node->getId ().c_str ());
      target = node->derefer ();
      g_assert (!instanceof (Refer *, target));
      object = obtainExecutionObject (target->derefer ());
      object->addAlias (id);
      return object;
    }

  if (instanceof (Switch *, node))
    {
      TRACE ("creating switch %s", node->getId ().c_str ());
      object = new ExecutionObjectSwitch (_ginga, id, node);
      event = new PresentationEvent
        (_ginga, node->getLambda ()->getId () + "<pres>", object,
         (Area *)(node->getLambda ()));
      object->addEvent (event);
      goto done;
    }

  if (instanceof (Context *, node))
    {
      TRACE ("creating context %s", node->getId ().c_str ());
      object = new ExecutionObjectContext (_ginga, id, node);
      event = new PresentationEvent
        (_ginga, node->getLambda ()->getId () + "<pres>", object,
         (Area *)(node->getLambda ()));
      object->addEvent (event);

      // Context *docCtx= cast (Context *, node);
      // ExecutionObjectContext *ctx
      //   = cast (ExecutionObjectContext *, object);
      // if (parent != nullptr)
      //   object->initParent (parent);
      // _scheduler->addObject (object);
      // for (auto link: *(docCtx->getLinks ()))
      //   ctx->setLinkCompiled (createLink (link, ctx));
      goto done;
      return object;
    }

  g_assert (instanceof (Media *, node));
  TRACE ("creating media %s", node->getId ().c_str ());
  Media *media;
  media = cast (Media *, node);
  g_assert_nonnull (media);
  if (media->isSettings ())
    {
      object = new ExecutionObjectSettings (_ginga, id, node);
      _ruleAdapter->setSettings (object);
    }
  else
    {
      object = new ExecutionObject (_ginga, id, node);
      compileExecutionObjectLinks
        (object, node, cast (ExecutionObjectContext *, parent));
    }

 done:
  g_assert_nonnull (object);
  if (parent != nullptr)
    object->initParent (parent);
  _scheduler->addObject (object);
  return object;
}

NclLink *
Converter::createLink (Link *docLink)
{
  Connector *connector;
  NclLink *link;

  g_assert_nonnull (docLink);
  connector = cast (Connector *, docLink->getConnector ());
  g_assert_nonnull (connector);

  link = new NclLink ();

  // Add conditions.
  for (auto connCond: *connector->getConditions ())
    {
      for (auto bind: docLink->getBinds (connCond))
        {
          NclCondition *cond;
          cond = this->createCondition (connCond, bind);
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
          action = this->createAction (connAct, bind);
          g_assert_nonnull (action);
          g_assert (link->addAction (action));
        }
    }

  return link;
}

NclCondition *
Converter::createCondition (Condition *connCondition, Bind *bind)
{
  NclEvent *event;

  g_assert_nonnull (connCondition);
  g_assert_nonnull (bind);

  event = createEvent (bind);
  g_assert_nonnull (event);
  return new NclCondition (event, connCondition->getTransition ());
}

NclAction *
Converter::createAction (Action *connAction, Bind *bind)
{
  EventType eventType;
  EventStateTransition transition;

  NclEvent *event;
  NclAction *action;

  g_assert_nonnull (connAction);
  g_assert_nonnull (bind);

  eventType = bind->getRole ()->getEventType ();
  transition = connAction->getTransition ();

  event = createEvent (bind);
  g_assert_nonnull (event);
  event->setType (eventType);

  action = new NclAction (event, transition, _scheduler);
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
