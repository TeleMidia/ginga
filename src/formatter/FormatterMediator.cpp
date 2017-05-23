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
#include "FormatterMediator.h"

#include "FormatterBaseDevice.h"
#include "PresentationContext.h"
#include "Parser.h"

GINGA_FORMATTER_BEGIN

EntryEventListener::EntryEventListener (Player *player, const string &interfaceId)
{
  this->player = player;
  hasStartPoint = interfaceId != "";
  eventsRunning = 0;

  Thread::mutexInit (&evMutex, false);
}

EntryEventListener::~EntryEventListener ()
{
  set<NclFormatterEvent *>::iterator i;

  player = NULL;

  Thread::mutexLock (&evMutex);
  i = events.begin ();
  while (i != events.end ())
    {
      if (NclFormatterEvent::hasInstance (*i, false))
        {
          (*i)->removeEventListener (this);
        }
      ++i;
    }

  events.clear ();
  Thread::mutexUnlock (&evMutex);

  Thread::mutexDestroy (&evMutex);
}

void
EntryEventListener::listenEvent (NclFormatterEvent *event)
{
  Thread::mutexLock (&evMutex);
  events.insert (event);
  Thread::mutexUnlock (&evMutex);
  event->addEventListener (this);
}

void
EntryEventListener::eventStateChanged (void *event, short transition,
                                       arg_unused (short previousState))
{
  string interfaceId;
  Port *port;
  short eventType;
  string value = "";
  NclFormatterEvent *ev;
  NclExecutionObject *obj;
  NclCompositeExecutionObject *cObj = NULL;

  if (unlikely (player != NULL))
    return;

  ev = (NclFormatterEvent *) event;
  port = ((FormatterMediator *) player)->getPortFromEvent (ev);

  if (unlikely (port == NULL))
    return;

  obj = (NclExecutionObject *)(ev->getExecutionObject ());
  if (obj != NULL)
    {
      cObj = (NclCompositeExecutionObject *)obj->getParentObject ();
    }

  eventType = ev->getEventType ();
  if (eventType == EventUtil::EVT_ATTRIBUTION)
    {
      eventType = IPlayer::TYPE_ATTRIBUTION;
      value = ((NclAttributionEvent *)ev)->getCurrentValue ();
    }
  else
    {
      eventType = IPlayer::TYPE_PRESENTATION;
    }

  interfaceId = port->getId ();

  clog << "EntryEventListener::eventStateChanged ";
  if (cObj != NULL)
    {
      clog << "parent id = '" << cObj->getId () << "' ";
      clog << "parent is running = '" << cObj->isSleeping ();
      clog << "' ";
    }
  clog << "interface id = '";
  clog << interfaceId << "'";
  clog << " events running = '" << eventsRunning;
  clog << "'" << endl;

  switch (transition)
    {
    case EventUtil::TR_STARTS:
      player->notifyPlayerListeners (IPlayer::PL_NOTIFY_START,
                                     interfaceId, eventType, value);

      if (hasStartPoint && eventsRunning == 0)
        {
          player->notifyPlayerListeners (IPlayer::PL_NOTIFY_START,
                                         "", eventType, value);
        }

      eventsRunning++;
      break;

    case EventUtil::TR_PAUSES:
      player->notifyPlayerListeners (IPlayer::PL_NOTIFY_PAUSE,
                                     interfaceId, eventType, value);

      break;

    case EventUtil::TR_RESUMES:
      player->notifyPlayerListeners (IPlayer::PL_NOTIFY_RESUME,
                                     interfaceId, eventType, value);

      break;

    case EventUtil::TR_STOPS:
      player->notifyPlayerListeners (IPlayer::PL_NOTIFY_STOP,
                                     interfaceId, eventType, value);

      if (hasStartPoint && eventsRunning == 1)
        {
          player->notifyPlayerListeners (IPlayer::PL_NOTIFY_STOP,
                                         "", eventType, value);
        }

      eventsRunning--;
      break;

    case EventUtil::TR_ABORTS:
      player->notifyPlayerListeners (IPlayer::PL_NOTIFY_ABORT,
                                     interfaceId, eventType, value);

      if (hasStartPoint && eventsRunning == 1)
        {
          player->notifyPlayerListeners (IPlayer::PL_NOTIFY_ABORT,
                                         "", eventType, value);
        }

      eventsRunning--;
      break;

    default:
      break;
    }
}

short
EntryEventListener::getPriorityType ()
{
  return PT_OBJECT;
}

FormatterMediator::FormatterMediator () : Player ("")
{
  NclPlayerData *data;

  data = new NclPlayerData;
  data->baseId = "";
  data->devClass = 0;
  data->docId = "";
  data->focusManager = NULL;
  data->nodeId = "";
  data->parentDocId = "";
  data->playerId = "";
  data->transparency = 0;
  Ginga_Display->getSize (&data->w, &data->h);
  this->data = data;

  this->currentFile = "";
  this->currentDocument = NULL;

  string deviceName = "systemScreen(0)";
  deviceLayout = new DeviceLayout (deviceName);
  deviceLayout->addDevice (deviceName, data->w, data->h);

  presContext = new PresentationContext ();

  multiDevice = new FormatterBaseDevice (deviceLayout, data->w, data->h);

  multiDevice->setPresentationContex (presContext);

  playerManager = new AdapterPlayerManager (data);

  ruleAdapter = new RuleAdapter (presContext);
  compiler = new FormatterConverter (ruleAdapter);

  scheduler = new FormatterScheduler (playerManager, ruleAdapter,
                                      multiDevice, compiler);

  scheduler->addSchedulerListener (this);
  compiler->setScheduler (scheduler);
  compiler->setLinkActionListener (scheduler);

  if (data->focusManager != NULL)
    {
      scheduler->getFocusManager ()->setParent (
          (FormatterFocusManager *)(data->focusManager));
    }
  else
    {
      scheduler->setKeyHandler (true);
    }

  data->focusManager = scheduler->getFocusManager ();

  documentEvents.clear ();
  documentEntryEvents.clear ();
  portsToEntryEvents.clear ();

  entryEventListener = NULL;

  ((FormatterFocusManager *)(data->focusManager))
      ->setMotionBoundaries (data->w, data->h);

  docCompiled = false;

  Thread::mutexInit (&pteMutex, true);
}

FormatterMediator::~FormatterMediator ()
{
  string docId = "";
  NclExecutionObject *bodyObject;
  map<string, NclFormatterEvent *>::iterator i;
  map<string, vector<NclFormatterEvent *> *>::iterator j;
  vector<NclFormatterEvent *>::iterator k;

  clog << "FormatterMediator::~FormatterMediator '";
  clog << data->playerId << "'";
  clog << endl;

  docCompiled = false;

  if (scheduler != NULL)
    {
      scheduler->removeSchedulerListener (this);
    }

  if (entryEventListener != NULL)
    {
      delete entryEventListener;
      entryEventListener = NULL;
    }

  if (currentDocument != NULL)
    {
      docId = currentDocument->getId ();
      if (compiler != NULL)
        {
          bodyObject = compiler->getObjectFromNodeId (docId);
          if (bodyObject == NULL)
            {
              bodyObject = compiler->getObjectFromNodeId (
                  currentDocument->getBody ()->getId ());
            }
          if (bodyObject != NULL)
            {
              compiler->removeExecutionObject (bodyObject);
            }
        }
      if (!removeDocument ())
        {
          clog << "FormatterMediator::~FormatterMediator Warning! Can't";
          clog << " remove document '" << docId << "'";
          clog << endl;
        }
      currentDocument = NULL;
    }

  i = documentEvents.begin ();
  while (i != documentEvents.end ())
    {
      delete i->second;
      ++i;
    }
  documentEvents.clear ();

  j = documentEntryEvents.begin ();
  while (j != documentEntryEvents.end ())
    {
      k = j->second->begin ();
      while (k != j->second->end ())
        {
          delete *k;
          ++k;
        }
      delete j->second;
      ++j;
    }
  documentEntryEvents.clear ();

  Thread::mutexLock (&pteMutex);
  portsToEntryEvents.clear ();
  Thread::mutexUnlock (&pteMutex);

  if (ruleAdapter != NULL)
    {
      delete ruleAdapter;
      ruleAdapter = NULL;
    }

  if (scheduler != NULL)
    {
      delete scheduler;
      scheduler = NULL;
    }

  if (compiler != NULL)
    {
      delete compiler;
      compiler = NULL;
    }

  if (data != NULL)
    {
      delete data;
      data = NULL;
    }

  if (deviceLayout != NULL)
    {
      delete deviceLayout;
      deviceLayout = NULL;
    }

  if (multiDevice != NULL)
    {
      delete multiDevice;
      multiDevice = NULL;
    }

  if (presContext != NULL)
    {
      delete presContext;
      presContext = NULL;
    }

  if (playerManager != NULL)
    {
      delete playerManager;
      playerManager = NULL;
    }
  clog << "FormatterMediator::~FormatterMediator all done" << endl;
}

void *
FormatterMediator::addDocument (const string &file)
{
  NclParser compiler (this->deviceLayout);

  this->currentFile = xpathmakeabs (file);
  this->currentDocument = compiler.parse (file);
  g_assert_nonnull (this->currentDocument);

  return this->currentDocument;
}

bool
FormatterMediator::removeDocument ()
{
  this->stop ();
  g_assert_nonnull (this->currentDocument);
  delete this->currentDocument;
  return true;
}

Port *
FormatterMediator::getPortFromEvent (NclFormatterEvent *event)
{
  ContextNode *context;
  Port *port = NULL;
  Anchor *anchor;
  guint i, size;
  map<Port *, NclFormatterEvent *>::iterator j;

  if (event->instanceOf ("NclAnchorEvent"))
    {
      anchor = ((NclAnchorEvent *)event)->getAnchor ();
      context = currentDocument->getBody ();
      size = context->getNumPorts ();

      for (i = 0; i < size; i++)
        {
          port = context->getPort (i);
          if (anchor == port->getEndInterfacePoint ())
            {
              return port;
            }
        }
    }
  else
    {
      Thread::mutexLock (&pteMutex);
      j = portsToEntryEvents.begin ();
      while (j != portsToEntryEvents.end ())
        {
          if (j->second == event)
            {
              Thread::mutexUnlock (&pteMutex);
              return j->first;
            }
          ++j;
        }
      Thread::mutexUnlock (&pteMutex);
    }

  return NULL;
}

void
FormatterMediator::initializeSettingNodes (Node *node)
{
  vector<Node *> *nodes;
  vector<Node *>::iterator i;
  NclNodeNesting *perspective;
  NclExecutionObject *object;
  NodeEntity *nodeEntity;

  if (!node->instanceOf ("CompositeNode"))
    {
      clog << "FormatterMediator::initializeSettingNodes return";
      clog << endl;
      return;
    }

  nodes = ((CompositeNode *)node)->getNodes ();
  if (nodes != NULL)
    {
      i = nodes->begin ();
      while (i != nodes->end ())
        {
          nodeEntity = (NodeEntity *)((*i)->getDataEntity ());
          clog << "FormatterMediator::initializeSettingNodes checking '";
          clog << nodeEntity->getId () << "'";
          clog << endl;

          if (nodeEntity->instanceOf ("ContentNode"))
            {
              if (((ContentNode *)nodeEntity)->isSettingNode ())
                {
                  perspective
                    = new NclNodeNesting ((*i)->getPerspective ());

                  object = ((FormatterConverter *)compiler)
                               ->getExecutionObjectFromPerspective (
                                   perspective, NULL,
                                   ((FormatterConverter *)compiler)
                                       ->getDepthLevel ());

                  if (object != NULL)
                    {
                      clog << "FormatterMediator::";
                      clog << "initializeSettingNodes ";
                      clog << "created setting node '" << object->getId ();
                      clog << "'" << endl;
                    }

                  delete perspective;
                }
            }
          else if (nodeEntity->instanceOf ("CompositeNode"))
            {
              initializeSettingNodes (nodeEntity);
            }
          ++i;
        }
    }
}

bool
FormatterMediator::compileDocument (const string &id)
{
  vector<NclFormatterEvent *> *events;
  vector<Port *> *ports;
  ContextNode *body;
  NclNodeNesting *persp;

  NclFormatterEvent *evt;
  NclExecutionObject *execobj;
  NclCompositeExecutionObject *parent;

  g_assert (this->documentEvents.count (id) == 0);

  body = this->currentDocument->getBody ();
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
      return true;
    }

  g_assert_null (this->entryEventListener);
  this->entryEventListener = new EntryEventListener (this, "");

  persp = new NclNodeNesting ();
  persp->insertAnchorNode (body);

  // Get port events.
  events = new vector<NclFormatterEvent *>;
  for (guint i = 0; i < ports->size (); i++)
    {
      Port *port;

      port = ports->at (i);
      g_assert_nonnull (port);

      evt = compiler->insertContext (persp, port);
      g_assert_nonnull (evt);

      Thread::mutexLock (&pteMutex);
      this->portsToEntryEvents[port] = evt;
      Thread::mutexUnlock (&pteMutex);

      events->push_back (evt);
      this->entryEventListener->listenEvent (evt);
    }

  delete ports;
  delete persp;

  g_assert (!events->empty ());
  initializeSettingNodes (body);

  evt = events->at (0);

  execobj = (NclExecutionObject *)(evt->getExecutionObject ());
  g_assert_nonnull (execobj);

  parent = (NclCompositeExecutionObject *)(execobj->getParentObject ());
  g_assert_nonnull (parent);

  this->documentEvents[id] = evt;
  this->documentEntryEvents[id] = events;

  docCompiled = true;
  return true;
}

NclFormatterEvent *
FormatterMediator::getEntryEvent (const string &interfaceId,
                                  vector<NclFormatterEvent *> *events)
{
  map<Port *, NclFormatterEvent *>::iterator i;
  vector<NclFormatterEvent *>::iterator j;
  NclFormatterEvent *entryEvent = NULL;

  Thread::mutexLock (&pteMutex);
  i = portsToEntryEvents.begin ();
  while (i != portsToEntryEvents.end ())
    {
      if (i->first->getId () == interfaceId)
        {
          j = events->begin ();
          while (j != events->end ())
            {
              if (i->second == (*j))
                {
                  entryEvent = i->second;
                  break;
                }
              ++j;
            }
        }
      ++i;
    }
  Thread::mutexUnlock (&pteMutex);

  return entryEvent;
}

void
FormatterMediator::presentationCompleted (arg_unused (NclFormatterEvent *documentEvent))
{
  string documentId;
  map<string, NclFormatterEvent *>::iterator i;

  clog << "FormatterMediator::presentationCompleted for '";
  clog << data->playerId;
  clog << "'" << endl;

  docCompiled = false;

  documentEvents.clear ();
  documentEntryEvents.clear ();

  Player::notifyPlayerListeners (PL_NOTIFY_STOP, data->playerId,
                                 TYPE_PRESENTATION);
}

bool
FormatterMediator::play ()
{
  NclFormatterEvent *docevt;
  vector<NclFormatterEvent *> *evts;
  string id;

  g_assert_nonnull (this->currentDocument);
  id = currentDocument->getId ();

  g_assert (!this->docCompiled);
  compileDocument (id);

  Player::play ();

  if (unlikely (this->documentEvents.count (id) == 0))
    {
      Player::stop ();
      return true;
    }

  g_assert (this->documentEvents.count (id) > 0);
  docevt = this->documentEvents[id];
  g_assert_nonnull (docevt);

  evts = this->documentEntryEvents[id];
  g_assert_nonnull (evts);

  scheduler->startDocument (docevt, evts);
  return true;
}

void
FormatterMediator::stop ()
{
  NclFormatterEvent *evt;
  string id;

  g_assert_nonnull (this->currentDocument);
  Player::stop ();

  id = currentDocument->getId ();
  evt = documentEvents[id];
  g_assert_nonnull (evt);
  this->scheduler->stopDocument (evt);
}

void
FormatterMediator::abort ()
{
  this->stop ();
}

void
FormatterMediator::pause ()
{
  NclFormatterEvent *evt;
  string id;

  g_assert_nonnull (this->currentDocument);
  Player::pause ();

  id = currentDocument->getId ();
  evt = documentEvents[id];
  g_assert_nonnull (evt);
  this->scheduler->pauseDocument (evt);
}

void
FormatterMediator::resume ()
{
  NclFormatterEvent *evt;
  string id;

  g_assert_nonnull (this->currentDocument);
  Player::resume ();

  id = currentDocument->getId ();
  evt = documentEvents[id];
  g_assert_nonnull (evt);
  this->scheduler->resumeDocument (evt);
}

GINGA_FORMATTER_END
