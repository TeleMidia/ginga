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
#include "InteractiveChannelManager.h"
#include "CurlInteractiveChannel.h"

GINGA_IC_BEGIN

InteractiveChannelManager *InteractiveChannelManager::_instance = NULL;
InteractiveChannelManager::InteractiveChannelManager ()
{
  ics = new set<CurlInteractiveChannel *>;
  urisIcs = new map<string, CurlInteractiveChannel *>;
}

InteractiveChannelManager::~InteractiveChannelManager ()
{
  if (ics != NULL)
    {
      delete ics;
    }

  delete _instance;
}

bool
InteractiveChannelManager::hasInteractiveChannel ()
{
  CurlInteractiveChannel *ic;
  set<string> *objects;
  string symbol;
  set<string>::iterator i;
  ic = new CurlInteractiveChannel ();
  if (ic->hasConnection ())
    {
      delete ic;
      return true;
    }
  delete ic;
  clog << "InteractiveChannelManager::hasInteractiveChannel";
  clog << " return false" << endl;
  return false;
}

InteractiveChannelManager *
InteractiveChannelManager::getInstance ()
{
  if (_instance == NULL)
    {
      _instance = new InteractiveChannelManager ();
    }
  return _instance;
}

set<CurlInteractiveChannel *> *
InteractiveChannelManager::getInteractiveChannels ()
{
  return ics;
}

void
InteractiveChannelManager::clearInteractiveChannelManager ()
{
  releaseInteractiveChannels ();
}

void
InteractiveChannelManager::releaseInteractiveChannel (
    CurlInteractiveChannel *ic)
{

  set<CurlInteractiveChannel *>::iterator i;
  map<string, CurlInteractiveChannel *>::iterator j;

  i = ics->find (ic);
  if (ics->end () != i)
    {
      (*i)->releaseUrl ();
      delete *i;
      ics->erase (i);

      j = urisIcs->begin ();
      while (j != urisIcs->end ())
        {
          if (j->second == ic)
            {
              urisIcs->erase (j);
              return;
            }
          ++j;
        }
    }
}

CurlInteractiveChannel *
InteractiveChannelManager::createInteractiveChannel (string rUri)
{

  CurlInteractiveChannel *ic = NULL;

  if (rUri.length () > 7 && rUri.substr (0, 7) == "http://")
    {
      ic = new CurlInteractiveChannel ();
    }

  if (ic != NULL)
    {
      ics->insert (ic);
      (*urisIcs)[rUri] = ic;
    }

  return ic;
}

CurlInteractiveChannel *
InteractiveChannelManager::getInteractiveChannel (string remoteUri)
{

  if (urisIcs->count (remoteUri) != 0)
    {
      return (*urisIcs)[remoteUri];
    }

  return NULL;
}

void
InteractiveChannelManager::releaseInteractiveChannels ()
{
  set<CurlInteractiveChannel *>::iterator i;
  urisIcs->clear ();
  i = ics->begin ();
  while (i != ics->end ())
    {
      delete *i;
      ++i;
    }
  ics->clear ();
}

void
InteractiveChannelManager::performPendingUrls ()
{
  pthread_t id1;

  clog << "InteractiveChannelManager::performPendingUrls for '";
  clog << ics->size () << "' urls" << endl;

  if (!ics->empty ())
    {
      pthread_create (&id1, 0, InteractiveChannelManager::asyncPerform,
                      this);

      pthread_detach (id1);
    }
}

void *
InteractiveChannelManager::asyncPerform (void *thiz)
{
  InteractiveChannelManager *icm;
  set<CurlInteractiveChannel *> *icSet;
  set<CurlInteractiveChannel *>::iterator i;

  icm = (InteractiveChannelManager *)thiz;
  icSet = icm->getInteractiveChannels ();

  i = icSet->begin ();
  while (i != icSet->end ())
    {
      if ((*i)->performUrl ())
        {
          ++i;
        }
    }

  icm->releaseInteractiveChannels ();

  return NULL;
}

GINGA_IC_END
