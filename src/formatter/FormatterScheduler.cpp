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
#include "ginga-color-table.h"
#include "FormatterScheduler.h"

#include "FormatterConverter.h"
#include "Parser.h"

#include "mb/Display.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_FORMATTER_BEGIN

FormatterScheduler::FormatterScheduler ()
{
  int w, h;
  Ginga_Display->getSize (&w, &h);

  this->presContext = new PresentationContext ();
  this->ruleAdapter = new RuleAdapter (presContext);
  this->compiler = new FormatterConverter (this->ruleAdapter);
  this->compiler->setLinkActionListener (this);
  this->playerManager = new AdapterPlayerManager ();
  this->focusManager = new FormatterFocusManager
    (this->playerManager, this->presContext, this, this->compiler);

  this->focusManager->setKeyHandler (true);
  Thread::mutexInit (&mutexD, true);
  Thread::mutexInit (&mutexActions, true);
}

FormatterScheduler::~FormatterScheduler ()
{
  set<NclLinkSimpleAction *>::iterator i;

  Thread::mutexLock (&mutexD);
  Thread::mutexLock (&mutexActions);

  for (auto action: this->actions)
    action->setSimpleActionListener (NULL);
  this->actions.clear ();

  Thread::mutexUnlock (&mutexActions);
  Thread::mutexDestroy (&mutexActions);

  ruleAdapter = NULL;
  presContext = NULL;

  if (focusManager != NULL)
    {
      delete focusManager;
      focusManager = NULL;
    }

  compiler = NULL;
  events.clear ();

  Thread::mutexUnlock (&mutexD);
  Thread::mutexDestroy (&mutexD);
}

void
FormatterScheduler::addAction (NclLinkSimpleAction *action)
{
  Thread::mutexLock (&mutexActions);
  actions.insert (action);
  Thread::mutexUnlock (&mutexActions);
}

void
FormatterScheduler::removeAction (NclLinkSimpleAction *action)
{
  set<NclLinkSimpleAction *>::iterator i;

  Thread::mutexLock (&mutexActions);
  i = actions.find (action);
  if (i != actions.end ())
    {
      actions.erase (i);
    }
  Thread::mutexUnlock (&mutexActions);
}

FormatterFocusManager *
FormatterScheduler::getFocusManager ()
{
  return focusManager;
}

bool
FormatterScheduler::setKeyHandler (bool isHandler)
{
  return focusManager->setKeyHandler (isHandler);
}

void
FormatterScheduler::scheduleAction (NclLinkSimpleAction *action)
{
  pthread_mutex_lock (&mutexActions);
  assert (action != NULL);
  runAction (action);

  pthread_mutex_unlock (&mutexActions);

  return;
}

void
FormatterScheduler::runAction (NclLinkSimpleAction *action)
{
  NclFormatterEvent *event = action->getEvent ();

  assert (event != NULL);

  if (event->instanceOf ("NclSelectionEvent"))
    {
      event->start ();
      delete action;
      return;
    }

  runAction (event, action);
}

void
FormatterScheduler::runAction (NclFormatterEvent *event,
                               NclLinkSimpleAction *action)
{
  NclExecutionObject *obj;
  NclCascadingDescriptor *descriptor;
  AdapterFormatterPlayer *player;
  Player *playerContent;
  SDLWindow* win = NULL;

  obj = event->getExecutionObject ();
  g_assert_nonnull (obj);

  g_warning ("scheduler: running action '%s' over event '%s'",
           action->getTypeString ().c_str (),
           event->getId ().c_str ());

  g_assert (obj->isCompiled ());

  if (obj->instanceOf ("NclExecutionObjectSwitch")
      && event->instanceOf ("NclSwitchEvent"))
    {
      runActionOverSwitch ((NclExecutionObjectSwitch *)obj,
                           (NclSwitchEvent *)event, action);
      return;
    }

  if (obj->instanceOf ("NclCompositeExecutionObject")
      && (obj->getDescriptor () == NULL
          || obj->getDescriptor ()->getPlayerName ()
          == ""))
    {
      runActionOverComposition
        ((NclCompositeExecutionObject *)obj, action);
      return;
    }

  if (event->instanceOf ("NclAttributionEvent"))
    {
      runActionOverProperty (event, action);
      return;
    }

  player = this->playerManager->getObjectPlayer (obj);

  if (unlikely (player == NULL))
    {
      g_warning ("scheduler: no player to decode '%s', skipping action",
                 obj->getId ().c_str ());
      return;
    }

  if (obj->instanceOf ("NclApplicationExecutionObject")
      && !event->instanceOf ("NclAttributionEvent"))
    {
      runActionOverApplicationObject
        ((NclApplicationExecutionObject *)obj, event,
         (AdapterApplicationPlayer *)player, action);
      return;
    }

  switch (action->getType ())
    {
    case ACT_START:
      if (!player->hasPrepared ())
        {
          if (ruleAdapter->adaptDescriptor (obj))
            {
              descriptor = obj->getDescriptor ();
              if (descriptor != NULL)
                descriptor->setFormatterLayout();
            }

          if (unlikely (!player->prepare (obj,
                                          (NclPresentationEvent *) event)))
            {
              g_warning ("scheduler: failed to prepare player");
              return;
            }

          playerContent = player->getPlayer ();
          g_assert_nonnull (playerContent);

          win = this->prepareFormatterRegion (obj);

          // FIXME: Sometimes win is NULL!
          // g_assert_nonnull (win);

          player->setOutputWindow (win);
          event->addEventListener (this);
        }

      if (unlikely (!player->start ()))
        {
          g_warning ("scheduler: failed to start player");
          if (event->getCurrentState () == EventUtil::ST_SLEEPING)
            event->removeEventListener (this);
        }
      break;

    case ACT_PAUSE:
      if (unlikely (!player->pause ()))
        g_warning ("scheduler: failed to pause player");
      break;

    case ACT_RESUME:
      if (unlikely (!player->resume ()))
        g_warning ("scheduler: failed to resume player");
      break;

    case ACT_ABORT:
      if (unlikely (!player->abort ()))
        g_warning ("scheduler: failed to abort player");
      break;

    case ACT_STOP:
      if (unlikely (!player->stop ()))
        g_warning ("scheduler: failed to stop player");
      break;

    default:
      g_assert_not_reached ();
    }
}

void
FormatterScheduler::runActionOverProperty (NclFormatterEvent *event,
                                           NclLinkSimpleAction *action)
{
  SimpleActionType actionType;
  string propName, propValue;

  NodeEntity *dataObject;
  NclExecutionObject *executionObject;
  AdapterFormatterPlayer *player;
  Animation *anim;

  executionObject = (NclExecutionObject *)(event->getExecutionObject ());
  dataObject
      = (NodeEntity *)(executionObject->getDataObject ()->getDataEntity ());

  if (dataObject->instanceOf ("ContentNode")
      && ((ContentNode *)dataObject)->isSettingNode ()
      && action->instanceOf ("NclLinkAssignmentAction"))
    {
      propName = ((NclAttributionEvent *)event)
                     ->getAnchor ()
                     ->getPropertyName ();

      propValue = ((NclLinkAssignmentAction *)action)->getValue ();
      if (propValue != "" && propValue.substr (0, 1) == "$")
        {
          propValue = solveImplicitRefAssessment (
              propValue, (NclAttributionEvent *)event);
        }

      event->start ();
      ((NclAttributionEvent *)event)->setValue (propValue);

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
          presContext->setPropertyValue (propName, propValue);
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

      player = playerManager->getObjectPlayer (executionObject);
      actionType = action->getType ();

      switch (actionType)
        {
        case ACT_START:
        case ACT_SET:
          clog << "FormatterScheduler::runActionOverProperty";
          clog << " over '" << event->getId () << "' for '";
          clog << executionObject->getId () << "' player '";
          clog << player << "'" << endl;
          if (event->getCurrentState () != EventUtil::ST_SLEEPING)
            {
              clog << "FormatterScheduler::runActionOverProperty";
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
                      propValue, (NclAttributionEvent *)event);

                  clog << "FormatterScheduler::runActionOverProperty";
                  clog << " IMPLICIT found '" << propValue;
                  clog << "' for event '" << event->getId () << "'";
                  clog << endl;
                }

              event->start ();
              ((NclAttributionEvent *)event)->setValue (propValue);
            }
          else
            {
              event->start ();
            }

          if (anim != NULL)
            {
              //dragon fang
              string durVal, byVal;

              durVal = anim->getDuration ();
              if (durVal.substr (0, 1) == "$")
                {
                  anim->setDuration (solveImplicitRefAssessment (
                      durVal, (NclAttributionEvent *)event));
                }

              byVal = anim->getBy ();
              if (byVal.substr (0, 1) == "$")
                {
                  anim->setDuration (solveImplicitRefAssessment (
                      byVal, (NclAttributionEvent *)event));
                }

              anim->setDuration (durVal);
              anim->setBy (byVal);

              player->getPlayer()->
                setAnimatorProperties(durVal,((NclAttributionEvent *)event)
                                      ->getAnchor ()
                                      ->getPropertyName (),propValue);
            }
          else if (player != NULL && player->hasPrepared ())
            {
              player->setPropertyValue ((NclAttributionEvent *)event,
                                        propValue);

              event->stop ();
            }
          else
            {
              executionObject->setPropertyValue (
                  (NclAttributionEvent *)event, propValue);

              event->stop ();
            }

          break;

        default:
          clog << "FormatterScheduler::runActionOverProperty";
          clog << "unknown actionType = '" << actionType << "'";
          clog << endl;
          break;
        }
    }
}

void
FormatterScheduler::runActionOverApplicationObject (
    NclApplicationExecutionObject *executionObject,
    NclFormatterEvent *event, AdapterApplicationPlayer *player,
    NclLinkSimpleAction *action)
{
  NclCascadingDescriptor *descriptor;
  Player *playerContent;

  double time = (double) xruntime_ms ();
  int actionType = action->getType ();
  SDLWindow * win = NULL;

  switch (actionType)
    {
    case ACT_START:
      g_warning ("runActionOverApplicationObject START '%s'",
                 event->getId().c_str());

      if (!player->hasPrepared ())
        {
          if (ruleAdapter->adaptDescriptor (executionObject))
            {
              descriptor = executionObject->getDescriptor ();
              if (descriptor != NULL)
                descriptor->setFormatterLayout ();
            }

          player->prepare (executionObject, (NclPresentationEvent *) event);
          playerContent = player->getPlayer ();
          if (playerContent != NULL)
            {
              win = this->prepareFormatterRegion (executionObject);
              player->setOutputWindow (win);
            }
        }
      else
        {
          player->prepare (executionObject, (NclPresentationEvent *) event);
        }

      event->addEventListener (this);
      if (player->setAndLockCurrentEvent (event))
        {
          if (!player->start ())
            {
              g_warning ("can't start '%s'",
                         executionObject->getId().c_str());

              // checking if player failed to start
              if (event->getCurrentState () == EventUtil::ST_SLEEPING)
                event->removeEventListener (this);
            }

          player->unlockCurrentEvent (event);
        }

      time = (double) xruntime_ms () - time;
      g_warning ("runActionOverApp '%s' took '%fms' to start",
                 executionObject->getId().c_str(),
                 time);
      break;

    case ACT_PAUSE:
      if (player->setAndLockCurrentEvent (event))
        {
          player->pause ();
          player->unlockCurrentEvent (event);
        }

      break;

    case ACT_RESUME:
      if (player->setAndLockCurrentEvent (event))
        {
          player->resume ();
          player->unlockCurrentEvent (event);
        }

      break;

    case ACT_ABORT:
      if (player->setAndLockCurrentEvent (event))
        {
          player->abort ();
          player->unlockCurrentEvent (event);
        }

      break;

    case ACT_STOP:
      if (player->setAndLockCurrentEvent (event))
        {
          player->stop ();
          player->unlockCurrentEvent (event);
        }

      break;
    default:
      g_assert_not_reached ();
    }
}

void
FormatterScheduler::runActionOverComposition (
    NclCompositeExecutionObject *compositeObject,
    NclLinkSimpleAction *action)
{
  CompositeNode *compositeNode;
  Port *port;
  NclNodeNesting *compositionPerspective;
  NclNodeNesting *perspective;

  map<string, NclExecutionObject *> *objects;
  map<string, NclExecutionObject *>::iterator j;
  NclExecutionObject *childObject;

  AdapterFormatterPlayer *pAdapter;
  NclAttributionEvent *attrEvent;
  NclFormatterEvent *event;
  string propName;
  string propValue;

  vector<Node *> *nestedSeq;

  NclFormatterEvent *childEvent;
  int i, size = 0;
  vector<NclFormatterEvent *> *events;
  short eventType = -1;

  clog << "FormatterScheduler::runActionOverComposition ";
  clog << "action '" << action->getType () << "' over COMPOSITION '";
  clog << compositeObject->getId () << "'" << endl;

  if (action->getType () == ACT_START
      || action->getType () == ACT_SET)
    {
      event = action->getEvent ();
      if (event != NULL)
        {
          eventType = event->getEventType ();
          if (eventType < 0)
            {
              if (event->instanceOf ("NclPresentationEvent"))
                {
                  eventType = EventUtil::EVT_PRESENTATION;
                }
              else if (event->instanceOf ("NclAttributionEvent"))
                {
                  eventType = EventUtil::EVT_ATTRIBUTION;
                }
              else if (event->instanceOf ("NclSwitchEvent"))
                {
                  eventType = EventUtil::EVT_PRESENTATION;
                }
              else
                {
                  clog << "FormatterScheduler::runActionOverComposition ";
                  clog << "Warning! action '" << action->getType () << "'";
                  clog << " over COMPOSITION '";
                  clog << compositeObject->getId ();
                  clog << "' has an unknown eventType" << endl;
                }
            }
        }

      if (eventType == EventUtil::EVT_ATTRIBUTION)
        {
          event = action->getEvent ();
          if (!event->instanceOf ("NclAttributionEvent"))
            {
              clog << "FormatterScheduler::runActionOverComposition SET ";
              clog << "Warning! event ins't of attribution type";
              clog << endl;
              return;
            }

          attrEvent = (NclAttributionEvent *)event;
          propName = attrEvent->getAnchor ()->getPropertyName ();
          propValue = ((NclLinkAssignmentAction *)action)->getValue ();
          event = compositeObject->getEventFromAnchorId (propName);

          if (event != NULL)
            {
              event->start ();
              compositeObject->setPropertyValue (
                  (NclAttributionEvent *)event, propValue);

              ((NclAttributionEvent *)event)->setValue (propValue);
              event->stop ();
            }
          else
            {
              compositeObject->setPropertyValue (attrEvent, propValue);
              attrEvent->stop ();
            }

          objects = compositeObject->getExecutionObjects ();
          if (objects == NULL)
              return;

          j = objects->begin ();
          while (j != objects->end ())
            {
              childObject = j->second;
              if (childObject->instanceOf ("NclCompositeExecutionObject"))
                {
                  clog << "FormatterScheduler::runActionOverComposition ";
                  clog << "'" << compositeObject->getId () << "' has '";
                  clog << childObject->getId () << "' as its child ";
                  clog << "using recursive call";
                  clog << endl;

                  runActionOverComposition (
                      (NclCompositeExecutionObject *)childObject, action);
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
                      pAdapter = playerManager->getObjectPlayer (childObject);

                      if (pAdapter != NULL)
                        {
                          pAdapter->setPropertyValue (attrEvent, propValue);
                        }
                    }
                }
              ++j;
            }
        }
      else if (eventType == EventUtil::EVT_PRESENTATION)
        {
          compositeObject->suspendLinkEvaluation (false);

          compositeNode
              = (CompositeNode *)(compositeObject->getDataObject ()
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
                  = compositeObject->getNodePerspective ();
            }

          events = new vector<NclFormatterEvent *>;
          for (i = 0; i < size; i++)
            {
              port = compositeNode->getPort (i);
              perspective = compositionPerspective->copy ();
              nestedSeq = port->getMapNodeNesting ();
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
                          = (NclPresentationEvent
                                 *)(compiler
                                        ->getEvent (
                                            childObject,
                                            port->getEndInterfacePoint (),
                                            EventUtil::EVT_PRESENTATION,
                                            ""));

                      if (childEvent != NULL)
                        {
                          events->push_back (childEvent);

                          clog << "FormatterScheduler::";
                          clog << "runActionOverComposition '";
                          clog << compositeObject->getId () << "'";
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

          size = (int) events->size ();

          clog << "FormatterScheduler::runActionOverComposition ";
          clog << "action '" << action->getType () << "' over ";
          clog << "COMPOSITION '" << compositeObject->getId ();
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
          eventType = event->getEventType ();
        }

      if ((eventType == EventUtil::EVT_PRESENTATION)
          && (action->getType () == ACT_STOP
              || action->getType () == ACT_ABORT))
        {
          if (compositeObject->getWholeContentPresentationEvent () == event)
            {
              compositeObject->suspendLinkEvaluation (true);
            }
        }

      events = new vector<NclFormatterEvent *>;

      compositeNode = (CompositeNode *)(compositeObject->getDataObject ()
                                            ->getDataEntity ());

      objects = compositeObject->getExecutionObjects ();
      if (objects != NULL)
        {
          j = objects->begin ();
          while (j != objects->end ())
            {
              childObject = j->second;

              clog << "FormatterScheduler::runActionOverComposition";
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

          compositeObject
              = (NclCompositeExecutionObject *) (compiler
                    ->getExecutionObjectFromPerspective (
                        compositionPerspective, NULL));

          delete compositionPerspective;

          objects = compositeObject->getExecutionObjects ();
          if (objects != NULL)
            {
              j = objects->begin ();
              while (j != objects->end ())
                {
                  childObject = j->second;

                  clog << "FormatterScheduler::runActionOverComposition";
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
        }

      clog << "FormatterScheduler::runActionOverComposition (else) ";
      clog << "action '" << action->getType () << "' over ";
      clog << "COMPOSITION '" << compositeObject->getId ();
      clog << "' (objects = '" << objects;
      clog << "'): '" << size << "' EVENTS FOUND" << endl;

      size = (int) events->size ();
      for (i = 0; i < size; i++)
        {
          runAction ((*events)[i], action);
        }

      delete events;
      events = NULL;
    }
}

void
FormatterScheduler::runActionOverSwitch (
    NclExecutionObjectSwitch *switchObject, NclSwitchEvent *event,
    NclLinkSimpleAction *action)
{
  NclExecutionObject *selectedObject;
  NclFormatterEvent *selectedEvent;

  selectedObject = switchObject->getSelectedObject ();
  if (selectedObject == NULL)
    {
      selectedObject = compiler->processExecutionObjectSwitch (switchObject);

      if (selectedObject == NULL)
        {
          clog << "FormatterScheduler::runActionOverSwitch Warning!";
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
      runSwitchEvent (switchObject, event, selectedObject, action);
    }

  if (action->getType () == ACT_STOP
      || action->getType () == ACT_ABORT)
    {
      switchObject->select (NULL);
    }
}

void
FormatterScheduler::runSwitchEvent (NclExecutionObjectSwitch *switchObject,
                                    NclSwitchEvent *switchEvent,
                                    NclExecutionObject *selectedObject,
                                    NclLinkSimpleAction *action)
{
  NclFormatterEvent *selectedEvent;
  SwitchPort *switchPort;
  vector<Port *> *mappings;
  vector<Port *>::iterator i;
  Port *mapping;
  NclNodeNesting *nodePerspective;
  vector<Node *> *nestedSeq;
  NclExecutionObject *endPointObject;

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
              nodePerspective = switchObject->getNodePerspective ();
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
                                    switchEvent->getEventType (),
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
FormatterScheduler::solveImplicitRefAssessment (const string &propValue,
                                                NclAttributionEvent *event)
{
  NclFormatterEvent *refEvent;
  NclExecutionObject *refObject;
  string auxVal = "", roleId = "";

  if (propValue != "")
    {
      roleId = propValue.substr (1, propValue.length ());
    }

  refEvent = ((NclAttributionEvent *)event)
                 ->getImplicitRefAssessmentEvent (roleId);

  if (refEvent != NULL)
    {
      auxVal = ((NclAttributionEvent *)refEvent)->getCurrentValue ();
      refObject = ((NclExecutionObject *)(refEvent->getExecutionObject ()));

      clog << "FormatterScheduler::solveImplicitRefAssessment refEvent";
      clog << " for '" << refObject->getId () << "' is '";
      clog << refEvent->getId () << "', got '" << propValue << "'";
      clog << endl;

      return auxVal;
    }
  else
    {
      clog << "FormatterScheduler::solveImplicitRefAssessment warning!";
      clog << " refEvent not found for '" << event->getId () << "', ";
      clog << " NclLinkAssignmentAction value is '" << propValue;
      clog << "'" << endl;
      clog << endl;
    }

  return "";
}

void
FormatterScheduler::startEvent (NclFormatterEvent *event)
{
  NclLinkSimpleAction *fakeAction;

  fakeAction = new NclLinkSimpleAction (event, ACT_START);
  runAction (fakeAction);
  delete fakeAction;
}

void
FormatterScheduler::stopEvent (NclFormatterEvent *event)
{
  NclLinkSimpleAction *fakeAction;

  fakeAction = new NclLinkSimpleAction (event, ACT_STOP);
  runAction (fakeAction);
  delete fakeAction;
}

void
FormatterScheduler::pauseEvent (NclFormatterEvent *event)
{
  NclLinkSimpleAction *fakeAction;

  fakeAction = new NclLinkSimpleAction (event, ACT_PAUSE);
  runAction (fakeAction);
  delete fakeAction;
}

void
FormatterScheduler::resumeEvent (NclFormatterEvent *event)
{
  NclLinkSimpleAction *fakeAction;

  fakeAction = new NclLinkSimpleAction (event, ACT_RESUME);
  runAction (fakeAction);
  delete fakeAction;
}

void
FormatterScheduler::startDocument (const string &file)
{
  string id;
  ContextNode *body;
  vector<NclFormatterEvent *> *entryevts;
  NclNodeNesting *persp;

  // Parse document.
  NclParser compiler;
  this->file = xpathmakeabs (file);
  this->doc = compiler.parse (file);
  g_assert_nonnull (this->doc);

  id = this->doc->getId ();
  body = this->doc->getBody ();
  if (unlikely (body == NULL))
    syntax_error ("document has no body");

  // Get entry events (i.e., those mapped by ports).
  persp = new NclNodeNesting ();
  persp->insertAnchorNode (body);
  entryevts = new vector<NclFormatterEvent *>;
  for (auto port: *body->getPorts ())
    {
      NclFormatterEvent *evt = this->compiler->insertContext (persp, port);
      g_assert_nonnull (evt);
      entryevts->push_back (evt);
    }
  delete persp;

  if (unlikely (entryevts->empty ()))
    {
      g_warning ("document has no ports");
      return;
    }

  // Create execution object for settings and initialize it.
  vector <Node *> *settings = this->doc->getSettingsNodes ();
  for (auto node: *settings)
    {
      ContentNode *content;
      NclExecutionObject *execobj;

      persp = new NclNodeNesting (node->getPerspective ());
      execobj = this->compiler->getExecutionObjectFromPerspective
        (persp, NULL);
      g_assert_nonnull (execobj);

      g_debug ("settings: processing '%s'", persp->getId ().c_str ());
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
          name = prop->getPropertyName ();
          value = prop->getPropertyValue ();
          if (value == "")
            continue;           // nothing to do

          g_debug ("settings: set %s='%s'", name.c_str (), value.c_str ());
          this->presContext->setPropertyValue (name, value);
        }
    }
  delete settings;

  // Start entry events.
  for (auto event: *entryevts)
    {
      event->addEventListener (this);
      this->events.push_back (event);
      this->startEvent (event);
    }
  delete entryevts;
}

void
FormatterScheduler::eventStateChanged (NclFormatterEvent *event,
                                       short transition,
                                       arg_unused (short previousState))
{
  NclExecutionObject *object;
  AdapterFormatterPlayer *player;
  vector<NclFormatterEvent *>::iterator it;
  bool contains;
  bool hasOther;

  clog << "FormatterScheduler::eventStateChanged '";
  clog << event->getId () << "' transition '" << transition;
  clog << "'" << endl;

  hasOther = false;
  contains = false;

  for (auto evt: this->events)
    {
      if (evt == event)
        {
          contains = true;
        }
      else if (evt->getCurrentState () != EventUtil::ST_SLEEPING)
        {
          hasOther = true;
        }
    }

  if (contains)
    {
      switch (transition)
        {
        case EventUtil::TR_STOPS:
        case EventUtil::TR_ABORTS:
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
        case EventUtil::TR_STARTS:
          object = event->getExecutionObject ();

          player = playerManager->getObjectPlayer (object);
          if (player != NULL)
            {
              this->showObject (object);

              focusManager->showObject (object);
            }
          break;

        case EventUtil::TR_STOPS:
          if (((NclPresentationEvent *)event)->getRepetitions () == 0)
            {
              bool hideObj = true;
              event->removeEventListener (this);
              object = event->getExecutionObject ();

              if (object->instanceOf ("NclApplicationExecutionObject"))
                {
                  if (!((NclApplicationExecutionObject *)object)
                           ->isSleeping ())
                    {
                      hideObj = false;
                    }
                }

              if (hideObj)
                {
                  //dragon head
                  clog << "FormatterScheduler::eventStateChanged '";
                  clog << event->getId ();
                  clog << "' STOPS: hideObject '" << object->getId ();
                  clog << endl;

                  this->focusManager->hideObject (object);
                  this->hideObject (object);

                  player = playerManager->getObjectPlayer (object);
                  if (player != NULL
                      && player->getPlayer () != NULL)
                    {
                    }
                }
            }
          break;

        case EventUtil::TR_ABORTS:
          {
            bool hideObj = true;

            event->removeEventListener (this);
            object = (NclExecutionObject *)(event->getExecutionObject ());

            if (object->instanceOf ("NclApplicationExecutionObject"))
              {
                if (!((NclApplicationExecutionObject *)object)->isSleeping ())
                  {
                    hideObj = false;
                  }
              }
            if (hideObj)
              {
                clog << "FormatterScheduler::eventStateChanged '";
                clog << event->getId ();
                clog << "' ABORTS: hideObject '" << object->getId ();
                clog << endl;

                this->focusManager->hideObject (object);
                this->hideObject (object);

                player = playerManager->getObjectPlayer (object);
              }
            break;
          }

        case EventUtil::TR_PAUSES:
          {
            clog << "FormatterScheduler::eventStateChanged\nPAUSES:\n";
            clog << endl;
            break;
          }

        case EventUtil::TR_RESUMES:
          {
            clog << "FormatterScheduler::eventStateChanged\nRESUMES:\n";
            clog << endl;
            break;
          }

        default:
          g_assert_not_reached ();
        }
    }
}

SDLWindow*
FormatterScheduler::prepareFormatterRegion (NclExecutionObject *obj)
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
FormatterScheduler::showObject (NclExecutionObject *obj)
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
  reg->showContent ();
}

void
FormatterScheduler::hideObject (NclExecutionObject *obj)
{
  NclCascadingDescriptor *desc;
  NclFormatterRegion *reg;

  desc = obj->getDescriptor ();
  if (desc == NULL)
    return;                     // nothing to do

  reg = desc->getFormatterRegion ();
  if (reg == NULL)
    return;                     // nothing to do

  reg->hideContent ();
}

GINGA_FORMATTER_END
