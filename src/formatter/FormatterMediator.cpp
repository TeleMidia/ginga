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

#include "mb/Display.h"
#include "FormatterBaseDevice.h"

#include "PresentationContext.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

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
  data->x = 0;
  data->y = 0;
  Ginga_Display->getSize (&data->w, &data->h);
  data->privateBaseContext = new PrivateBaseContext ();
  this->data = data;

  this->currentFile = "";
  this->currentDocument = NULL;

  string deviceName = "systemScreen(0)";
  deviceLayout = new DeviceLayout (deviceName);
  deviceLayout->addDevice (deviceName, data->x, data->y, data->w, data->h);

  presContext = new PresentationContext ();

  multiDevice = new FormatterBaseDevice (
      deviceLayout, data->playerId, data->x, data->y,
      data->w, data->h);

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

  privateBaseContext = (PrivateBaseContext *)(data->privateBaseContext);

  privateBaseContext->createPrivateBase (data->baseId);
  ((FormatterFocusManager *)(data->focusManager))
      ->setMotionBoundaries (data->x, data->y, data->w, data->h);

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
      if (!removeDocument (docId))
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

void
FormatterMediator::setMrl (const string &mrl, bool visible)
{
  this->mrl = mrl;
  this->visible = visible;
}

void
FormatterMediator::setParentLayout (void *parentLayout)
{
  NclFormatterLayout *mainLayout;

  clog << "FormatterMediator::setParentLayout in '" << data->playerId;
  clog << "'" << endl;

  if (multiDevice != NULL && parentLayout != NULL
      && multiDevice->getMainLayout () != NULL)
    {
      mainLayout = (NclFormatterLayout *)(multiDevice->getMainLayout ());
      ((NclFormatterLayout *)parentLayout)
          ->addChild (data->baseId, mainLayout);
    }
}

void *
FormatterMediator::setCurrentDocument (const string &file)
{
  g_assert_null (currentDocument);
  this->currentFile = xpathmakeabs (file);
  currentDocument = (NclDocument *) addDocument (currentFile);
  g_assert_nonnull (currentDocument);

  return currentDocument;
}

void *
FormatterMediator::addDocument (const string &docLocation)
{
  NclDocument *addedDoc = NULL;

  addedDoc = privateBaseContext->addDocument (docLocation, deviceLayout);
  g_assert_nonnull (addedDoc);

  data->docId = addedDoc->getId ();

  g_assert (prepareDocument (addedDoc->getId ()));
  return addedDoc;
}

bool
FormatterMediator::removeDocument (const string &documentId)
{
  NclDocument *document;

  if (documentEvents.count (documentId) != 0)
    {
      stopDocument (documentId);
    }

  document = privateBaseContext->removeDocument (documentId);
  g_assert_nonnull (document);
  delete document;
  return true;
}

ContextNode *
FormatterMediator::getDocumentContext (const string &documentId)
{
  NclDocument *nclDocument;

  if (documentEvents.count (documentId) != 0)
    {
      return NULL;
    }

  nclDocument = privateBaseContext->getDocument (documentId);
  g_assert_nonnull (nclDocument);
  return nclDocument->getBody ();
}

void
FormatterMediator::setDepthLevel (int level)
{
  compiler->setDepthLevel (level);
}

int
FormatterMediator::getDepthLevel ()
{
  return compiler->getDepthLevel ();
}

Port *
FormatterMediator::getPortFromEvent (NclFormatterEvent *event)
{
  ContextNode *context;
  Port *port = NULL;
  Anchor *anchor;
  int i, size;
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

vector<Port *> *
FormatterMediator::getContextPorts (ContextNode *context,
                                    const string &interfaceId)
{
  int i, size;
  InterfacePoint *entryPoint;
  Port *port;
  vector<Port *> *ports = new vector<Port *>;

  if (interfaceId == "")
    {
      size = context->getNumPorts ();

      for (i = 0; i < size; i++)
        {
          port = context->getPort (i);
          if (port != NULL)
            {
              entryPoint = port->getEndInterfacePoint ();

              if (entryPoint != NULL)
                {
                  if (entryPoint->instanceOf ("ContentAnchor")
                      || entryPoint->instanceOf ("LabeledAnchor")
                      || entryPoint->instanceOf ("PropertyAnchor"))
                    {
                      ports->push_back (port);
                    }
                }
            }
        }
    }
  else
    {
      port = context->getPort (interfaceId);
      if (port != NULL)
        {
          ports->push_back (port);
        }
    }

  return ports;
}

vector<NclFormatterEvent *> *
FormatterMediator::processDocument (const string &documentId, const string &interfaceId)
{
  vector<NclFormatterEvent *> *entryEvents;
  vector<Port *> *ports = NULL;
  ContextNode *context;
  Port *port;
  int i, size;
  NclNodeNesting *contextPerspective;
  NclFormatterEvent *event;

  port = NULL;

  // look for the entry point perspective
  context = getDocumentContext (documentId);
  if (context == NULL || !context->instanceOf ("CompositeNode"))
    {
      // document has no body
      clog << "FormatterMediator::processDocument warning! Doc '";
      clog << documentId;
      clog << "': without body!" << endl;
      return NULL;
    }

  ports = getContextPorts (context, "");

  if (ports->empty ())
    {
      // interfaceId not found
      clog << "FormatterMediator::processDocument warning! Doc '";
      clog << documentId;
      clog << "': without interfaces" << endl;
      delete ports;
      return NULL;
    }

  contextPerspective = new NclNodeNesting
    (privateBaseContext->getPrivateBase ());

  contextPerspective->insertAnchorNode (context);

  if (entryEventListener == NULL)
    {
      entryEventListener = new EntryEventListener (this, interfaceId);
    }

  entryEvents = new vector<NclFormatterEvent *>;
  size = (int) ports->size ();
  for (i = 0; i < size; i++)
    {
      port = (*ports)[i];

      event = compiler->insertContext (contextPerspective, port);
      if (event != NULL)
        {
          if (port->getId () == interfaceId || interfaceId == "")
            {
              Thread::mutexLock (&pteMutex);
              portsToEntryEvents[port] = event;
              Thread::mutexUnlock (&pteMutex);
              entryEvents->push_back (event);
            }

          entryEventListener->listenEvent (event);
        }
    }

  delete ports;
  delete contextPerspective;

  if (entryEvents->empty ())
    {
      clog << "FormatterMediator::processDocument warning! Doc '";
      clog << documentId;
      clog << "': without entry events" << endl;

      delete entryEvents;
      return NULL;
    }

  initializeSettingNodes (context);
  return entryEvents;
}

void
FormatterMediator::initializeSettingNodes (Node *node)
{
  vector<Node *> *nodes;
  vector<Node *>::iterator i;
  NclNodeNesting *perspective;
  NclExecutionObject *object;
  NodeEntity *nodeEntity;

  // clog << "FormatterScheduler::initializeSettingNodes" << endl;

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

  // clog << "FormatterScheduler::initializeSettingNodes all done" << endl;
}

bool
FormatterMediator::compileDocument (const string &documentId)
{
  vector<NclFormatterEvent *> *entryEvents;
  map<string, NclFormatterEvent *>::iterator i;
  vector<NclFormatterEvent *>::iterator j, k;
  NclFormatterEvent *event;
  NclExecutionObject *executionObject;
  NclCompositeExecutionObject *parentObject;
  NclFormatterEvent *documentEvent;

  i = documentEvents.find (documentId);
  if (i == documentEvents.end ())
    {
      entryEvents = processDocument (documentId, "");
      if (entryEvents == NULL)
        {
          return false;
        }

      event = (*entryEvents)[0];
      executionObject
          = (NclExecutionObject *)(event->getExecutionObject ());
      parentObject
          = (NclCompositeExecutionObject *)(executionObject
                                                ->getParentObject ());

      if (parentObject != NULL)
        {
          while (parentObject->getParentObject () != NULL)
            {
              executionObject = parentObject;
              parentObject = (NclCompositeExecutionObject
                                  *)(parentObject->getParentObject ());
            }

          documentEvent
              = executionObject->getWholeContentPresentationEvent ();
        }
      else
        {
          documentEvent = event;
        }

      documentEvents[documentId] = documentEvent;
      documentEntryEvents[documentId] = entryEvents;
    }

  docCompiled = true;
  return true;
}

bool
FormatterMediator::prepareDocument (const string &documentId)
{
  string src, docLocation;
  ContextNode *body;

  body = getDocumentContext (documentId);
  if (body == NULL)
    {
      clog << "FormatterMediator::prepareDocument warning! Doc '";
      clog << documentId;
      clog << "': without body!" << endl;
      return false;
    }

  docLocation = privateBaseContext->getDocumentLocation (documentId);
  if (docLocation == "")
      return false;
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

bool
FormatterMediator::startDocument (const string &documentId, const string &interfaceId)
{
  vector<NclFormatterEvent *> *entryEvents;
  vector<NclFormatterEvent *> filteredEvents;
  NclFormatterEvent *documentEvent;
  NclFormatterEvent *entryEvent;

  if (!docCompiled)
    {
      compileDocument (documentId);
    }

  if (docCompiled)
    {
      if (documentEvents.count (documentId) != 0)
        {
          documentEvent = documentEvents[documentId];
          entryEvents = documentEntryEvents[documentId];

          if (interfaceId == "")
            {
              AdapterFormatterPlayer::printAction ("startApp::"
                                                   + documentId);

              scheduler->startDocument (documentEvent, entryEvents);
            }
          else
            {
              entryEvent = getEntryEvent (interfaceId, entryEvents);
              if (entryEvent == NULL)
                {
                  return false;
                }

              filteredEvents.push_back (entryEvent);

              AdapterFormatterPlayer::printAction ("startApp::"
                                                   + documentId);

              scheduler->startDocument (documentEvent, &filteredEvents);
            }

          return true;
        }
    }

  clog << "FormatterMediator::startDocument return false for ";
  clog << "document ID = '" << documentId << "' interface ID = '";
  clog << interfaceId << "'" << endl;

  return false;
}

bool
FormatterMediator::stopDocument (const string &documentId)
{
  NclFormatterEvent *documentEvent;

  clog << "FormatterMediator::stopDocument from '";
  clog << documentId << "'" << endl;

  if (documentEvents.count (documentId) == 0)
    {
      clog << "FormatterMediator::stopDocument can't stop document '";
      clog << documentId << "'" << endl;
      return false;
    }

  documentEvent = documentEvents[documentId];

  AdapterFormatterPlayer::printAction (
      "stopApp",
      (NclExecutionObject *)documentEvent->getExecutionObject ());

  scheduler->stopDocument (documentEvent);

  return true;
}

bool
FormatterMediator::pauseDocument (const string &documentId)
{
  NclFormatterEvent *documentEvent;

  if (documentEvents.count (documentId) == 0)
    {
      return false;
    }

  clog << "FormatterMediator::pauseDocument '" << documentId << "'";
  clog << endl;
  documentEvent = documentEvents[documentId];
  scheduler->pauseDocument (documentEvent);
  return true;
}

bool
FormatterMediator::resumeDocument (const string &documentId)
{
  NclFormatterEvent *documentEvent;

  if (documentEvents.count (documentId) == 0)
    {
      return false;
    }

  clog << "FormatterMediator::resumeDocument '" << documentId << "'";
  clog << endl;
  documentEvent = documentEvents[documentId];
  scheduler->resumeDocument (documentEvent);
  return true;
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

  AdapterFormatterPlayer::printAction ("stopApp::" + data->docId);

  Player::notifyPlayerListeners (PL_NOTIFY_STOP, data->playerId,
                                 TYPE_PRESENTATION);
}

void
FormatterMediator::addListener (IPlayerListener *listener)
{
  Player::addListener (listener);
}

void
FormatterMediator::removeListener (IPlayerListener *listener)
{
  Player::removeListener (listener);
}

void
FormatterMediator::notifyPlayerListeners (short code, const string &paremeter,
                                          short type, const string &value)
{
  Player::notifyPlayerListeners (code, paremeter, type, value);
}

void
FormatterMediator::setMediaTime (guint32 newTime)
{
  Player::setMediaTime (newTime);
}

guint32
FormatterMediator::getMediaTime ()
{
  return Player::getMediaTime ();
}

double
FormatterMediator::getTotalMediaTime ()
{
  return Player::getTotalMediaTime ();
}

bool
FormatterMediator::setKeyHandler (bool isHandler)
{
  scheduler->setKeyHandler (isHandler);
  return isHandler;
}

void
FormatterMediator::setScope (const string &scope, short type, double initTime,
                             double endTime, double outTransDur)
{
  Player::setScope (scope, type, initTime, endTime, outTransDur);
}

bool
FormatterMediator::play ()
{
  g_assert_nonnull (currentDocument);
  Player::play ();
  return startDocument (currentDocument->getId (), scope);
}

void
FormatterMediator::stop ()
{
  if (currentDocument != NULL)
    {
      Player::stop ();
      stopDocument (currentDocument->getId ());
    }
}

void
FormatterMediator::abort ()
{
  stop ();
}

void
FormatterMediator::pause ()
{
  if (currentDocument != NULL)
    {
      Player::pause ();
      pauseDocument (currentDocument->getId ());
    }
}

void
FormatterMediator::resume ()
{
  if (currentDocument != NULL)
    {
      Player::resume ();
      resumeDocument (currentDocument->getId ());
    }
}

string
FormatterMediator::getPropertyValue (const string &name)
{
  map<Port *, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *portEvent = NULL;
  string value = "";

  Thread::mutexLock (&pteMutex);
  i = portsToEntryEvents.begin ();
  while (i != portsToEntryEvents.end ())
    {
      if (i->first->getId () == name)
        {
          portEvent = i->second;
          break;
        }
      ++i;
    }
  Thread::mutexUnlock (&pteMutex);

  if (portEvent != NULL && portEvent->instanceOf ("NclAttributionEvent"))
    {
      value = ((NclAttributionEvent *)portEvent)->getCurrentValue ();
    }
  else
    {
      value = Player::getPropertyValue (name);
    }

  return value;
}

void
FormatterMediator::setPropertyValue (const string &name, const string &value)
{
  INclAttributeValueMaintainer *valueMaintainer;
  PropertyAnchor *anchor;

  map<Port *, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *portEvent = NULL;

  Thread::mutexLock (&pteMutex);
  i = portsToEntryEvents.begin ();
  while (i != portsToEntryEvents.end ())
    {
      if (i->first->getId () == name)
        {
          portEvent = i->second;
          break;
        }
      ++i;
    }
  Thread::mutexUnlock (&pteMutex);

  if (portEvent != NULL && portEvent->instanceOf ("NclAttributionEvent"))
    {
      if (portEvent->getCurrentState () == EventUtil::ST_SLEEPING)
        {
          valueMaintainer
              = ((NclAttributionEvent *)portEvent)->getValueMaintainer ();

          anchor = ((NclAttributionEvent *)portEvent)->getAnchor ();

          portEvent->start ();
          if (valueMaintainer != NULL && anchor != NULL)
            {
              valueMaintainer->setPropertyValue (anchor->getId (), value);
            }

          ((NclAttributionEvent *)portEvent)->setValue (value);
          portEvent->stop ();

          if (value
              != ((NclAttributionEvent *)portEvent)->getCurrentValue ())
            {
              clog << "FormatterMediator::setPropertyValue Warning! ";
              clog << "Attributed value = '" << value << "'";
              clog << "BUT current value = '";
              clog
                  << ((NclAttributionEvent *)portEvent)->getCurrentValue ();
              clog << "'";
              clog << endl;
            }
        }
      else
        {
          clog << "FormatterMediator::setPropertyValue Warning! ";
          clog << "Trying to perform an attribution in an event ";
          clog << "that is not sleeping: " << name << endl;
          return;
        }
    }
  else
    {
      Player::setPropertyValue (name, value);
    }
}

void
FormatterMediator::setReferenceTimePlayer (IPlayer *player)
{
  Player::setReferenceTimePlayer (player);
}

void
FormatterMediator::addTimeReferPlayer (IPlayer *referPlayer)
{
  Player::addTimeReferPlayer (referPlayer);
}

void
FormatterMediator::removeTimeReferPlayer (IPlayer *referPlayer)
{
  Player::removeTimeReferPlayer (referPlayer);
}

void
FormatterMediator::notifyReferPlayers (int transition)
{
  Player::notifyReferPlayers (transition);
}

void
FormatterMediator::timebaseObjectTransitionCallback (int transition)
{
  Player::timebaseObjectTransitionCallback (transition);
}

void
FormatterMediator::setTimeBasePlayer (IPlayer *timeBasePlayer)
{
  Player::setTimeBasePlayer (timeBasePlayer);
}

bool
FormatterMediator::isVisible ()
{
  return Player::isVisible ();
}

void
FormatterMediator::setVisible (bool visible)
{
  set<NclExecutionObject *>::iterator i;
  set<NclExecutionObject *> *objects;
  NclExecutionObject *object;
  string strVisible = "true";
  NclFormatterEvent *event = NULL;
  PropertyAnchor *property = NULL;
  bool fakeEvent = false;

  if (!visible)
    {
      strVisible = "false";
    }

  objects = compiler->getRunningObjects ();
  i = objects->begin ();
  while (i != objects->end ())
    {
      object = *i;
      event = object->getEventFromAnchorId ("visible");
      if (event == NULL)
        {
          property = new PropertyAnchor ("visible");
          property->setPropertyValue (strVisible);
          event = new NclAttributionEvent ("visible", object, property,
                                           presContext);

          fakeEvent = true;
        }
      else
        {
          fakeEvent = false;
        }

      if (event->instanceOf ("NclAttributionEvent"))
        {
          playerManager->setVisible (object->getId (), strVisible,
                                     (NclAttributionEvent *)event);
        }

      if (fakeEvent)
        {
          delete event;
          delete property;
        }
      ++i;
    }
  Player::setVisible (visible);

  delete objects;
}

bool
FormatterMediator::immediatelyStart ()
{
  return Player::immediatelyStart ();
}

void
FormatterMediator::setImmediatelyStart (bool immediatelyStartVal)
{
  Player::setImmediatelyStart (immediatelyStartVal);
}

void
FormatterMediator::forceNaturalEnd (bool forceIt)
{
  clog << "FormatterMediator::forceNaturalEnd" << endl;
  if (forceIt)
    {
      stopDocument (currentDocument->getId ());
    }
}

bool
FormatterMediator::isForcedNaturalEnd ()
{
  return Player::isForcedNaturalEnd ();
}

bool
FormatterMediator::setOutWindow (SDLWindow* windowId)
{
  return Player::setOutWindow (windowId);
}

void
FormatterMediator::setCurrentScope (const string &scopeId)
{
  Player::setScope (scopeId);
}

string
FormatterMediator::getActiveUris (vector<string> *uris)
{
  return getDepUris (uris, 2);
}

string
FormatterMediator::getDepUris (vector<string> *uris, int targetDev)
{
  CompositeNode *node;
  ContextNode *parent;
  ContextNode *body;
  vector<Node *> *nodes;
  vector<Node *> *childs;
  vector<Node *>::iterator i;
  string baseUri = "", childBaseUri = "";

  if (currentDocument == NULL)
    {
      return "";
    }

  body = currentDocument->getBody ();
  nodes = body->getNodes ();
  if (nodes == NULL)
    {
      return "";
    }

  i = nodes->begin ();
  while (i != nodes->end ())
    {
      if ((*i)->instanceOf ("ContextNode"))
        {
          parent = (ContextNode *)(*i);
          childs = parent->getNodes ();
          if (childs != NULL)
            {
              childBaseUri = getDepUrisFromNodes (uris, childs, targetDev);
              baseUri = getBaseUri (childBaseUri, baseUri);
            }
        }
      else if ((*i)->instanceOf ("CompositeNode"))
        {
          node = (CompositeNode *)(*i);
          childs = node->getNodes ();
          if (childs != NULL)
            {
              childBaseUri = getDepUrisFromNodes (uris, childs, targetDev);
              baseUri = getBaseUri (childBaseUri, baseUri);
            }
        }
      else
        {
          childBaseUri = getDepUriFromNode (uris, *i, targetDev);
          baseUri = getBaseUri (childBaseUri, baseUri);
        }
      ++i;
    }

  return baseUri;
}

string
FormatterMediator::getDepUrisFromNodes (vector<string> *uris,
                                        vector<Node *> *nodes,
                                        int targetDev)
{
  CompositeNode *node;
  ContextNode *parent;
  vector<Node *> *childs;
  vector<Node *>::iterator i;
  string baseUri = "", childBaseUri = "";

  i = nodes->begin ();
  while (i != nodes->end ())
    {
      if ((*i)->instanceOf ("ContextNode"))
        {
          parent = (ContextNode *)(*i);
          childs = parent->getNodes ();
          if (childs != NULL)
            {
              childBaseUri = getDepUrisFromNodes (uris, childs, targetDev);
              baseUri = getBaseUri (childBaseUri, baseUri);
            }
        }
      else if ((*i)->instanceOf ("CompositeNode"))
        {
          node = (CompositeNode *)(*i);
          childs = node->getNodes ();
          if (childs != NULL)
            {
              childBaseUri = getDepUrisFromNodes (uris, childs, targetDev);
              baseUri = getBaseUri (childBaseUri, baseUri);
            }
        }
      else
        {
          childBaseUri = getDepUriFromNode (uris, *i, targetDev);
          baseUri = getBaseUri (childBaseUri, baseUri);
        }
      ++i;
    }

  return baseUri;
}

string
FormatterMediator::getDepUriFromNode (vector<string> *uris, Node *node,
                                      int targetDev)
{
  GenericDescriptor *descriptor;
  LayoutRegion *ncmRegion;
  NodeEntity *nodeEntity;
  Content *content;
  string src = "";

  if (!node->getDataEntity ()->instanceOf ("NodeEntity"))
    {
      return "";
    }

  content = ((NodeEntity *)(node->getDataEntity ()))->getContent ();
  if (content != NULL && content->instanceOf ("ReferenceContent"))
    {
      src = ((ReferenceContent *)content)->getCompleteReferenceUrl ();
      if (src != "")
        {
          if (targetDev == 0)
            {
              uris->push_back (src);
            }
          else
            {
              nodeEntity = ((NodeEntity *)node->getDataEntity ());
              descriptor = nodeEntity->getDescriptor ();
              if (descriptor != NULL
                  && !(descriptor->instanceOf ("DescriptorSwitch")))
                {
                  clog << "FormatterMediator::getDepUriFromNode ";
                  clog << "checking descriptor '";
                  clog << ((Descriptor *)descriptor)->getId ();
                  clog << "'" << endl;
                  // TODO: find descriptor switch urls
                  ncmRegion = ((Descriptor *)descriptor)->getRegion ();
                  if (ncmRegion != NULL
                      && ncmRegion->getDeviceClass () == targetDev)
                    {
                      uris->push_back (src);
                    }
                }
            }
        }
    }

  return src;
}

string
FormatterMediator::getBaseUri (const string &baseA, const string &baseB)
{
  string base = "";

  if (baseA == "")
    {
      base = baseB;
    }
  else if (baseB != "")
    {
      if (baseB.length () < baseA.length ())
        {
          base = baseB;
        }
    }
  else
    {
      base = baseA;
    }

  return base;
}

PresentationContext *
FormatterMediator::getPresentationContext ()
{
  return presContext;
}

GINGA_FORMATTER_END
