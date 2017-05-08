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
#include "ContextManager.h"

#include "GingaUser.h"
#include "SystemInfo.h"

#include "system/Thread.h"
using namespace ::ginga::system;

#include "util/functions.h"
using namespace ::ginga::util;

GINGA_CTXMGMT_BEGIN

ContextManager *ContextManager::_instance = NULL;

ContextManager::ContextManager ()
{
  curUserId = -1;
  systemInfo = new SystemInfo ();
  systemInfo->setSystemTable (getUserProfile (getCurrentUserId ()));
  Thread::mutexInit (&groupsMutex, false);
}

ContextManager::~ContextManager ()
{
  map<int, GingaUser *>::iterator i;
  set<IContextListener *>::iterator j;

  Thread::mutexLock (&groupsMutex);
  i = users.begin ();
  while (i != users.end ())
    {
      delete i->second;
      ++i;
    }
  Thread::mutexUnlock (&groupsMutex);
  Thread::mutexDestroy (&groupsMutex);
}

ContextManager *
ContextManager::getInstance ()
{
  if (_instance == NULL)
    _instance = new ContextManager ();

  return _instance;
}

void
ContextManager::addContextVar (int userId, const string &varName,
                               const string &varValue)
{
  map<int, map<string, string> *>::iterator i;
  map<string, string> *vars;

  i = contexts.find (userId);
  if (i != contexts.end ())
    {
      vars = i->second;
    }
  else
    {
      vars = new map<string, string>;
      contexts[userId] = vars;
    }

  (*vars)[varName] = varValue;
}

void
ContextManager::addUser (GingaUser *newUser)
{
  int id;

  id = newUser->getUserId ();
  if (users.count (id) != 0)
    {
      clog << "ContextManager::addUser Warning! ";
      clog << "Trying to add the same user twice (id = '";
      clog << id << "'" << endl;
      return;
    }

  users[id] = newUser;
}

void
ContextManager::setCurrentUserId (int userId)
{
  curUserId = userId;
}

int
ContextManager::getCurrentUserId ()
{
  return curUserId;
}

GingaUser *
ContextManager::getUser (int userId)
{
  map<int, GingaUser *>::iterator i;

  i = users.find (userId);
  if (i != users.end ())
    {
      return i->second;
    }

  clog << "ContextManager::getUser Warning! can't find user '";
  clog << userId << "'" << endl;
  return NULL;
}

map<string, string> *
ContextManager::getUserMap (int userId)
{
  map<int, map<string, string> *>::iterator i;

  i = contexts.find (userId);
  if (i != contexts.end ())
    {
      return i->second;
    }

  clog << "ContextManager::getUserProfile Warning! can't find profile ";
  clog << "of user '" << userId << "'" << endl;
  return NULL;
}

map<string, string> *
ContextManager::getUserProfile (int userId)
{
  map<string, string> *userMap;
  map<string, string> *userProfile;

  userMap = getUserMap (userId);
  if (userMap == NULL)
    {
      return NULL;
    }

  userProfile = new map<string, string> (*userMap);
  return userProfile;
}

map<string, string> *
ContextManager::getUsersNames ()
{
  map<string, string> *names;
  map<int, GingaUser *>::iterator i;

  names = new map<string, string>;
  i = users.begin ();
  while (i != users.end ())
    {
      (*names)[i->second->getUserName ()] = "";
      ++i;
    }

  if (names->empty ())
    {
      delete names;
      names = NULL;
    }

  return names;
}

SystemInfo *
ContextManager::getSystemInfo ()
{
  return systemInfo;
}

void
ContextManager::addContextListener (IContextListener *listener)
{
  clog << "ContextManager::addContextListener" << endl;
  Thread::mutexLock (&groupsMutex);
  ctxListeners.insert (listener);
  Thread::mutexUnlock (&groupsMutex);
}

void
ContextManager::removeContextListener (IContextListener *listener)
{
  set<IContextListener *>::iterator i;

  clog << "ContextManager::removeContextListener" << endl;
  Thread::mutexLock (&groupsMutex);
  i = ctxListeners.find (listener);
  if (i != ctxListeners.end ())
    {
      ctxListeners.erase (i);
    }
  Thread::mutexUnlock (&groupsMutex);
}

void
ContextManager::setGlobalVar (const string &varName, const string &varValue)
{
  set<IContextListener *>::iterator i;

  clog << "ContextManager::setGlobalVar(" << varName << ", ";
  clog << varValue << ") " << endl;

  Thread::mutexLock (&groupsMutex);
  i = ctxListeners.begin ();
  while (i != ctxListeners.end ())
    {
      (*i)->receiveGlobalAttribution (varName, varValue);
      ++i;
    }
  Thread::mutexUnlock (&groupsMutex);
}

GINGA_CTXMGMT_END
