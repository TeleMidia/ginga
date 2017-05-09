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
#include "FormatterActiveDevice.h"
#include "multidev/ActiveDeviceDomain.h"
#include "multidev/BaseDeviceDomain.h"
#include "FormatterMediator.h"

#include "mb/Key.h"
#include "mb/InputManager.h"
#include "mb/Display.h"
using namespace ::ginga::mb;

#include "FormatterMediator.h"

#include "system/Thread.h"
using namespace ::ginga::system;

#include "ncl/DeviceLayout.h"
using namespace ::ginga::ncl;

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_FORMATTER_BEGIN

FormatterActiveDevice::FormatterActiveDevice (DeviceLayout *deviceLayout,
                                              int x, int y, int w, int h,
                                              bool useMulticast,
                                              int srvPort)
    : FormatterMultiDevice (deviceLayout, x, y, w, h,
                            useMulticast, srvPort)
{
  this->deviceServicePort = srvPort;

  img_dev = string (GINGA_MB_DATADIR) + "active-device.png";
  img_reset = string (GINGA_MB_DATADIR) + "active-device-reset.png";

  set<int> *evs;
  contentsInfo = new map<string, string>;
  deviceClass = DeviceDomain::CT_ACTIVE;
  formatter = NULL;

  Ginga_Display->getSize (&defaultWidth, &defaultHeight);

  mainLayout = new NclFormatterLayout (x, y, w, h);
  layoutManager[deviceClass] = mainLayout;

  evs = new set<int>;
  evs->insert (Key::KEY_TAP);
  evs->insert (Key::KEY_F11);
  evs->insert (Key::KEY_F10);
  im->addInputEventListener (this, evs);

  if (fileExists (img_dev))
    {
      serialized = Ginga_Display->createWindow (0, 0, DV_QVGA_WIDTH,
                                     DV_QVGA_HEIGHT, -1.0);

      s = Ginga_Display->createRenderedSurfaceFromImageFile (img_dev.c_str ());

      int cap = serialized->getCap ("ALPHACHANNEL");
      serialized->setCaps (cap);
      serialized->draw ();

      serialized->show ();
      serialized->renderFrom (s);
   

      delete s;
      s = 0;
    }
  else
    {
      clog
          << "FormatterActiveDevice::constructor Warning! File not found: ";
      clog << img_dev << endl;
    }

  TCPServerSocket *servSock;
  bool tryServerSocket = true;
  deviceServicePort = 4444;
  while (tryServerSocket)
    {
      try
        {
          clog << "FormatterActiveDevice:: trying use port "
               << deviceServicePort << "..." << endl;
          servSock = new TCPServerSocket (deviceServicePort);
          tryServerSocket = false;
        }
      catch (SocketException &e)
        {
          deviceServicePort++;
          clog << "FormatterActiveDevice::deviceServicePort"
               << deviceServicePort
               << "already in use. Exception error: " << e.what () << endl;
        }
    }
  char *srvPortStr = g_strdup_printf ("%d", deviceServicePort);
  tmp_dir = string (g_get_tmp_dir ()) + "/" + srvPortStr
            + SystemCompat::getIUriD ();
  g_mkdir (tmp_dir.c_str (), 0755);

  if (rdm == NULL)
    {
      rdm = RemoteDeviceManager::getInstance ();
      ((RemoteDeviceManager *)rdm)
          ->setDeviceDomain (
              new ActiveDeviceDomain (useMulticast, deviceServicePort));
    }

  rdm->setDeviceInfo (deviceClass, w, h, "");
  rdm->addListener (this);
  privateBaseManager = new PrivateBaseManager ();

  ContentTypeManager::getInstance ()->setMimeFile (string (GINGA_DATADIR)
                                                   + "mimetypes.ini");

  listening = true;
  try
    {
      // TCPServerSocket
      // servSock(FormatterActiveDevice::COMMAND_SERVER_PORT);
      while (listening)
        {
          clog << "FormatterActiveDevice::FormatterActiveDevice ";
          clog << "waiting servSock.accept() on port " << srvPortStr
               << endl;

          tcpSocket = servSock->accept ();

          clog << "FormatterActiveDevice::FormatterActiveDevice ";
          clog << "servSock accepted" << endl;

          // handleTCPClient(servSock.accept());
          if (serialized)
            {
              serialized->hide ();
            }
          handleTCPClient (tcpSocket);
        }
      servSock->cleanUp ();
    }
  catch (SocketException &e)
    {
      cerr << e.what () << endl;
      clog << "FormatterActiveDevice::End of Connection";
      clog << " with Base Device" << endl;
      listening = false;
    }
}

FormatterActiveDevice::~FormatterActiveDevice ()
{
  // lock();

  listening = false;
  delete Ginga_Display;

  if (privateBaseManager != NULL)
    {
      delete privateBaseManager;
      privateBaseManager = NULL;
    }
  if (rdm != NULL)
    {
      rdm->release ();
      rdm = NULL;
    }

  if (im != NULL)
    {
      im->removeInputEventListener (this);
      im = NULL;
    }

  if (formatter != NULL)
    {
      delete formatter;
      formatter = NULL;
    }

  if (initVars.size () > 0)
    {
      initVars.clear ();
    }

  // unlock();
  clog << "FormatterActiveDevice::~FormatterActiveDevice";
  clog << " all done" << endl;
}

bool
FormatterActiveDevice::socketSend (TCPSocket *sock, const string &payload)
{
  char *buffer;
  int plSize;

  if (sock == NULL)
    {
      return false;
    }

  buffer = deconst (char *, payload.c_str ());
  plSize = (int)payload.size ();

  try
    {
      sock->send (buffer, plSize);
    }
  catch (SocketException &e)
    {
      cerr << e.what () << endl;
    }

  return false;
}

void
FormatterActiveDevice::connectedToBaseDevice (unsigned int domainAddr)
{
  clog << "FormatterActiveDevice::connectedToDomainService '";
  clog << domainAddr << "'" << endl;

  hasRemoteDevices = true;
  im->addInputEventListener (this, NULL);
}

bool
FormatterActiveDevice::receiveRemoteEvent (int remoteDevClass,
                                           int eventType,
                                           const string &eventContent)
{
  vector<string> *args;
  if (eventType == DeviceDomain::FT_ATTRIBUTIONEVENT)
    {
      if (remoteDevClass == -1)
        {
          // Only sends to parent device vars within the "parent." namespace

          int tokenPos = eventContent.find ("parent.");

          if (tokenPos == 0)
            {
              stringstream sst;
              sst << "EVT ATTR " << eventContent;
              string evtattr = sst.str ();

              clog << "FormatterActiveDevice::";
              clog << "post " << evtattr << endl;
              socketSend (tcpSocket, evtattr);
            }
        }
    }

  if (remoteDevClass == DeviceDomain::CT_BASE)
    {
      if (eventType == DeviceDomain::FT_PRESENTATIONEVENT)
        {
          if (eventContent.find ("::") != std::string::npos)
            {
              args = split (eventContent, "::");
              if (args->size () == 2)
                {
                  if ((*args)[0] == "start")
                    {
                      formatter->play ();
                    }
                  else if ((*args)[0] == "stop")
                    {
                      formatter->stop ();
                      /*
                      player->stop();
                      delete player;
                      player = NULL;
                      */
                      // TODO: check
                    }
                }
              delete args;
              return true;
            }
        }
    }

  return false;
}

bool
FormatterActiveDevice::receiveRemoteContent (int remoteDevClass,
                                             const string &contentUri)
{
  map<string, string>::iterator i;

  clog << "FormatterActiveDevice::receiveRemoteContent from class '";
  clog << remoteDevClass << "' and contentUri '" << contentUri << "'";
  clog << endl;

  i = contentsInfo->find (contentUri);
  if (contentUri.find (".ncl") != std::string::npos
      && i != contentsInfo->end ())
    {
      return true;
    }

  return false;
}

bool
FormatterActiveDevice::receiveRemoteContentInfo (const string &contentId,
                                                 const string &contentUri)
{
  (*contentsInfo)[contentUri] = contentId;
  return true;
}

void 
FormatterActiveDevice::keyInputCallback (SDL_EventType evtType, SDL_Keycode key){ 

  if (key == SDLK_F11 || key == SDLK_F10)
          abort ();
  
 /* if (code == Key::KEY_TAP)
    {
      int currentX;
      int currentY;
      ev->getAxisValue (&currentX, &currentY, NULL);
      tapObject (deviceClass, currentX, currentY);
    } */  
}

bool
FormatterActiveDevice::openDocument (const string &contentUri)
{
  if (formatter == NULL)
    {
      formatter = createNCLPlayer ();
    }

  formatter->setCurrentDocument (contentUri);
  ((FormatterMediator *)formatter)
      ->getPresentationContext ()
      ->setRemoteDeviceListener ((IRemoteDeviceListener *)this);
  // formatter->getPresentationContext()->setGlobalVarListener(this);
  /*
  if (presContext == NULL)  {
          clog << "FormatterActiveDevice::presContext == NULL"<<endl;
          presContext = ((FormatterMediator
  *)formatter)->getPresentationContext();
          presContext->setGlobalVarListener(this);
  }
  else {
          clog << "FormatterActiveDevice::presContext != NULL"<<endl;
          //presContext = ((FormatterMediator
  *)formatter)->getPresentationContext();
          presContext->setGlobalVarListener((IContextListener*)this);
  }
  */
  // unlock();
  return (formatter != NULL);
}

NclPlayerData *
FormatterActiveDevice::createNclPlayerData ()
{
  NclPlayerData *data = NULL;

  data = new NclPlayerData;
  data->baseId = "";
  data->playerId = "";
  data->devClass = 0;
  data->x = xOffset;
  data->y = yOffset;
  data->w = defaultWidth;
  data->h = defaultHeight;
  data->enableGfx = true;
  data->parentDocId = "";
  data->nodeId = "";
  data->docId = "";
  data->transparency = 0;
  data->focusManager = NULL;
  data->privateBaseManager = privateBaseManager;
  data->editListener = (IPlayerListener *)this;

  return data;
}

INCLPlayer *
FormatterActiveDevice::createNCLPlayer ()
{
  INCLPlayer *fmt = NULL;
  NclPlayerData *data = NULL;
  data = createNclPlayerData ();
  data->baseId = 1;
  data->playerId = "active-device";
  data->privateBaseManager = privateBaseManager;

  fmt = new FormatterMediator (data);

  fmt->addListener (this);

  return fmt;
}

/* translates the command code from string to the const int values */
int
FormatterActiveDevice::getCommandCode (const string &com)
{
  if (com.compare ("ADD") == 0)
    {
      return FormatterActiveDevice::ADD_DOCUMENT;
    }
  else if (com.compare ("REMOVE") == 0)
    {
      return FormatterActiveDevice::REMOVE_DOCUMENT;
    }
  else if (com.compare ("START") == 0)
    {
      return FormatterActiveDevice::START_DOCUMENT;
    }
  else if (com.compare ("STOP") == 0)
    {
      return FormatterActiveDevice::STOP_DOCUMENT;
    }
  else if (com.compare ("PAUSE") == 0)
    {
      return FormatterActiveDevice::PAUSE_DOCUMENT;
    }
  else if (com.compare ("RESUME") == 0)
    {
      return FormatterActiveDevice::RESUME_DOCUMENT;
    }
  else if (com.compare ("SET") == 0)
    {
      return FormatterActiveDevice::SET_VAR;
    }
  else if (com.compare ("SELECT") == 0)
    {
      return FormatterActiveDevice::SELECTION;
    }

  return 0;
}

GINGA_FORMATTER_END
