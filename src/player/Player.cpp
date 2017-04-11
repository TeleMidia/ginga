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
#include "Player.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

#include "util/functions.h"
using namespace ::ginga::util;

GINGA_PLAYER_BEGIN

Player::Player (const string &mrl)
{
  Thread::mutexInit (&listM, false);
  Thread::mutexInit (&lockedListM, false);
  Thread::mutexInit (&referM, false);
  Thread::mutexInit (&pnMutex, false);

  this->mrl = mrl;
  this->outputWindow = 0;
  this->window = NULL;
  this->notifying = false;
  this->presented = false;
  this->visible = true;
  this->immediatelyStartVar = false;
  this->status = SLEEPING;
  this->forcedNaturalEnd = false;
  this->scope = "";
  this->scopeType = -1;
  this->scopeInitTime = -1;
  this->scopeEndTime = -1;
  this->outTransTime = -1;
  this->notifyContentUpdate = false;
  this->mirrorSrc = NULL;

  this->initStartTime = 0;
  this->initPauseTime = 0;
  this->accTimePlaying = 0;
  this->accTimePaused = 0; 

  
}

Player::~Player ()
{

  set<IPlayer *>::iterator i;

  this->status = SLEEPING;

  Thread::mutexLock (&listM);
  listeners.clear ();

  Thread::mutexLock (&lockedListM);
  lockedListeners.clear ();

  if (mirrorSrc != NULL)
    {
      ((Player *)mirrorSrc)->removeMirror (this);
    }

  i = mirrors.begin ();
  while (i != mirrors.end ())
    {
      (*i)->setMirrorSrc (NULL);
      ++i;
    }
  mirrors.clear ();

  g_assert_null (outputWindow);

/*  if (Ginga_Display->hasSurface (surface))
    {
      delete surface;
      surface = 0;
    } */

  Thread::mutexLock (&referM);
  referredPlayers.clear ();

  properties.clear ();

  Thread::mutexLock (&pnMutex);
  pendingNotifications.clear ();

  Thread::mutexUnlock (&referM);
  Thread::mutexUnlock (&lockedListM);
  Thread::mutexUnlock (&listM);
  Thread::mutexUnlock (&pnMutex);

  Thread::mutexDestroy (&referM);
  Thread::mutexDestroy (&lockedListM);
  Thread::mutexDestroy (&listM);
  Thread::mutexDestroy (&pnMutex);
}

void
Player::setMirrorSrc (IPlayer *mirrorSrc)
{
  this->mirrorSrc = mirrorSrc;
}

void
Player::addMirror (IPlayer *mirror)
{
  this->mirrors.insert (mirror);
}

bool
Player::removeMirror (IPlayer *mirror)
{
  set<IPlayer *>::iterator i;

  i = mirrors.find (mirror);
  if (i != mirrors.end ())
    {
      mirrors.erase (i);
      return true;
    }

  return false;
}

void
Player::setMrl (const string &mrl, bool visible)
{
  this->mrl = mrl;
  this->visible = visible;
}

void
Player::setNotifyContentUpdate (bool notify)
{
  this->notifyContentUpdate = notify;
}

void
Player::addListener (IPlayerListener *listener)
{
  LockedPlayerListener *lpl = NULL;

  if (notifying)
    {
      Thread::mutexLock (&lockedListM);
      lpl = new LockedPlayerListener;
      lpl->isAdd = true;
      lpl->l = listener;

      lockedListeners.push_back (lpl);
      Thread::mutexUnlock (&lockedListM);
    }
  else
    {
      Thread::mutexLock (&listM);
      listeners.insert (listener);
      Thread::mutexUnlock (&listM);
    }
}

void
Player::removeListener (IPlayerListener *listener)
{
  LockedPlayerListener *lpl = NULL;
  set<IPlayerListener *>::iterator i;

  if (notifying)
    {
      Thread::mutexLock (&lockedListM);
      lpl = new LockedPlayerListener;
      lpl->isAdd = false;
      lpl->l = listener;

      lockedListeners.push_back (lpl);
      Thread::mutexUnlock (&lockedListM);
    }
  else
    {
      Thread::mutexLock (&listM);
      i = listeners.find (listener);
      if (i != listeners.end ())
        {
          listeners.erase (i);
        }
      Thread::mutexUnlock (&listM);
    }
}

void
Player::performLockedListenersRequest ()
{
  LockedPlayerListener *lpl;
  vector<LockedPlayerListener *>::iterator i;
  IPlayerListener *listener;
  set<IPlayerListener *>::iterator j;

  Thread::mutexLock (&lockedListM);
  i = lockedListeners.begin ();
  while (i != lockedListeners.end ())
    {
      lpl = *i;
      listener = lpl->l;

      if (lpl->isAdd)
        {
          listeners.insert (listener);
        }
      else
        {
          j = listeners.find (listener);
          if (j != listeners.end ())
            {
              listeners.erase (j);
            }
        }

      delete lpl;
      ++i;
    }

  lockedListeners.clear ();
  Thread::mutexUnlock (&lockedListM);
}

void
Player::notifyPlayerListeners (short code,
                               const string &parameter,
                               short type,
                               const string &value)
{
  PendingNotification *pn;
  vector<PendingNotification *>::iterator i;

  string p;
  string v;
  set<IPlayerListener *> *clone = NULL;

  if (notifying)
    {
      Thread::mutexLock (&pnMutex);
      pn = new PendingNotification;
      pn->code = code;
      pn->parameter = parameter;
      pn->type = type;
      pn->value = value;

      pendingNotifications.push_back (pn);
      Thread::mutexUnlock (&pnMutex);

      return;
    }

  notifying = true;
  Thread::mutexLock (&listM);
  notifying = true;

  performLockedListenersRequest ();

  if (listeners.empty ())
    {
      if (code == PL_NOTIFY_STOP)
        {
          presented = true;
        }
      Thread::mutexUnlock (&listM);
      notifying = false;
      return;
    }

  if (code == PL_NOTIFY_NCLEDIT)
    {
      ntsNotifyPlayerListeners (&listeners, code, parameter, type, value);
    }
  else
    {
      if (code == PL_NOTIFY_STOP)
        {
          presented = true;
        }

      ntsNotifyPlayerListeners (&listeners, code, parameter, type, value);
    }

  Thread::mutexLock (&pnMutex);
  if (!pendingNotifications.empty ())
    {
      clone = new set<IPlayerListener *> (listeners);
    }

  Thread::mutexUnlock (&listM);
  notifying = false;

  if (clone != NULL && !clone->empty ())
    {
      i = pendingNotifications.begin ();
      while (i != pendingNotifications.end ())
        {
          (*i)->clone = new set<IPlayerListener *> (*clone);

          pthread_t tId;
          pthread_attr_t tattr;

          pthread_attr_init (&tattr);
          pthread_attr_setdetachstate (&tattr, PTHREAD_CREATE_DETACHED);
          pthread_attr_setscope (&tattr, PTHREAD_SCOPE_SYSTEM);

          pthread_create (&tId, &tattr, Player::detachedNotifier, (*i));
          pthread_detach (tId);

          ++i;
        }
      pendingNotifications.clear ();
      delete clone;
    }
  Thread::mutexUnlock (&pnMutex);
}

void *
Player::detachedNotifier (void *ptr)
{
  PendingNotification *pn = (PendingNotification *)ptr;

  ntsNotifyPlayerListeners (pn->clone, pn->code, pn->parameter, pn->type,
                            pn->value);

  delete pn->clone;
  delete pn;

  return NULL;
}

void
Player::ntsNotifyPlayerListeners (set<IPlayerListener *> *list,
                                  short code,
                                  const string &parameter,
                                  short type,
                                  const string &value)
{
  set<IPlayerListener *>::iterator i;

  i = list->begin ();
  while (i != list->end ())
    {
      if ((*i) != NULL)
        {
          (*i)->updateStatus (code, parameter, type, value);
        }
      ++i;
    }
}


void
Player::setMediaTime (guint32 newTime)
{
  this->initStartTime = (guint32)g_get_monotonic_time();
  this->initPauseTime = 0;
  this->accTimePlaying = newTime*1000;  //input is in mili but glib is in micro, needs mult by 1000;
  this->accTimePaused = 0; 
}

guint32
Player::getMediaTime ()
{
  if (status == PAUSED)
      return this->accTimePlaying/1000;
    
  guint32 curTime = (guint32)g_get_monotonic_time() - this->initStartTime;
     
  return (this->accTimePlaying + curTime - this->accTimePaused)/1000;
}

double
Player::getTotalMediaTime ()
{
  return -1.0;
}

void
Player::setScope (const string &scope, short type, double initTime, double endTime,
                  double outTransDur)
{
  clog << "Player::setScope '" << scope << "'" << endl;
  this->scope = scope;
  this->scopeType = type;
  this->scopeInitTime = initTime;
  this->scopeEndTime = endTime;
  this->outTransTime = outTransDur;
}

bool
Player::play ()
{
  this->forcedNaturalEnd = false;
  this->initStartTime = (guint32)g_get_monotonic_time();
  this->status = OCCURRING;

  return true;
}

void
Player::stop ()
{
  this->initStartTime = 0;
  this->initPauseTime = 0;
  this->accTimePlaying = 0;
  this->accTimePaused = 0; 

  this->status = SLEEPING;
  
}

void
Player::abort ()
{
  stop ();
}

void
Player::pause ()
{
  this->accTimePlaying +=  ( (guint32)g_get_monotonic_time() - this->initStartTime);
  this->initPauseTime = (guint32)g_get_monotonic_time();
   this->status = PAUSED;
}

void
Player::resume ()
{
  this->initStartTime = (guint32)g_get_monotonic_time();

  if(this->initPauseTime > 0)
      this->accTimePaused +=  ((guint32)g_get_monotonic_time() - this->initPauseTime);

  this->status = OCCURRING;
}

string
Player::getPropertyValue (string const &name)
{
  if (properties.count (name) != 0)
    {
      return properties[name];
    }

  return "";
}

void
Player::setPropertyValue (const string &name, const string &value)
{
  vector<string> *params = NULL;

  if (outputWindow != 0)
    {
      if (name == "bounds")
        {
          params = split (value, ",");
          if (params->size () == 4)
            {
              outputWindow->setBounds (
                                   xstrto_int ((*params)[0]),
                                   xstrto_int ((*params)[1]),
                                   xstrto_int ((*params)[2]),
                                   xstrto_int ((*params)[3]));
            }
          delete params;
        }
      else if (name == "location")
        {
          params = split (value, ",");
          if (params->size () == 2)
            {
              outputWindow->moveTo (
                                xstrto_int ((*params)[0]),
                                xstrto_int ((*params)[1]));
            }
          delete params;
        }
      else if (name == "size")
        {
          params = split (value, ",");
          if (params->size () == 2)
            {
              outputWindow->resize (
                                xstrto_int ((*params)[0]),
                                xstrto_int ((*params)[1]));
            }
          delete params;
        }
      else if (name == "left")
        {
          outputWindow->setX (
                          xstrto_int (value));
        }
      else if (name == "top")
        {
          outputWindow->setY (
                          xstrto_int (value));
        }
      else if (name == "width")
        {
          outputWindow->setW (
                          xstrto_int (value));
        }
      else if (name == "height")
        {
          outputWindow->setH (
                          xstrto_int (value));
        }
      else if (name == "transparency")
        {
        //  outputWindow->setCurrentTransparency (
         //                                   xstrto_uint8 (value));
        }
    }
  properties[name] = value;
}

void
Player::addTimeReferPlayer (IPlayer *referPlayer)
{
  Thread::mutexLock (&referM);
  referredPlayers.insert (referPlayer);
  Thread::mutexUnlock (&referM);
}

void
Player::removeTimeReferPlayer (IPlayer *referPlayer)
{
  set<IPlayer *>::iterator i;

  Thread::mutexLock (&referM);
  i = referredPlayers.find (referPlayer);
  if (i != referredPlayers.end ())
    {
      referredPlayers.erase (i);
      Thread::mutexUnlock (&referM);
      return;
    }
  Thread::mutexUnlock (&referM);
}

void
Player::notifyReferPlayers (int transition)
{ 
  set<IPlayer *>::iterator i;

  Thread::mutexLock (&referM);
  i = referredPlayers.begin ();
  while (i != referredPlayers.end ())
    {
      (*i)->timebaseObjectTransitionCallback (transition);
      ++i;
    }
  Thread::mutexUnlock (&referM); 
}

void
Player::timebaseObjectTransitionCallback (int transition)
{
  if (transition == PL_NOTIFY_STOP)
    {
      setReferenceTimePlayer (NULL);
      stop ();
    }
}

void
Player::setTimeBasePlayer (IPlayer *timeBasePlayer)
{
  clog << "Player::setTimeBasePlayer" << endl;
  if (timeBasePlayer != NULL)
    {
      this->timeBasePlayer = timeBasePlayer;
      this->timeBasePlayer->addTimeReferPlayer (this);
    }
}

bool
Player::hasPresented ()
{
  return presented;
}

void
Player::setPresented (bool presented)
{
  this->presented = presented;
}

bool
Player::isVisible ()
{
  return this->visible;
}

void
Player::setVisible (bool visible)
{
  this->visible = visible;
}

bool
Player::immediatelyStart ()
{
  return immediatelyStartVar;
}

void
Player::setImmediatelyStart (bool immediattelyStartVal)
{
  this->immediatelyStartVar = immediattelyStartVal;
}

void
Player::forceNaturalEnd (bool forceIt)
{
  forcedNaturalEnd = forceIt;
  if (forceIt)
    {
      notifyPlayerListeners (PL_NOTIFY_STOP);
    }
}

bool
Player::isForcedNaturalEnd ()
{
  if (mrl == "")
    {
      return false;
    }
  return forcedNaturalEnd;
}

bool
Player::setOutWindow (SDLWindow* windowId)
{
  this->window = windowId;
  return true;
}

PLAYER_STATUS 
Player::getMediaStatus(){
   return this->status;
}

GINGA_PLAYER_END
