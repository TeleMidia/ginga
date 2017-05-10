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


// Private methods.

gpointer
PresentationEngineManager::runThreadWrapper (gpointer data)
{
  g_assert_nonnull (data);
  ((PresentationEngineManager *) data)->runThread ();
  return NULL;
}

void
PresentationEngineManager::runThread ()
{
  g_assert_nonnull (this->formatter);
  this->formatter->play ();
}


// Public methods.

PresentationEngineManager::PresentationEngineManager (const string &file)
{
  NclPlayerData *data;

  data = new NclPlayerData;
  g_assert_nonnull (data);

  data->baseId = "-1";
  data->devClass = 0;
  data->docId = "";
  data->focusManager = NULL;
  data->nodeId = "";
  data->parentDocId = "";
  data->playerId = file;
  data->transparency = 0;
  data->x = 0;
  data->y = 0;
  Ginga_Display->getSize (&data->w, &data->h);

  data->privateBaseManager = new PrivateBaseManager ();
  g_assert_nonnull (data->privateBaseManager);

  this->formatter = new FormatterMediator (data);
  g_assert_nonnull (this->formatter);

  this->formatter->setCurrentDocument (file);
}

PresentationEngineManager::~PresentationEngineManager ()
{
  g_assert_nonnull (this->formatter);
  delete this->formatter;
}

void
PresentationEngineManager::run ()
{
  g_thread_new ("PEM", runThreadWrapper, this);
}

GINGA_LSSM_END
