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
#include "FormatterConverter.h"

#include "NclLinkTransitionTriggerCondition.h"

#include "FormatterMultiDevice.h"
#include "FormatterScheduler.h"

#include "ncl/EventUtil.h"
using namespace ::ginga::ncl;

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_FORMATTER_BEGIN

FormatterScheduler::FormatterScheduler (AdapterPlayerManager *playerManager,
                                        RuleAdapter *ruleAdapter,
                                        FormatterMultiDevice *multiDevice,
                                        void *compiler)
{
  this->playerManager = playerManager;
  this->ruleAdapter = ruleAdapter;
  this->presContext = ruleAdapter->getPresentationContext ();
  this->multiDevPres = multiDevice;
  this->compiler = compiler;
  this->running = false;
  this->focusManager = new FormatterFocusManager (
      this->playerManager, presContext, multiDevPres, this,
      (FormatterConverter *)compiler);

  this->presContext->setGlobalVarListener (this);

  this->multiDevPres->setFocusManager (this->focusManager);

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

  if (presContext != NULL)
    {
      this->presContext->setGlobalVarListener (NULL);
    }

  ruleAdapter = NULL;
  presContext = NULL;

  if (focusManager != NULL)
    {
      delete focusManager;
      focusManager = NULL;
    }

  schedulerListeners.clear ();
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

void *
FormatterScheduler::getFormatterLayout (void *descriptor, void *object)
{
  return ((FormatterMultiDevice *)multiDevPres)
      ->getFormatterLayout ((NclCascadingDescriptor *)descriptor,
                            (NclExecutionObject *)object);
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
FormatterScheduler::setTimeBaseObject (NclExecutionObject *object,
                                       AdapterFormatterPlayer *objectPlayer,
                                       string nodeId)
{
  NclExecutionObject *documentObject;
  NclExecutionObject *parentObject;
  NclExecutionObject *timeBaseObject;

  Node *documentNode;
  Node *compositeNode;
  Node *timeBaseNode;
  NclNodeNesting *perspective;
  NclNodeNesting *compositePerspective;
  AdapterFormatterPlayer *timeBasePlayer;

  if (nodeId.find_last_of ('#') != std::string::npos)
    {
      return;
    }

  documentObject = object;
  parentObject = (NclExecutionObject *)(documentObject->getParentObject ());
  if (parentObject != NULL)
    {
      while (parentObject->getParentObject () != NULL)
        {
          documentObject = parentObject;
          if (documentObject->getDataObject ()->instanceOf ("ReferNode"))
            {
              break;
            }
          parentObject
              = (NclExecutionObject *)(documentObject->getParentObject ());
        }
    }

  if (documentObject == NULL || documentObject->getDataObject () == NULL)
    {
      return;
    }

  documentNode = documentObject->getDataObject ();
  if (documentNode->instanceOf ("ReferNode"))
    {
      compositeNode
          = (NodeEntity *)((ReferNode *)documentNode)->getReferredEntity ();
    }
  else
    {
      compositeNode = documentNode;
    }

  if (compositeNode == NULL
      || !(compositeNode->instanceOf ("CompositeNode")))
    {
      return;
    }

  timeBaseNode
      = ((CompositeNode *)compositeNode)->recursivelyGetNode (nodeId);

  if (timeBaseNode == NULL || !(timeBaseNode->instanceOf ("ContentNode")))
    {
      return;
    }

  perspective = new NclNodeNesting (timeBaseNode->getPerspective ());
  if (documentNode->instanceOf ("ReferNode"))
    {
      perspective->removeHeadNode ();
      compositePerspective
          = new NclNodeNesting (documentNode->getPerspective ());

      compositePerspective->append (perspective);
      perspective = compositePerspective;
    }

  try
    {
      timeBaseObject
          = ((FormatterConverter *)compiler)
                ->getExecutionObjectFromPerspective (
                    perspective, NULL,
                    ((FormatterConverter *)compiler)->getDepthLevel ());

      if (timeBaseObject != NULL)
        {
          timeBasePlayer
              = (AdapterFormatterPlayer *)playerManager->getObjectPlayer (
                  timeBaseObject);
          if (timeBasePlayer != NULL)
            {
              objectPlayer->setTimeBasePlayer (timeBasePlayer);
            }
        }
    }
  catch (exception *exc)
    {
      return;
    }
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
                  descriptor->setFormatterLayout
                    (getFormatterLayout (descriptor, executionObject));
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

          if (executionObject->getDescriptor () != NULL)
            {
              attValue = (executionObject->getDescriptor ())
                ->getParameterValue ("x-timeBaseObject");
              if (attValue != "")
                {
                  setTimeBaseObject (executionObject, player, attValue);
                }
            }

          winId = ((FormatterMultiDevice *) multiDevPres)
            ->prepareFormatterRegion (executionObject);

          g_assert_nonnull (winId);
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

      /*clog << "FormatterScheduler::runActionOverProperty settingnode";
      clog << " evId '" << event->getId() << "' for '";
      clog << executionObject->getId() << "' propName '";
      clog << porpName << "', propValue '" << propValue << "'" << endl;*/

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

              AnimationController::startAnimation (
                  executionObject, playerManager, player,
                  (NclAttributionEvent *)event, propValue, anim);

              anim->setDuration (durVal);
              anim->setBy (byVal);
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

  double time = xruntime_ms ();
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
                  descriptor->setFormatterLayout (
                      getFormatterLayout (descriptor, executionObject));
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

          if (executionObject->getDescriptor () != NULL)
            {
              // look for a reference time base player
              attValue
                  = executionObject->getDescriptor ()->getParameterValue (
                      "x-timeBaseObject");

              if (attValue != "")
                {
                  setTimeBaseObject (executionObject, player, attValue);
                }
            }

          if (playerContent != NULL)
            {
              winId
                  = ((FormatterMultiDevice *)multiDevPres)
                        ->prepareFormatterRegion (
                            executionObject);

              player->setOutputWindow (winId);
              if (player->getObjectDevice () == 0)
                {
                }
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

      time = xruntime_ms () - time;
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

          /*clog << "FormatterScheduler::runActionOverComposition ";
          clog << "Run SET action over COMPOSITION '";
          clog << compositeObject->getId() << "' event '";
          clog << propName << "' value '";
          clog << propValue << "'";
          clog << endl;*/

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
                                        perspective, NULL,
                                        ((FormatterConverter *)compiler)
                                            ->getDepthLevel ());

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
                        compositionPerspective, NULL,
                        ((FormatterConverter *)compiler)->getDepthLevel ());

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
                                           nodePerspective, NULL,
                                           ((FormatterConverter *)compiler)
                                               ->getDepthLevel ());

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

  value = presContext->getPropertyValue (DEFAULT_FOCUS_BORDER_COLOR);
  if (value != "")
    {
      SDL_Color *c = new SDL_Color();
      ginga_color_input_to_sdl_color(value,c);
      focusManager->setDefaultFocusBorderColor(c);
    }

  value = presContext->getPropertyValue (DEFAULT_FOCUS_BORDER_WIDTH);
  if (value != "")
    {
      focusManager->setDefaultFocusBorderWidth (xstrto_int (value));
    }

  value = presContext->getPropertyValue (DEFAULT_SEL_BORDER_COLOR);
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
FormatterScheduler::startDocument (NclFormatterEvent *documentEvent,
                                   vector<NclFormatterEvent *> *entryEvents)
{
  NclExecutionObject *object;
  vector<NclFormatterEvent *>::iterator it;
  int docEvents = 0;
  int i, size;
  NclFormatterEvent *event;

  if (documentEvent == NULL || entryEvents == NULL)
    {
      clog << "FormatterScheduler::startDocument Warning! ";
      clog << "documentEvent == NULL || entryEvents == NULL" << endl;
      return;
    }

  if (entryEvents->empty ())
    {
      clog << "FormatterScheduler::startDocument Warning! ";
      clog << "entryEvents is empty" << endl;
      return;
    }

  if (isDocumentRunning (documentEvent))
    {
      size = (int) entryEvents->size ();
      for (i = 0; i < size; i++)
        {
          event = (*entryEvents)[i];

          if (event->getCurrentState () == EventUtil::ST_SLEEPING)
            {
              startEvent (event);
            }
        }
      return;
    }

  Thread::mutexLock (&mutexD);
  for (it = documentEvents.begin (); it != documentEvents.end (); ++it)
    {
      if (*it == documentEvent)
        {
          clog << "FormatterScheduler::startDocument Warning! ";
          clog << "Can't start document through event '";
          clog << documentEvent->getId () << "'" << endl;

          Thread::mutexUnlock (&mutexD);
          return;
        }
    }

  clog << "FormatterScheduler::startDocument Through event '";
  clog << documentEvent->getId () << "'" << endl;
  documentEvent->addEventListener (this);
  documentEvents.push_back (documentEvent);

  Thread::mutexLock (&lMutex);
  listening.insert (documentEvent);
  Thread::mutexUnlock (&lMutex);

  documentStatus[documentEvent] = true;
  Thread::mutexUnlock (&mutexD);

  object = (NclExecutionObject *)(documentEvent->getExecutionObject ());
  initializeDocumentSettings (object->getDataObject ());
  initializeDefaultSettings ();

  size = (int) entryEvents->size ();
  for (i = 0; i < size; i++)
    {
      event = (*entryEvents)[i];

      startEvent (event);
      if (event->getCurrentState () != EventUtil::ST_SLEEPING)
        {
          docEvents++;
        }
    }

  if (docEvents == 0) {
          clog << "FormatterScheduler::startDocument 0 events running";
          clog << " stopping document" << endl;
          stopDocument(documentEvent);
  }

  clog << "FormatterScheduler::startDocument Through event '";
  clog << documentEvent->getId () << "' started '" << docEvents << "'";
  clog << " events" << endl;
}

void
FormatterScheduler::removeDocument (NclFormatterEvent *documentEvent)
{
  NclExecutionObject *obj;
  vector<NclFormatterEvent *>::iterator i;
  map<NclFormatterEvent *, bool>::iterator j;

  // TODO: do a better way to remove documents (see lockComposite)
  obj = (NclExecutionObject *)(documentEvent->getExecutionObject ());

  clog << "FormatterScheduler::removeDocument through '";
  clog << obj->getId () << "'" << endl;

  if (compiler != NULL && obj != NULL)
    {
      ((FormatterConverter *)compiler)->removeExecutionObject (obj);
    }

  Thread::mutexLock (&mutexD);
  for (i = documentEvents.begin (); i != documentEvents.end (); ++i)
    {
      if (*i == documentEvent)
        {
          documentEvents.erase (i);
          break;
        }
    }

  j = documentStatus.find (documentEvent);
  if (j != documentStatus.end ())
    {
      documentStatus.erase (j);
    }
  Thread::mutexUnlock (&mutexD);
}

void
FormatterScheduler::stopDocument (NclFormatterEvent *documentEvent)
{
  NclExecutionObject *executionObject;
  vector<IFormatterSchedulerListener *>::iterator i;
  IFormatterSchedulerListener *listener;

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

      i = schedulerListeners.begin ();
      while (i != schedulerListeners.end ())
        {
          listener = *i;
          listener->presentationCompleted (documentEvent);

          schedulerListeners.erase (i);
          i = schedulerListeners.begin ();
        }
    }
  else
    {
      documentEvents.clear ();
      Thread::mutexUnlock (&mutexD);
    }
}

void
FormatterScheduler::pauseDocument (NclFormatterEvent *documentEvent)
{
  vector<NclFormatterEvent *>::iterator i;
  for (i = documentEvents.begin (); i != documentEvents.end (); ++i)
    {
      if (*i == documentEvent)
        {
          Thread::mutexLock (&mutexD);
          documentStatus[documentEvent] = false;
          Thread::mutexUnlock (&mutexD);
          pauseEvent (documentEvent);
          break;
        }
    }
}

void
FormatterScheduler::resumeDocument (NclFormatterEvent *documentEvent)
{
  bool contains;
  contains = false;
  vector<NclFormatterEvent *>::iterator i;
  for (i = documentEvents.begin (); i != documentEvents.end (); ++i)
    {
      if (*i == documentEvent)
        {
          contains = true;
          break;
        }
    }

  if (contains)
    {
      resumeEvent (documentEvent);
      Thread::mutexLock (&mutexD);
      documentStatus[documentEvent] = true;
      Thread::mutexUnlock (&mutexD);
    }
}

void
FormatterScheduler::stopAllDocuments ()
{
  int i, size;
  vector<NclFormatterEvent *> *auxDocEventList;
  NclFormatterEvent *documentEvent;

  if (!documentEvents.empty ())
    {
      auxDocEventList = new vector<NclFormatterEvent *> (documentEvents);

      size = (int) auxDocEventList->size ();
      for (i = 0; i < size; i++)
        {
          documentEvent = (*auxDocEventList)[i];
          stopDocument (documentEvent);
        }

      auxDocEventList->clear ();
      delete auxDocEventList;
      auxDocEventList = NULL;
    }
}

void
FormatterScheduler::pauseAllDocuments ()
{
  int i, size;
  NclFormatterEvent *documentEvent;

  if (!documentEvents.empty ())
    {
      size = (int) documentEvents.size ();
      for (i = 0; i < size; i++)
        {
          documentEvent = documentEvents[i];
          pauseDocument (documentEvent);
        }
    }
}

void
FormatterScheduler::resumeAllDocuments ()
{
  if (documentEvents.empty ())
    return;                     // nothing to do

  for (size_t i = 0; i < documentEvents.size (); i++)
    resumeDocument (documentEvents[i]);
}

void
FormatterScheduler::eventStateChanged (void *someEvent, short transition,
                                       arg_unused (short previousState))
{
  NclExecutionObject *object;
  AdapterFormatterPlayer *player;
  vector<IFormatterSchedulerListener *>::iterator i;
  vector<NclFormatterEvent *>::iterator it;
  IFormatterSchedulerListener *listener;
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

              i = schedulerListeners.begin ();
              while (i != schedulerListeners.end ())
                {
                  listener = *i;
                  listener->presentationCompleted (event);

                  ++i;
                }

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
              ((FormatterMultiDevice *)multiDevPres)->showObject (object);

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

                  focusManager->hideObject (object);
                  ((FormatterMultiDevice *)multiDevPres)
                      ->hideObject (object);

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

                focusManager->hideObject (object);
                ((FormatterMultiDevice *)multiDevPres)->hideObject (object);

                player = (AdapterFormatterPlayer *)
                  playerManager->getObjectPlayer (object);
                if (player != NULL && player->getPlayer () != NULL
                    && player->getObjectDevice () == 0)
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

short
FormatterScheduler::getPriorityType ()
{
  return INclEventListener::PT_CORE;
}

void
FormatterScheduler::addSchedulerListener (
    IFormatterSchedulerListener *listener)
{
  bool contains;
  contains = false;
  vector<IFormatterSchedulerListener *>::iterator i;
  for (i = schedulerListeners.begin (); i != schedulerListeners.end (); ++i)
    {
      if (*i == listener)
        {
          contains = true;
          break;
        }
    }

  if (!contains)
    {
      schedulerListeners.push_back (listener);
    }
}

void
FormatterScheduler::removeSchedulerListener (
    IFormatterSchedulerListener *listener)
{
  vector<IFormatterSchedulerListener *>::iterator i;
  for (i = schedulerListeners.begin (); i != schedulerListeners.end (); ++i)
    {
      if (*i == listener)
        {
          schedulerListeners.erase (i);
          return;
        }
    }
}

void
FormatterScheduler::receiveGlobalAttribution (const string &pName,
                                              const string &value)
{
  set<NclExecutionObject *> *objs;
  set<NclExecutionObject *>::iterator i;

  set<string> *names;
  set<string>::iterator j;

  NclExecutionObject *object;
  NclFormatterEvent *event;
  NclLinkSimpleAction *fakeAction;

  clog << "FormatterScheduler::receiveGlobalAttribution ";
  clog << "prop = '" << pName << "', value = '" << value;
  clog << "'" << endl;

  objs = ((FormatterConverter *)compiler)->getSettingNodeObjects ();
  if (objs == NULL || objs->empty ())
    {
      clog << "FormatterScheduler::receiveGlobalAttribution ";
      clog << "there is 0 setting objects" << endl;
      return;
    }

  // call runAction
  i = objs->begin ();
  while (i != objs->end ())
    {
      object = *i;
      event = object->getEventFromAnchorId (pName);
      if (event != NULL && event->instanceOf ("NclAttributionEvent"))
        {
          fakeAction = new NclLinkAssignmentAction (
              event, ACT_START, value);

          runAction (fakeAction);
          delete fakeAction;
          return;
        }

      clog << "FormatterScheduler::receiveGlobalAttribution ";
      clog << "can't find anchor with name = '" << pName;
      clog << "' inside object '" << object->getId () << "'";
      clog << endl;

      ++i;
    }

  names = presContext->getPropertyNames ();
  j = names->find (pName);
  if (j != names->end ())
    {
      presContext->setPropertyValue (pName, value);
    }

  delete names;
}

GINGA_FORMATTER_END
