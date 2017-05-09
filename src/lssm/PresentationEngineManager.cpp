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

#include "mb/Display.h"
using namespace ::ginga::mb;


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
  this->paused = false;
  this->hasInteractivity = true;
  this->closed = false;
  this->hasTMPNotification = false;
  this->currentPrivateBaseId = -1;
  this->timeBaseProvider = NULL;
  privateBaseManager = new PrivateBaseManager ();

  ContentTypeManager::getInstance ()->setMimeFile (string (GINGA_DATADIR)
                                                   + "mimetypes.ini");
}

PresentationEngineManager::~PresentationEngineManager ()
{
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
PresentationEngineManager::close ()
{
  map<int, set<INCLPlayer *> *>::iterator i;
  closed = true;
  lock ();
  formattersToRelease.clear ();
  formatters.clear ();
  unlock ();
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

//--

gpointer
PresentationEngineManager::startPresentationThreadWrapper (gpointer data)
{
  g_assert_nonnull (data);
  ((PresentationEngineManager *) data)->startPresentationThread();
  return NULL;
}

void
PresentationEngineManager::startPresentationThread (){
   
   INCLPlayer *formatter;

  clog << "PresentationEngineManager::startPresentation" << endl;
  if (hasInteractivity)
    {
      if (!getNclPlayer (nclFile, &formatter))
        {
          clog << "PresentationEngineManager::startPresentation ";
          clog << "can't start! formatter for '" << nclFile;
          clog << "' is NULL";
          clog << endl;
          return ;
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
      return ;
    }

  return ;

}

bool
PresentationEngineManager::startPresentation (const string &nclFile,
                                              const string &interfId)
{
   this->nclFile = nclFile;
   this->interfId = interfId;

   g_thread_new ("startPresentation", startPresentationThreadWrapper, this);

   return TRUE;

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

void *
PresentationEngineManager::getDsmccListener ()
{
  return this->dsmccListener;
}

void
PresentationEngineManager::pausePressed ()
{
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
PresentationEngineManager::waitUnlockCondition ()
{
  Thread::waitForUnlockCondition ();
}

void
PresentationEngineManager::presentationCompleted (const string &formatterId)
{
  releaseFormatter (formatterId);
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

    default:
      break;
    }
}

void *
PresentationEngineManager::eventReceived (void *ptr)
{
  struct inputEventNotification *ev;
  string parameter;

  ev = (struct inputEventNotification *)ptr;

  const int code = ev->code;
  parameter = ev->parameter;

  clog << "PresentationEngineManager::eventReceived '";
  clog << code << "'" << endl;

  delete (struct inputEventNotification *)ptr;

  return NULL;
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
PresentationEngineManager::updateFormatters (short command)
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

        case UC_PAUSE:
          paused = true;
          formatter->pause ();
          break;

        case UC_RESUME:
          paused = false;
          formatter->resume ();
          break;

        default:
          break;
        }
      ++i;
    }

  if (command == UC_STOP)
    {
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
