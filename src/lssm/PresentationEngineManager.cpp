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
#include "PresentationEngineManager.h"

#include "formatter/FormatterMediator.h"
using namespace ::ginga::formatter;

GINGA_LSSM_BEGIN

PresentationEngineManager::PresentationEngineManager ()
{
  this->mutexInit ();
  privateBaseManager = new PrivateBaseManager ();

  ContentTypeManager::getInstance ()->setMimeFile (string (GINGA_DATADIR)
                                                   + "mimetypes.ini");
}

PresentationEngineManager::~PresentationEngineManager ()
{
  this->mutexClear ();
}

INCLPlayer *
PresentationEngineManager::createNclPlayer (const string &baseId,
                                            const string &fname)
{
  NclPlayerData *data = NULL;
  data = createNclPlayerData ();

  data->devClass = 0;
  data->baseId = baseId;
  data->playerId = fname;
  data->privateBaseManager = privateBaseManager;

  this->formatter = new FormatterMediator (data);
  this->formatter->setCurrentDocument (fname);
  return this->formatter;
}

NclPlayerData *
PresentationEngineManager::createNclPlayerData ()
{
  NclPlayerData *data = NULL;


  data = new NclPlayerData;
  data->baseId = "";
  data->playerId = "";
  data->x = 0;
  data->y = 0;
  Ginga_Display->getSize (&data->w, &data->h);
  data->parentDocId = "";
  data->nodeId = "";
  data->docId = "";
  data->transparency = 0;
  data->focusManager = NULL;
  data->privateBaseManager = NULL;
  data->editListener = NULL;

  return data;
}

bool
PresentationEngineManager::openNclFile (const string &fname)
{
  INCLPlayer *formatter;

  this->lock ();
  formatter = createNclPlayer ("-1", fname);
  this->unlock ();

  return (formatter != NULL);
}

//--

gpointer
PresentationEngineManager::startPresentationThreadWrapper (gpointer data)
{
  g_assert_nonnull (data);
  ((PresentationEngineManager *) data)->startPresentationThread();
  return NULL;
}

void
PresentationEngineManager::startPresentationThread ()
{
  g_assert_nonnull (this->formatter);
  this->formatter->play ();
}

bool
PresentationEngineManager::startPresentation (arg_unused (const string &nclFile),
                                              arg_unused (const string &interfId))
{
   g_thread_new ("startPresentation", startPresentationThreadWrapper, this);

   return TRUE;

}

GINGA_LSSM_END
