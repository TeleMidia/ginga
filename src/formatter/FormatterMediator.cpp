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

FormatterMediator::FormatterMediator ()
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

  //scheduler->addSchedulerListener (this);
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

  ((FormatterFocusManager *)(data->focusManager))
      ->setMotionBoundaries (data->w, data->h);

}

FormatterMediator::~FormatterMediator ()
{
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
FormatterMediator::compileDocument (const string &id)
{
  vector<NclFormatterEvent *> *events;
  vector<Port *> *ports;
  ContextNode *body;
  ContentNode *settings;
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
  settings = this->currentDocument->getSettingsNode ();
  if (settings != NULL)
    {
      int depth;

      persp = new NclNodeNesting (settings->getPerspective ());
      depth = this->compiler->getDepthLevel ();

      execobj = this->compiler
        ->getExecutionObjectFromPerspective (persp, NULL, depth);
      g_assert_nonnull (execobj);
    }

  g_assert (!events->empty ());
  evt = events->at (0);
  g_assert_nonnull (evt);

  execobj = (NclExecutionObject *)(evt->getExecutionObject ());
  g_assert_nonnull (execobj);

  parent = (NclCompositeExecutionObject *)(execobj->getParentObject ());
  g_assert_nonnull (parent);

  this->documentEvents[id] = evt;
  this->documentEntryEvents[id] = events;

  return true;
}

bool
FormatterMediator::play ()
{
  NclFormatterEvent *docevt;
  vector<NclFormatterEvent *> *evts;
  string id;

  g_assert_nonnull (this->currentDocument);
  id = currentDocument->getId ();

  compileDocument (id);

  if (unlikely (this->documentEvents.count (id) == 0))
    return true;

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

  id = currentDocument->getId ();
  evt = documentEvents[id];
  g_assert_nonnull (evt);
  this->scheduler->stopDocument (evt);
}

GINGA_FORMATTER_END
