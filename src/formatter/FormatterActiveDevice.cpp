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

#include "mb/CodeMap.h"
#include "mb/InputManager.h"
#include "mb/SDLDisplay.h"
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

  defaultWidth = Ginga_Display->getWidthResolution ();
  defaultHeight = Ginga_Display->getHeightResolution ();

  mainLayout = new NclFormatterLayout (x, y, w, h);
  layoutManager[deviceClass] = mainLayout;

  evs = new set<int>;
  evs->insert (CodeMap::KEY_TAP);
  evs->insert (CodeMap::KEY_F11);
  evs->insert (CodeMap::KEY_F10);
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
      serialized->lowerToBottom ();

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
  //Ginga_Display->releaseScreen ();

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
FormatterActiveDevice::socketSend (TCPSocket *sock, string payload)
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
                                           string eventContent)
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
                                             string contentUri)
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
FormatterActiveDevice::receiveRemoteContentInfo (string contentId,
                                                 string contentUri)
{
  (*contentsInfo)[contentUri] = contentId;
  return true;
}

bool
FormatterActiveDevice::userEventReceived (SDLInputEvent *ev)
{
  string mnemonicCode;
  int currentX;
  int currentY;
  int code;

  code = ev->getKeyCode ();
  if (code == CodeMap::KEY_F11 || code == CodeMap::KEY_F10)
    {
      abort ();
    }
  if (code == CodeMap::KEY_TAP)
    {
      ev->getAxisValue (&currentX, &currentY, NULL);
      tapObject (deviceClass, currentX, currentY);
    }

  return true;
}

bool
FormatterActiveDevice::openDocument (string contentUri)
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
FormatterActiveDevice::getCommandCode (string *com)
{
  if (com->compare ("ADD") == 0)
    {
      return FormatterActiveDevice::ADD_DOCUMENT;
    }
  else if (com->compare ("REMOVE") == 0)
    {
      return FormatterActiveDevice::REMOVE_DOCUMENT;
    }
  else if (com->compare ("START") == 0)
    {
      return FormatterActiveDevice::START_DOCUMENT;
    }
  else if (com->compare ("STOP") == 0)
    {
      return FormatterActiveDevice::STOP_DOCUMENT;
    }
  else if (com->compare ("PAUSE") == 0)
    {
      return FormatterActiveDevice::PAUSE_DOCUMENT;
    }
  else if (com->compare ("RESUME") == 0)
    {
      return FormatterActiveDevice::RESUME_DOCUMENT;
    }
  else if (com->compare ("SET") == 0)
    {
      return FormatterActiveDevice::SET_VAR;
    }
  else if (com->compare ("SELECT") == 0)
    {
      return FormatterActiveDevice::SELECTION;
    }

  return 0;
}

/*
 * handles a command coming from TCP, which controls the formatter
 * command syntax
 * 	  <ID> <NPT> <COMMAND> <PAYLOAD_DESC> <PAYLOAD_SIZE>\n<PAYLOAD>
 */
bool
FormatterActiveDevice::handleTCPCommand (arg_unused (string sid), arg_unused (string snpt),
                                         string scommand,
                                         string spayload_desc,
                                         string payload)
{
  bool handled = false;
  clog << "FormatterActiveDevice::handleTCPCommand scommand=" << scommand
       << endl;
  clog << "FormatterActiveDevice::handleTCPCommand spayload_desc='"
       << spayload_desc << endl;

  string appName = spayload_desc.substr (0, spayload_desc.rfind ("."))
                   + SystemCompat::getIUriD ();
  string appPath = tmp_dir + appName;
  clog << "FormatterActiveDevice::handleTCPCommand appName=" << appName
       << endl;
  ;
  clog << "FormatterActiveDevice::handleTCPCommand appPath=" << appPath
       << endl;
  string zip_dump = tmp_dir + "tmpzip.zip";
  clog << "FormatterActiveDevice::handleTCPCommand zip_dump=" << zip_dump
       << endl;

  int command_id = getCommandCode (&scommand);
  switch (command_id)
    {
    case FormatterActiveDevice::ADD_DOCUMENT:
      {
        clog << "FormatterActiveDevice::ADD_DOCUMENT" << endl;
        g_mkdir (appPath.c_str (), 0755);
        writeFileFromBase64 (payload, deconst (char *, zip_dump.c_str ()));
        SystemCompat::unzip_file (deconst (char *, zip_dump.c_str ()),
                                  deconst (char *, appPath.c_str ()));
        remove (deconst (char *, zip_dump.c_str ()));
        clog << "FormatterActiveDevice:: unzip app=" << spayload_desc
             << " with payload size=" << strlen (payload.c_str ())
             << "in dir=" << appPath << endl;
        handled = true;
      }
      break;
    case FormatterActiveDevice::REMOVE_DOCUMENT:
      {
        clog << "FormatterActiveDevice::REMOVE DOCUMENT" << endl;
        handled = true;
      }
      break;
    case FormatterActiveDevice::START_DOCUMENT:
      {
        clog << "FormatterActiveDevice::START:" << spayload_desc << endl;
        if (!payload.empty ())
          {
            g_mkdir (appPath.c_str (), 0755);
            writeFileFromBase64 (payload, deconst (char *, zip_dump.c_str ()));
            SystemCompat::unzip_file (deconst (char *, zip_dump.c_str ()),
                                      deconst (char *, appPath.c_str ()));
            remove (deconst (char *, zip_dump.c_str ()));
            clog << "FormatterActiveDevice:: unzip app=" << spayload_desc
                 << " with payload size=" << strlen (payload.c_str ())
                 << "in dir=" << appPath << endl;
          }
        string full_path = appPath + spayload_desc;
        if (openDocument (full_path))
          {
            clog << "FormatterActiveDevice::START_DOCUMENT play "
                 << full_path << endl;
            formatter->setKeyHandler (true);
            formatter->play ();

            // using formatter->setPropertyValue(pname,pvalue);
            // would do Player (gingacc-player) properties[name] = value
            map<string, string>::iterator it;
            for (it = initVars.begin (); it != initVars.end (); it++)
              {
                string pname = (string) (*it).first;
                string pvalue = (string) (*it).second;
                ((FormatterMediator *)formatter)
                    ->getPresentationContext ()
                    ->setPropertyValue (pname, pvalue);
              }
          }
        else
          {
            clog << "FormatterActiveDevice::START_DOCUMENT: " << full_path;
            clog << " open failure!" << endl;
          }
        handled = true;
      }
      break;
    case FormatterActiveDevice::STOP_DOCUMENT:
      {
        clog << "FormatterActiveDevice::STOP DOCUMENT" << endl;
        string full_path = string ("");
        full_path.append (appPath);
        full_path.append (spayload_desc);
        if (currentDocUri.compare (full_path) == 0)
          {
            if (formatter != NULL)
              {
                formatter->stop ();
              }
            currentDocUri = "";
          }

        handled = true;
      }
      break;
    case FormatterActiveDevice::PAUSE_DOCUMENT:
      {
        clog << "FormatterActiveDevice::PAUSE DOCUMENT" << endl;

        string full_path = string ("");
        full_path.append (appPath);
        full_path.append (spayload_desc);

        if (currentDocUri.compare (full_path) == 0)
          {
            if (formatter != NULL)
              {
                formatter->pause ();
              }
          }
        handled = true;
      }
      break;
    case FormatterActiveDevice::RESUME_DOCUMENT:
      {
        clog << "FormatterActiveDevice::RESUME DOCUMENT" << endl;
        // spayload_desc = appPath + spayload_desc;

        string full_path = string ("");
        full_path.append (appPath);
        full_path.append (spayload_desc);
        if (currentDocUri.compare (full_path) == 0)
          {
            if (formatter != NULL)
              {
                formatter->resume ();
              }
          }
        handled = true;
      }
      break;
    case FormatterActiveDevice::SET_VAR:
      {
        string pname, pvalue;
        size_t pos;

        pos = spayload_desc.find ("=");
        pname = spayload_desc.substr (0, pos);
        pvalue = spayload_desc.substr (pos + 1);
        if (formatter != NULL)
          {
            // TODO: check if formatter is active?
            formatter->setPropertyValue (pname, pvalue);
            clog << "FormatterActiveDevice::SET VAR " << pname << " = "
                 << pvalue << endl;
          }
        else
          {
            initVars[pname] = pvalue;
            clog << "FormatterActiveDevice::SET VAR (init) " << pname
                 << " = ";
            clog << initVars[pname] << endl;
            // parent session initialization vars
          }
        handled = true;
      }
      break;
    case FormatterActiveDevice::SELECTION:
      {
        clog << "FormatterActiveDevice::SELECTION" << endl;
        // TODO: handle selection
        handled = true;
      }
      break;
    default:
      break;
    }
  return handled;
}

/* TCP client handling function */
void
FormatterActiveDevice::handleTCPClient (TCPSocket *sock)
{
  unsigned int i;
  size_t pos;
  string buf;
  bool valid_command = false;

  string sid, snpt, scommand, spayload_desc, spayload_size;
  string payload = "";
  int payload_size;
  stringstream ss;
  char buffer[FormatterActiveDevice::RCVBUFSIZE + 1] = { 0 };
  int recvMsgSize;
  bool reading = true;
  vector<string> tokens;
  // char pri[100];
  char pri[FormatterActiveDevice::RCVBUFSIZE]; // first line; MAX command
                                               // size
  char *sec;                                   // second line

  if (rdm != NULL)
    {
      rdm->release ();
    }

  clog << "FormatterActiveDevice::Handling connection from: ";

  try
    {
      clog << sock->getForeignAddress () << ":";
    }
  catch (SocketException &e)
    {
      cerr << "FormatterActiveDevice::Unable to get foreign address";
      cerr << endl;
    }

  try
    {
      clog << sock->getForeignPort ();
    }
  catch (SocketException &e)
    {
      cerr << "FormatterActiveDevice::Unable to get foreign port" << endl;
    }

  clog << endl;
  /////////

  // while ((recvMsgSize = sock->recv(
  // buffer, FormatterActiveDevice::RCVBUFSIZE)) > 0)

  while (reading)
    {
      clog << "FormatterActiveDevice:: waiting" << endl;
      recvMsgSize = 0;
      valid_command = false;
      payload = "";
      buf = "";
      payload_size = 0;

      recvMsgSize = sock->recv (buffer, FormatterActiveDevice::RCVBUFSIZE);
      // TODO: improve read/write to buffer using a loop to assure it gets
      // at
      // least 100b
      // the above line is ok for usage over a local network

      // clog << "FormatterActiveDevice recv Msg" << buffer << endl;

      if (recvMsgSize <= 0)
        {
          clog << "FormatterActiveDevice: Lost connection to base device";
          clog << endl;
          clog << "FormatterActiveDevice: restart ginga as an active";
          clog << " device";
          clog << " again if you wish to search for a base device.";
          clog << endl;
          reading = false;

          // TODO: player->stop()? flag to define this?
          // TODO: only change image if nothing is playing?

          serialized = Ginga_Display->createWindow (0, 0, DV_QVGA_WIDTH,
                                         DV_QVGA_HEIGHT, -1.0);

          if (fileExists (img_reset))
            {
              s = Ginga_Display->createRenderedSurfaceFromImageFile (
                  img_reset.c_str ());

              int cap
                  = serialized->getCap ("ALPHACHANNEL");
              serialized->setCaps (cap);
              serialized->draw ();

              serialized->show ();
              serialized->renderFrom (s);
              serialized->lowerToBottom ();

              delete s;
              s = 0;
            }
          break;
        }

      // Splitting the two lines of a command (the second is optional)
      sec = strchr (buffer, '\n');
      pos = sec - buffer + 1;
      strncpy (pri, buffer, pos);
      pri[pos] = '\0'; // first line (command)

      ss << pri;

      payload_size = 0;
      tokens.reserve (5);

      while (ss >> buf)
        {
          tokens.push_back (buf);
        }

      // improve the quality of the code below
      // command with more than 5 tokens = error
      if ((tokens.size () > 5) || (tokens.size () < 4))
        {
          clog << "FormatterActiveDevice::received an invalid command";
          clog << endl;

          tokens.clear ();
          for (i = 0; i < RCVBUFSIZE + 1; i++)
            {
              buffer[i] = 0;
            }
          break;
        }

      // no payload
      if (tokens.size () == 4)
        {
          sid = string (tokens[0]);
          snpt = string (tokens[1]);
          scommand = string (tokens[2]);
          spayload_desc = string (tokens[3]);
          valid_command
              = handleTCPCommand (sid, snpt, scommand, spayload_desc, "");
        }
      else if (tokens.size () >= 5)
        {
          sid = string (tokens[0]);
          snpt = string (tokens[1]);
          scommand = string (tokens[2]);
          spayload_desc = string (tokens[3]);
          spayload_size = string (tokens[4]);

          payload_size = ::atoi (spayload_size.c_str ());

          clog << "FormatterActiveDevice::Payload size=";
          clog << payload_size << endl;

          // check all sizes
          // There is another line for the payload
          if (payload_size > 0)
            {
              int pri_len = (int)strlen (pri);
              // pri_len--;
              int diff = recvMsgSize - (pri_len);
              ++sec;
              ++sec;
              // TODO: fix the lines above (\n removal) with portable
              // solution

              sec[diff] = '\0'; // part of the payload (second line)

              if (payload_size > diff)
                {
                  int rest = ((payload_size - diff) + 1);
                  int buff_size = 1024;
                  int received = 0;
                  int par_rec;
                  char par_payload[1025]; // buff_size

                  payload.append (sec); // mounting payload
                  while (received < rest)
                    {
                      par_rec = sock->recv (par_payload, buff_size);
                      received = received + par_rec;
                      par_payload[par_rec] = '\0';
                      payload.append (par_payload, par_rec);
                    }
                  clog << "FormatterActiveDevice::received_size = ";
                  clog << received << endl;

                  //						rest_payload[rest-1]
                  //=
                  //'\0';
                  // payload.append(rest_payload);
                }
            }
          else
            {
              int pri_len = (int)strlen (pri);
              int diff = recvMsgSize - (pri_len);
              sec[diff] = '\0'; // part of the payload (second line)

              ++sec;
#if defined(_MSC_VER)
              // TODO: fix this
              ++sec;
#endif

              payload.append (sec);
            }

          // clog << "::payload = "<<payload << endl;
          //++sec;
          // clog << "::PAYLOAD: " << payload << endl; //sec = payload
          valid_command = handleTCPCommand (sid, snpt, scommand,
                                            spayload_desc, payload);
        }

      while (tokens.size () > 0)
        {
          tokens.pop_back ();
        }

      ss.flush ();
      ss.clear ();

      // payload.clear();

      for (i = 0; i < FormatterActiveDevice::RCVBUFSIZE + 1; i++)
        {
          buffer[i] = 0;
        }

      if (valid_command)
        {
          // sock->send("OK\n",3);
          clog << "FormatterActiveDevice::received a valid command";
          clog << endl;
        }
      else
        {
          // sock->send("NOK\n",4);
          clog << "FormatterActiveDevice::received an invalid command";
          clog << endl;
        }

    } // while
  delete sock;
}

GINGA_FORMATTER_END
