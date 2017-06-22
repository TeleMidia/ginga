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
#include "Scheduler.h"

#include "Converter.h"
#include "Parser.h"

#include "mb/Display.h"

GINGA_FORMATTER_BEGIN

Scheduler::Scheduler ()
{
  this->settings = new Settings ();
  this->ruleAdapter = new RuleAdapter (settings);
  this->compiler = new Converter (this->ruleAdapter);
  this->compiler->setLinkActionListener (this);
  this->focusManager = new FocusManager
    (this, this->settings, this, this->compiler);
  this->focusManager->setKeyHandler (true);
}

Scheduler::~Scheduler ()
{
  for (auto action: this->actions)
    action->setSimpleActionListener (NULL);

  this->actions.clear ();

  ruleAdapter = NULL;
  settings = NULL;

  if (focusManager != NULL)
    {
      delete focusManager;
      focusManager = NULL;
    }

  compiler = NULL;
  events.clear ();

  for (auto &i: _objectPlayers)
    delete i.second;

  _objectPlayers.clear ();
}

void
Scheduler::addAction (NclLinkSimpleAction *action)
{
  actions.insert (action);
}

void
Scheduler::removeAction (NclLinkSimpleAction *action)
{
  set<NclLinkSimpleAction *>::iterator i;

  i = actions.find (action);
  if (i != actions.end ())
    actions.erase (i);
}

FocusManager *
Scheduler::getFocusManager ()
{
  return focusManager;
}

bool
Scheduler::setKeyHandler (bool isHandler)
{
  return focusManager->setKeyHandler (isHandler);
}

void
Scheduler::scheduleAction (NclLinkSimpleAction *action)
{
  assert (action != NULL);
  runAction (action);
  return;
}

void
Scheduler::runAction (NclLinkSimpleAction *action)
{
  NclEvent *event = action->getEvent ();

  assert (event != NULL);

  if (event->instanceOf ("SelectionEvent"))
    {
      event->start ();
      delete action;
      return;
    }

  runAction (event, action);
}

void
Scheduler::runAction (NclEvent *event, NclLinkSimpleAction *action)
{
  ExecutionObject *obj;
  NclCascadingDescriptor *descriptor;
  PlayerAdapter *player;
  SDLWindow *win = NULL;

  obj = event->getExecutionObject ();
  g_assert_nonnull (obj);

  TRACE ("running action '%s' over event '%s'",
         action->getTypeString ().c_str (),
         event->getId ().c_str ());

  if (obj->instanceOf ("ExecutionObjectSwitch")
      && event->instanceOf ("SwitchEvent"))
    {
      runActionOverSwitch ((ExecutionObjectSwitch *)obj,
                           (SwitchEvent *)event, action);
      return;
    }

  if (obj->instanceOf ("ExecutionObjectContext"))
    {
      runActionOverComposition
        ((ExecutionObjectContext *) obj, action);
      return;
    }

  if (event->instanceOf ("AttributionEvent"))
    {
      runActionOverProperty (event, action);
      return;
    }

  player = this->getObjectPlayer (obj);

  if (unlikely (player == NULL))
    {
      WARNING ("no player to decode '%s', empty object",
               obj->getId ().c_str ());
      return;
    }

  g_assert (!obj->instanceOf ("ExecutionObjectContext"));
  switch (action->getType ())
    {
    case ACT_START:
      {
        if (obj->isOccurring ())
          break;                  // nothing to do

        g_assert (!player->hasPrepared ());

        if (ruleAdapter->adaptDescriptor (obj))
          {
            descriptor = obj->getDescriptor ();
            if (descriptor != NULL)
              descriptor->setFormatterLayout();
          }

        // --------------
        NodeEntity *dataObject;
        NodeEntity *entity;
        Content *content;
        string mime;
        string mrl;

        dataObject = dynamic_cast<NodeEntity *>(obj->getDataObject ());
        g_assert_nonnull (dataObject);

        entity = dynamic_cast <NodeEntity *>(dataObject->getDataEntity ());
        g_assert_nonnull (entity);

        g_assert (entity->instanceOf ("ContentNode"));

        content = dynamic_cast<NodeEntity *>
          (dataObject->getDataEntity ())->getContent();

        if (content)
          {
            ReferenceContent *referContent
              = dynamic_cast <ReferenceContent *>(content);
            g_assert_nonnull (referContent);
            mrl = referContent->getCompleteReferenceUrl ();
          }
        else
          {
            WARNING ("object %s has no content", obj->getId ().c_str ());
            mrl = "";
          }

        mime = ((ContentNode *) entity)->getNodeType ();
        g_assert (player->prepare (mrl, mime));

        NclCascadingDescriptor *descriptor;
        PropertyAnchor *property;

        descriptor = obj->getDescriptor ();
        if (descriptor != nullptr)
          {
            NclFormatterRegion *fregion = descriptor->getFormatterRegion ();
            if (fregion != nullptr)
              {
                LayoutRegion *region;
                SDL_Rect rect;
                int z, zorder;
                region = fregion->getLayoutRegion ();
                g_assert_nonnull (region);
                rect = region->getRect ();
                region->getZ (&z, &zorder);
                player->setRect (rect);
                player->setZ (z, zorder);
              }

            for (Parameter &param: descriptor->getParameters ())
              player->setProperty (param.getName (), param.getValue ());
          }

        ContentNode *contentNode = dynamic_cast <ContentNode *> (dataObject);
        g_assert_nonnull (contentNode);

        for (Anchor *anchor: contentNode->getAnchors ())
          {
            string name, value;

            property = dynamic_cast <PropertyAnchor *> (anchor);
            if (!property)
              continue;
            name = property->getName ();
            value = property->getValue ();
            player->setProperty (name, value);
          }

        for (NclEvent *evt: obj->getEvents ())
          {
            g_assert_nonnull (evt);
            AttributionEvent *attributionEvt
              = dynamic_cast <AttributionEvent *> (evt);
            if (attributionEvt)
              {
                property = attributionEvt->getAnchor ();
                attributionEvt->setPlayerAdapter (player);
              }
          }

        g_assert (event->getCurrentState () == EventState::SLEEPING);
        obj->prepare (event, 0);
        // -------------

        win = this->prepareFormatterRegion (obj);
        player->setOutputWindow (win);
        event->addListener (this);

        player->_object = obj;  // FIXME!!

        g_assert (obj->start ());
        if (unlikely (!player->start ()))
          {
            WARNING ("failed to start player of '%s'",
                     obj->getId ().c_str ());
            if (event->getCurrentState () == EventState::SLEEPING)
              event->removeListener (this);
          }
      break;
      }

    case ACT_PAUSE:
      if (!obj->isOccurring ())
        break;                 // nothing to do
      g_assert (obj->pause ());
      if (unlikely (!player->hasPrepared ()))
        {
          WARNING ("trying to pause an unprepared player: '%s'",
                   obj->getId ().c_str ());
          break;
        }
      g_assert (player->pause ());
      break;

    case ACT_RESUME:
      if (!obj->isPaused ())
        break;                 // nothing to do
      g_assert (obj->resume ());
      if (unlikely (!player->hasPrepared ()))
        {
          WARNING ("trying to resume an unprepared player: '%s'",
                   obj->getId ().c_str ());
          break;
        }
      g_assert (player->resume ());
      break;

    case ACT_ABORT:
      if (obj->isSleeping ())
        break;                  // nothing to do
      for (NclEvent *evt: obj->getEvents ())
        {
          g_assert_nonnull(evt);
          AttributionEvent *attributionEvt
            = dynamic_cast <AttributionEvent *> (evt);
          if (attributionEvt)
            attributionEvt->setPlayerAdapter (nullptr);
        }
      g_assert (obj->abort ());
      if (unlikely (!player->hasPrepared ()))
        {
          WARNING ("trying to abort an unprepared player: '%s'",
                   obj->getId ().c_str ());
          break;
        }
      g_assert (player->abort ());
      this->removePlayer (obj);
      break;

    case ACT_STOP:
      if (obj->isSleeping ())
        break;                  // nothing to do
      for (NclEvent *evt: obj->getEvents ())
        {
          g_assert_nonnull(evt);
          AttributionEvent *attributionEvt
            = dynamic_cast <AttributionEvent *> (evt);
          if (attributionEvt)
            attributionEvt->setPlayerAdapter (nullptr);
        }
      g_assert (obj->stop ());
      g_assert (obj->unprepare ());
      if (unlikely (!player->hasPrepared ()))
        {
          WARNING ("trying to stop an unprepared player: '%s'",
                   obj->getId ().c_str ());
          break;
        }
      g_assert (player->stop ());
      this->removePlayer (obj);
      break;

    default:
      g_assert_not_reached ();
    }
}

void
Scheduler::runActionOverProperty (NclEvent *event,
                                  NclLinkSimpleAction *action)
{
  SimpleActionType actionType;
  string propName, propValue;

  NodeEntity *dataObject;
  ExecutionObject *executionObject;
  PlayerAdapter *player;
  Animation *anim;

  executionObject = (ExecutionObject *)(event->getExecutionObject ());
  dataObject
      = (NodeEntity *)(executionObject->getDataObject ()->getDataEntity ());

  if (dataObject->instanceOf ("ContentNode")
      && ((ContentNode *)dataObject)->isSettingNode ()
      && action->instanceOf ("NclLinkAssignmentAction"))
    {
      propName = ((AttributionEvent *)event)
                     ->getAnchor ()
                     ->getName ();

      propValue = ((NclLinkAssignmentAction *)action)->getValue ();
      if (propValue != "" && propValue.substr (0, 1) == "$")
        {
          propValue = solveImplicitRefAssessment (
              propValue, (AttributionEvent *)event);
        }

      event->start ();
      ((AttributionEvent *)event)->setValue (propValue);

      if (propName == "service.currentFocus")
        {
          focusManager->setFocus (propValue);
        }
      else if (propName == "service.currentKeyMaster")
        {
          focusManager->setKeyMaster (propValue);
        }
      else
        {
          settings->set (propName, propValue);
        }
      event->stop ();
    }
  else
    {
      if (action->instanceOf ("NclLinkAssignmentAction"))
        {
          anim = ((NclLinkAssignmentAction *)action)->getAnimation ();
        }
      else
        {
          anim = NULL;
        }

      player = this->getObjectPlayer (executionObject);
      actionType = action->getType ();

      switch (actionType)
        {
        case ACT_START:
        case ACT_SET:
          clog << "Scheduler::runActionOverProperty";
          clog << " over '" << event->getId () << "' for '";
          clog << executionObject->getId () << "' player '";
          clog << player << "'" << endl;
          if (event->getCurrentState () != EventState::SLEEPING)
            {
              clog << "Scheduler::runActionOverProperty";
              clog << " trying to set an event that is not ";
              clog << "sleeping: '" << event->getId () << "'" << endl;
              return;
            }

          if (action->instanceOf ("NclLinkAssignmentAction"))
            {
              propValue = ((NclLinkAssignmentAction *)action)->getValue ();
              if (propValue != "" && propValue.substr (0, 1) == "$")
                {
                  propValue = solveImplicitRefAssessment (
                      propValue, (AttributionEvent *)event);

                  clog << "Scheduler::runActionOverProperty";
                  clog << " IMPLICIT found '" << propValue;
                  clog << "' for event '" << event->getId () << "'";
                  clog << endl;
                }

              event->start ();
              ((AttributionEvent *)event)->setValue (propValue);
            }
          else
            {
              event->start ();
            }

          if (anim != NULL)
            {
              string durVal, byVal;

              durVal = anim->getDuration ();
              if (durVal.substr (0, 1) == "$")
                {
                  anim->setDuration (solveImplicitRefAssessment (
                      durVal, (AttributionEvent *)event));
                }

              byVal = anim->getBy ();
              if (byVal.substr (0, 1) == "$")
                {
                  anim->setDuration (solveImplicitRefAssessment (
                      byVal, (AttributionEvent *)event));
                }

              anim->setDuration (durVal);
              anim->setBy (byVal);

              player->getPlayer()->
                setAnimatorProperties(durVal,((AttributionEvent *)event)
                                      ->getAnchor ()
                                      ->getName (), propValue);
            }
          else if (player != NULL && player->hasPrepared ())
            {
              player->setProperty ((AttributionEvent *)event,
                                        propValue);

              event->stop ();
            }
          else
            {
              event->stop ();
            }

          break;

        default:
          g_assert_not_reached ();
        }
    }
}

void
Scheduler::runActionOverComposition (ExecutionObjectContext *compObj,
                                     NclLinkSimpleAction *action)
{
  CompositeNode *compositeNode;
  Port *port;
  NclNodeNesting *compositionPerspective;
  NclNodeNesting *perspective;

  map<string, ExecutionObject *> *objects;
  map<string, ExecutionObject *>::iterator j;
  ExecutionObject *childObject;

  PlayerAdapter *pAdapter;
  AttributionEvent *attrEvent;
  NclEvent *event;
  string propName;
  string propValue;

  vector<Node *> *nestedSeq;

  NclEvent *childEvent;
  size_t i, size = 0;
  vector<NclEvent *> *events;
  EventType eventType = EventType::UNKNOWN;

  clog << "Scheduler::runActionOverComposition ";
  clog << "action '" << action->getType () << "' over COMPOSITION '";
  clog << compObj->getId () << "'" << endl;

  if (action->getType () == ACT_START
      || action->getType () == ACT_SET)
    {
      event = action->getEvent ();
      if (event != NULL)
        {
          eventType = event->getType ();
          if (eventType == EventType::UNKNOWN)
            {
              if (event->instanceOf ("PresentationEvent"))
                {
                  eventType = EventType::PRESENTATION;
                }
              else if (event->instanceOf ("AttributionEvent"))
                {
                  eventType = EventType::ATTRIBUTION;
                }
              else if (event->instanceOf ("SwitchEvent"))
                {
                  eventType = EventType::PRESENTATION;
                }
              else
                {
                  clog << "Scheduler::runActionOverComposition ";
                  clog << "Warning! action '" << action->getType () << "'";
                  clog << " over COMPOSITION '";
                  clog << compObj->getId ();
                  clog << "' has an unknown eventType" << endl;
                }
            }
        }

      if (eventType == EventType::ATTRIBUTION)
        {
          event = action->getEvent ();
          if (!event->instanceOf ("AttributionEvent"))
            {
              clog << "Scheduler::runActionOverComposition SET ";
              clog << "Warning! event ins't of attribution type";
              clog << endl;
              return;
            }

          attrEvent = (AttributionEvent *)event;
          propName = attrEvent->getAnchor ()->getName ();
          propValue = ((NclLinkAssignmentAction *)action)->getValue ();
          event = compObj->getEventFromAnchorId (propName);

          if (event != NULL)
            {
              event->start ();
              ((AttributionEvent *)event)->setValue (propValue);
              event->stop ();
            }
          else
            {
              attrEvent->stop ();
            }

          objects = compObj->getExecutionObjects ();
          if (objects == NULL)
              return;

          j = objects->begin ();
          while (j != objects->end ())
            {
              childObject = j->second;
              if (childObject->instanceOf ("ExecutionObjectContext"))
                {
                  clog << "Scheduler::runActionOverComposition ";
                  clog << "'" << compObj->getId () << "' has '";
                  clog << childObject->getId () << "' as its child ";
                  clog << "using recursive call";
                  clog << endl;

                  runActionOverComposition (
                      (ExecutionObjectContext *)childObject, action);
                }
              else
                {
                  childEvent = childObject->getEventFromAnchorId (propName);
                  if (childEvent != NULL)
                    { // attribution with transition
                      runAction (childEvent, action);
                    }
                  else
                    { // force attribution
                      pAdapter = this->getObjectPlayer (childObject);

                      if (pAdapter != NULL)
                        {
                          pAdapter->setProperty (attrEvent, propValue);
                        }
                    }
                }
              ++j;
            }
        }
      else if (eventType == EventType::PRESENTATION)
        {
          compObj->suspendLinkEvaluation (false);

          compositeNode
              = (CompositeNode *)(compObj->getDataObject ()
                                      ->getDataEntity ());

          size = compositeNode->getNumPorts ();

          if (compositeNode->getParentComposition () == NULL)
            {
              compositionPerspective
                  = new NclNodeNesting (compositeNode->getPerspective ());
            }
          else
            {
              compositionPerspective
                  = compObj->getNodePerspective ();
            }

          events = new vector<NclEvent *>;
          for (i = 0; i < size; i++)
            {
              port = compositeNode->getPort ((unsigned int) i);
              g_assert_nonnull (port);

              perspective = compositionPerspective->copy ();
              g_assert_nonnull (perspective);

              nestedSeq = port->getMapNodeNesting ();
              g_assert_nonnull (nestedSeq);

              perspective->append (nestedSeq);
              try
                {
                  childObject = compiler->getExecutionObjectFromPerspective (
                                                  perspective, NULL);

                  if (childObject != NULL
                      && port->getEndInterfacePoint () != NULL
                      && port->getEndInterfacePoint ()->instanceOf (
                             "ContentAnchor"))
                    {
                      childEvent
                          = (PresentationEvent
                                 *)(compiler
                                        ->getEvent (
                                            childObject,
                                            port->getEndInterfacePoint (),
                                            EventType::PRESENTATION,
                                            ""));

                      if (childEvent != NULL)
                        {
                          events->push_back (childEvent);

                          clog << "Scheduler::";
                          clog << "runActionOverComposition '";
                          clog << compObj->getId () << "'";
                          clog << " dataCompositeObject = '";
                          clog << compositeNode->getId () << "' ";
                          clog << " dataCompositeObjectParent = '";
                          clog << compositeNode->getParentComposition ();
                          clog << "' ";
                          clog << "perspective = '";
                          clog << perspective->getId ();
                          clog << "' adding event '";
                          clog << childEvent->getId ();
                          clog << "' (child object = '";
                          clog << childObject->getId ();
                          clog << "', port = '";
                          clog << port->getId ();
                          clog << "')";
                          clog << endl;
                        }
                    }
                }
              catch (exception *exc)
                {
                  // keep on starting child objects
                }

              delete nestedSeq;
              delete perspective;
            }

          delete compositionPerspective;

          size = events->size ();

          clog << "Scheduler::runActionOverComposition ";
          clog << "action '" << action->getType () << "' over ";
          clog << "COMPOSITION '" << compObj->getId ();
          clog << "': '" << size << "' EVENTS FOUND" << endl;

          for (i = 0; i < size; i++)
            {
              runAction ((*events)[i], action);
            }
          delete events;
          events = NULL;
        }
    }
  else
    {
      event = action->getEvent ();
      if (event != NULL)
        {
          eventType = event->getType ();
        }

      if ((eventType == EventType::PRESENTATION)
          && (action->getType () == ACT_STOP
              || action->getType () == ACT_ABORT))
        {
          if (compObj->getWholeContentPresentationEvent () == event)
            {
              compObj->suspendLinkEvaluation (true);
            }
        }

      events = new vector<NclEvent *>;

      compositeNode = (CompositeNode *)(compObj->getDataObject ()
                                            ->getDataEntity ());

      objects = compObj->getExecutionObjects ();
      if (objects != NULL)
        {
          j = objects->begin ();
          while (j != objects->end ())
            {
              childObject = j->second;

              clog << "Scheduler::runActionOverComposition";
              clog << " getting main event of '";
              clog << childObject->getId () << "'";
              clog << endl;

              childEvent = childObject->getMainEvent ();
              if (childEvent == NULL)
                {
                  childEvent
                      = childObject->getWholeContentPresentationEvent ();
                }

              if (childEvent != NULL)
                {
                  events->push_back (childEvent);
                }
              ++j;
            }
          delete objects;
          objects = NULL;
        }

      if (compositeNode->getParentComposition () == NULL)
        {
          compositionPerspective
              = new NclNodeNesting (compositeNode->getPerspective ());

          compObj
              = (ExecutionObjectContext *) (compiler
                    ->getExecutionObjectFromPerspective (
                        compositionPerspective, NULL));

          delete compositionPerspective;

          objects = compObj->getExecutionObjects ();
          if (objects != NULL)
            {
              j = objects->begin ();
              while (j != objects->end ())
                {
                  childObject = j->second;

                  clog << "Scheduler::runActionOverComposition";
                  clog << " getting main event of '";
                  clog << childObject->getId () << "'";
                  clog << endl;

                  childEvent = childObject->getMainEvent ();
                  if (childEvent == NULL)
                    {
                      childEvent
                          = childObject
                                ->getWholeContentPresentationEvent ();
                    }

                  if (childEvent != NULL)
                    {
                      events->push_back (childEvent);
                    }
                  ++j;
                }
              delete objects;
              objects = NULL;
            }

          // *** QUIT PRESENTATION ***
          Ginga_Display->quit ();
        }

      clog << "Scheduler::runActionOverComposition (else) ";
      clog << "action '" << action->getType () << "' over ";
      clog << "COMPOSITION '" << compObj->getId ();
      clog << "' (objects = '" << objects;
      clog << "'): '" << size << "' EVENTS FOUND" << endl;

      size = events->size ();
      for (i = 0; i < size; i++)
        {
          runAction ((*events)[i], action);
        }

      delete events;
      events = NULL;
    }
}

void
Scheduler::runActionOverSwitch (ExecutionObjectSwitch *switchObj,
                                SwitchEvent *event,
                                NclLinkSimpleAction *action)
{
  ExecutionObject *selectedObject;
  NclEvent *selectedEvent;

  selectedObject = switchObj->getSelectedObject ();
  if (selectedObject == NULL)
    {
      selectedObject = compiler->processExecutionObjectSwitch (switchObj);

      if (selectedObject == NULL)
        {
          clog << "Scheduler::runActionOverSwitch Warning!";
          clog << " Can't solve switch" << endl;
          return;
        }
    }

  selectedEvent = event->getMappedEvent ();
  if (selectedEvent != NULL)
    {
      runAction (selectedEvent, action);
    }
  else
    {
      runSwitchEvent (switchObj, event, selectedObject, action);
    }

  if (action->getType () == ACT_STOP
      || action->getType () == ACT_ABORT)
    {
      switchObj->select (NULL);
    }
}

void
Scheduler::runSwitchEvent (ExecutionObjectSwitch *switchObj,
                                    SwitchEvent *switchEvent,
                                    ExecutionObject *selectedObject,
                                    NclLinkSimpleAction *action)
{
  NclEvent *selectedEvent;
  SwitchPort *switchPort;
  vector<Port *> *mappings;
  vector<Port *>::iterator i;
  Port *mapping;
  NclNodeNesting *nodePerspective;
  vector<Node *> *nestedSeq;
  ExecutionObject *endPointObject;

  selectedEvent = NULL;
  switchPort = (SwitchPort *)(switchEvent->getInterfacePoint ());
  mappings = switchPort->getPorts ();
  if (mappings != NULL)
    {
      i = mappings->begin ();
      while (i != mappings->end ())
        {
          mapping = *i;
          if (mapping->getNode () == selectedObject->getDataObject ())
            {
              nodePerspective = switchObj->getNodePerspective ();
              nestedSeq = mapping->getMapNodeNesting ();
              nodePerspective->append (nestedSeq);
              delete nestedSeq;
              try
                {
                  endPointObject
                      = compiler
                            ->getExecutionObjectFromPerspective (
                                  nodePerspective, NULL);

                  if (endPointObject != NULL)
                    {
                      selectedEvent
                          = compiler
                                ->getEvent (
                                    endPointObject,
                                    mapping->getEndInterfacePoint (),
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
          ++i;
        }
    }

  if (selectedEvent != NULL)
    {
      switchEvent->setMappedEvent (selectedEvent);
      runAction (selectedEvent, action);
    }
}

string
Scheduler::solveImplicitRefAssessment (const string &propValue,
                                                AttributionEvent *event)
{
  NclEvent *refEvent;
  ExecutionObject *refObject;
  string auxVal = "", roleId = "";

  if (propValue != "")
    {
      roleId = propValue.substr (1, propValue.length ());
    }

  refEvent = ((AttributionEvent *)event)
                 ->getImplicitRefAssessmentEvent (roleId);

  if (refEvent != NULL)
    {
      auxVal = ((AttributionEvent *)refEvent)->getCurrentValue ();
      refObject = ((ExecutionObject *)(refEvent->getExecutionObject ()));

      clog << "Scheduler::solveImplicitRefAssessment refEvent";
      clog << " for '" << refObject->getId () << "' is '";
      clog << refEvent->getId () << "', got '" << propValue << "'";
      clog << endl;

      return auxVal;
    }
  else
    {
      clog << "Scheduler::solveImplicitRefAssessment warning!";
      clog << " refEvent not found for '" << event->getId () << "', ";
      clog << " NclLinkAssignmentAction value is '" << propValue;
      clog << "'" << endl;
      clog << endl;
    }

  return "";
}

void
Scheduler::startEvent (NclEvent *event)
{
  NclLinkSimpleAction *fakeAction;

  fakeAction = new NclLinkSimpleAction (event, ACT_START);
  runAction (fakeAction);
  delete fakeAction;
}

void
Scheduler::stopEvent (NclEvent *event)
{
  NclLinkSimpleAction *fakeAction;

  fakeAction = new NclLinkSimpleAction (event, ACT_STOP);
  runAction (fakeAction);
  delete fakeAction;
}

void
Scheduler::pauseEvent (NclEvent *event)
{
  NclLinkSimpleAction *fakeAction;

  fakeAction = new NclLinkSimpleAction (event, ACT_PAUSE);
  runAction (fakeAction);
  delete fakeAction;
}

void
Scheduler::resumeEvent (NclEvent *event)
{
  NclLinkSimpleAction *fakeAction;

  fakeAction = new NclLinkSimpleAction (event, ACT_RESUME);
  runAction (fakeAction);
  delete fakeAction;
}

void
Scheduler::startDocument (const string &file)
{
  string id;
  ContextNode *body;
  vector<Port *> *ports;
  vector<NclEvent *> *entryevts;
  NclNodeNesting *persp;

  // Parse document.
  NclParser compiler;
  this->file = xpathmakeabs (file);
  this->doc = compiler.parse (file);
  g_assert_nonnull (this->doc);

  id = this->doc->getId ();
  body = this->doc->getBody ();
  if (unlikely (body == NULL))
    ERROR_SYNTAX ("document has no body");

  // Get entry events (i.e., those mapped by ports).
  ports = body->getPorts ();
  if (unlikely (ports == NULL))
    ERROR ("document has no ports");

  persp = new NclNodeNesting ();
  persp->insertAnchorNode (body);
  entryevts = new vector<NclEvent *>;
  for (auto port: *ports)
    {
      NclEvent *evt = this->compiler->insertContext (persp, port);
      g_assert_nonnull (evt);
      entryevts->push_back (evt);
    }
  delete persp;

  if (unlikely (entryevts->empty ()))
    {
      WARNING ("document has no ports");
      return;
    }

  // Create execution object for settings node and initialize it.
  vector <Node *> *settings = this->doc->getSettingsNodes ();
  for (auto node: *settings)
    {
      ContentNode *content;
      ExecutionObject *execobj;

      persp = new NclNodeNesting (node->getPerspective ());
      execobj = this->compiler->getExecutionObjectFromPerspective
        (persp, NULL);
      g_assert_nonnull (execobj);

      TRACE ("processing '%s'", persp->getId ().c_str ());
      delete persp;

      content = (ContentNode *) node;
      for (auto anchor: content->getAnchors ())
        {
          PropertyAnchor *prop;
          string name;
          string value;

          if (!anchor->instanceOf ("PropertyAnchor"))
            continue;           // nothing to do

          prop = (PropertyAnchor *) anchor;
          name = prop->getName ();
          value = prop->getValue ();
          if (value == "")
            continue;           // nothing to do

          TRACE ("seting %s='%s'", name.c_str (), value.c_str ());
          this->settings->set (name, value);
        }
    }
  delete settings;

  // Start entry events.
  for (auto event: *entryevts)
    {
      event->addListener (this);
      this->events.push_back (event);
      this->startEvent (event);
    }
  delete entryevts;
}

void
Scheduler::eventStateChanged (
    NclEvent *event,
    EventStateTransition transition,
    arg_unused (EventState previousState))
{
  ExecutionObject *object;
  PlayerAdapter *player;
  vector<NclEvent *>::iterator it;
  bool contains;
  bool hasOther;

  clog << "Scheduler::eventStateChanged '";
  clog << event->getId () << "' transition '" << static_cast<int> (transition);
  clog << "'" << endl;

  hasOther = false;
  contains = false;

  for (auto evt: this->events)
    {
      if (evt == event)
        {
          contains = true;
        }
      else if (evt->getCurrentState () != EventState::SLEEPING)
        {
          hasOther = true;
        }
    }

  if (contains)
    {
      switch (transition)
        {
        case EventStateTransition::STOPS:
        case EventStateTransition::ABORTS:
          if (!hasOther)
            {
              events.clear ();

              // we can't remove the document,
              // since it can be started again
              // removeDocument(event);
            }
          break;
        default:
          break;
        }
    }
  else
    {
      switch (transition)
        {
        case EventStateTransition::STARTS:
          object = event->getExecutionObject ();

          player = this->getObjectPlayer (object);
          if (player != NULL)
            {
              this->showObject (object);
              focusManager->showObject (object);
            }
          break;

        case EventStateTransition::STOPS:
          if (((PresentationEvent *)event)->getRepetitions () == 0)
            {
              bool hideObj = true;
              event->removeListener (this);
              object = event->getExecutionObject ();

              if (hideObj)
                {
                  clog << "Scheduler::eventStateChanged '";
                  clog << event->getId ();
                  clog << "' STOPS: hideObject '" << object->getId ();
                  clog << endl;

                  this->focusManager->hideObject (object);
                  this->hideObject (object);

                  player = this->getObjectPlayer (object);
                }
            }
          break;

        case EventStateTransition::ABORTS:
          {
            bool hideObj = true;

            event->removeListener (this);
            object = (ExecutionObject *)(event->getExecutionObject ());

            if (hideObj)
              {
                clog << "Scheduler::eventStateChanged '";
                clog << event->getId ();
                clog << "' ABORTS: hideObject '" << object->getId ();
                clog << endl;

                this->focusManager->hideObject (object);
                this->hideObject (object);

                player = this->getObjectPlayer (object);
              }
            break;
          }

        case EventStateTransition::PAUSES:
          {
            clog << "Scheduler::eventStateChanged\nPAUSES:\n";
            clog << endl;
            break;
          }

        case EventStateTransition::RESUMES:
          {
            clog << "Scheduler::eventStateChanged\nRESUMES:\n";
            clog << endl;
            break;
          }

        default:
          g_assert_not_reached ();
        }
    }
}

SDLWindow*
Scheduler::prepareFormatterRegion (ExecutionObject *obj)
{
  NclCascadingDescriptor *desc;
  NclFormatterRegion *reg;

  g_assert_nonnull (obj);

  desc = obj->getDescriptor ();
  if (desc == NULL)
    return NULL;                // nothing to do

  reg = desc->getFormatterRegion ();
  if (reg == NULL)
    return NULL;                // nothing to do

  return reg->prepareOutputDisplay ();
}

void
Scheduler::showObject (ExecutionObject *obj)
{
  NclCascadingDescriptor *desc;
  NclFormatterRegion *reg;

  desc = obj->getDescriptor ();
  if (desc == NULL)
    return;                     // nothing to do

  reg = desc->getFormatterRegion ();
  if (reg == NULL)
    return;                     // nothing to do

  reg->setGhostRegion (true);
}

void
Scheduler::hideObject (ExecutionObject *obj)
{
  NclCascadingDescriptor *desc;
  NclFormatterRegion *reg;

  desc = obj->getDescriptor ();
  if (desc == NULL)
    return;                     // nothing to do

  reg = desc->getFormatterRegion ();
  if (reg == NULL)
    return;                     // nothing to do
}

bool
Scheduler::removePlayer (ExecutionObject *exObject)
{
  if (!ExecutionObject::hasInstance (exObject, false))
    return false;

  auto i = _objectPlayers.find (exObject->getId ());
  if (i != _objectPlayers.end ())
    {
      delete i->second; // delete PlayerAdapter
      _objectPlayers.erase (i);

      return true;
    }

  return false;
}

PlayerAdapter *
Scheduler::initializePlayer (ExecutionObject *object)
{
  g_assert_nonnull (object);

  NodeEntity *entity
      = (NodeEntity *)(object->getDataObject ()->getDataEntity ());
  g_assert_nonnull (entity);

  ContentNode *contentNode = dynamic_cast<ContentNode *> (entity);
  g_assert_nonnull (contentNode);

  if (contentNode->isSettingNode ())
    return nullptr;             // nothing to do

  PlayerAdapter *adapter = new PlayerAdapter ();
  _objectPlayers[object->getId ()] = adapter;

  return adapter;
}

PlayerAdapter *
Scheduler::getObjectPlayer (ExecutionObject *execObj)
{
  PlayerAdapter *player = nullptr;
  string objId;

  objId = execObj->getId ();
  auto i = _objectPlayers.find (objId);
  if (i == _objectPlayers.end ())
    {
      player = initializePlayer (execObj);
    }
  else
    {
      player = i->second;
    }
  return player;
}

GINGA_FORMATTER_END
