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
  ProxyEvent *switchEvent;
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
      switchEvent = cast (ProxyEvent *, event);
      g_assert_nonnull (switchEvent);

      interfacePoint = switchEvent->getAnchor ();
      auto lambdaAnchor = cast (AreaLambda *, interfacePoint);
      if (lambdaAnchor)
        {
          mappedEvent = selectedObject->obtainEvent
            (switchEvent->getType (), selectedNode->getLambda (),
             switchEvent->getKey ());
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
              mappedEvent = endPointObject->obtainEvent
                (switchEvent->getType (), interfacePoint,
                 switchEvent->getKey ());
              break;
            }
        }

      if (mappedEvent != nullptr)
        {
          switchEvent->setTarget (mappedEvent);
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
      return executionObject->getLambda (EventType::PRESENTATION);
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
  event = executionObject->obtainEvent
    (bind->getRole ()->getEventType (), interfacePoint, key);

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
      if ((object = _scheduler->getObjectByIdOrAlias (id)) != nullptr)
        return object;
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
      g_assert_nonnull
        (object->obtainEvent (EventType::PRESENTATION,
                              node->getLambda (), ""));
      goto done;
    }

  if (instanceof (Context *, node))
    {
      TRACE ("creating context %s", node->getId ().c_str ());
      object = new ExecutionObjectContext (_ginga, id, node);
      g_assert_nonnull
        (object->obtainEvent (EventType::PRESENTATION,
                              node->getLambda (), ""));

      g_assert_nonnull (object);
      if (parent != nullptr)
        object->initParent (parent);
      g_assert (_scheduler->addObject (object));

      // Process links.
      Context *ctx = cast (Context *, node);
      for (auto link: *(ctx->getLinks ()))
        g_assert_nonnull (createLink (link));

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
    }

 done:
  g_assert_nonnull (object);
  if (parent != nullptr)
    object->initParent (parent);
  g_assert (_scheduler->addObject (object));
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

  link = new NclLink (_ginga);

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

  action = new NclAction (event, transition);
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

      action->setDuration (dur);
      action->setValue (value);
    }

  return action;
}


GINGA_FORMATTER_END
