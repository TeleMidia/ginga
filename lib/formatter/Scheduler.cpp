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
#include "Scheduler.h"
#include "Converter.h"

#include "ncl/ParserXercesC.h"
#include "ncl/ParserLibXML.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN


// Public.

Scheduler::Scheduler (GingaInternal *ginga)
{
  g_assert_nonnull (ginga);
  _ginga = ginga;
  _ginga->setData ("scheduler", this);
  _converter = nullptr;
  _doc = nullptr;
}

Scheduler::~Scheduler ()
{
  for (auto obj: _objects)
    delete obj;
  _objects.clear ();

  for (auto evt: _events)
    delete evt;
  _events.clear ();

  if (_converter != nullptr)
    delete _converter;
}

bool
Scheduler::run (const string &file, string *errmsg)
{
  string id;
  Context *body;
  const vector<Port *> *ports;
  vector<NclEvent *> *entryevts;
  NclNodeNesting *persp;
  int w, h;

  // Parse document.
  w = _ginga->getOptionInt ("width");
  h = _ginga->getOptionInt ("height");

  if (!_ginga->getOptionBool ("experimental"))
    _doc = ParserXercesC::parse (file, w, h, errmsg);
  else
    _doc = ParserLibXML::parseFile (file, w, h, errmsg);
  if (unlikely (_doc == nullptr))
    return false;               // syntax error

  id = _doc->getId ();
  body = _doc->getRoot ();
  g_assert_nonnull (body);

  // Get entry events (i.e., those mapped by ports).
  ports = body->getPorts ();
  g_assert_nonnull (ports);
  if (unlikely (ports->size () == 0))
    {
      *errmsg = "Document has no ports";
      return false;
    }

  // Insert dummy settings node.
  Media *dummy =  new Media (_doc, "__settings__", true);
  body->addNode (dummy);
  Property *prop = new Property (_doc, "service.currentFocus");
  prop->setValue ("");
  dummy->addAnchor (prop);

  // Create and load converter.
  _converter = new Converter (_ginga, this, new RuleAdapter ());
  persp = new NclNodeNesting ();
  persp->insertAnchorNode (body);
  entryevts = new vector<NclEvent *>;
  for (auto port: *ports)
    {
      NclEvent *evt = _converter->insertContext (persp, port);
      g_assert_nonnull (evt);
      entryevts->push_back (evt);
    }
  delete persp;
  g_assert_false (entryevts->empty ()); // doc has no ports

  // Create execution object for settings node and initialize it.
  ExecutionObjectSettings *settings = nullptr;
  vector <Node *> *nodes = _doc->getSettingsNodes ();
  for (auto node: *nodes)
    {
      Media *content;

      if (settings == nullptr)
        {
          ExecutionObject *obj;
          persp = new NclNodeNesting (node->getPerspective ());
          obj = _converter
            ->getExecutionObjectFromPerspective (persp);
          g_assert_nonnull (obj);
          delete persp;

          settings = cast (ExecutionObjectSettings *, obj);
          g_assert_nonnull (settings);
        }

      content = (Media *) node;
      for (auto anchor: *content->getAnchors ())
        {
          Property *prop;
          string name;
          string value;

          if (!instanceof (Property *, anchor))
            continue;           // nothing to do

          prop = cast (Property *, anchor);
          name = prop->getName ();
          value = prop->getValue ();
          if (value == "")
            continue;           // nothing to do

          cast (ExecutionObject *, settings)
            ->setProperty (name, "", value, 0);
        }
    }
  delete nodes;

  // Set global settings object.
  g_assert_nonnull (settings);
  _ginga->setData ("settings", settings);

  // Start entry events.
  for (auto event: *entryevts)
    {
      NclAction *fakeAction;
      fakeAction = new NclAction (event, EventStateTransition::START, this);
      runAction (event, fakeAction);
      delete fakeAction;
    }
  delete entryevts;

  // Refresh current focus.
  settings->updateCurrentFocus ("");

  // Success.
  return true;
}

#define SET_ACCESS_DEFN(Name, Type, Var)                \
  set<Type *> *                                         \
  Scheduler::get##Name##s ()                            \
  {                                                     \
    return &(Var);                                      \
  }                                                     \
  bool                                                  \
  Scheduler::has##Name (Type *elt)                      \
  {                                                     \
    set<Type *>::iterator it;                           \
    if ((it = (Var).find (elt)) == (Var).end ())        \
      return false;                                     \
    return true;                                        \
  }                                                     \
  Type *                                                \
  Scheduler::get##Name##ById (const string &id)         \
  {                                                     \
    for (auto *elt: (Var))                              \
      if (elt->getId () == id)                          \
        return elt;                                     \
    return nullptr;                                     \
  }                                                     \
  void                                                  \
  Scheduler::add##Name (Type *elt)                      \
  {                                                     \
    g_assert_nonnull (elt);                             \
    (Var).insert (elt);                                 \
  }

SET_ACCESS_DEFN (Object, ExecutionObject, _objects)
SET_ACCESS_DEFN (Event, NclEvent, _events)

ExecutionObject *
Scheduler::getObjectByIdOrAlias (const string &id)
{
  ExecutionObject *obj;
  if ((obj = this->getObjectById (id)) != nullptr)
    return obj;
  for (auto obj: _objects)
    if (obj->hasAlias (id))
      return obj;
  return nullptr;
}

void
Scheduler::scheduleAction (NclAction *action)
{
  runAction (action->getEvent (), action);
}


// Private.

void
Scheduler::runAction (NclEvent *event, NclAction *action)
{
  ExecutionObject *obj;
  string name;

  obj = event->getExecutionObject ();
  g_assert_nonnull (obj);

  switch (action->getEventStateTransition ())   // fixme
    {
    case EventStateTransition::START:
      name = "start";
      break;
    case EventStateTransition::PAUSE:
      name = "pause";
      break;
    case EventStateTransition::RESUME:
      name = "resume";
      break;
    case EventStateTransition::STOP:
      name = "stop";
      break;
    case EventStateTransition::ABORT:
      name = "abort";
      break;
    default:
      g_assert_not_reached ();
    }

  TRACE ("running %s over %s",
         name.c_str (), obj->getId ().c_str ());

  if (instanceof (SelectionEvent *, event))
    {
      event->start ();
      delete action;
      return;
    }

  if (instanceof (ExecutionObjectSwitch *, obj)
      && instanceof (SwitchEvent *, event))
    {
      this->runActionOverSwitch ((ExecutionObjectSwitch *) obj,
                                 (SwitchEvent *) event, action);
      return;
    }

  if (instanceof (ExecutionObjectContext *, obj))
    {
      this->runActionOverComposition
        ((ExecutionObjectContext *) obj, action);
      return;
    }

  if (instanceof (AttributionEvent *, event))
    {
      AttributionEvent *attevt;
      Property *property;

      string name;
      string from;
      string to;

      GingaTime dur;

      g_assert (action->getEventStateTransition () == EventStateTransition::START);

      attevt = (AttributionEvent *) event;
      if (event->getCurrentState () != EventState::SLEEPING)
        return;                 // nothing to do

      property = attevt->getAnchor ();
      g_assert_nonnull (property);

      name = property->getName ();
      from = property->getValue ();
      to = attevt->solveImplicitRefAssessment (action->getValue ());

      string s;
      s = attevt->solveImplicitRefAssessment (action->getDuration ());
      dur = ginga_parse_time (s);

      attevt->start ();
      attevt->setValue (to);
      obj->setProperty (name, from, to, dur);

      // TODO: Wrap this in a closure to be called at the end of animation.
      attevt->stop ();
      return;
    }

  switch (action->getEventStateTransition ())
    {
    case EventStateTransition::START:
      obj->prepare (event);
      g_assert (obj->start ());
      break;
    case EventStateTransition::STOP:
      obj->stop ();
      break;
    case EventStateTransition::PAUSE:
      g_assert (obj->pause ());
      break;
    case EventStateTransition::RESUME:
      g_assert (obj->resume ());
      break;
    case EventStateTransition::ABORT:
      g_assert (obj->abort ());
      break;
    default:
      g_assert_not_reached ();
    }
}

void
Scheduler::runActionOverComposition (ExecutionObjectContext *ctxObj,
                                     NclAction *action)
{
  NclEvent *event;
  EventType type;
  EventStateTransition acttype;

  Node *node;
  Entity *entity;
  Composition *compNode;
  NclNodeNesting *compPerspective;

  event = action->getEvent ();
  g_assert_nonnull (event);

  type = event->getType ();

  if (type == EventType::ATTRIBUTION)
    {
      ERROR_NOT_IMPLEMENTED
        ("context property attribution is not supported");
    }

  if (type == EventType::SELECTION)
    {
      WARNING ("trying to select composition '%s'",
               ctxObj->getId ().c_str ());
      return;
    }

  node = ctxObj->getNode ();
  g_assert_nonnull (node);

  entity = cast (Entity *, node);
  g_assert_nonnull (entity);

  compNode = cast (Composition *, entity);
  g_assert_nonnull (compNode);

  if (compNode->getParent () == nullptr)
    {
      compPerspective
        = new NclNodeNesting (compNode->getPerspective ());
    }
  else
    {
      compPerspective
        = ctxObj->getNodePerspective ();
    }

  acttype = action->getEventStateTransition ();
  if (acttype == EventStateTransition::START) // start all ports
    {
      ctxObj->suspendLinkEvaluation (false);
      for (auto port: *compNode->getPorts ())
        {
          NclNodeNesting *persp;
          vector<Node *> nestedSeq;
          Anchor *iface;

          ExecutionObject *child;
          NclEvent *evt;

          persp = compPerspective->copy ();
          g_assert_nonnull (persp);

          nestedSeq = port->getMapNodeNesting ();
          persp->append (&nestedSeq);

          // Create or get the execution object mapped by port.
          child = _converter
            ->getExecutionObjectFromPerspective (persp);
          g_assert (child);

          iface = port->getFinalInterface ();
          g_assert_nonnull (iface);

          if (!instanceof (Area *, iface))
            continue;           // nothing to do

          evt = _converter->getEvent (child, iface,
                                      EventType::PRESENTATION, "");
          g_assert_nonnull (evt);
          g_assert (instanceof (PresentationEvent *, evt));

          runAction (evt, action);

          delete persp;
        }
    }
  else if (acttype == EventStateTransition::STOP) // stop all children
    {
      ctxObj->suspendLinkEvaluation (true);
      for (const auto pair: *ctxObj->getExecutionObjects ())
        {
          ExecutionObject *child;
          NclEvent *evt;

          child = pair.second;
          evt = child->getMainEvent ();
          if (evt == nullptr)
            evt = child->getWholeContentPresentationEvent ();
          if (evt == nullptr)
            continue;
          runAction (evt, action);
        }
      ctxObj->suspendLinkEvaluation (false);
    }
  else if (acttype == EventStateTransition::ABORT)
    {
      ERROR_NOT_IMPLEMENTED ("action 'abort' is not supported");
    }
  else if (acttype == EventStateTransition::PAUSE)
    {
      ERROR_NOT_IMPLEMENTED ("action 'pause' is not supported");
    }
  else if (acttype == EventStateTransition::RESUME)
    {
      ERROR_NOT_IMPLEMENTED ("action 'resume' is not supported");
    }
  else
    {
      g_assert_not_reached ();
    }
  delete compPerspective;
}

void
Scheduler::runActionOverSwitch (ExecutionObjectSwitch *switchObj,
                                SwitchEvent *event,
                                NclAction *action)
{
  ExecutionObject *selectedObject;
  NclEvent *selectedEvent;

  selectedObject = switchObj->getSelectedObject ();
  if (selectedObject == nullptr)
    {
      selectedObject = _converter->processExecutionObjectSwitch (switchObj);

      if (selectedObject == nullptr)
        {
          return;
        }
    }

  selectedEvent = event->getMappedEvent ();
  if (selectedEvent != nullptr)
    {
      runAction (selectedEvent, action);
    }
  else
    {
      runSwitchEvent (switchObj, event, selectedObject, action);
    }

  if (action->getEventStateTransition () == EventStateTransition::STOP
      || action->getEventStateTransition () == EventStateTransition::ABORT)
    {
      switchObj->select (nullptr);
    }
}

void
Scheduler::runSwitchEvent (ExecutionObjectSwitch *switchObj,
                                    SwitchEvent *switchEvent,
                                    ExecutionObject *selectedObject,
                                    NclAction *action)
{
  NclEvent *selectedEvent;
  SwitchPort *switchPort;
  vector<Port *>::iterator i;
  NclNodeNesting *nodePerspective;
  vector<Node *> nestedSeq;
  ExecutionObject *endPointObject;

  selectedEvent = nullptr;
  switchPort = (SwitchPort *)(switchEvent->getInterface ());
  for (auto mapping: *switchPort->getPorts ())
    {
      if (mapping->getNode () == selectedObject->getNode ())
        {
          nodePerspective = switchObj->getNodePerspective ();
          nestedSeq = mapping->getMapNodeNesting ();
          nodePerspective->append (&nestedSeq);
          try
            {
              endPointObject
                = _converter
                ->getExecutionObjectFromPerspective (nodePerspective);

              if (endPointObject != nullptr)
                {
                  selectedEvent
                    = _converter
                    ->getEvent (
                                endPointObject,
                                mapping->getFinalInterface (),
                                switchEvent->getType (),
                                switchEvent->getKey ());
                }
            }
          catch (exception *exc)
            {
              // continue
            }

          break;
        }
    }

  if (selectedEvent != nullptr)
    {
      switchEvent->setMappedEvent (selectedEvent);
      runAction (selectedEvent, action);
    }
}

GINGA_FORMATTER_END
