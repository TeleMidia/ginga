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
#include "RemoteEventService.h"

#include "util/Base64.h"

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "ctxmgmt/ContextManager.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_MULTIDEV_BEGIN

ContextManager *RemoteEventService::contextManager = NULL;
RemoteEventService *RemoteEventService::_instance = NULL;

RemoteEventService::RemoteEventService ()
{
  RemoteEventService::contextManager = ContextManager::getInstance ();
  clog << "RemoteEventService::new RemoteEventService()" << endl;

  groups.clear ();
  Thread::mutexInit (&groupsMutex, false);
}

void
RemoteEventService::setBaseDeviceNCLPath (string base_device_ncl_path)
{
  this->base_device_ncl_path = base_device_ncl_path;
}

RemoteEventService::~RemoteEventService ()
{
  map<int, TcpSocketService *>::iterator i;

  Thread::mutexLock (&groupsMutex);
  i = groups.begin ();
  while (i != groups.end ())
    {
      delete i->second;
      ++i;
    }
  groups.clear ();

  Thread::mutexUnlock (&groupsMutex);
  Thread::mutexDestroy (&groupsMutex);
}

RemoteEventService *
RemoteEventService::getInstance ()
{
  if (_instance == NULL)
    {
      _instance = new RemoteEventService ();
    }
  return _instance;
}

void
RemoteEventService::addDeviceClass (unsigned int id)
{
  Thread::mutexLock (&groupsMutex);
  if (groups.count (id) == 0)
    {
      groups[id]
          = new TcpSocketService (RemoteEventService::DEFAULT_PORT, this);
    }
  else
    {
      clog << "RemoteEventService::addDeviceClass Warning! Trying to ";
      clog << "add the same device class '" << id << "' twice!" << endl;
    }
  Thread::mutexUnlock (&groupsMutex);
}

void
RemoteEventService::addDevice (unsigned int device_class,
                               unsigned int device_id, char *addr,
                               int srvPort, bool isLocalConnection)
{
  map<int, TcpSocketService *>::iterator i;
  TcpSocketService *tss;

  Thread::mutexLock (&groupsMutex);
  i = groups.find (device_class);
  if (i == groups.end ())
    {
      Thread::mutexUnlock (&groupsMutex);
      return;
    }

  tss = i->second;
  tss->addConnection (device_id, addr, srvPort, isLocalConnection);
  clog << "RemoteEventService :: TcpSocketService->addConnection";
  clog << "device_id=" << device_id << endl;

  Thread::mutexUnlock (&groupsMutex);
}

void
RemoteEventService::addDocument (unsigned int device_class, char *name,
                                 char *body)
{
  TcpSocketService *tss;

  Thread::mutexLock (&groupsMutex);
  if (groups.count (device_class) == 0)
    {
      Thread::mutexUnlock (&groupsMutex);
      return;
    }

  tss = groups[device_class];
  tss->postTcpCommand (deconst (char *, "ADD"), 0, name, body);
  Thread::mutexUnlock (&groupsMutex);
}

void
RemoteEventService::startDocument (unsigned int device_class, char *name)
{
  TcpSocketService *tss;

  string str_name = string (name);

  Thread::mutexLock (&groupsMutex);
  if (groups.count (device_class) == 0)
    {
      Thread::mutexUnlock (&groupsMutex);
      return;
    }

  tss = groups[device_class];

  clog << "RemoteEventService::startDocument " << name << endl;

  size_t pos_fname = str_name.find_last_of ("/\\");
  string doc_name;

  if (pos_fname != std::string::npos)
    {
      doc_name = string (str_name.substr (pos_fname + 1));
    }
  else
    {
      doc_name = str_name;
    }

  string str_path = string (str_name.substr (0, pos_fname));

  int pos_fdir = str_path.find_last_of ("/\\");

  string last_dir_name = string (str_path.substr (pos_fdir + 1));

  string doc_rel_path = doc_name;

  string zipDumpStr = string (g_get_tmp_dir ()) + "/basetmp.zip";

  string dir_app
      = SystemCompat::getPath (base_device_ncl_path)
        + SystemCompat::getIUriD ()
        + SystemCompat::getPath (SystemCompat::updatePath (string (name)));

  clog << "RemoteEventService::startDocument dir app=" << dir_app << endl;
  // clog << "RemoteEventService::tmp dir="<<string(zip_dump)<<endl;

  SystemCompat::zip_directory (zipDumpStr, dir_app,
                               SystemCompat::getIUriD ());
  clog << "RemoteEventService::startDocument zip_directory all done!"
       << endl;

  string zip_base64 = getBase64FromFile (zipDumpStr);
  clog << "RemoteEventService::startDocument getBase64 all done!" << endl;
  remove (zipDumpStr.c_str ());
  // TODO: prefetch. add w/o start

  if (zip_base64 != "")
    {
      clog << "RemoteEventService::zipb64.len = ";
      clog << zip_base64.length () << endl;
      tss->postTcpCommand (deconst (char *, "START"), 0,
                           deconst (char *, doc_rel_path.c_str ()),
                           deconst (char *, zip_base64.c_str ()));

      clog << "RemoteEventService:: START name=" << doc_rel_path << endl;
    }

  Thread::mutexUnlock (&groupsMutex);
}

void
RemoteEventService::stopDocument (unsigned int device_class, char *name)
{
  TcpSocketService *tss;

  Thread::mutexLock (&groupsMutex);
  if (groups.count (device_class) == 0)
    {
      Thread::mutexUnlock (&groupsMutex);
      return;
    }

  tss = groups[device_class];
  clog << "RemoteEventService::stopDocument " << name << endl;
  tss->postTcpCommand (deconst (char *, "STOP"), 0, name, deconst (char *, ""));
  Thread::mutexUnlock (&groupsMutex);
}

bool
RemoteEventService::newDeviceConnected (arg_unused (int newDevClass), arg_unused (int w), arg_unused (int h))
{
  return false;
}

void
RemoteEventService::connectedToBaseDevice (unsigned arg_unused (int domainAddr))
{
}

bool
RemoteEventService::receiveRemoteContent (arg_unused (int remoteDevClass),
                                          arg_unused (string contentUri))
{
  return false;
}

bool
RemoteEventService::receiveRemoteContent (arg_unused (int remoteDevClass), arg_unused (char *stream),
                                          arg_unused (int streamSize))
{
  return false;
}

bool
RemoteEventService::receiveRemoteContentInfo (arg_unused (string contentId),
                                              arg_unused (string contentUri))
{
  return false;
}

bool
RemoteEventService::receiveRemoteEvent (arg_unused (int remoteDevClass), arg_unused (int eventType),
                                        arg_unused (string eventContent))
{
  if (eventType == 5)
    {
      string name, value;
      size_t pos;
      pos = eventContent.find ("=");
      name = eventContent.substr (0, pos - 1);
      value = eventContent.substr (pos + 2);
      RemoteEventService::contextManager->setGlobalVar (name, value);
    }
  return true;
}

GINGA_MULTIDEV_END
