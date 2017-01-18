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

#include "config.h"
#include "gingancl/FormatterMediator.h"

#include "mb/LocalScreenManager.h"
#include "system/GingaLocatorFactory.h"
#include "PrefetchManager.h"
#include "FormatterBaseDevice.h"

#include "PresentationContext.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adaptation::context;

#include "system/GingaLocatorFactory.h"
using namespace ::ginga::system;

#if WITH_MULTIDEVICE
#include "multidevice/DeviceDomain.h"
#endif

BR_PUCRIO_TELEMIDIA_GINGA_NCL_BEGIN

PrefetchManager *FormatterMediator::pm = NULL;

EntryEventListener::EntryEventListener (Player *player, string interfaceId)
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
                                       short previousState)
{

  string interfaceId;
  Port *port;
  short eventType;
  string value = "";
  NclFormatterEvent *ev;
  NclExecutionObject *obj;
  NclCompositeExecutionObject *cObj = NULL;

  if (player != NULL)
    {
      ev = (NclFormatterEvent *)event;
      port = ((FormatterMediator *)player)->getPortFromEvent (ev);

      if (port != NULL)
        {
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
    }
}

short
EntryEventListener::getPriorityType ()
{
  return PT_OBJECT;
}

FormatterMediator::FormatterMediator (NclPlayerData *pData)
    : Player (pData->screenId, "")
{

  string deviceName = "systemScreen(" + itos (pData->devClass) + ")";

  this->data = pData;
  this->currentFile = "";
  this->currentDocument = NULL;

  if (pm == NULL)
    {
      pm = PrefetchManager::getInstance ();
    }

  deviceLayout = new DeviceLayout (deviceName);
  deviceLayout->addDevice (deviceName, data->x, data->y, data->w, data->h);

  presContext = new PresentationContext (data->screenId);

  multiDevice = new FormatterBaseDevice (
      data->screenId, deviceLayout, data->playerId, data->x, data->y,
      data->w, data->h, data->enableMulticast, 22222);

  multiDevice->setPresentationContex (presContext);

  playerManager = new AdapterPlayerManager (data);

  compileErrors.clear ();

  isEmbedded = data->parentDocId != "";
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

  privateBaseManager = (PrivateBaseManager *)(data->privateBaseManager);

  privateBaseManager->createPrivateBase (data->baseId);
  playerManager->setNclEditListener ((IPlayerListener *)data->editListener);
  ((FormatterFocusManager *)(data->focusManager))
      ->setMotionBoundaries (data->x, data->y, data->w, data->h);

  enablePrefetch = false;
  docCompiled = false;

  Thread::mutexInit (&pteMutex, true);

  printData (data);
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

  release ();

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

  compileErrors.clear ();

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
FormatterMediator::setMirrorSrc (IPlayer *mirrorSrc)
{
  Player::setMirrorSrc (mirrorSrc);
}

void
FormatterMediator::printGingaWindows ()
{
  if (multiDevice != NULL)
    {
      multiDevice->printGingaWindows ();
    }
}

void
FormatterMediator::release ()
{
  if (pm != NULL)
    {
      pm->release ();
      pm = NULL;
    }
}

set<string> *
FormatterMediator::createPortIdList ()
{
  set<string> *portIds = NULL;
  map<Port *, NclFormatterEvent *>::iterator i;

  Thread::mutexLock (&pteMutex);
  if (!portsToEntryEvents.empty ())
    {
      portIds = new set<string>;
      i = portsToEntryEvents.begin ();
      while (i != portsToEntryEvents.end ())
        {
          portIds->insert (i->first->getId ());
          ++i;
        }
    }
  Thread::mutexUnlock (&pteMutex);

  return portIds;
}

short
FormatterMediator::getMappedInterfaceType (string portId)
{
  short interfaceType = -1;
  map<Port *, NclFormatterEvent *>::iterator i;

  Thread::mutexLock (&pteMutex);
  i = portsToEntryEvents.begin ();
  while (i != portsToEntryEvents.end ())
    {
      if (i->first->getId () == portId)
        {
          if (i->second->instanceOf ("NclAttributionEvent"))
            {
              interfaceType = EventUtil::EVT_ATTRIBUTION;
            }
          else if (i->second->instanceOf ("NclPresentationEvent"))
            {
              interfaceType = EventUtil::EVT_PRESENTATION;
            }
          else if (i->second->instanceOf ("NclSelectionEvent"))
            {
              interfaceType = EventUtil::EVT_SELECTION;
            }
          else
            {
              interfaceType = EventUtil::EVT_COMPOSITION;
            }

          break;
        }
      ++i;
    }
  Thread::mutexUnlock (&pteMutex);
  return interfaceType;
}

void
FormatterMediator::setMrl (string mrl, bool visible)
{
  this->mrl = mrl;
  this->visible = visible;
}

void
FormatterMediator::printData (NclPlayerData *data)
{
  clog << "FormatterMediator::printData(" << this << ")";
  clog << " screenId: " << data->screenId;
  clog << " baseId: " << data->baseId;
  clog << " playerId: " << data->playerId;
  clog << " devClass: " << data->devClass;
  clog << " x: " << data->x;
  clog << " y: " << data->y;
  clog << " w: " << data->w;
  clog << " h: " << data->h;
  clog << " enableGfx: " << data->enableGfx;
  clog << " parentDocId: " << data->parentDocId;
  clog << " docId: " << data->docId;
  clog << endl;
}

void
FormatterMediator::setTimeBaseProvider (ITimeBaseProvider *timeBaseProvider)
{

  playerManager->setTimeBaseProvider (timeBaseProvider);
}

void
FormatterMediator::setBackgroundImage (string uri)
{
  clog << endl << endl;
  clog << "FormatterMediator::setBackgroundImage '";
  clog << uri << "'" << endl;
  clog << endl << endl;

  multiDevice->setBackgroundImage (uri);
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

string
FormatterMediator::getScreenShot ()
{
  return multiDevice->getScreenShot ();
}

vector<string> *
FormatterMediator::getCompileErrors ()
{
  return &compileErrors;
}

void *
FormatterMediator::setCurrentDocument (string fName)
{
  vector<string> *uris;
  string baseUri;

  if (currentDocument != NULL)
    {
      clog << "FormatterMediator::setCurrentDocument currentDocument ";
      clog << "!= NULL";
      clog << endl;
      return NULL;
    }

  if (SystemCompat::isAbsolutePath (fName))
    {
      currentFile = fName;
    }
  else
    {
      currentFile = SystemCompat::getUserCurrentPath () + fName;
    }

  if (currentFile.length () > 7 && currentFile.substr (0, 7) == "http://")
    {

      if (pm == NULL)
        {
          clog << "FormatterMediator::setCurrentDocument ";
          clog << "Warning! Can't solve remote NCL '" << currentFile;
          clog << "': NULL prefecthManager";
          clog << endl;
          return NULL;
        }

      if (!pm->hasIChannel ())
        {
          clog << "FormatterMediator::setCurrentDocument ";
          clog << "Warning! Can't solve remote NCL '" << currentFile;
          clog << "': no channels found in prefecthManager";
          clog << endl;
          return NULL;
        }

      currentFile = pm->createDocumentPrefetcher (currentFile);
      if (enablePrefetch)
        {
          solveRemoteNclDeps (currentFile, true);
        }
    }
  else if (enablePrefetch)
    {
      solveRemoteNclDeps (currentFile, false);
    }

  currentDocument = (NclDocument *)addDocument (currentFile);

  if (currentDocument != NULL)
    {
      uris = new vector<string>;
      baseUri = getActiveUris (uris);

      std::string::size_type pos
          = currentFile.rfind (SystemCompat::getIUriD ());
      if (pos != string::npos)
        {
          baseUri = currentFile.substr (0, pos + 1);
        }
      else
        {
          baseUri = "";
        }

      multiDevice->addActiveUris (baseUri, uris);

      compileDocument (currentDocument->getId ());
    }

  return currentDocument;
}

void *
FormatterMediator::addDocument (string docLocation)
{
  NclDocument *addedDoc = NULL;

  if (isEmbedded)
    {
      clog << "FormatterMediator::addDocument(" << this << ")";
      clog << " call embedDocument '" << docLocation << "'";
      clog << endl;
      addedDoc = privateBaseManager->embedDocument (
          data->baseId, data->parentDocId, data->nodeId, docLocation,
          deviceLayout);
    }
  else
    {
      clog << "FormatterMediator::addDocument(" << this << ")";
      clog << " call addDocument '" << docLocation << "'";
      clog << endl;
      addedDoc = privateBaseManager->addDocument (data->baseId, docLocation,
                                                  deviceLayout);
    }

  if (addedDoc == NULL)
    {
      clog << "FormatterMediator::addDocument Warning!";
      clog << " Can't add document for '";
      clog << docLocation << "'";
      clog << endl;
      return NULL;
    }

  data->docId = addedDoc->getId ();

  if (prepareDocument (addedDoc->getId ()))
    {
      return addedDoc;
    }

  clog << "FormatterMediator::addDocument Warning!";
  clog << " Can't prepare document for '";
  clog << docLocation << "'";
  clog << endl;

  privateBaseManager->removeDocument (data->baseId, data->docId);
  delete addedDoc;

  return NULL;
}

bool
FormatterMediator::removeDocument (string documentId)
{
  NclDocument *document;

  if (documentEvents.count (documentId) != 0)
    {
      stopDocument (documentId);
    }

  if (isEmbedded && documentId == data->docId)
    {
      document = privateBaseManager->removeEmbeddedDocument (
          data->baseId, data->parentDocId, data->nodeId);
    }
  else
    {
      document
          = privateBaseManager->removeDocument (data->baseId, documentId);
    }

  if (document != NULL)
    {
      delete document;
      return true;
    }
  else
    {
      return false;
    }
}

ContextNode *
FormatterMediator::getDocumentContext (string documentId)
{
  NclDocument *nclDocument;

  if (documentEvents.count (documentId) != 0)
    {
      return NULL;
    }

  if (isEmbedded && documentId == data->docId)
    {
      nclDocument = privateBaseManager->getEmbeddedDocument (
          data->baseId, data->parentDocId, data->nodeId);
    }
  else
    {
      nclDocument
          = privateBaseManager->getDocument (data->baseId, documentId);
    }

  if (nclDocument == NULL)
    {
      return NULL;
    }

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
                                    string interfaceId)
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
FormatterMediator::processDocument (string documentId, string interfaceId)
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

  contextPerspective
      = new NclNodeNesting (privateBaseManager->getPrivateBase (data->baseId));

  contextPerspective->insertAnchorNode (context);

  if (entryEventListener == NULL)
    {
      entryEventListener = new EntryEventListener (this, interfaceId);
    }

  entryEvents = new vector<NclFormatterEvent *>;
  size = ports->size ();
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

                  if (isEmbedded)
                    {
                      perspective = new NclNodeNesting (
                          privateBaseManager->getPrivateBase (
                              data->baseId));

                      perspective->append ((*i)->getPerspective ());
                    }
                  else
                    {
                      perspective
                          = new NclNodeNesting ((*i)->getPerspective ());
                    }

                  clog << "FormatterMediator::initializeSettingNodes '";
                  clog << " id = '" << (*i)->getId () << "'";
                  clog << " node perspective = '";
                  clog << perspective->getId () << "'";
                  clog << endl;

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

vector<NclFormatterEvent *> *
FormatterMediator::getDocumentEntryEvent (string documentId)
{

  if (documentEntryEvents.count (documentId) != 0)
    {
      return documentEntryEvents[documentId];
    }
  else
    {
      return NULL;
    }
}

bool
FormatterMediator::compileDocument (string documentId)
{
  vector<NclFormatterEvent *> *entryEvents;
  vector<NclFormatterEvent *> *oldEntryEvents;
  map<string, NclFormatterEvent *>::iterator i;
  vector<NclFormatterEvent *>::iterator j, k;
  NclFormatterEvent *event;
  NclExecutionObject *executionObject;
  NclCompositeExecutionObject *parentObject;
  NclFormatterEvent *documentEvent;
  bool newEntryEvent;

  i = documentEvents.find (documentId);
  if (i == documentEvents.end ())
    {
      entryEvents = processDocument (documentId, "");
      if (entryEvents == NULL)
        {
          return false;
        }

      event = (*entryEvents)[0];
      executionObject = (NclExecutionObject *)(event->getExecutionObject ());
      parentObject = (NclCompositeExecutionObject *)(executionObject
                                                      ->getParentObject ());

      if (parentObject != NULL)
        {
          while (parentObject->getParentObject () != NULL)
            {
              executionObject = parentObject;
              parentObject
                  = (NclCompositeExecutionObject *)(parentObject
                                                     ->getParentObject ());
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
FormatterMediator::prepareDocument (string documentId)
{
  NclDocument *doc;
  string src, docLocation;
  bool isRemoteDoc;

  DescriptorBase *db;
  vector<GenericDescriptor *> *descs;

  ContextNode *body;
  vector<Node *> *nodes;

  body = getDocumentContext (documentId);
  if (body == NULL || pm == NULL)
    {
      // document has no body
      clog << "FormatterMediator::prepareDocument warning! Doc '";
      clog << documentId;
      clog << "': without body!" << endl;
      return false;
    }

  if (isEmbedded && documentId == data->docId)
    {
      docLocation = privateBaseManager->getEmbeddedDocumentLocation (
          data->baseId, data->parentDocId, data->nodeId);
    }
  else
    {
      docLocation = privateBaseManager->getDocumentLocation (data->baseId,
                                                             documentId);
    }

  if (docLocation == "")
    {
      return false;
    }

  if (enablePrefetch)
    {
      isRemoteDoc = pm->hasRemoteLocation (docLocation);

      // solving remote descriptors URIs
      if (isEmbedded && documentId == data->docId)
        {
          doc = privateBaseManager->getEmbeddedDocument (
              data->baseId, data->parentDocId, data->nodeId);
        }
      else
        {
          doc = privateBaseManager->getDocument (data->baseId, documentId);
        }

      if (doc != NULL)
        {
          db = doc->getDescriptorBase ();
          if (db != NULL)
            {
              descs = db->getDescriptors ();
              if (descs != NULL)
                {
                  solveRemoteDescriptorsUris (docLocation, descs,
                                              isRemoteDoc);
                }
            }
        }

      // solving remote nodes URIs
      // clog << "FormatterMediator::prepareDocument '" << docLocation;
      // clog << "'" << endl;
      nodes = body->getNodes ();
      if (nodes != NULL)
        {
          solveRemoteNodesUris (docLocation, nodes, isRemoteDoc);
        }

      if (pm != NULL)
        {
          pm->getScheduledContents ();
        }
    }

  return true;
}

void
FormatterMediator::solveRemoteDescriptorsUris (
    string docLocation, vector<GenericDescriptor *> *descs,
    bool isRemoteDoc)
{

  string src;
  GenericDescriptor *desc;
  vector<GenericDescriptor *>::iterator i;
  FocusDecoration *fd;

  i = descs->begin ();
  while (i != descs->end ())
    {
      desc = *i;
      if (desc->instanceOf ("Descriptor"))
        {
          fd = ((Descriptor *)desc)->getFocusDecoration ();
        }
      else
        {
          fd = NULL;
        }

      if (fd != NULL)
        {
          src = fd->getFocusSelSrc ();
          // clog << "FormatterMediator::solveRemoteDescriptorsUris old ";
          // clog << "focusSelSrc='" << src << "'" << endl;
          if (src != "")
            {
              src = solveRemoteSourceUri (docLocation, src);
              fd->setFocusSelSrc (src);
            }
          // clog << "FormatterMediator::solveRemoteDescriptorsUris new ";
          // clog << "focusSelSrc='" << src << "'" << endl;

          src = fd->getFocusSrc ();
          // clog << "FormatterMediator::solveRemoteDescriptorsUris old ";
          // clog << "focusSrc='" << src << "'" << endl;
          if (src != "")
            {
              src = solveRemoteSourceUri (docLocation, src);
              fd->setFocusSrc (src);
            }
          // clog << "FormatterMediator::solveRemoteDescriptorsUris new ";
          // clog << "focusSrc='" << src << "'" << endl;
        }
      ++i;
    }
}

void
FormatterMediator::solveRemoteNodesUris (string docLocation,
                                         vector<Node *> *nodes,
                                         bool isRemoteDoc)
{

  CompositeNode *node;
  vector<Node *> *childs;
  vector<Node *>::iterator i;
  Content *content;
  string src, mime;
  string::size_type mimePos;

  i = nodes->begin ();
  while (i != nodes->end ())
    {
      if ((*i)->instanceOf ("CompositeNode"))
        {
          node = (CompositeNode *)(*i);
          childs = node->getNodes ();
          if (childs != NULL)
            {
              solveRemoteNodesUris (docLocation, childs, isRemoteDoc);
            }
        }
      else if ((*i)->getDataEntity ()->instanceOf ("NodeEntity"))
        {
          content = ((NodeEntity *)((*i)->getDataEntity ()))->getContent ();
          if (content != NULL && content->instanceOf ("ReferenceContent"))
            {

              src = ((ReferenceContent *)content)
                        ->getCompleteReferenceUrl ();

              if (src != "" && src.find (".") != std::string::npos)
                {
                  mimePos = src.find_last_of (".") + 1;
                  mime = src.substr (mimePos, src.length () - mimePos);

                  if (mime.length () == 2 || mime.length () > 3
                      || mime == "com" || mime == "org" || mime == "ht"
                      || mime == "htm" || mime == "html" || mime == "xhtml")
                    {

                      if (src.substr (0, 1) == SystemCompat::getIUriD ())
                        {
                          src = SystemCompat::updatePath (src);
                        }
                    }
                  else if (mime == "lua")
                    {
                      src = SystemCompat::updatePath (solveRemoteSourceUri (
                          docLocation, SystemCompat::updatePath (src)));

                      solveRemoteLuaDeps (docLocation, src, isRemoteDoc);
                    }
                  else
                    {
                      src = SystemCompat::updatePath (
                          solveRemoteSourceUri (docLocation, src));
                    }

                  ((ReferenceContent *)content)->setReference (src);
                }
            }
        }
      ++i;
    }
}

void
FormatterMediator::solveRemoteNclDeps (string docLocation, bool isRemoteDoc)
{

  string docRoot, nclDep;
  ifstream fis;

  docRoot = docLocation.substr (
      0, docLocation.find_last_of (SystemCompat::getIUriD ()));

  fis.open (docLocation.c_str (), ifstream::in);
  if (!fis.is_open ())
    {
      clog << "FormatterMediator::solveRemoteNclDeps Warning!";
      clog << " Can't solve ncl";
      clog << " deps on '" << docLocation << "'" << endl;
      return;
    }

  while (fis.good ())
    {
      fis >> nclDep;
      if (nclDep.find ("documentURI") != std::string::npos)
        {
          while (nclDep.find ("\"") == std::string::npos && fis.good ())
            {
              fis >> nclDep;
            }

          if (nclDep.find ("\"") != std::string::npos
              && nclDep.find_first_of ("\"") != nclDep.find_last_of ("\""))
            {

              nclDep
                  = nclDep.substr (nclDep.find_first_of ("\"") + 1,
                                   nclDep.find_last_of ("\"")
                                       - (nclDep.find_first_of ("\"") + 1));

              if ((!isRemoteDoc
                   && nclDep.find ("http://") == std::string::npos)
                  || nclDep == "")
                {

                  continue;
                }

              nclDep = solveRemoteSourceUri (docLocation, nclDep);
              if (pm != NULL)
                {
                  pm->getScheduledContent (nclDep);
                }
              else
                {
                  clog << "FormatterMediator::solveRemoteNclDeps";
                  clog << " Warning! ";
                  clog << "PrefetchManager is NULL" << endl;
                  break;
                }
            }
        }
    }

  fis.close ();
}

void
FormatterMediator::solveRemoteLuaDeps (string docLocation, string src,
                                       bool isRemoteDoc)
{

  string clientLuaDepsSrc, clientLuaDepsRoot, luaDep;
  ifstream fis;

  if (!isRemoteDoc && src.find ("http://") == std::string::npos)
    {
      return;
    }

  clientLuaDepsSrc = src.substr (0, src.find_last_of (".")) + ".deps";
  clientLuaDepsSrc = solveRemoteSourceUri (docLocation, clientLuaDepsSrc);
  clientLuaDepsRoot = clientLuaDepsSrc.substr (
      0, clientLuaDepsSrc.find_last_of (SystemCompat::getIUriD ()));

  if (pm != NULL)
    {
      pm->getScheduledContent (clientLuaDepsSrc);
    }
  else
    {
      clog << "FormatterMediator::solveRemoteLuaDeps";
      clog << " Warning! PrefetchManager ";
      clog << "is NULL" << endl;
    }

  fis.open (clientLuaDepsSrc.c_str (), ifstream::in);
  if (!fis.is_open ())
    {
      clog << "FormatterMediator::solveRemoteLuaDeps";
      clog << " Warning! Can't solve lua";
      clog << " deps on '" << clientLuaDepsSrc << "'" << endl;
      return;
    }

  while (fis.good ())
    {
      fis >> luaDep;
      luaDep = clientLuaDepsRoot + SystemCompat::getIUriD () + luaDep;
      solveRemoteSourceUri (docLocation, luaDep);
    }

  fis.close ();
}

string
FormatterMediator::solveRemoteSourceUri (string localDocUri, string src)
{

  return pm->createSourcePrefetcher (localDocUri, src);
}

NclFormatterEvent *
FormatterMediator::getEntryEvent (string interfaceId,
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
FormatterMediator::startDocument (string documentId, string interfaceId)
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
FormatterMediator::stopDocument (string documentId)
{
  NclFormatterEvent *documentEvent;

  clog << "FormatterMediator::stopDocument from '";
  clog << documentId << "'" << endl;

  if (pm != NULL)
    {
      pm->releaseContents ();
    }

  if (documentEvents.count (documentId) == 0)
    {
      clog << "FormatterMediator::stopDocument can't stop document '";
      clog << documentId << "'" << endl;
      return false;
    }

  documentEvent = documentEvents[documentId];

  AdapterFormatterPlayer::printAction (
      "stopApp", (NclExecutionObject *)documentEvent->getExecutionObject ());

  scheduler->stopDocument (documentEvent);

  /*if (documentEvents != NULL && documentEvents.count(documentId) != 0) {
          documentEvents.erase(documentEvents.find(documentId));
  }

  if (documentEntryEvents != NULL) {
          if (documentEntryEvents.count(documentId) != 0) {
                  documentEntryEvents.erase(
                                  documentEntryEvents.find(documentId));
          }
  }*/

  return true;
}

bool
FormatterMediator::pauseDocument (string documentId)
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
FormatterMediator::resumeDocument (string documentId)
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
FormatterMediator::presentationCompleted (NclFormatterEvent *documentEvent)
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

bool
FormatterMediator::nclEdit (string nclEditApi)
{
  string commandTag
      = trim (nclEditApi.substr (0, nclEditApi.find_first_of (",")));

  /*return editingCommand(commandTag, nclEditApi.substr(
                  nclEditApi.find_first_of(",") + 1,
                  nclEditApi.length() - (nclEditApi.find_first_of(",") +
     1)));*/

  return editingCommand (commandTag, nclEditApi);
}

bool
FormatterMediator::editingCommand (string commandTag,
                                   string privateDataPayload)
{

  vector<string> *args;
  vector<string>::iterator i;
  string arg, uri, ior, docUri, docIor, uName, docId;
  GingaLocatorFactory *glf = NULL;

  glf = GingaLocatorFactory::getInstance ();

  args = split (privateDataPayload, ",", "'");
  i = args->begin ();
  arg = *i;
  ++i;

  clog << "FormatterMediator::editingCommand(" << commandTag << "): '";
  clog << args->size () << "'";
  clog << endl;

  // parse command
  if (commandTag == EC_START_DOCUMENT)
    {
      clog << "FormatterMediator::editingCommand startDocument" << endl;
      string interfaceId;
      docId = *i;
      ++i;

      if (args->size () == 2)
        {
          if (startDocument (docId, ""))
            {
              setKeyHandler (true);
            }
        }
      else if (args->size () == 3)
        {
          interfaceId = *i;
          if (startDocument (docId, interfaceId))
            {
              setKeyHandler (true);
            }
        }
      else
        {
          clog << "FormatterMediator::editingCommand warning!";
          clog << " can't startDocument";
          clog << " wrong number of params: " << args->size ();
          clog << endl;
        }
    }
  else if (commandTag == EC_STOP_DOCUMENT)
    {
      stopDocument (*i);
    }
  else if (commandTag == EC_PAUSE_DOCUMENT)
    {
      pauseDocument (*i);
    }
  else if (commandTag == EC_RESUME_DOCUMENT)
    {
      resumeDocument (*i);
    }
  else if (commandTag == EC_ADD_REGION)
    {
      clog << "FormatterMediator::editingCommand addRegion" << endl;
      string regionId, regionBaseId, xmlRegion;
      LayoutRegion *region;

      if (args->size () == 5)
        {
          docId = *i;
          ++i;
          regionBaseId = *i;
          ++i;
          regionId = *i;
          ++i;
          xmlRegion = *i;
          region = addRegion (docId, regionBaseId, regionId, xmlRegion);
          if (region != NULL)
            {
              clog << "FormatterMediator::editingCommand region added '";
              clog << region->getId () << "'" << endl;
            }
          else
            {
              clog << "FormatterMediator::editingCommand warning! ";
              clog << "can't add region docId='" << docId;
              clog << "' regionBaseId='" << regionBaseId;
              clog << "' regionId='" << regionId;
              clog << "' xml='" << xmlRegion;
              clog << "'" << endl;
            }
        }
      else
        {
          clog << "FormatterMediator::editingCommand warning! ";
          clog << "can't addRegion";
          clog << " wrong number of params: " << args->size ();
          clog << endl;
        }
    }
  else if (commandTag == EC_REMOVE_REGION)
    {
      LayoutRegion *reg;
      string regionId, regionBaseId;

      if (args->size () == 4)
        {
          regionBaseId = *i;
          ++i;
          docId = *i;
          ++i;
          regionId = *i;
          reg = removeRegion (docId, regionBaseId, regionId);
          if (reg != NULL)
            {
              clog << "FormatterMediator::editingCommand del region '";
              clog << reg->getId () << "'" << endl;
              delete reg;
            }
        }
    }
  else if (commandTag == EC_ADD_REGION_BASE)
    {
      string xmlRegionBase;
      RegionBase *regionBase;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          xmlRegionBase = *i;
          regionBase = addRegionBase (docId, xmlRegionBase);
          if (regionBase != NULL)
            {
              clog << "FormatterMediator::editingCommand ";
              clog << "regionBase added '";
              clog << regionBase->getId () << "'" << endl;
            }
          else
            {
              clog << "FormatterMediator::editingCommand warning! ";
              clog << "cant addrgbase doc='";
              clog << docId << "' xml='" << xmlRegionBase << "'";
              clog << endl;
            }
        }
      else
        {
          clog << "FormatterMediator::editingCommand warning! ";
          clog << "can't addRegionBase";
          clog << " wrong number of params: " << args->size ();
          clog << endl;
        }
    }
  else if (commandTag == EC_REMOVE_REGION_BASE)
    {
      string baseId;
      RegionBase *rb;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          baseId = *i;
          rb = removeRegionBase (docId, baseId);
          if (rb != NULL)
            {
              clog << "FormatterMediator::editingCommand del regbase '";
              clog << rb->getId () << "'" << endl;
              delete rb;
            }
        }
    }
  else if (commandTag == EC_ADD_RULE)
    {
      string xmlRule;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          xmlRule = *i;
          addRule (docId, xmlRule);
        }
    }
  else if (commandTag == EC_REMOVE_RULE)
    {
      string ruleId;
      Rule *deadRule;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          ruleId = *i;
          deadRule = removeRule (docId, ruleId);
          if (deadRule != NULL)
            {
              clog << "FormatterMediator::editingCommand del rule '";
              clog << deadRule->getId () << "'" << endl;
              delete deadRule;
            }
        }
    }
  else if (commandTag == EC_ADD_RULE_BASE)
    {
      string xmlRuleBase;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          xmlRuleBase = *i;
          addRuleBase (docId, xmlRuleBase);
        }
    }
  else if (commandTag == EC_REMOVE_RULE_BASE)
    {
      string baseId;
      RuleBase *deadRuleBase;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          baseId = *i;
          deadRuleBase = removeRuleBase (docId, baseId);
          if (deadRuleBase != NULL)
            {
              clog << "FormatterMediator::editingCommand del rulebase '";
              clog << deadRuleBase->getId () << "'" << endl;
              delete deadRuleBase;
            }
        }
    }
  else if (commandTag == EC_ADD_CONNECTOR)
    {
      string xmlConnector;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          xmlConnector = *i;
          addConnector (docId, xmlConnector);
        }
    }
  else if (commandTag == EC_REMOVE_CONNECTOR)
    {
      string connectorId;
      Connector *deadConnector;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          connectorId = *i;
          deadConnector = removeConnector (docId, connectorId);
          if (deadConnector != NULL)
            {
              clog << "FormatterMediator::editingCommand del conn '";
              clog << deadConnector->getId () << "'" << endl;
              delete deadConnector;
            }
        }
    }
  else if (commandTag == EC_ADD_CONNECTOR_BASE)
    {
      clog << "FormatterMediator::editingCommand addConnectorBase";
      clog << endl;
      string xmlConnectorBase;
      ConnectorBase *connBase;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          xmlConnectorBase = *i;
          connBase = addConnectorBase (docId, xmlConnectorBase);
          if (connBase != NULL)
            {
              clog << "FormatterMediator::editingCommand ";
              clog << "connBase added '";
              clog << connBase->getId () << "'" << endl;
            }
          else
            {
              clog << "FormatterMediator::editingCommand warning! ";
              clog << "cant addconnBs doc='";
              clog << docId << "' xmlConnBs='" << xmlConnectorBase;
              clog << "'" << endl;
            }
        }
      else
        {
          clog << "FormatterMediator::editingCommand warning!";
          clog << " can't addConnBase";
          clog << " wrong number of params: " << args->size ();
          clog << endl;
        }
    }
  else if (commandTag == EC_REMOVE_CONNECTOR_BASE)
    {
      string baseId;
      ConnectorBase *deadConBase;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          baseId = *i;
          deadConBase = removeConnectorBase (docId, baseId);
          if (deadConBase != NULL)
            {
              clog << "FormatterMediator::editingCommand del conbase '";
              clog << deadConBase->getId () << "'" << endl;
              delete deadConBase;
            }
        }
    }
  else if (commandTag == EC_ADD_DESCRIPTOR)
    {
      clog << "FormatterMediator::editingCommand addDescriptor" << endl;
      string xmlDescriptor;
      GenericDescriptor *desc;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          xmlDescriptor = *i;
          desc = addDescriptor (docId, xmlDescriptor);
          if (desc != NULL)
            {
              clog << "FormatterMediator::editingCommand ";
              clog << "descriptor added '";
              clog << desc->getId () << "'" << endl;
            }
          else
            {
              clog << "FormatterMediator::editingCommand warning! ";
              clog << "cant addDesc doc='";
              clog << docId << "' xmlDesc='" << xmlDescriptor;
              clog << "'" << endl;
            }
        }
      else
        {
          clog << "FormatterMediator::editingCommand warning!";
          clog << " can't addDesc";
          clog << " wrong number of params: " << args->size ();
          clog << endl;
        }
    }
  else if (commandTag == EC_REMOVE_DESCRIPTOR)
    {
      string descId;
      GenericDescriptor *deadDesc;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          descId = *i;
          deadDesc = removeDescriptor (docId, descId);
          if (deadDesc != NULL)
            {
              clog << "FormatterMediator::editingCommand del desc '";
              clog << deadDesc->getId () << "'" << endl;
              delete deadDesc;
            }
        }
    }
  else if (commandTag == EC_ADD_DESCRIPTOR_SWITCH)
    {
      string xmlDesc;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          xmlDesc = *i;
          addDescriptor (docId, xmlDesc);
        }
    }
  else if (commandTag == EC_REMOVE_DESCRIPTOR_SWITCH)
    {
      string descId;
      GenericDescriptor *deadSDesc;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          descId = *i;
          deadSDesc = removeDescriptor (docId, descId);
          if (deadSDesc != NULL)
            {
              clog << "FormatterMediator::editingCommand del sdesc '";
              clog << deadSDesc->getId () << "'" << endl;
              delete deadSDesc;
            }
        }
    }
  else if (commandTag == EC_ADD_DESCRIPTOR_BASE)
    {
      clog << "FormatterMediator::editingCommand addDescriptorBase";
      clog << endl;
      string xmlDescBase;
      DescriptorBase *dBase;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          xmlDescBase = *i;
          dBase = addDescriptorBase (docId, xmlDescBase);
          if (dBase != NULL)
            {
              clog << "FormatterMediator::editingCommand dBase added '";
              clog << dBase->getId () << "'" << endl;
            }
          else
            {
              clog << "FormatterMediator::editingCommand warning! ";
              clog << "cant adddBase doc='";
              clog << docId << "' xmlDescBase='";
              clog << xmlDescBase << "'" << endl;
            }
        }
      else
        {
          clog << "FormatterMediator::editingCommand warning! ";
          clog << "can't addDescBase";
          clog << " wrong number of params: " << args->size ();
          clog << endl;
        }
    }
  else if (commandTag == EC_REMOVE_DESCRIPTOR_BASE)
    {
      string baseId;
      DescriptorBase *deadDB;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          baseId = *i;
          deadDB = removeDescriptorBase (docId, baseId);
          if (deadDB != NULL)
            {
              clog << "FormatterMediator::editingCommand del descBase '";
              clog << deadDB->getId () << "'" << endl;
              delete deadDB;
            }
        }
    }
  else if (commandTag == EC_ADD_TRANSITION)
    {
      string xmlTrans;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          xmlTrans = *i;
          addTransition (docId, xmlTrans);
        }
    }
  else if (commandTag == EC_REMOVE_TRANSITION)
    {
      string transId;
      Transition *deadTrans;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          transId = *i;
          deadTrans = removeTransition (docId, transId);
          if (deadTrans != NULL)
            {
              clog << "FormatterMediator::editingCommand deltransition '";
              clog << deadTrans->getId () << "'" << endl;
              delete deadTrans;
            }
        }
    }
  else if (commandTag == EC_ADD_TRANSITION_BASE)
    {
      string xmlTransBase;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          xmlTransBase = *i;
          addTransitionBase (docId, xmlTransBase);
        }
    }
  else if (commandTag == EC_REMOVE_TRANSITION_BASE)
    {
      string baseId;
      TransitionBase *deadTB;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          baseId = *i;
          deadTB = removeTransitionBase (docId, baseId);
          if (deadTB != NULL)
            {
              clog << "FormatterMediator::editingCommand deltransbase '";
              clog << deadTB->getId () << "'" << endl;
              delete deadTB;
            }
        }
    }
  else if (commandTag == EC_ADD_IMPORT_BASE)
    {
      string docBaseId, xmlImportBase;

      if (args->size () == 4)
        {
          docId = *i;
          ++i;
          docBaseId = *i;
          ++i;
          xmlImportBase = *i;
          addImportBase (docId, docBaseId, xmlImportBase);
        }
    }
  else if (commandTag == EC_REMOVE_IMPORT_BASE)
    {
      string docBaseId;
      Base *deadBase;

      if (args->size () == 4)
        {
          docId = *i;
          ++i;
          docBaseId = *i;
          ++i;
          docUri = *i;
          deadBase = removeImportBase (docId, docBaseId, docUri);
          if (deadBase != NULL)
            {
              clog << "FormatterMediator::editingCommand delBase '";
              clog << deadBase->getId () << "'" << endl;
              delete deadBase;
            }
        }
    }
  else if (commandTag == EC_ADD_IMPORTED_DOCUMENT_BASE)
    {
      string xmlImportedBase;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          xmlImportedBase = *i;
          addImportedDocumentBase (docId, xmlImportedBase);
        }
    }
  else if (commandTag == EC_REMOVE_IMPORTED_DOCUMENT_BASE)
    {
      string baseId;
      NclDocument *deadDoc;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          baseId = *i;
          deadDoc = removeImportedDocumentBase (docId, baseId);
          if (deadDoc != NULL)
            {
              clog << "FormatterMediator::editingCommand deldoc '";
              clog << deadDoc->getId () << "'" << endl;
              delete deadDoc;
            }
        }
    }
  else if (commandTag == EC_ADD_IMPORT_NCL)
    {
      string xmlImportNcl;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          xmlImportNcl = *i;
          addImportNCL (docId, xmlImportNcl);
        }
    }
  else if (commandTag == EC_REMOVE_IMPORT_NCL)
    {
      NclDocument *deadDoc;

      if (args->size () == 3)
        {
          docId = *i;
          ++i;
          docUri = *i;
          deadDoc = removeImportNCL (docId, docUri);
          if (deadDoc != NULL)
            {
              clog << "FormatterMediator::editingCommand deldoc '";
              clog << deadDoc->getId () << "'" << endl;
              delete deadDoc;
            }
        }
    }
  else if (commandTag == EC_ADD_NODE)
    {
      clog << "FormatterMediator::editingCommand addNode" << endl;
      string compositeId, xmlNode;
      Node *n;

      if (args->size () == 4)
        {
          docId = *i;
          ++i;
          compositeId = *i;
          ++i;
          xmlNode = *i;
          n = addNode (docId, compositeId, xmlNode);
          if (n != NULL)
            {
              clog << "FormatterMediator::editingCommand node added '";
              clog << n->getId () << "'" << endl;
            }
          else
            {
              clog << "FormatterMediator::editingCommand warning! ";
              clog << "cant addnode doc='";
              clog << docId << "' compositeId='" << compositeId;
              clog << "' xmlNode='";
              clog << xmlNode << "'" << endl;
            }
        }
      else
        {
          clog << "FormatterMediator::editingCommand warning!";
          clog << "can't addNode";
          clog << " wrong number of params: " << args->size ();
          clog << endl;
        }
    }
  else if (commandTag == EC_REMOVE_NODE)
    {
      string compositeId, nodeId;
      Node *deadNode;

      if (args->size () == 4)
        {
          docId = *i;
          ++i;
          compositeId = *i;
          ++i;
          nodeId = *i;
          deadNode = removeNode (docId, compositeId, nodeId);
          if (deadNode != NULL)
            {
              clog << "FormatterMediator::editingCommand delnode '";
              clog << deadNode->getId () << "'" << endl;
              delete deadNode;
            }
        }
    }
  else if (commandTag == EC_ADD_INTERFACE)
    {
      clog << "FormatterMediator::editingCommand addInterface" << endl;
      string nodeId, xmlInterface;
      InterfacePoint *intPoint;

      if (args->size () == 4)
        {
          docId = *i;
          ++i;
          nodeId = *i;
          ++i;
          xmlInterface = *i;
          intPoint = addInterface (docId, nodeId, xmlInterface);
          clog << "FormatterMediator::editingCommand addInterf doc='";
          clog << docId << "' nodeId='" << nodeId << "' xml='";
          clog << xmlInterface << "'" << endl;

          if (intPoint != NULL)
            {
              clog << "FormatterMediator::editingCommand ";
              clog << "interface added '";
              clog << intPoint->getId () << "'" << endl;
            }
          else
            {
              clog << "FormatterMediator::editingCommand warning! ";
              clog << "cant addInterf doc='";
              clog << docId << "' nodeId='" << nodeId << "' xml='";
              clog << xmlInterface << "'" << endl;
            }
        }
      else
        {
          clog << "FormatterMediator::editingCommand warning!";
          clog << " can't addInterface";
          clog << " wrong number of params: " << args->size ();
          clog << endl;
        }
    }
  else if (commandTag == EC_REMOVE_INTERFACE)
    {
      string nodeId, interfaceId;
      InterfacePoint *deadInt;

      if (args->size () == 4)
        {
          docId = *i;
          ++i;
          nodeId = *i;
          ++i;
          interfaceId = *i;
          deadInt = removeInterface (docId, nodeId, interfaceId);
          if (deadInt != NULL)
            {
              clog << "FormatterMediator::editingCommand delint '";
              clog << deadInt->getId () << "'" << endl;
              delete deadInt;
            }
        }
    }
  else if (commandTag == EC_ADD_LINK)
    {
      string compositeId, xmlLink;
      Link *ncmLink;

      clog << "FormatterMediator::editingCommand addLink" << endl;
      if (args->size () == 4)
        {
          docId = *i;
          ++i;
          compositeId = *i;
          ++i;
          xmlLink = *i;
          ncmLink = addLink (docId, compositeId, xmlLink);
          if (ncmLink == NULL)
            {
              clog << "FormatterMediator::editingCommand Warning! ";
              clog << "Can't add link inside context '" << compositeId;
              clog << "' of document '" << docId << "'" << endl;
            }
          else
            {
              clog << "FormatterMediator::editingCommand Link '";
              clog << ncmLink->getId () << "' successfully added.";
              clog << endl;
            }
        }
      else
        {
          clog << "FormatterMediator::editingCommand Warning! ";
          clog << "Can't add link: invalid number of parameters" << endl;
        }
    }
  else if (commandTag == EC_REMOVE_LINK)
    {
      string compositeId, linkId;
      Link *ncmLink;

      if (args->size () == 4)
        {
          docId = *i;
          ++i;
          compositeId = *i;
          ++i;
          linkId = *i;
          ncmLink = removeLink (docId, compositeId, linkId);
          if (ncmLink == NULL)
            {
              clog << "FormatterMediator::editingCommand Warning! ";
              clog << "Can't remove link '" << linkId;
              clog << "' inside context '" << compositeId;
              clog << "' of document '" << docId << "'" << endl;
            }
          else
            {
              clog << "FormatterMediator::editingCommand Link '";
              clog << ncmLink->getId () << "' successfully removed.";
              clog << endl;
              delete ncmLink;
            }
        }
    }
  else if (commandTag == EC_SET_PROPERTY_VALUE)
    {
      string nodeId, propId, value;

      if (args->size () == 5)
        {
          docId = *i;
          ++i;
          nodeId = *i;
          ++i;
          propId = *i;
          ++i;
          value = *i;
          setPropertyValue (docId, nodeId, propId, value);
        }
    }

  delete args;
  args = NULL;
  return true;
}

LayoutRegion *
FormatterMediator::addRegion (string documentId, string regionBaseId,
                              string regionId, string xmlRegion)
{

  return privateBaseManager->addRegion (data->baseId, documentId,
                                        regionBaseId, regionId, xmlRegion);
}

LayoutRegion *
FormatterMediator::removeRegion (string documentId, string regionBaseId,
                                 string regionId)
{

  return privateBaseManager->removeRegion (data->baseId, documentId,
                                           regionBaseId, regionId);
}

RegionBase *
FormatterMediator::addRegionBase (string documentId, string xmlRegionBase)
{

  return privateBaseManager->addRegionBase (data->baseId, documentId,
                                            xmlRegionBase);
}

RegionBase *
FormatterMediator::removeRegionBase (string documentId, string regionBaseId)
{

  return privateBaseManager->removeRegionBase (data->baseId, documentId,
                                               regionBaseId);
}

Rule *
FormatterMediator::addRule (string documentId, string xmlRule)
{
  return privateBaseManager->addRule (data->baseId, documentId, xmlRule);
}

Rule *
FormatterMediator::removeRule (string documentId, string ruleId)
{
  return privateBaseManager->removeRule (data->baseId, documentId, ruleId);
}

RuleBase *
FormatterMediator::addRuleBase (string documentId, string xmlRuleBase)
{

  return privateBaseManager->addRuleBase (data->baseId, documentId,
                                          xmlRuleBase);
}

RuleBase *
FormatterMediator::removeRuleBase (string documentId, string ruleBaseId)
{

  return privateBaseManager->removeRuleBase (data->baseId, documentId,
                                             ruleBaseId);
}

Transition *
FormatterMediator::addTransition (string documentId, string xmlTransition)
{

  return privateBaseManager->addTransition (data->baseId, documentId,
                                            xmlTransition);
}

Transition *
FormatterMediator::removeTransition (string documentId, string transitionId)
{

  return privateBaseManager->removeTransition (data->baseId, documentId,
                                               transitionId);
}

TransitionBase *
FormatterMediator::addTransitionBase (string documentId,
                                      string xmlTransitionBase)
{

  return privateBaseManager->addTransitionBase (data->baseId, documentId,
                                                xmlTransitionBase);
}

TransitionBase *
FormatterMediator::removeTransitionBase (string documentId,
                                         string transitionBaseId)
{

  return privateBaseManager->removeTransitionBase (data->baseId, documentId,
                                                   transitionBaseId);
}

Connector *
FormatterMediator::addConnector (string documentId, string xmlConnector)
{

  return privateBaseManager->addConnector (data->baseId, documentId,
                                           xmlConnector);
}

Connector *
FormatterMediator::removeConnector (string documentId, string connectorId)
{

  return privateBaseManager->removeConnector (data->baseId, documentId,
                                              connectorId);
}

ConnectorBase *
FormatterMediator::addConnectorBase (string documentId,
                                     string xmlConnectorBase)
{

  return privateBaseManager->addConnectorBase (data->baseId, documentId,
                                               xmlConnectorBase);
}

ConnectorBase *
FormatterMediator::removeConnectorBase (string documentId,
                                        string connectorBaseId)
{

  return privateBaseManager->removeConnectorBase (data->baseId, documentId,
                                                  connectorBaseId);
}

GenericDescriptor *
FormatterMediator::addDescriptor (string documentId, string xmlDescriptor)
{

  return privateBaseManager->addDescriptor (data->baseId, documentId,
                                            xmlDescriptor);
}

GenericDescriptor *
FormatterMediator::removeDescriptor (string documentId, string descriptorId)
{

  return privateBaseManager->removeDescriptor (data->baseId, documentId,
                                               descriptorId);
}

DescriptorBase *
FormatterMediator::addDescriptorBase (string documentId,
                                      string xmlDescriptorBase)
{

  return privateBaseManager->addDescriptorBase (data->baseId, documentId,
                                                xmlDescriptorBase);
}

DescriptorBase *
FormatterMediator::removeDescriptorBase (string documentId,
                                         string descriptorBaseId)
{

  return privateBaseManager->removeDescriptorBase (data->baseId, documentId,
                                                   descriptorBaseId);
}

Base *
FormatterMediator::addImportBase (string documentId, string docBaseId,
                                  string xmlImportBase)
{

  return privateBaseManager->addImportBase (data->baseId, documentId,
                                            docBaseId, xmlImportBase);
}

Base *
FormatterMediator::removeImportBase (string documentId, string docBaseId,
                                     string documentURI)
{

  return privateBaseManager->removeImportBase (data->baseId, documentId,
                                               docBaseId, documentURI);
}

NclDocument *
FormatterMediator::addImportedDocumentBase (string documentId,
                                            string xmlImportedDocumentBase)
{

  return privateBaseManager->addImportedDocumentBase (
      data->baseId, documentId, xmlImportedDocumentBase);
}

NclDocument *
FormatterMediator::removeImportedDocumentBase (
    string documentId, string importedDocumentBaseId)
{

  return privateBaseManager->removeImportedDocumentBase (
      data->baseId, documentId, importedDocumentBaseId);
}

NclDocument *
FormatterMediator::addImportNCL (string documentId, string xmlImportNCL)
{

  return privateBaseManager->addImportNCL (data->baseId, documentId,
                                           xmlImportNCL);
}

NclDocument *
FormatterMediator::removeImportNCL (string documentId, string documentURI)
{

  return privateBaseManager->removeImportNCL (data->baseId, documentId,
                                              documentURI);
}

void
FormatterMediator::processInsertedReferNode (ReferNode *referNode)
{
  NodeEntity *nodeEntity;
  NclExecutionObject *executionObject;
  NclCompositeExecutionObject *parentObject;
  int depthLevel;
  string instanceType;

  nodeEntity = (NodeEntity *)(referNode->getDataEntity ());
  instanceType = referNode->getInstanceType ();
  if (nodeEntity->instanceOf ("ContentNode") && instanceType == "instSame")
    {

      executionObject = compiler->hasExecutionObject (nodeEntity, NULL);
      if (executionObject != NULL)
        {
          parentObject
              = compiler->addSameInstance (executionObject, referNode);

          if (parentObject != NULL)
            {
              depthLevel = compiler->getDepthLevel ();
              if (depthLevel > 0)
                {
                  depthLevel = depthLevel - 1;
                }

              compiler->compileExecutionObjectLinks (
                  executionObject, referNode, parentObject, depthLevel);
            }
        }
      else
        {
          clog << "FormatterMediator::processInsertedReferNode referred "
                  "object";
          clog << " is NULL for '";
          clog << nodeEntity->getId () << "' instType = '";
          clog << instanceType << "'" << endl;
        }
    }
  else
    {
      clog << "FormatterMediator::processInsertedReferNode referred object";
      clog << " for '" << nodeEntity->getId () << "' instType = '";
      clog << instanceType << "'" << endl;
    }
}

void
FormatterMediator::processInsertedComposition (CompositeNode *composition)
{
  vector<Node *> *nodes;

  nodes = composition->getNodes ();
  if (nodes != NULL)
    {
      vector<Node *>::iterator i;
      i = nodes->begin ();
      while (i != nodes->begin ())
        {
          if ((*i)->instanceOf ("ReferNode"))
            {
              processInsertedReferNode ((ReferNode *)(*i));
            }
          else if ((*i)->instanceOf ("CompositeNode"))
            {
              processInsertedComposition ((CompositeNode *)(*i));
            }
        }
    }
}

Node *
FormatterMediator::addNode (string documentId, string compositeId,
                            string xmlNode)
{

  Node *node;

  node = privateBaseManager->addNode (data->baseId, documentId, compositeId,
                                      xmlNode);

  if (node == NULL)
    {
      return NULL;
    }

  if (node->instanceOf ("ReferNode"))
    {
      processInsertedReferNode ((ReferNode *)node);
    }
  else if (node->instanceOf ("CompositeNode"))
    {
      // look for child nodes with refer and newInstance=false
      processInsertedComposition ((CompositeNode *)node);
    }

  return node;
}

Node *
FormatterMediator::removeNode (string documentId, string compositeId,
                               string nodeId)
{

  NclDocument *document;

  document = privateBaseManager->getDocument (data->baseId, documentId);

  if (document == NULL)
    {
      // document does not exist
      return NULL;
    }

  Node *parentNode;

  parentNode = document->getNode (compositeId);
  if (parentNode == NULL || !(parentNode->instanceOf ("ContextNode")))
    {
      // composite node (compositeId) does exist or is not a context node
      return NULL;
    }

  Node *node;
  CompositeNode *compositeNode;

  compositeNode = (ContextNode *)parentNode;
  node = compositeNode->getNode (nodeId);
  if (node == NULL)
    {
      // node (nodeId) is not a compositeId child node
      return NULL;
    }

  NclExecutionObject *executionObject;
  set<Anchor *> *nodeInterfaces;
  set<Anchor *>::iterator setIt;
  vector<Anchor *> *anchors;
  vector<Anchor *>::iterator it;
  vector<Port *> *ports;
  vector<Port *>::iterator j;
  InterfacePoint *nodeInterface;

  // remove all node interfaces
  nodeInterfaces = new set<Anchor *>;

  anchors = node->getAnchors ();
  if (anchors != NULL)
    {
      it = anchors->begin ();
      while (it != anchors->end ())
        {
          nodeInterfaces->insert (*it);
          ++it;
        }
    }

  if (node->instanceOf ("CompositeNode"))
    {
      ports = ((CompositeNode *)node)->getPorts ();
      if (ports != NULL)
        {
          j = ports->begin ();
          while (j != ports->end ())
            {
              nodeInterfaces->insert ((Anchor *)(*j));
            }
        }
    }

  /*
                  TODO: is it here?
                  setIt = nodeInterfaces->begin();
                  while (setIt != nodeInterfaces->end()) {
                          nodeInterface = *setIt;
                          removeInterface(node, nodeInterface);
                          ++setIt;
                  }
  */
  // remove the execution object
  executionObject = compiler->hasExecutionObject (node, NULL);
  if (executionObject != NULL)
    {
      if (node->instanceOf ("ReferNode")
          && ((ReferNode *)node)->getInstanceType () != "new")
        {

          // remove the object entry
          compiler->removeExecutionObject (executionObject,
                                           (ReferNode *)node);
        }
      else
        {

          // remove the whole execution object
          if (executionObject->getMainEvent () != NULL)
            {
              scheduler->stopEvent (executionObject->getMainEvent ());
            }

          compiler->removeExecutionObject (executionObject);
        }
    }

  compositeNode->removeNode (node);

  // TODO: is it here?
  setIt = nodeInterfaces->begin ();
  while (setIt != nodeInterfaces->end ())
    {
      nodeInterface = *setIt;
      removeInterface (node, nodeInterface);
      ++setIt;
    }

  return node;
}

InterfacePoint *
FormatterMediator::addInterface (string documentId, string nodeId,
                                 string xmlInterface)
{

  return privateBaseManager->addInterface (data->baseId, documentId, nodeId,
                                           xmlInterface);
}

void
FormatterMediator::removeInterfaceMappings (Node *node,
                                            InterfacePoint *interfacePoint,
                                            CompositeNode *composition)
{

  vector<Port *> *portsToBeRemoved;
  vector<Port *> *ports;
  vector<Port *> *mappings;
  vector<Port *>::iterator j, k;

  int i, size;
  Port *port;
  Port *mapping;

  if (composition == NULL)
    {
      return;
    }

  portsToBeRemoved = new vector<Port *>;

  ports = composition->getPorts ();
  if (ports != NULL)
    {
      j = ports->begin ();
      while (j != ports->end ())
        {
          port = *j;

          if (port->instanceOf ("SwitchPort"))
            {
              mappings = ((SwitchPort *)port)->getPorts ();
              if (mappings != NULL)
                {
                  k = mappings->begin ();
                  while (k != mappings->end ())
                    {
                      mapping = (Port *)(*k);
                      if (mapping->getNode () == node
                          && mapping->getInterfacePoint ()
                                 == interfacePoint)
                        {

                          portsToBeRemoved->push_back (port);
                          break;
                        }
                      ++k;
                    }
                }
            }
          else if (port->getNode () == node
                   && port->getInterfacePoint () == interfacePoint)
            {

              portsToBeRemoved->push_back (port);
            }
          ++j;
        }
    }

  size = portsToBeRemoved->size ();
  for (i = 0; i < size; i++)
    {
      port = (Port *)((*portsToBeRemoved)[i]);
      removeInterface (composition, port);
    }

  delete portsToBeRemoved;
}

void
FormatterMediator::removeInterfaceLinks (Node *node,
                                         InterfacePoint *interfacePoint,
                                         ContextNode *composition)
{

  set<Link *> *linksToBeRemoved;
  set<Link *>::iterator i;

  set<Link *> *links;
  set<Link *>::iterator j;
  vector<Bind *> *binds;
  vector<Bind *>::iterator k;

  Link *ncmLink;
  Bind *bind;

  linksToBeRemoved = new set<Link *>;
  links = composition->getLinks ();
  if (links != NULL)
    {
      j = links->begin ();
      while (j != links->end ())
        {
          ncmLink = (Link *)(*j);
          // verify if node and interface point participate in link
          binds = ncmLink->getBinds ();
          if (binds != NULL)
            {
              k = binds->begin ();
              while (k != binds->end ())
                {
                  bind = (Bind *)(*k);
                  if (bind->getNode () == node
                      && bind->getInterfacePoint () == interfacePoint)
                    {

                      linksToBeRemoved->insert (ncmLink);
                      break;
                    }
                  ++k;
                }
            }
          ++j;
        }
    }

  i = linksToBeRemoved->begin ();
  while (i != linksToBeRemoved->end ())
    {
      ncmLink = *i;
      removeLink (composition, ncmLink);
      ++i;
    }

  delete linksToBeRemoved;
}

void
FormatterMediator::removeInterface (Node *node,
                                    InterfacePoint *interfacePoint)
{

  CompositeNode *parentNode;

  parentNode = (CompositeNode *)(node->getParentComposition ());
  removeInterfaceMappings (node, interfacePoint, parentNode);

  if (parentNode != NULL && parentNode->instanceOf ("ContextNode"))
    {
      removeInterfaceLinks (node, interfacePoint,
                            (ContextNode *)parentNode);
    }

  if (interfacePoint->instanceOf ("Anchor"))
    {
      node->removeAnchor ((Anchor *)interfacePoint);
    }
  else if (node->instanceOf ("CompositeNode")
           && interfacePoint->instanceOf ("Port"))
    {

      ((CompositeNode *)node)->removePort ((Port *)interfacePoint);
      // TODO verify if a special treatment is necessary for switch ports
    }
}

InterfacePoint *
FormatterMediator::removeInterface (string documentId, string nodeId,
                                    string interfaceId)
{

  NclDocument *document;
  Node *node;
  InterfacePoint *interfacePoint;

  document = privateBaseManager->getDocument (data->baseId, documentId);

  if (document == NULL)
    {
      // document does not exist
      return NULL;
    }

  node = document->getNode (nodeId);
  if (node == NULL)
    {
      // node (nodeId) does not exist
      return NULL;
    }

  interfacePoint = node->getAnchor (interfaceId);
  if (interfacePoint == NULL && node->instanceOf ("CompositeNode"))
    {
      interfacePoint = ((CompositeNode *)node)->getPort (interfaceId);
    }

  if (interfacePoint == NULL)
    {
      // interface (interfaceId) does not exist or
      // does not pertain to node
      return NULL;
    }

  removeInterface (node, interfacePoint);
  return interfacePoint;
}

Link *
FormatterMediator::addLink (string documentId, string compositeId,
                            string xmlLink)
{

  NclFormatterCausalLink *fLink;
  AdapterFormatterPlayer *player;
  NclFormatterEvent *event;
  NclExecutionObject *object;
  vector<NclFormatterEvent *> *events;
  vector<NclFormatterEvent *>::iterator i;
  Link *ncmLink;
  NclDocument *document;
  ContextNode *contextNode;

  ncmLink = privateBaseManager->addLink (data->baseId, documentId,
                                         compositeId, xmlLink);

  if (ncmLink != NULL)
    {
      document = privateBaseManager->getDocument (data->baseId, documentId);

      contextNode = (ContextNode *)(document->getNode (compositeId));
      if (ncmLink->instanceOf ("CausalLink"))
        {
          fLink = compiler->addCausalLink (contextNode,
                                           (CausalLink *)ncmLink);

          if (fLink != NULL)
            {
              events = fLink->getEvents ();
              i = events->begin ();
              while (i != events->end ())
                {
                  clog << "FormatterMediator::addLink event '";
                  clog << (*i)->getId () << "'" << endl;

                  object = (NclExecutionObject *)(*i)->getExecutionObject ();
                  if (object == NULL)
                    {
                      clog << "FormatterMediator::addLink Warning! ";
                      clog << "event '" << (*i)->getId () << "' ";
                      clog << "has a NULL EO" << endl;
                      ++i;
                      continue;
                    }

                  event = object->getMainEvent ();
                  if (event != NULL
                      && event->getCurrentState ()
                             == EventUtil::ST_OCCURRING)
                    {

                      player = (AdapterFormatterPlayer *)
                                   playerManager->getObjectPlayer (object);
                      if (player != NULL)
                        {
                          player->checkAnchorMonitor ();
                        }
                    }

                  ++i;
                }
            }
        }
    }
  return ncmLink;
}

void
FormatterMediator::removeLink (ContextNode *composition, Link *ncmLink)
{

  NclCompositeExecutionObject *compositeObject;

  if (composition->instanceOf ("CompositeNode"))
    {
      compositeObject
          = (NclCompositeExecutionObject *)(compiler->hasExecutionObject (
              (CompositeNode *)composition, NULL));

      if (compositeObject != NULL)
        {
          compositeObject->removeNcmLink (ncmLink);
        }
    }
  composition->removeLink (ncmLink);
}

Link *
FormatterMediator::removeLink (string documentId, string compositeId,
                               string linkId)
{

  NclDocument *document;
  Node *node;
  ContextNode *contextNode;
  Link *ncmLink;

  document = privateBaseManager->getDocument (data->baseId, documentId);

  if (document == NULL)
    {
      // document does not exist
      return NULL;
    }

  node = document->getNode (compositeId);
  if (node == NULL || !(node->instanceOf ("ContextNode")))
    {
      // composite node (compositeId) does exist or is not a context node
      return NULL;
    }

  contextNode = (ContextNode *)node;
  ncmLink = contextNode->getLink (linkId);
  if (ncmLink == NULL)
    {
      // link (linkId) is not a nodeId child link
      return NULL;
    }

  removeLink (contextNode, ncmLink);
  return ncmLink;
}

bool
FormatterMediator::setPropertyValue (string documentId, string nodeId,
                                     string propertyId, string value)
{

  NclDocument *document;
  Node *node;
  Anchor *anchor;
  NclNodeNesting *perspective;
  NclExecutionObject *executionObject;
  NclFormatterEvent *event;
  NclLinkAssignmentAction *setAction;

  document = privateBaseManager->getDocument (data->baseId, documentId);
  if (document == NULL)
    {
      // document does not exist
      return false;
    }

  node = document->getNode (nodeId);
  if (node == NULL)
    {
      // node (nodeId) does exist
      return false;
    }

  anchor = node->getAnchor (propertyId);
  if (!(anchor->instanceOf ("PropertyAnchor")))
    {
      // interface (interfaceId) is not a property
      return false;
    }

  perspective = new NclNodeNesting (node->getPerspective ());
  try
    {
      /*clog << "FormatterMediator::setPropertyValue '";
      clog << " perspective = '" << perspective->getId() << "'";
      clog << endl;*/

      executionObject = compiler->getExecutionObjectFromPerspective (
          perspective, NULL, compiler->getDepthLevel ());
    }
  catch (ObjectCreationForbiddenException *exc)
    {
      return false;
    }

  event = compiler->getEvent (executionObject, anchor,
                              EventUtil::EVT_ATTRIBUTION, "");

  if (event == NULL || !(event->instanceOf ("NclAttributionEvent")))
    {
      return false;
    }

  setAction = new NclLinkAssignmentAction ((NclAttributionEvent *)event,
                                        SimpleAction::ACT_START, value);

  ((INclLinkActionListener *)scheduler)->scheduleAction (NULL, setAction);
  return true;
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
FormatterMediator::notifyPlayerListeners (short code, string paremeter,
                                          short type, string value)
{

  Player::notifyPlayerListeners (code, paremeter, type, value);
}

void
FormatterMediator::setSurface (GingaSurfaceID surface)
{
  Player::setSurface (surface);
}

GingaSurfaceID
FormatterMediator::getSurface ()
{
  GingaSurfaceID s = 0;

  s = Player::getSurface ();
  dm->clearSurfaceContent (s);
  dm->flipSurface (s);

  return s;
}

void
FormatterMediator::flip ()
{
  GingaWindowID w = 0;

  if (data->devClass == 0)
    {
      if (surface != 0 && dm->hasSurface (myScreen, surface))
        {
          w = dm->getSurfaceParentWindow (surface);
          if (w != 0)
            {
              dm->clearWindowContent (myScreen, w);
              dm->validateWindow (myScreen, w);
            }
        }
    }
  else if (data->devClass == 1)
    {
      multiDevice->updatePassiveDevices ();
    }
}

void
FormatterMediator::setMediaTime (double newTime)
{
  Player::setMediaTime (newTime);
}

double
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
FormatterMediator::setScope (string scope, short type, double initTime,
                             double endTime, double outTransDur)
{

  Player::setScope (scope, type, initTime, endTime, outTransDur);
  clog << "FormatterMediator::setScope '" << scope << "' type '" << type;
  clog << "' initTime '" << initTime << "' endTime '" << endTime;
  clog << endl;
}

bool
FormatterMediator::play ()
{
  bool documentStarted;
  // SDLWindow* window;

  if (currentDocument != NULL)
    {
      Player::play ();
      documentStarted = startDocument (currentDocument->getId (), scope);
      checkScopeTime ();

      /*if (data->devClass == 1) {
              clog << "FormatterMediator::play (PASSIVE)";
              clog << " surface = '" << surface;
              clog << "'";
              if (surface != NULL) {
                      window = (SDLWindow*)(surface->getParentWindow());

                      clog << " parent window = '";
                      clog << window << "'";
                      if (window != NULL) {
                              clog << " with ID = '";
                              clog << (long)window->getId() << "'";
                              clog << endl;

                              multiDevice->serializeScreen(1, window);
                      }
                      clog << endl;
              }

      } else {
              clog << "FormatterMediator::play (BASE)";
              clog << " surface = '" << surface;
              clog << "'";
              if (surface != NULL) {
                      window = (SDLWindow*)(surface->getParentWindow());

                      clog << " parent window = '";
                      clog << window << "'";
                      if (window != NULL) {
                              clog << " with ID = '";
                              clog << (long)window->getId() << "'";
                      }
                      clog << endl;
              }
      }*/

      return documentStarted;
    }
  else
    {
      clog << "FormatterMediator::play()";
      clog << " Warning! Trying to start a NULL doc";
      clog << " for '" << data->playerId << "'" << endl;

      return false;
    }
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
FormatterMediator::getPropertyValue (string name)
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
FormatterMediator::setPropertyValue (string name, string value)
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

          if (value != ((NclAttributionEvent *)portEvent)->getCurrentValue ())
            {

              clog << "FormatterMediator::setPropertyValue Warning! ";
              clog << "Attributed value = '" << value << "'";
              clog << "BUT current value = '";
              clog << ((NclAttributionEvent *)portEvent)->getCurrentValue ();
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
FormatterMediator::hasPresented ()
{
  return Player::hasPresented ();
}

void
FormatterMediator::setPresented (bool presented)
{
  Player::setPresented (presented);
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
FormatterMediator::setOutWindow (GingaWindowID windowId)
{
  return Player::setOutWindow (windowId);
}

void
FormatterMediator::setCurrentScope (string scopeId)
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
FormatterMediator::getBaseUri (string baseA, string baseB)
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

void
FormatterMediator::timeShift (string direction)
{
  playerManager->timeShift (direction);
}

PresentationContext *
FormatterMediator::getPresentationContext ()
{
  return presContext;
}

BR_PUCRIO_TELEMIDIA_GINGA_NCL_END
