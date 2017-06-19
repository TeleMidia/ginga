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
#include "FormatterScheduler.h"

#include "Converter.h"
#include "Parser.h"

#include "mb/Display.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_FORMATTER_BEGIN

FormatterScheduler::FormatterScheduler ()
{
  int w, h;
  Ginga_Display->getSize (&w, &h);

  this->settings = new Settings ();
  this->ruleAdapter = new RuleAdapter (settings);
  this->compiler = new Converter (this->ruleAdapter);
  this->compiler->setLinkActionListener (this);
  this->focusManager = new FocusManager
    (this, this->settings, this, this->compiler);
  this->focusManager->setKeyHandler (true);
}

FormatterScheduler::~FormatterScheduler ()
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
FormatterScheduler::addAction (NclLinkSimpleAction *action)
{
  actions.insert (action);
}

void
FormatterScheduler::removeAction (NclLinkSimpleAction *action)
{
  set<NclLinkSimpleAction *>::iterator i;

  i = actions.find (action);
  if (i != actions.end ())
    actions.erase (i);
}

FocusManager *
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
  assert (action != NULL);
  runAction (action);
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
  ExecutionObject *obj;
  NclCascadingDescriptor *descriptor;
  PlayerAdapter *player;
  SDLWindow *win = NULL;

  obj = event->getExecutionObject ();
  g_assert_nonnull (obj);

  TRACE ("running action '%s' over event '%s'",
         action->getTypeString ().c_str (),
         event->getId ().c_str ());

  g_assert (obj->isCompiled ());

  if (obj->instanceOf ("ExecutionObjectSwitch")
      && event->instanceOf ("NclSwitchEvent"))
    {
      runActionOverSwitch ((ExecutionObjectSwitch *)obj,
                           (NclSwitchEvent *)event, action);
      return;
    }

  if (obj->instanceOf ("ExecutionObjectContext")
      && (obj->getDescriptor () == NULL))
    {
      runActionOverComposition
        ((ExecutionObjectContext *) obj, action);
      return;
    }

  if (event->instanceOf ("NclAttributionEvent"))
    {
      runActionOverProperty (event, action);
      return;
    }

  player = this->getObjectPlayer (obj);

  if (unlikely (player == NULL))
    {
      WARNING ("no player to decode '%s', skipping action",
               obj->getId ().c_str ());
      return;
    }

  if (obj->instanceOf ("ExecutionObjectApplication")
      && !event->instanceOf ("ExecutionObjectApplication"))
    {
      runActionOverApplicationObject
        ((ExecutionObjectApplication *)obj, event,
         player, action);
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

          if (unlikely (!player
                        ->prepare (obj, (NclPresentationEvent *) event)))
            {
              WARNING ("failed to prepare player of '%s'",
                       obj->getId ().c_str ());
              return;
            }

          win = this->prepareFormatterRegion (obj);

          // FIXME: Sometimes win is NULL!
          // g_assert_nonnull (win);

          player->setOutputWindow (win);
          event->addEventListener (this);
        }

      if (unlikely (!player->start ()))
        {
          WARNING ("failed to start player of '%s'",
                   obj->getId ().c_str ());
          if (event->getCurrentState () == EventUtil::ST_SLEEPING)
            event->removeEventListener (this);
        }
      break;

    case ACT_PAUSE:
      if (unlikely (!player->pause ()))
        WARNING ("failed to pause player of '%s'", obj->getId ().c_str ());
      break;

    case ACT_RESUME:
      if (unlikely (!player->resume ()))
        WARNING ("failed to resume player of '%s'", obj->getId ().c_str ());
      break;

    case ACT_ABORT:
      if (unlikely (!player->abort ()))
        WARNING ("failed to abort player of '%s'", obj->getId ().c_str ());
      break;

    case ACT_STOP:
      if (unlikely (!player->stop ()))
        WARNING ("failed to stop player of '%s'", obj->getId ().c_str ());
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
      propName = ((NclAttributionEvent *)event)
                     ->getAnchor ()
                     ->getName ();

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

              anim->setDuration (durVal);
              anim->setBy (byVal);

              player->getPlayer()->
                setAnimatorProperties(durVal,((NclAttributionEvent *)event)
                                      ->getAnchor ()
                                      ->getName (), propValue);
            }
          else if (player != NULL && player->hasPrepared ())
            {
              player->setProperty ((NclAttributionEvent *)event,
                                        propValue);

              event->stop ();
            }
          else
            {
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
    ExecutionObjectApplication *executionObject,
    NclFormatterEvent *event, PlayerAdapter *player,
    NclLinkSimpleAction *action)
{
  NclCascadingDescriptor *descriptor;
  Player *playerContent;

  int actionType = action->getType ();
  SDLWindow *win = NULL;

  switch (actionType)
    {
    case ACT_START:
      TRACE ("START '%s'", event->getId().c_str());
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
      if (player->setCurrentEvent (event))
        {
          if (!player->start ())
            {
              WARNING ("cannot start '%s'",
                       executionObject->getId ().c_str());

              // checking if player failed to start
              if (event->getCurrentState () == EventUtil::ST_SLEEPING)
                event->removeEventListener (this);
            }
        }
      break;

    case ACT_PAUSE:
      if (player->setCurrentEvent (event))
        player->pause ();
      break;

    case ACT_RESUME:
      if (player->setCurrentEvent (event))
        player->resume ();
      break;

    case ACT_ABORT:
      if (player->setCurrentEvent (event))
        player->abort ();
      break;

    case ACT_STOP:
      if (player->setCurrentEvent (event))
        player->stop ();
      break;

    default:
      g_assert_not_reached ();
    }
}

void
FormatterScheduler::runActionOverComposition (
    ExecutionObjectContext *compositeObject,
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
          propName = attrEvent->getAnchor ()->getName ();
          propValue = ((NclLinkAssignmentAction *)action)->getValue ();
          event = compositeObject->getEventFromAnchorId (propName);

          if (event != NULL)
            {
              event->start ();
              ((NclAttributionEvent *)event)->setValue (propValue);
              event->stop ();
            }
          else
            {
              attrEvent->stop ();
            }

          objects = compositeObject->getExecutionObjects ();
          if (objects == NULL)
              return;

          j = objects->begin ();
          while (j != objects->end ())
            {
              childObject = j->second;
              if (childObject->instanceOf ("ExecutionObjectContext"))
                {
                  clog << "FormatterScheduler::runActionOverComposition ";
                  clog << "'" << compositeObject->getId () << "' has '";
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
              = (ExecutionObjectContext *) (compiler
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
    ExecutionObjectSwitch *switchObject, NclSwitchEvent *event,
    NclLinkSimpleAction *action)
{
  ExecutionObject *selectedObject;
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
FormatterScheduler::runSwitchEvent (ExecutionObjectSwitch *switchObject,
                                    NclSwitchEvent *switchEvent,
                                    ExecutionObject *selectedObject,
                                    NclLinkSimpleAction *action)
{
  NclFormatterEvent *selectedEvent;
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
  ExecutionObject *refObject;
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
      refObject = ((ExecutionObject *)(refEvent->getExecutionObject ()));

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
    ERROR_SYNTAX ("document has no body");

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
  ExecutionObject *object;
  PlayerAdapter *player;
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

          player = this->getObjectPlayer (object);
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

              if (object->instanceOf ("ExecutionObjectApplication"))
                {
                  if (!((ExecutionObjectApplication *)object)
                           ->isSleeping ())
                    {
                      hideObj = false;
                    }
                }

              if (hideObj)
                {
                  clog << "FormatterScheduler::eventStateChanged '";
                  clog << event->getId ();
                  clog << "' STOPS: hideObject '" << object->getId ();
                  clog << endl;

                  this->focusManager->hideObject (object);
                  this->hideObject (object);

                  player = this->getObjectPlayer (object);
                }
            }
          break;

        case EventUtil::TR_ABORTS:
          {
            bool hideObj = true;

            event->removeEventListener (this);
            object = (ExecutionObject *)(event->getExecutionObject ());

            if (object->instanceOf ("ExecutionObjectApplication"))
              {
                if (!((ExecutionObjectApplication *)object)->isSleeping ())
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

                player = this->getObjectPlayer (object);
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
FormatterScheduler::prepareFormatterRegion (ExecutionObject *obj)
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
FormatterScheduler::showObject (ExecutionObject *obj)
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
FormatterScheduler::hideObject (ExecutionObject *obj)
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

bool
FormatterScheduler::removePlayer (ExecutionObject *exObject)
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
FormatterScheduler::initializePlayer (ExecutionObject *object)
{
  g_assert_nonnull (object);

  NodeEntity *entity
      = (NodeEntity *)(object->getDataObject ()->getDataEntity ());
  g_assert_nonnull (entity);

  ContentNode *contentNode = dynamic_cast<ContentNode *> (entity);
  g_assert_nonnull (contentNode);

  if (contentNode->isSettingNode ())
    return nullptr;             // nothing to do

  PlayerAdapter *adapter = new PlayerAdapter (this);
  _objectPlayers[object->getId ()] = adapter;

  return adapter;
}

PlayerAdapter *
FormatterScheduler::getObjectPlayer (ExecutionObject *execObj)
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
