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
  usersUri = string (GINGA_CONTEXTMANAGER_DATADIR) + "users.ini";
  contextsUri = string (GINGA_CONTEXTMANAGER_DATADIR) + "contexts.ini";
  curUserId = -1;
  systemInfo = new SystemInfo ();

  initializeUsers ();
  initializeContexts ();

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
    {
      _instance = new ContextManager ();
    }

  return _instance;
}

void
ContextManager::initializeUsers ()
{
  GingaUser *newUser;
  ifstream fis;
  string line = "", name = "", location = "", passwd = "";
  int id = -1, age = -1;
  char genre = 'a';
  bool validUser = false;
  bool invalidUser = false;

  fis.open (usersUri.c_str (), ifstream::in);

  if (!fis.is_open ())
    {
      clog << "ContextManager::initializeUsers() Warning: can't open ";
      clog << "config file '" << usersUri << "'" << endl;
      return;
    }

  while (fis.good ())
    {
      if (line == "::")
        {
          fis >> line;
          if (line == "=")
            {
              fis >> line;
              curUserId = ::std::stoi (line);
            }
          else
            {
              invalidUser = true;
              clog << "ContextManager::initializeUsers ";
              clog << "warning! currentUser token must to be ':: ='";
              clog << endl;
            }
        }

      if (line != "||")
        {
          fis >> line;
        }
      else
        {
          fis >> line;
          if (line == "=")
            {
              fis >> line;
              id = (int)::ginga::util::stof (line);
              if (id >= 0)
                {
                  fis >> line;
                  name = line;
                  if (name != "")
                    {
                      fis >> line;
                      passwd = line;
                      if (passwd != "")
                        {
                          fis >> line;
                          age = (int)::ginga::util::stof (line);
                          if (age >= 0)
                            {
                              fis >> line;
                              location = line;
                              if (location != "")
                                {
                                  fis >> line;
                                  if (line == "m" || line == "f")
                                    {

                                      genre = line[0];
                                      validUser = true;
                                      invalidUser = false;
                                    }
                                  else
                                    {
                                      invalidUser = true;
                                      clog << "ContextManager::";
                                      clog << "initializeUsers ";
                                      clog << "warning! genre != m and f";
                                      clog << endl;
                                    }
                                }
                              else
                                {
                                  invalidUser = true;
                                  clog << "ContextManager::";
                                  clog << "initializeUsers ";
                                  clog << "warning! NULL location";
                                  clog << endl;
                                }
                            }
                          else
                            {
                              invalidUser = true;
                              clog << "ContextManager::initializeUsers ";
                              clog << "warning! age < 0" << endl;
                            }
                        }
                      else
                        {
                          invalidUser = true;
                          clog << "ContextManager::initializeUsers ";
                          clog << "warning! NULL passwd." << endl;
                        }
                    }
                  else
                    {
                      invalidUser = true;
                      clog << "ContextManager::initializeUsers warning! ";
                      clog << "name == ''" << endl;
                    }
                }
              else
                {
                  invalidUser = true;
                  clog << "ContextManager::initializeUsers warning! ";
                  clog << "token < 0" << endl;
                }
            }
          else
            {
              invalidUser = true;
              clog << "ContextManager::initializeUsers warning! ";
              clog << "token != '='" << endl;
            }
        }

      if (validUser)
        {
          validUser = false;
          newUser = new GingaUser (id, name, passwd);
          newUser->setUserAge (passwd, age);
          newUser->setUserLocation (passwd, location);
          newUser->setUserGenre (passwd, genre);

          addUser (newUser);
        }
      else if (invalidUser)
        {
          clog << "ContextManager::initializeUsers warning! Invalid ";
          clog << "user '" << curUserId << "'" << endl;
        }
    }

  fis.close ();
}

void
ContextManager::initializeContexts ()
{
  ifstream fis;
  string line = "", key = "", value = "";
  int id = -1;

  fis.open (contextsUri.c_str (), ifstream::in);

  if (!fis.is_open ())
    {
      clog << "ContextManager::initializeContexts() Warning: can't open ";
      clog << "cfg file '" << contextsUri << "'" << endl;
      return;
    }

  while (fis.good ())
    {
      if (line == "::")
        {
          fis >> line;
          if (line == "=")
            {
              fis >> line;
              curUserId = std::stoi (line);
            }
        }

      if (line != "||")
        {
          fis >> line;
        }
      else
        {
          fis >> line;
          if (line == "=")
            {
              fis >> line;
              id = (int)::ginga::util::stof (line);
              if (id >= 0)
                {
                  while (fis.good ())
                    {
                      fis >> line;
                      if (line == "||")
                        {
                          break;
                        }

                      key = line;
                      fis >> line;
                      if (line == "=")
                        {
                          fis >> line;
                          value = line;
                          addContextVar (id, key, value);
                        }
                    }
                }
            }
        }
    }

  fis.close ();
}

void
ContextManager::addContextVar (int userId, string varName, string varValue)
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
ContextManager::saveUsersAccounts ()
{
  FILE *fd;
  map<int, GingaUser *>::iterator i;

  remove (usersUri.c_str ());
  fd = fopen (usersUri.c_str (), "w+b");
  if (fd == 0)
    return;

  GingaUser::saveString (fd, ":: =");
  GingaUser::saveString (fd, itos (curUserId));
  GingaUser::saveString (fd, "\n");

  i = users.begin ();
  while (i != users.end ())
    {
      i->second->saveTo (fd);
      GingaUser::saveString (fd, "\n");
      ++i;
    }

  fclose (fd);
}

void
ContextManager::saveUsersProfiles ()
{
  FILE *fd;
  map<int, map<string, string> *>::iterator i;

  remove (contextsUri.c_str ());
  fd = fopen (contextsUri.c_str (), "w+b");
  if (fd == NULL)
    return;

  GingaUser::saveString (fd, ":: =");
  GingaUser::saveString (fd, itos (curUserId));
  GingaUser::saveString (fd, "\n");

  i = contexts.begin ();
  while (i != contexts.end ())
    {
      saveProfile (fd, i->first, i->second);
      ++i;
    }

  fclose (fd);
}

void
ContextManager::saveProfile (FILE *fd, int userId,
                             map<string, string> *profile)
{

  map<string, string>::iterator i;

  GingaUser::saveString (fd, "|| =");
  GingaUser::saveString (fd, itos (userId));
  GingaUser::saveString (fd, "\n");

  i = profile->begin ();
  while (i != profile->end ())
    {
      GingaUser::saveString (fd, i->first);
      GingaUser::saveString (fd, "=");
      GingaUser::saveString (fd, i->second);
      GingaUser::saveString (fd, "\n");
      ++i;
    }
}

void
ContextManager::setCurrentUserId (int userId)
{
  if (users.count (userId) != 0)
    {
      curUserId = userId;
    }
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
ContextManager::listUsersNicks ()
{
  map<int, GingaUser *>::iterator i;

  clog << "ContextManager::listUsersNicks '";
  i = users.begin ();
  while (i != users.end ())
    {
      clog << i->second->getUserName () << "' ";
      ++i;
    }
  clog << endl;
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
ContextManager::setGlobalVar (string varName, string varValue)
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
