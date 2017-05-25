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

#include "FormatterMultiDevice.h"
#include "PresentationContext.h"
#include "Parser.h"

GINGA_FORMATTER_BEGIN

FormatterMediator::FormatterMediator ()
{
  NclPlayerData *data;
  string id;

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

  presContext = new PresentationContext ();

  multiDevice = new FormatterMultiDevice (data->w, data->h);

  multiDevice->setPresentationContex (presContext);

  playerManager = new AdapterPlayerManager (data);

  ruleAdapter = new RuleAdapter (presContext);
  compiler = new FormatterConverter (ruleAdapter);

  scheduler = new FormatterScheduler (playerManager, ruleAdapter,
                                      multiDevice, compiler);

  compiler->setScheduler (scheduler);
  compiler->setLinkActionListener (scheduler);

  if (data->focusManager != NULL)
    {
    }
  else
    {
      scheduler->setKeyHandler (true);
    }

  data->focusManager = scheduler->getFocusManager ();

  documentEvents.clear ();
  documentEntryEvents.clear ();

}

bool
FormatterMediator::play (const string &file)
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
  this->currentFile = xpathmakeabs (file);
  this->currentDocument = compiler.parse (file);
  g_assert_nonnull (this->currentDocument);

  id = this->currentDocument->getId ();
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
  vector <Node *> *settings = this->currentDocument->getSettingsNodes ();
  g_assert_nonnull (settings);
  for (guint i = 0; i < settings->size (); i++)
    {
      int depth;

      persp = new NclNodeNesting ((settings->at (i))->getPerspective ());
      depth = this->compiler->getDepthLevel ();

      execobj = this->compiler
        ->getExecutionObjectFromPerspective (persp, NULL, depth);
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

  this->documentEvents[id] = evt;
  this->documentEntryEvents[id] = events;

  scheduler->startDocument (evt, events);

  return true;
}

GINGA_FORMATTER_END
