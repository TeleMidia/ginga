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
  this->presContext = ruleAdapter->getPresentationContext ();
  this->compiler = new FormatterConverter (ruleAdapter);
  this->compiler->setLinkActionListener (this);
  this->compiler->setScheduler (this);
  this->layout = new NclFormatterLayout (w, h);
  this->playerManager = new AdapterPlayerManager ();
  this->focusManager = new FormatterFocusManager
    (this->playerManager, this->presContext, this,
     (FormatterConverter *) this->compiler);

  this->focusManager->setKeyHandler (true);
  this->running = false;
  Thread::mutexInit (&mutexD, true);
  Thread::mutexInit (&mutexActions, true);
  Thread::mutexInit (&lMutex, true);
}

FormatterScheduler::~FormatterScheduler ()
{
  set<void *>::iterator i;
  set<NclFormatterEvent *>::iterator j;
  NclLinkSimpleAction *action;

  clog << "FormatterScheduler::~FormatterScheduler(" << this << ")";
  clog << endl;

  running = false;

  Thread::mutexLock (&lMutex);
  Thread::mutexLock (&mutexD);
  Thread::mutexLock (&mutexActions);

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

  i = actions.begin ();
  while (i != actions.end ())
    {
      action = (NclLinkSimpleAction *)(*i);
      action->setSimpleActionListener (NULL);
      ++i;
    }
  actions.clear ();
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
  documentEvents.clear ();
  documentStatus.clear ();

  Thread::mutexUnlock (&mutexD);
  Thread::mutexDestroy (&mutexD);
}

void
FormatterScheduler::addAction (void *action)
{
  Thread::mutexLock (&mutexActions);
  actions.insert (action);
  Thread::mutexUnlock (&mutexActions);
}

void
FormatterScheduler::removeAction (void *action)
{
  set<void *>::iterator i;
  vector<NclLinkSimpleAction *>::iterator j;

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

NclFormatterLayout *
FormatterScheduler::getFormatterLayout ()
{
  return this->layout;
}

bool
FormatterScheduler::setKeyHandler (bool isHandler)
{
  return focusManager->setKeyHandler (isHandler);
}

bool
FormatterScheduler::isDocumentRunning (NclFormatterEvent *event)
{
  NclExecutionObject *executionObject;
  NclCompositeExecutionObject *parentObject;
  NclFormatterEvent *documentEvent;

  executionObject = (NclExecutionObject *)(event->getExecutionObject ());
  parentObject = (NclCompositeExecutionObject *)(executionObject
                                                     ->getParentObject ());

  if (parentObject != NULL)
    {
      while (parentObject->getParentObject () != NULL)
        {
          executionObject = (NclExecutionObject *)(parentObject);
          parentObject
              = (NclCompositeExecutionObject *)(parentObject
                                                    ->getParentObject ());
        }

      documentEvent = executionObject->getWholeContentPresentationEvent ();
    }
  else
    {
      documentEvent = event;
    }

  Thread::mutexLock (&mutexD);
  if (documentStatus.count (documentEvent) != 0)
    {
      bool eventStatus = documentStatus[documentEvent];
      Thread::mutexUnlock (&mutexD);
      return eventStatus;
    }

  Thread::mutexUnlock (&mutexD);
  return false;
}

void
FormatterScheduler::scheduleAction (void *someAction)
{
  pthread_mutex_lock (&mutexActions);
  assert (someAction != NULL);
  runAction ((NclLinkSimpleAction *)someAction);

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
  NclExecutionObject *executionObject;
  NclCascadingDescriptor *descriptor;
  AdapterFormatterPlayer *player;
  IPlayer *playerContent;
  string attName;
  string attValue;
  SDLWindow* winId = 0;

  executionObject = (NclExecutionObject *)(event->getExecutionObject ());
  g_assert_nonnull (executionObject);

  g_debug ("scheduler: running action '%s' over event '%s'",
           action->getTypeString ().c_str (),
           event->getId ().c_str ());

  g_assert (executionObject->isCompiled ());

  if (executionObject->instanceOf ("NclExecutionObjectSwitch")
      && event->instanceOf ("NclSwitchEvent"))
    {
      runActionOverSwitch ((NclExecutionObjectSwitch *)executionObject,
                           (NclSwitchEvent *)event, action);
      return;
    }

  if (executionObject->instanceOf ("NclCompositeExecutionObject")
      && (executionObject->getDescriptor () == NULL
          || executionObject->getDescriptor ()->getPlayerName ()
          == ""))
    {
      runActionOverComposition
        ((NclCompositeExecutionObject *)executionObject, action);
      return;
    }

  if (event->instanceOf ("NclAttributionEvent"))
    {
      runActionOverProperty (event, action);
      return;
    }

  player = (AdapterFormatterPlayer *)
    playerManager->getObjectPlayer (executionObject);

  if (unlikely (player == NULL))
    {
      g_warning ("scheduler: no player to decode '%s', skipping action",
                 executionObject->getId ().c_str ());
      return;
    }

  if (executionObject->instanceOf ("NclApplicationExecutionObject")
      && !event->instanceOf ("NclAttributionEvent"))
    {
      runActionOverApplicationObject
        ((NclApplicationExecutionObject *)executionObject, event,
         player, action);
      return;
    }

  switch (action->getType ())
    {
    case ACT_START:
      if (!player->hasPrepared ())
        {
          if (ruleAdapter->adaptDescriptor (executionObject))
            {
              descriptor = executionObject->getDescriptor ();
              if (descriptor != NULL)
                {
                  descriptor->setFormatterLayout(getFormatterLayout ());
                }
            }

          if (unlikely (!player->prepare (executionObject,
                                          (NclPresentationEvent *)event)))
            {
              g_warning ("scheduler: failed to prepare player");
              return;
            }

          playerContent = player->getPlayer ();
          g_assert_nonnull (playerContent);

          winId = this->prepareFormatterRegion (executionObject);

          // FIXME: Sometimes winId is NULL!
          // g_assert_nonnull (winId);

          player->setOutputWindow (winId);

          event->addEventListener (this);

          Thread::mutexLock (&lMutex);
          listening.insert (event);
          Thread::mutexUnlock (&lMutex);
        }

      if (unlikely (!player->start ()))
        {
          g_warning ("scheduler: failed to start player");
          if (event->getCurrentState () == EventUtil::ST_SLEEPING)
            {
              set<NclFormatterEvent *>::iterator it;

              event->removeEventListener (this);

              Thread::mutexLock (&lMutex);
              it = listening.find (event);
              if (it != listening.end ())
                listening.erase (it);
              Thread::mutexUnlock (&lMutex);
            }
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

      player = (AdapterFormatterPlayer *)playerManager->getObjectPlayer (
          executionObject);
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

              ((Player*)player->getPlayer())->
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
    NclFormatterEvent *event, AdapterFormatterPlayer *player,
    NclLinkSimpleAction *action)
{
  NclCascadingDescriptor *descriptor;
  IPlayer *playerContent;

  string attValue, attName;

  double time = (double) xruntime_ms ();
  int actionType = action->getType ();
  SDLWindow* winId = 0;

  clog << "FormatterScheduler::";
  clog << "runActionOverApplicationObject ACTION = '";
  clog << actionType << "' event = '" << event->getId ();
  clog << "'" << endl;

  switch (actionType)
    {
    case ACT_START:
      if (!player->hasPrepared ())
        {
          if (ruleAdapter->adaptDescriptor (executionObject))
            {
              descriptor = executionObject->getDescriptor ();
              if (descriptor != NULL)
                {
                  descriptor->setFormatterLayout (getFormatterLayout ());
                }
            }

          clog << "FormatterScheduler::";
          clog << "runActionOverApplicationObject ";
          clog << "START '" << event->getId ();
          clog << "' call player->prepare1";
          clog << endl;

          player->prepare (executionObject, event);

          playerContent = player->getPlayer ();
          if (playerContent != NULL)
            {
            }


          if (playerContent != NULL)
            {
              winId = this->prepareFormatterRegion (executionObject);

              player->setOutputWindow (winId);
            }
        }
      else
        {
          clog << "FormatterScheduler::";
          clog << "runActionOverApplicationObject ";
          clog << "START '" << event->getId ();
          clog << "' call player->prepare2";
          clog << endl;

          player->prepare (executionObject, event);
        }

      event->addEventListener (this);
      Thread::mutexLock (&lMutex);
      listening.insert (event);
      Thread::mutexUnlock (&lMutex);
      if (((AdapterApplicationPlayer *)player)
              ->setAndLockCurrentEvent (event))
        {
          if (!player->start ())
            {
              clog << "FormatterScheduler::";
              clog << "runActionOverApplicationObject can't start '";
              clog << executionObject->getId () << "'";
              clog << endl;

              set<NclFormatterEvent *>::iterator it;

              // checking if player failed to start
              if (event->getCurrentState () == EventUtil::ST_SLEEPING)
                {
                  event->removeEventListener (this);
                  Thread::mutexLock (&lMutex);
                  it = listening.find (event);
                  if (it != listening.end ())
                    {
                      listening.erase (it);
                    }
                  Thread::mutexUnlock (&lMutex);
                }
            }

          ((AdapterApplicationPlayer *)player)->unlockCurrentEvent (event);
        }

      time = (double) xruntime_ms () - time;
      clog << "FormatterScheduler::runActionOverApp takes '";
      clog << time << "' ms to start '";
      clog << executionObject->getId () << "'";
      clog << endl;
      break;

    case ACT_PAUSE:
      if (((AdapterApplicationPlayer *)player)
              ->setAndLockCurrentEvent (event))
        {
          player->pause ();
          ((AdapterApplicationPlayer *)player)->unlockCurrentEvent (event);
        }

      break;

    case ACT_RESUME:
      if (((AdapterApplicationPlayer *)player)
              ->setAndLockCurrentEvent (event))
        {
          player->resume ();
          ((AdapterApplicationPlayer *)player)->unlockCurrentEvent (event);
        }

      break;

    case ACT_ABORT:
      if (((AdapterApplicationPlayer *)player)
              ->setAndLockCurrentEvent (event))
        {
          player->abort ();
          ((AdapterApplicationPlayer *)player)->unlockCurrentEvent (event);
        }

      break;

    case ACT_STOP:
      if (((AdapterApplicationPlayer *)player)
              ->setAndLockCurrentEvent (event))
        {
          player->stop ();
          ((AdapterApplicationPlayer *)player)->unlockCurrentEvent (event);
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
            {
              /*
              clog << "FormatterScheduler::runActionOverComposition SET ";
              clog << "no childs found!";
              clog << endl;*/
              return;
            }

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
                      pAdapter = (AdapterFormatterPlayer *)playerManager
                                     ->getObjectPlayer (childObject);

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
                  childObject = ((FormatterConverter *)compiler)
                                    ->getExecutionObjectFromPerspective (
                                        perspective, NULL);

                  if (childObject != NULL
                      && port->getEndInterfacePoint () != NULL
                      && port->getEndInterfacePoint ()->instanceOf (
                             "ContentAnchor"))
                    {
                      childEvent
                          = (NclPresentationEvent
                                 *)(((FormatterConverter *)compiler)
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
              = (NclCompositeExecutionObject *)((FormatterConverter *)
                                                    compiler)
                    ->getExecutionObjectFromPerspective (
                        compositionPerspective, NULL);

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
      selectedObject = ((FormatterConverter *)compiler)
                           ->processExecutionObjectSwitch (switchObject);

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
                  endPointObject = ((FormatterConverter *)compiler)
                                       ->getExecutionObjectFromPerspective (
                                           nodePerspective, NULL);

                  if (endPointObject != NULL)
                    {
                      selectedEvent
                          = ((FormatterConverter *)compiler)
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
FormatterScheduler::initializeDefaultSettings ()
{
  string value;

  value = presContext->getPropertyValue ("system.defaultFocusBorderColor");
  if (value != "")
    {
      SDL_Color *c = new SDL_Color();
      ginga_color_input_to_sdl_color(value,c);
      focusManager->setDefaultFocusBorderColor(c);
    }

  value = presContext->getPropertyValue ("system.defaultFocusBorderWidth");
  if (value != "")
    {
      focusManager->setDefaultFocusBorderWidth (xstrtoint (value, 10));
    }

  value = presContext->getPropertyValue ("system.defaultSelBorderColor");
  if (value != "")
    {
      SDL_Color *c = new SDL_Color();
       ginga_color_input_to_sdl_color(value,c);
      focusManager->setDefaultSelBorderColor(c);
    }
}

void
FormatterScheduler::initializeDocumentSettings (Node *node)
{
  string value;
  vector<Anchor *> *anchors;
  vector<Anchor *>::iterator i;
  vector<Node *> *nodes;
  vector<Node *>::iterator j;
  Anchor *anchor;
  PropertyAnchor *attributeAnchor;

  if (node->instanceOf ("ContentNode"))
    {
      if (((ContentNode *)node)->isSettingNode ())
        {
          anchors = ((ContentNode *)node)->getAnchors ();
          if (anchors != NULL)
            {
              i = anchors->begin ();
              while (i != anchors->end ())
                {
                  anchor = (*i);
                  if (anchor->instanceOf ("PropertyAnchor"))
                    {
                      attributeAnchor = (PropertyAnchor *)anchor;
                      value = attributeAnchor->getPropertyValue ();
                      if (value != "")
                        {
                          presContext->setPropertyValue (
                              attributeAnchor->getPropertyName (), value);
                        }
                    }
                  ++i;
                }
            }
        }
    }
  else if (node->instanceOf ("CompositeNode"))
    {
      nodes = ((CompositeNode *)node)->getNodes ();
      if (nodes != NULL)
        {
          j = nodes->begin ();
          while (j != nodes->end ())
            {
              initializeDocumentSettings (*j);
              ++j;
            }
        }
    }
  else if (node->instanceOf ("ReferNode"))
    {
      initializeDocumentSettings (
          (NodeEntity *)((ReferNode *)node)->getDataEntity ());
    }
}

void
FormatterScheduler::startDocument (const string &file)
{
  string id;
  ContextNode *body;

  vector<Port *> *ports;
  vector<NclFormatterEvent *> *events;

  NclNodeNesting *persp;
  NclFormatterEvent *evt;

  NclExecutionObject *execobj;
  NclCompositeExecutionObject *parent;

  // Parse document.
  NclParser compiler;
  this->file = xpathmakeabs (file);
  this->doc = compiler.parse (file);
  g_assert_nonnull (this->doc);

  id = this->doc->getId ();
  body = this->doc->getBody ();
  if (unlikely (body == NULL))
    syntax_error ("document has no body");

  // Get Ports.
  ports = new vector<Port *>;
  for (guint i = 0; i < body->getNumPorts (); i++)
    {
      Port *port;
      InterfacePoint *ip;

      port = body->getPort (i);
      g_assert_nonnull (port);

      ip = port->getEndInterfacePoint ();
      g_assert_nonnull (ip);
      g_assert (ip->instanceOf ("ContentAnchor")
                || ip->instanceOf ("LabeledAnchor")
                || ip->instanceOf ("PropertyAnchor"));

      ports->push_back (port);
    }

  if (ports->empty ())
    {
      g_warning ("document has no ports");
      delete ports;
      return;
    }

  persp = new NclNodeNesting ();
  persp->insertAnchorNode (body);

  // Get port events.
  events = new vector<NclFormatterEvent *>;
  for (guint i = 0; i < ports->size (); i++)
    {
      Port *port;

      port = ports->at (i);
      g_assert_nonnull (port);

      evt = this->compiler->insertContext (persp, port);
      g_assert_nonnull (evt);

      events->push_back (evt);
    }

  delete ports;
  delete persp;

  // Create execution object for settings.
  vector <Node *> *settings = this->doc->getSettingsNodes ();
  g_assert_nonnull (settings);
  for (guint i = 0; i < settings->size (); i++)
    {
      persp = new NclNodeNesting ((settings->at (i))->getPerspective ());
      execobj = this->compiler
        ->getExecutionObjectFromPerspective (persp, NULL);
      g_assert_nonnull (execobj);

      g_debug ("execution object %p for settings %s",
               execobj, persp->toString ().c_str ());

      delete persp;
    }
  delete settings;

  g_assert (!events->empty ());
  evt = events->at (0);
  g_assert_nonnull (evt);

  execobj = (NclExecutionObject *)(evt->getExecutionObject ());
  g_assert_nonnull (execobj);

  parent = (NclCompositeExecutionObject *)(execobj->getParentObject ());
  g_assert_nonnull (parent);

  g_assert_nonnull (evt);
  g_assert_nonnull (events);
  g_assert (!events->empty ());
  g_assert (!isDocumentRunning (evt));

  Thread::mutexLock (&mutexD);

  clog << "FormatterScheduler::startDocument Through event '";
  clog << evt->getId () << "'" << endl;
  evt->addEventListener (this);
  documentEvents.push_back (evt);

  Thread::mutexLock (&lMutex);
  listening.insert (evt);
  Thread::mutexUnlock (&lMutex);

  documentStatus[evt] = true;
  Thread::mutexUnlock (&mutexD);

  execobj = (NclExecutionObject *)(evt->getExecutionObject ());
  initializeDocumentSettings (execobj->getDataObject ());
  initializeDefaultSettings ();

  int docEvents = 0;
  for (guint i = 0; i < events->size (); i++)
    {
      NclFormatterEvent *event = events->at (i);

      startEvent (event);
      if (event->getCurrentState () != EventUtil::ST_SLEEPING)
        {
          docEvents++;
        }
    }

  if (docEvents == 0) {
          clog << "FormatterScheduler::startDocument 0 events running";
          clog << " stopping document" << endl;
          stopDocument(evt);
  }

  clog << "FormatterScheduler::startDocument Through event '";
  clog << evt->getId () << "' started '" << docEvents << "'";
  clog << " events" << endl;
}

void
FormatterScheduler::stopDocument (NclFormatterEvent *documentEvent)
{
  NclExecutionObject *executionObject;

  clog << "FormatterScheduler::stopDocument through '";
  clog << documentEvent->getId () << "'" << endl;

  Thread::mutexLock (&mutexD);
  if (documentStatus.count (documentEvent) != 0)
    {
      documentEvent->removeEventListener (this);
      documentStatus[documentEvent] = false;

      documentEvents.clear ();
      Thread::mutexUnlock (&mutexD);

      executionObject
          = (NclExecutionObject *)(documentEvent->getExecutionObject ());

      if (executionObject->instanceOf ("NclCompositeExecutionObject"))
        {
          ((NclCompositeExecutionObject *)executionObject)
              ->setAllLinksAsUncompiled (true);
        }

      // we can't remove the document,
      // since it can be started again
      // removeDocument(documentEvent);
      stopEvent (documentEvent);
    }
  else
    {
      documentEvents.clear ();
      Thread::mutexUnlock (&mutexD);
    }
}

void
FormatterScheduler::eventStateChanged (void *someEvent, short transition,
                                       arg_unused (short previousState))
{
  NclExecutionObject *object;
  AdapterFormatterPlayer *player;
  vector<NclFormatterEvent *>::iterator it;
  NclFormatterEvent *event;
  bool contains;
  bool hasOther;

  event = (NclFormatterEvent *)someEvent;

  clog << "FormatterScheduler::eventStateChanged '";
  clog << event->getId () << "' transition '" << transition;
  clog << "'" << endl;

  hasOther = false;
  contains = false;

  it = documentEvents.begin ();
  while (it != documentEvents.end ())
    {
      if (*it == event)
        {
          contains = true;
        }
      else if ((*it)->getCurrentState () != EventUtil::ST_SLEEPING)
        {
          hasOther = true;
        }
      ++it;
    }

  if (contains)
    {
      switch (transition)
        {
        case EventUtil::TR_STOPS:
        case EventUtil::TR_ABORTS:
          if (!hasOther)
            {
              documentEvents.clear ();

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
          object = (NclExecutionObject *) (event->getExecutionObject ());

          player = (AdapterFormatterPlayer *) playerManager->getObjectPlayer (
                  object);
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
              object = (NclExecutionObject *)(event->getExecutionObject ());

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

                  player = (AdapterFormatterPlayer *)
                               playerManager->getObjectPlayer (object);
                  if (player != NULL && player->getPlayer () != NULL)
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

                player = (AdapterFormatterPlayer *)
                  playerManager->getObjectPlayer (object);
                if (player != NULL && player->getPlayer () != NULL)
                  {
                  }
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
FormatterScheduler::prepareFormatterRegion (
    NclExecutionObject *executionObject)
{
  NclCascadingDescriptor *descriptor;
  string regionId, plan = "";
  SDLWindow* windowId = 0;

  map<int, NclFormatterLayout *>::iterator i;

  descriptor = executionObject->getDescriptor ();
  if (descriptor != NULL)
    {
      if (descriptor->getFormatterRegion () != NULL)
        {
          plan = descriptor->getFormatterRegion ()->getPlan ();
        }
      windowId = layout->prepareFormatterRegion (executionObject, plan);
    }

  return windowId;
}

void
FormatterScheduler::showObject (NclExecutionObject *executionObject)
{
  NclFormatterLayout *layout;
  NclCascadingDescriptor *descriptor;
  NclFormatterRegion *fRegion;
  LayoutRegion *region;

  descriptor = executionObject->getDescriptor ();
  if (descriptor != NULL)
    {
      region = descriptor->getRegion ();
      layout = getFormatterLayout ();
      if (region != NULL && layout != NULL)
        {
          fRegion = descriptor->getFormatterRegion ();
          if (fRegion != NULL)
            {
              fRegion->setGhostRegion (true);
            }
          layout->showObject (executionObject);
        }
    }
}

void
FormatterScheduler::hideObject (NclExecutionObject *executionObject)
{
  NclFormatterLayout *layout;
  NclCascadingDescriptor *descriptor;
  LayoutRegion *region;
  string fileUri;

  descriptor = executionObject->getDescriptor ();
  if (descriptor != NULL)
    {
      region = descriptor->getRegion ();
      layout = getFormatterLayout ();
      if (region != NULL && layout != NULL)
        {
              layout->hideObject (executionObject);
        }
    }
}

GINGA_FORMATTER_END
