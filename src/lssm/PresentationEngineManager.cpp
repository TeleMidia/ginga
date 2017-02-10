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

#include "player/ProgramAV.h"
#include "player/IProgramAV.h"
#include "player/IApplicationPlayer.h"

#if WITH_ISDBT
#include "DataWrapperListener.h"
#include "isdbt-dataproc/NCLEventDescriptor.h"
#include "isdbt-tuner/Tuner.h"
using namespace ::ginga::dataproc;
using namespace ::ginga::tuner;
#endif

#include "formatter/FormatterMediator.h"
using namespace ::ginga::formatter;

#include "ncl/LayoutRegion.h"
#include "ncl/ContentTypeManager.h"
#include "ncl/NclDocument.h"
using namespace ::ginga::ncl;

#include "mb/Key.h"
#include "mb/Display.h"
#include "mb/InputManager.h"
using namespace ::ginga::mb;

#include "player/ShowButton.h"
using namespace ::ginga::player;

#include "system/GingaLocatorFactory.h"
using namespace ::ginga::system;

#include "util/functions.h"
using namespace ::ginga::util;

GINGA_LSSM_BEGIN

struct inputEventNotification
{
  PresentationEngineManager *p;
  int code;
  string parameter;
#if WITH_ISDBT
  Tuner *tuner;
#endif
  vector<string> *cmds;
};

bool PresentationEngineManager::autoProcess = false;

PresentationEngineManager::PresentationEngineManager (
    int devClass, int xOffset, int yOffset, int width, int height,
    bool enableGfx, bool useMulticast)
    : Thread ()
{
  g_mutex_init (&this->quit_mutex);
  g_cond_init (&this->quit_cond);

  x = 0;
  if (xOffset > 0)
    {
      x = xOffset;
    }

  y = 0;
  if (yOffset > 0)
    {
      y = yOffset;
    }

  enableMulticast = useMulticast;

  Ginga_Display->getSize (&w, &h);
  if (width > 0 && (width < w || w == 0))
    {
      w = width;
    }
  if (height > 0 && (height < h || h == 0))
    {
      h = height;
    }

  if (h == 0 || w == 0)
    {
      clog << "PEM::PEM Warning! Trying to create an ";
      clog << "invalid window" << endl;
      w = 1;
      h = 1;
    }

  this->devClass = devClass;
  this->enableGfx = enableGfx;
  this->dsmccListener = NULL;
  this->tuner = NULL;

#ifdef DataWrapperListener_H_
  this->dsmccListener = new DataWrapperListener (this);
#endif

  this->exitOnEnd = false;
  this->disableFKeys = false;

  this->paused = false;
  this->standAloneApp = true;
  this->isLocalNcl = true;
  this->hasInteractivity = true;
  this->closed = false;
  this->hasTMPNotification = false;
  this->isEmbedded = true;
  this->currentPrivateBaseId = -1;
  this->timeBaseProvider = NULL;
  this->im = Ginga_Display->getInputManager ();
  privateBaseManager = new PrivateBaseManager ();
  this->sb = new ShowButton ();

  ContentTypeManager::getInstance ()->setMimeFile (string (GINGA_DATADIR)
                                                   + "mimetypes.ini");
  im->setCommandEventListener (this);
}

PresentationEngineManager::~PresentationEngineManager ()
{
  clog << "PresentationEngineManager::~PresentationEngineManager" << endl;

  quit = false;
  g_mutex_clear (&this->quit_mutex);
  g_cond_clear (&this->quit_cond);

  if (!closed)
    {
      clog << "PresentationEngineManager::~PresentationEngineManager";
      clog << " closing" << endl;
      this->close ();
    }

  lock ();
  while (!formattersToRelease.empty ())
    {
      unlock ();
      g_usleep (1000);
      lock ();
    }
  unlock ();

  clog << "PresentationEngineManager::~PresentationEngineManager";
  clog << " all done" << endl;
}

void
PresentationEngineManager::setExitOnEnd (bool exitOnEnd)
{
  this->exitOnEnd = exitOnEnd;
}

void
PresentationEngineManager::setDisableFKeys (bool disableFKeys)
{
  this->disableFKeys = disableFKeys;
}

set<string> *
PresentationEngineManager::createPortIdList (const string &nclFile)
{
  INCLPlayer *formatter;
  set<string> *portIds = NULL;

  if (getNclPlayer (nclFile, &formatter))
    {
      portIds = formatter->createPortIdList ();
    }

  return portIds;
}

short
PresentationEngineManager::getMappedInterfaceType (const string &nclFile,
                                                   const string &portId)
{
  INCLPlayer *formatter;
  short miType = -1;

  if (getNclPlayer (nclFile, &formatter))
    {
      miType = formatter->getMappedInterfaceType (portId);
    }
  else
    {
      clog << "PresentationEngineManager::getMappedInterfaceType ";
      clog << "Can't find NCL Player for '" << nclFile << "'" << endl;
    }

  return miType;
}

void
PresentationEngineManager::autoMountOC (arg_unused (bool autoMountIt))
{
#ifdef WITH_ISDBT
  ((DataWrapperListener *)dsmccListener)->autoMountOC (autoMountIt);
#endif
}

void
PresentationEngineManager::setCurrentPrivateBaseId (unsigned int baseId)
{
  clog << "PresentationEngineManager::setCurrentPrivateBaseId '";
  clog << baseId << "'";
  clog << endl;
  this->currentPrivateBaseId = (int)baseId;
}

void
PresentationEngineManager::setTimeBaseProvider (ITimeBaseProvider *tmp)
{
  timeBaseProvider = tmp;
  hasTMPNotification = true;
}

void
PresentationEngineManager::setTimeBaseInfo (INCLPlayer *p)
{
  if (timeBaseProvider != NULL)
    {
      p->setTimeBaseProvider (timeBaseProvider);
    }
}

void
PresentationEngineManager::printGingaWindows ()
{
  map<string, INCLPlayer *>::iterator i;
  INCLPlayer *formatter;

  cout << "PresentationEngineManager::printGingaWindows" << endl;
  lock ();

  i = formatters.begin ();
  while (i != formatters.end ())
    {
      formatter = i->second;
      formatter->printGingaWindows ();
      ++i;
    }

  unlock ();
}

bool
PresentationEngineManager::nclEdit (const string &nclEditApi)
{
  string::size_type pos = nclEditApi.find_first_of (",");
  string commandTag = xstrchomp (nclEditApi.substr (0, pos));

  return editingCommand (
      commandTag,
      nclEditApi.substr (pos + 1, nclEditApi.length () - pos + 1));
}

bool
PresentationEngineManager::editingCommand (const string &commandTag,
                                           const string &commandPayload)
{
  vector<string> *args;
  vector<string>::iterator i;
  GingaLocatorFactory *glf = NULL;
  INCLPlayer *docPlayer = NULL;
  string baseId, docId;
  string docIor, docUri, arg, uri, ior, uName;
  glf = GingaLocatorFactory::getInstance ();

#if WITH_ISDBT
  args = split (commandPayload, ",");
  i = args->begin ();
  baseId = NCLEventDescriptor::extractMarks (*i);
  ++i;

  // parse command
  if (commandTag == EC_OPEN_BASE)
    {
    }
  else if (commandTag == EC_ACTIVATE_BASE)
    {
    }
  else if (commandTag == EC_DEACTIVATE_BASE)
    {
    }
  else if (commandTag == EC_SAVE_BASE)
    {
    }
  else if (commandTag == EC_CLOSE_BASE)
    {
    }
  else if (commandTag == EC_ADD_DOCUMENT)
    {
      clog << "PresentationEngineManager::editingCommand (addDocument)";
      clog << endl;

      docUri = "";
      docIor = "";
      while (i != args->end ())
        {
          if ((*i).find ("x-sbtvd://") != std::string::npos)
            {
              uri = NCLEventDescriptor::extractMarks (*i);
              ++i;
              ior = NCLEventDescriptor::extractMarks (*i);

              if (uri.find ("x-sbtvd://") != std::string::npos)
                {
                  uri = uri.substr (uri.find ("x-sbtvd://") + 10,
                                    uri.length ()
                                        - (uri.find ("x-sbtvd://") + 10));
                }

              if (docUri == "")
                {
                  docUri = uri;
                  docIor = ior;
                }

              clog << "PresentationEngineManager::editingCommand ";
              clog << " command '" << arg << "' creating locator ";
              clog << "uri '" << uri << "', ior '" << ior;
              clog << "'" << endl;
              if (glf != NULL)
                {
                  glf->createLocator (uri, ior);
                }
            }
          else
            {
              uri = *i;
              ior = uri;
              if (docUri == "")
                {
                  docIor = uri;
                  docUri = uri;
                }
            }

          ++i;
        }

      if (docUri == docIor)
        {
          clog << "PresentationEngineManager::editingCommand";
          clog << " calling addDocument '" << docUri;
          clog << "' in private base '" << baseId;
          clog << endl;

          lock ();
          docPlayer = createNclPlayer (baseId, docUri);
          unlock ();
        }
      else
        {
          clog << "PresentationEngineManager::editingCommand calling ";
          clog << "getLocation '" << docUri << "' for ior '";
          clog << docIor << "'" << endl;

          if (glf != NULL)
            {
              uri = glf->getLocation (docUri);
              uName = glf->getName (docIor);
              lock ();
              docPlayer = createNclPlayer (baseId, uri + uName);
              unlock ();
            }
        }
    }
  else if (commandTag == EC_DBG_START_PRESENTATION)
    {
      startPresentation (commandPayload, "");
    }
  else
    {
      // clog << "PresentationEngineManager::editingCommand not to base";
      // clog << endl;

      docId = NCLEventDescriptor::extractMarks (*i);
      if (getNclPlayer (baseId, docId, &docPlayer))
        {
          if (commandTag == EC_START_DOCUMENT)
            {
              setTimeBaseInfo (docPlayer);
            }
          return docPlayer->editingCommand (commandTag, commandPayload);
        }
      else
        {
          clog << "PresentationEngineManager::editingCommand can't ";
          clog << "find NCL player in base '" << baseId << "' for doc '";
          clog << docId << "'";
          clog << endl;
        }
    }

#endif // WITH_ISDBT

  return false;
}

bool
PresentationEngineManager::editingCommand (const string &editingCmd)
{
  string commandTag = "";
  string commandPayload = "";

#if WITH_ISDBT
  commandTag = NCLEventDescriptor::getCommandTag (editingCmd);
  commandPayload = NCLEventDescriptor::getPrivateDataPayload (editingCmd);
#endif

  return editingCommand (commandTag, commandPayload);
}

void
PresentationEngineManager::getScreenShot ()
{
  updateFormatters (UC_PRINTSCREEN);
}

void
PresentationEngineManager::close ()
{
  map<int, set<INCLPlayer *> *>::iterator i;

  closed = true;

  if (im != NULL)
    {
      im->removeInputEventListener (this);
      im->setCommandEventListener (NULL);
      delete im;
      im = NULL;
    }

  lock ();
  formattersToRelease.clear ();
  formatters.clear ();

  if (sb != NULL)
    {
      delete sb;
      sb = NULL;
    }
  unlock ();
}

void
PresentationEngineManager::registerKeys ()
{
  set<int> *keys;

  if (im == NULL)
    {
      return;
    }

  keys = new set<int>;

  keys->insert (Key::KEY_GREATER_THAN_SIGN);
  keys->insert (Key::KEY_LESS_THAN_SIGN);

  keys->insert (Key::KEY_SUPER);
  keys->insert (Key::KEY_PRINTSCREEN);

  keys->insert (Key::KEY_F10);
  keys->insert (Key::KEY_POWER);

  keys->insert (Key::KEY_F11);
  keys->insert (Key::KEY_STOP);

  keys->insert (Key::KEY_F12);
  keys->insert (Key::KEY_PAUSE);

#if WITH_ISDBT
  this->tuner = NULL;
  keys->insert (Key::KEY_PAGE_UP);
  keys->insert (Key::KEY_PAGE_DOWN);
  keys->insert (Key::KEY_CHANNEL_UP);
  keys->insert (Key::KEY_CHANNEL_DOWN);
#endif

  if (!commands.empty ())
    {
      keys->insert (Key::KEY_PLUS_SIGN);
    }

  im->addInputEventListener (this, keys);
}

bool
PresentationEngineManager::getIsLocalNcl ()
{
  return this->isLocalNcl;
}

void
PresentationEngineManager::setEmbedApp (bool isEmbedded)
{
  this->isEmbedded = isEmbedded;

  if (!isEmbedded)
    {
      registerKeys ();
    }
  else
    {
      im->removeInputEventListener (this);
    }
}

void
PresentationEngineManager::setIsLocalNcl (bool isLocal, void *tuner)
{
  if (this->tuner != NULL && this->tuner != tuner)
    {
#if WITH_ISDBT
      delete (Tuner *)(this->tuner);
#endif
    }

  this->tuner = tuner;
  this->isLocalNcl = isLocal;
}

void
PresentationEngineManager::setInteractivityInfo (bool hasInt)
{
  this->hasInteractivity = hasInt;
}

INCLPlayer *
PresentationEngineManager::createNclPlayer (const string &baseId,
                                            const string &fname)
{
  NclPlayerData *data = NULL;
  INCLPlayer *formatter = NULL;

  if (formatters.find (fname) != formatters.end ())
    {
      formatter = formatters[fname];
    }
  else
    {
      data = createNclPlayerData ();
      data->baseId = baseId;
      data->playerId = fname;
      data->privateBaseManager = privateBaseManager;
      data->enableMulticast = enableMulticast;
      formatter = new FormatterMediator (data);
      formatter->setCurrentDocument (fname);
      if (formatters.empty () && !isEmbedded)
        {
          registerKeys ();
        }

      formatter->addListener (this);
      formatters[fname] = formatter;
    }

  return formatter;
}

NclPlayerData *
PresentationEngineManager::createNclPlayerData ()
{
  NclPlayerData *data = NULL;

  data = new NclPlayerData;
  data->baseId = "";
  data->playerId = "";
  data->devClass = devClass;
  data->x = x;
  data->y = y;
  data->w = w;
  data->h = h;
  data->enableGfx = enableGfx;
  data->enableMulticast = enableMulticast;
  data->parentDocId = "";
  data->nodeId = "";
  data->docId = "";
  data->transparency = 0;
  data->focusManager = NULL;
  data->privateBaseManager = NULL;
  data->editListener = (IPlayerListener *)this;

  return data;
}

void
PresentationEngineManager::addPlayerListener (const string &nclFile,
                                              IPlayerListener *listener)
{
  map<string, INCLPlayer *>::iterator i;
  INCLPlayer *nclPlayer;

  clog << "PresentationEngineManager::addListener" << endl;

  lock ();
  i = formatters.find (nclFile);
  if (i != formatters.end ())
    {
      nclPlayer = i->second;

      nclPlayer->addListener (listener);
    }
  unlock ();
}

void
PresentationEngineManager::removePlayerListener (const string &nclFile,
                                                 IPlayerListener *listener)
{
  map<string, INCLPlayer *>::iterator i;
  INCLPlayer *nclPlayer;

  clog << "PresentationEngineManager::removeListener" << endl;

  lock ();
  i = formatters.find (nclFile);
  if (i != formatters.end ())
    {
      nclPlayer = i->second;

      nclPlayer->removeListener (listener);
    }
  unlock ();
}

bool
PresentationEngineManager::openNclFile (const string &fname)
{
  INCLPlayer *formatter;

  lock ();
  if (formatters.find (fname) != formatters.end ())
    {
      clog << "PresentationEngineManager::openNclFile";
      clog << " Warning! Trying to open the same NCL twice!";
      clog << endl;
      unlock ();
      return false;
    }

  formatter = createNclPlayer (xstrbuild ("%d", currentPrivateBaseId), fname);
  unlock ();

  return (formatter != NULL);
}

bool
PresentationEngineManager::startPresentation (const string &nclFile,
                                              const string &interfId)
{
  INCLPlayer *formatter;

  clog << "PresentationEngineManager::startPresentation" << endl;
  if (hasInteractivity)
    {
      if (!getNclPlayer (nclFile, &formatter))
        {
          openNclFile (nclFile);
          if (!getNclPlayer (nclFile, &formatter))
            {
              clog << "PresentationEngineManager::startPresentation ";
              clog << "can't start! formatter for '" << nclFile;
              clog << "' is NULL";
              clog << endl;
              return false;
            }
        }

      setTimeBaseInfo (formatter);

      clog << "PresentationEngineManager::startPresentation play '";
      clog << nclFile << "'";

      if (interfId != "")
        {
          clog << ", interfaceId = '" << interfId << "'";
          formatter->setCurrentScope (interfId);
        }

      clog << endl;
      formatter->play ();
    }
  else
    {
      clog << "PresentationEngineManager::startPresentation with ";
      clog << "disable-interactivity option" << endl;
      return false;
    }

  return true;
}

bool
PresentationEngineManager::stopPresentation (const string &nclFile)
{
  clog << "PresentationEngineManager::stopPresentation" << endl;
  map<string, INCLPlayer *>::iterator i;
  INCLPlayer *nclPlayer;
  bool stopped = false;

  lock ();
  i = formatters.find (nclFile);
  if (i != formatters.end ())
    {
      nclPlayer = i->second;
      formatters.erase (i);

      nclPlayer->removeListener (this);
      nclPlayer->stop ();

      clog << "PresentationEngineManager::stopPresentation deleting ";
      clog << "ncl player";
      clog << endl;
      delete nclPlayer;
      stopped = true;
    }

  unlock ();
  clog << "PresentationEngineManager::stopPresentation ALL DONE";
  clog << endl;
  return stopped;
}

bool
PresentationEngineManager::setPropertyValue (const string &nclFile,
                                             const string &interfaceId,
                                             const string &value)
{
  INCLPlayer *formatter;

  if (getNclPlayer (nclFile, &formatter))
    {
      formatter->setPropertyValue (interfaceId, value);
      return true;
    }

  return false;
}

string
PresentationEngineManager::getPropertyValue (const string &nclFile,
                                             const string &interfaceId)
{
  INCLPlayer *formatter;

  if (getNclPlayer (nclFile, &formatter))
    {
      return formatter->getPropertyValue (interfaceId);
    }

  return "";
}

bool
PresentationEngineManager::stopAllPresentations ()
{
  INCLPlayer *formatter;
  map<string, INCLPlayer *>::iterator i;

  clog << "PresentationEngineManager::stopAllPresentations" << endl;
  lock ();
  i = formatters.begin ();
  while (i != formatters.end ())
    {
      formatter = i->second;
      formatter->removeListener (this);
      formatter->stop ();
      ++i;
    }

  i = formatters.begin ();
  while (i != formatters.end ())
    {
      formatter = i->second;
      formatters.erase (i);
      delete formatter;
      i = formatters.begin ();
    }
  unlock ();

  return true;
}

bool
PresentationEngineManager::pausePresentation (const string &nclFile)
{
  clog << "PresentationEngineManager::pausePresentation" << endl;

  INCLPlayer *formatter;

  if (getNclPlayer (nclFile, &formatter))
    {
      formatter->pause ();
      return true;
    }

  clog << "PresentationEngineManager::pausePresentation can't";
  clog << " pause! formatter for '" << nclFile << "' is NULL";
  clog << endl;

  return false;
}

bool
PresentationEngineManager::resumePresentation (const string &nclFile)
{
  clog << "PresentationEngineManager::resumePresentation" << endl;
  INCLPlayer *formatter;

  if (getNclPlayer (nclFile, &formatter))
    {
      formatter->resume ();
      return true;
    }

  clog << "PresentationEngineManager::resumePresentation can't";
  clog << " resume! formatter for '" << nclFile << "' is NULL";
  clog << endl;

  return false;
}

bool
PresentationEngineManager::abortPresentation (const string &nclFile)
{
  clog << "PresentationEngineManager::stopPresentation" << endl;
  map<string, INCLPlayer *>::iterator i;
  INCLPlayer *nclPlayer;
  bool aborted = false;

  lock ();
  i = formatters.find (nclFile);
  if (i != formatters.end ())
    {
      nclPlayer = i->second;
      formatters.erase (i);

      nclPlayer->removeListener (this);
      nclPlayer->abort ();

      delete nclPlayer;
      aborted = true;
    }

  unlock ();
  return aborted;
}

void
PresentationEngineManager::openNclDocument (const string &docUri,
                                            int x, int y,
                                            int w, int h)
{
  clog << "PresentationEngineManager::openNclDocument docUri '";
  clog << docUri << "' x = '" << x << "', y = '" << y << "', w = '";
  clog << w << "', h = '" << h << "'" << endl;
}

void *
PresentationEngineManager::getDsmccListener ()
{
  return this->dsmccListener;
}

void
PresentationEngineManager::pausePressed ()
{
  clog << "PresentationEngineManager::pausePressed" << endl;
  if (paused)
    {
      updateFormatters (UC_RESUME);
    }
  else
    {
      updateFormatters (UC_PAUSE);
    }
}

void
PresentationEngineManager::setCmdFile (const string &cmdFile)
{
  ifstream fis;
  string cmd;

  clog << "PresentationEngineManager";
  clog << "::setCmdFile" << endl;

  autoProcess = false;

  fis.open (cmdFile.c_str (), ifstream::in);
  if (!fis.is_open ())
    {
      clog << "PresentationEngineManager";
      clog << "::setCmdFile Warning! can't open '" << cmdFile;
      clog << "'" << endl;
      return;
    }

  commands.clear ();

  while (fis.good ())
    {
      fis >> cmd;
      if (cmd != "" && cmd.substr (0, 1) != "#")
        {
          commands.push_back (cmd);
          if (cmd.find ("startPresentation") != std::string::npos)
            {
              autoProcess = true;
              break;
            }
        }
    }

  if (autoProcess)
    {
      pthread_t autoCmdId_;
      struct inputEventNotification *ev;

      ev = new struct inputEventNotification;
      ev->p = this;
      ev->cmds = new vector<string> (commands);

      pthread_create (&autoCmdId_, 0,
                      PresentationEngineManager::processAutoCmd,
                      (void *)ev);

      pthread_detach (autoCmdId_);
    }
}

void *
PresentationEngineManager::processAutoCmd (void *ptr)
{
  struct inputEventNotification *ev;
  PresentationEngineManager *p;
  string cmd;
  vector<string> *cmds;

  ev = (struct inputEventNotification *)ptr;
  p = ev->p;
  cmds = ev->cmds;
  delete ev;

  clog << "PresentationEngineManager::processAutoCmd" << endl;

  while (!cmds->empty ())
    {
      cmd = *(cmds->begin ());

      clog << "PresentationEngineManager::processAutoCmd ";
      clog << "RUNNING CURRENT COMMAND '" << cmd;
      clog << "'" << endl;

      p->readCommand (cmd);
      if (cmd.find ("startPresentation") != std::string::npos)
        {
          break;
        }
      cmds->erase (cmds->begin ());
    }

  delete cmds;
  clog << "PresentationEngineManager::processAutoCmd ";
  clog << "ALL DONE" << endl;
  return NULL;
}

void
PresentationEngineManager::waitUnlockCondition ()
{
  clog << "PresentationEngineManager::waitUnlockCondition" << endl;
  Thread::waitForUnlockCondition ();
}

void
PresentationEngineManager::presentationCompleted (const string &formatterId)
{
  clog << "PresentationEngineManager";
  clog << "::presentationCompleted for '" << formatterId;
  clog << "'" << endl;

  if (!isEmbedded)
    {
      releaseFormatter (formatterId);
    }
}

void
PresentationEngineManager::releaseFormatter (const string &formatterId)
{
  map<string, INCLPlayer *>::iterator i;
  INCLPlayer *formatter;

  clog << "PresentationEngineManager::releaseFormatter" << endl;
  lock ();
  if (formatterId == "")
    {
      i = formatters.begin ();
    }
  else
    {
      i = formatters.find (formatterId);
    }

  if (i != formatters.end ())
    {
      formatter = i->second;
      formatters.erase (i);
      unlock ();

      checkStatus ();

      lock ();
      formattersToRelease.insert (formatter);
      unlock ();
      Thread::startThread ();
    }
  else
    {
      unlock ();
      checkStatus ();
    }
}

bool
PresentationEngineManager::checkStatus ()
{
  bool checked = false;

  lock ();
  if (formatters.empty () && isLocalNcl)
    {
      if (sb != NULL)
        {
          sb->stop ();
        }

      g_usleep (50000);

      checked = true;
    }
  unlock ();

  if (checked)
    {
      if (exitOnEnd)
        {
          clog << "PresentationEngineManager::checkStatus: exit" << endl;
          exit (0);
        }

      clog << "PresentationEngineManager::checkStatus: closing" << endl;
      close ();
      g_usleep (600000);
      unlockConditionSatisfied ();
    }
  else
    {
      clog << "PresentationEngineManager::checkStatus: running" << endl;
    }

  return checked;
}

void
PresentationEngineManager::updateStatus (short code,
                                         const string &parameter,
                                         short type,
                                         arg_unused (const string &value))
{
  struct inputEventNotification *ev;

  switch (code)
    {
    case IPlayer::PL_NOTIFY_STOP:
      if (type == IPlayer::TYPE_PRESENTATION)
        {
          ev = new struct inputEventNotification;

          ev->p = this;
          ev->parameter = parameter;
          ev->code = code;

          pthread_t notifyThreadId_;
          pthread_create (&notifyThreadId_, 0,
                          PresentationEngineManager::eventReceived,
                          (void *)ev);

          pthread_detach (notifyThreadId_);
        }
      break;

    case IPlayer::PL_NOTIFY_NCLEDIT:
      clog << "PresentationEngineManager::updateStatus";
      clog << " NCLEDIT";
      clog << endl;

      nclEdit (parameter);
      break;

    default:
      break;
    }
}

bool
PresentationEngineManager::userEventReceived (InputEvent *ev)
{
  struct inputEventNotification *evR;
  int keyCode;

  keyCode = ev->getKeyCode ();

  clog << "PresentationEngineManager::userEventReceived" << endl;
  evR = new struct inputEventNotification;
  evR->p = this;
  evR->parameter = "";
  evR->code = keyCode;
  if (!commands.empty ())
    {
      evR->cmds = new vector<string> (commands);
    }
  else
    {
      evR->cmds = NULL;
    }

#if 0
  pthread_t notifyThreadId_;
  pthread_create (&notifyThreadId_, 0,
                  PresentationEngineManager::eventReceived, (void *)evR);

  pthread_detach (notifyThreadId_);
#endif
  PresentationEngineManager::eventReceived ((void *)evR);
  return true;
}

bool
PresentationEngineManager::cmdEventReceived (const string &command,
                                             const string &args)
{
  size_t token;
  string nCmd;
  string nArgs;

  clog << "PresentationEngineManager::cmdEventReceived" << endl;
  clog << "Command: " << command << endl;
  clog << "Parameters: " << args << endl;
  token = args.find_first_of (",");
  if (token == std::string::npos)
    {
      nCmd = args;
      nArgs = "";
    }
  else
    {
      nCmd = args.substr (0, token);
      nArgs = args.substr (token + 1, args.length () - (token + 1));
    }

  if (command == "start")
    {
      if (openNclFile (nCmd))
        {
          startPresentation (nCmd, nArgs);
        }
    }
  else if (command == "ncledit")
    {
      editingCommand (args);
    }
  return true;
}

void *
PresentationEngineManager::eventReceived (void *ptr)
{
  struct inputEventNotification *ev;
  PresentationEngineManager *p;
  string parameter;
  vector<string> *cmds;

  ev = (struct inputEventNotification *)ptr;

  const int code = ev->code;
  p = ev->p;
  parameter = ev->parameter;
  cmds = ev->cmds;

  clog << "PresentationEngineManager::eventReceived '";
  clog << code << "'" << endl;

  delete (struct inputEventNotification *)ptr;

  if (code == Key::KEY_QUIT)
    {
      cout << "PresentationEngineManager::eventReceived QUIT" << endl;

      //p->sb->stop (); show button
      p->setIsLocalNcl (true, NULL);
      p->stopAllPresentations ();
      g_mutex_lock (&p->quit_mutex);
      p->quit = true;
      g_cond_signal (&p->quit_cond);
      g_mutex_unlock (&p->quit_mutex);
    }
  else if (parameter != "" && code == IPlayer::PL_NOTIFY_STOP)
    {
      clog << "PresentationEngineManager::eventReceived: NOTIFY_STOP";
      clog << endl;

      g_usleep (100000);
      p->presentationCompleted (parameter);
    }
  else if (!p->disableFKeys
           && (code == Key::KEY_POWER || code == Key::KEY_F10
               || code == Key::KEY_STOP || code == Key::KEY_F11))
    {
      clog << "PresentationEngineManager::eventReceived: POWER_OFF";
      clog << endl;

      p->sb->stop ();
      g_usleep (500000);

      p->setIsLocalNcl (true, NULL);
      p->stopAllPresentations ();
      p->close ();
      g_usleep (500000);
      p->unlockConditionSatisfied ();
    }
  else if (code == Key::KEY_PRINTSCREEN || code == Key::KEY_SUPER)
    {
      p->getScreenShot ();
    }
  else if (!p->disableFKeys
           && (code == Key::KEY_PAUSE || code == Key::KEY_F12))
    {
      clog << "PresentationEngineManager::eventReceived: PAUSE";
      clog << endl;

      p->sb->pause ();
      p->pausePressed ();
    }
  else if (code == Key::KEY_PLUS_SIGN && cmds != NULL)
    {
      if (!cmds->empty () && !autoProcess)
        {
          string cmd = *(cmds->begin ());
          clog << "RUNNING CURRENT COMMAND '" << cmd;
          clog << "'" << endl;

          p->readCommand (cmd);
          cmds->erase (cmds->begin ());
          cmds->push_back (cmd);
        }
      delete cmds;
    }
  else if (code == Key::KEY_GREATER_THAN_SIGN)
    {
      clog << ">> TIME SHIFT >>" << endl;
      p->updateFormatters (UC_SHIFT, "forward");
    }
  else if (code == Key::KEY_LESS_THAN_SIGN)
    {
      clog << "<< TIME SHIFT <<" << endl;
      p->updateFormatters (UC_SHIFT, "backward");
    }
  else if (code == Key::KEY_SMALL_W)
    {
      cout << "<< GINGA WINDOW DEBUG <<" << endl;
      p->printGingaWindows ();
    }

  return NULL;
}

void
PresentationEngineManager::readCommand (const string &command)
{
  string cmdTag = "", cmdParams = "", editingCmd = "", cmdHeader = "";
  vector<string> *params;
  double delay;

  clog << "PresentationEngineManager::readCommand";
  clog << endl;

  if (command.find ("startPresentation") != std::string::npos)
    {
      cmdParams = command.substr (command.find_first_of ("(") + 1,
                                  command.length ()
                                      - (command.find_first_of ("(") + 1));

      cmdParams = cmdParams.substr (0, cmdParams.find_last_of (")"));

#if WITH_ISDBT
      clog << "PresentationEngineManager::readCommand checking tuner ...";
      clog << endl;

      while (!hasTMPNotification)
        {
          g_usleep (1000000);
        }

      clog << "PresentationEngineManager::readCommand tuner OK";
      clog << endl;
#endif

      params = split (cmdParams, ",");
      if (params->size () == 1)
        {
          editingCmd = (*params)[0];
        }
      else if (params->size () == 2)
        {
          delay = xstrtod (((*params)[0]));
          if (delay > 0)
            {
              Thread::mSleep ((long int) delay);
            }
          editingCmd = (*params)[1];
        }
      delete params;
      if (editingCmd != "")
        {
          startPresentation (editingCmd, "");
        }
    }
  else if (command.find ("addDocument") != std::string::npos)
    {
      cmdTag = "5";
    }
  else if (command.find ("startDocument") != std::string::npos)
    {
      cmdTag = "7";
    }

  if (cmdTag != "" && command.find ("(") != std::string::npos)
    {
      cmdParams = command.substr (command.find_first_of ("(") + 1,
                                  command.length ()
                                      - (command.find_first_of ("(") + 1));

      if (cmdParams.find (")") != std::string::npos)
        {
          cmdParams = cmdParams.substr (0, cmdParams.find_last_of (")"));
          editingCmd = cmdTag + "1" + cmdParams;
          cmdHeader = "02000000000";
          // cmdHeader[10] = editingCmd.length ();
          editingCommand (cmdHeader + editingCmd);
        }
    }
}

bool
PresentationEngineManager::getNclPlayer (const string &docLocation,
                                         INCLPlayer **nclPlayer)
{
  map<string, INCLPlayer *>::iterator i;

  lock ();
  i = formatters.find (docLocation);
  if (i != formatters.end ())
    {
      *nclPlayer = i->second;
      unlock ();

      return true;
    }
  unlock ();

  clog << "PresentationEngineManager::getNclPlayer could not find '";
  clog << docLocation << "'" << endl;
  return false;
}

bool
PresentationEngineManager::getNclPlayer (const string &baseId,
                                         const string &docId,
                                         INCLPlayer **nclPlayer)
{
  string docLocation;

  docLocation = privateBaseManager->getDocumentLocation (baseId, docId);
  return getNclPlayer (docLocation, nclPlayer);
}

void
PresentationEngineManager::updateFormatters (short command,
                                             const string &parameter)
{
  map<string, INCLPlayer *>::iterator i;
  INCLPlayer *formatter;
  clog << "PresentationEngineManager::updateFormatters" << endl;
  lock ();
  i = formatters.begin ();
  while (i != formatters.end ())
    {
      formatter = i->second;
      switch (command)
        {
        case UC_STOP:
          formatter->removeListener (this);
          formatter->stop ();
          formattersToRelease.insert (formatter);
          break;

        case UC_PRINTSCREEN:
          clog << "PRINTSCREEN" << endl;
          formatter->getScreenShot ();
          unlock ();
          return;

        case UC_PAUSE:
          paused = true;
          formatter->pause ();
          break;

        case UC_RESUME:
          paused = false;
          formatter->resume ();
          break;

        case UC_SHIFT:
          clog << "PresentationEngineManager::updateFormatters";
          clog << " shifting time" << endl;
          formatter->timeShift (parameter);
          unlock ();
          return;

        default:
          break;
        }
      ++i;
    }

  if (command == UC_STOP)
    {
      clog << "PresentationEngineManager::updateFormatters";
      clog << " UC_STOP" << endl;
      formatters.clear ();
      Thread::startThread ();
    }

  unlock ();
}

void
PresentationEngineManager::run ()
{
  INCLPlayer *formatter;
  set<INCLPlayer *>::iterator i;

  clog << "PresentationEngineManager::run" << endl;
  lock ();
  if (!formattersToRelease.empty ())
    {
      i = formattersToRelease.begin ();
      while (i != formattersToRelease.end ())
        {
          formatter = *i;
          formattersToRelease.erase (i);

          delete formatter;
          i = formattersToRelease.begin ();
        }
      formattersToRelease.clear ();
    }
  unlock ();
  clog << "PresentationEngineManager::run all done" << endl;
}

GINGA_LSSM_END
