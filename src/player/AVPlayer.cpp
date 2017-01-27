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
#include "AVPlayer.h"
#include "mb/Display.h"

#include "util/functions.h"
using namespace ::ginga::util;

GINGA_PLAYER_BEGIN

AVPlayer::AVPlayer (string mrl) : Thread (), Player (mrl)
{
  string::size_type pos;

  this->provider = 0;
  this->mainAV = false;
  this->buffered = false;
  this->isRemote = false;

  this->status = STOP;
  this->running = false;
  this->soundLevel = 1.0;
  this->win = 0;
  this->pSym = "";

  Thread::mutexInit (&pMutex, true);
  Thread::mutexInit (&tMutex, true);

  if (mrl.length () > 11 && mrl.substr (0, 11) == "sbtvd-ts://")
    {
      this->mainAV = true;
      pos = mrl.find ("#");
      if (pos != std::string::npos)
        {
          this->mrl = mrl.substr (pos + 1, mrl.length () - (pos + 1));
        }
      else
        {
          this->mrl = mrl.substr (11, mrl.length () - 11);
        }
      clog << "AVPlayer::AVPlayer MAINAV CREATED MRL = '";
      clog << this->mrl << "'" << endl;
    }
  else
    {
      createProvider ();
      this->scopeEndTime = getTotalMediaTime ();
    }
}

AVPlayer::~AVPlayer ()
{
  if (status != STOP)
    {
      stop ();
    }

  unlockConditionSatisfied ();

  Thread::mutexLock (&tMutex);
  if (surface != 0 && mainAV)
    {
      surface->setParentWindow (0);
    }

  if (mainAV)
    {
      if (win != 0)
        {
          delete win;
          win = 0;
        }
    }

  Thread::mutexLock (&pMutex);
  if (this->provider != 0)
    {
      release ();
    }

  Thread::mutexUnlock (&pMutex);
  Thread::mutexDestroy (&pMutex);

  Thread::mutexUnlock (&tMutex);
  Thread::mutexDestroy (&tMutex);
}

SDLSurface*
AVPlayer::getSurface ()
{
  if (provider == 0)
    {
      createProvider ();
      if (provider == 0)
        {
          clog << "AVPlayer::getSurface() can't create provider" << endl;
        }
    }

  return Player::getSurface ();
}

void
AVPlayer::createProvider (void)
{
  Thread::mutexLock (&pMutex);
  clog << "AVPlayer::createProvider '" << mrl << "'" << endl;

  if (mrl.substr (0, 7) == "rtsp://" || mrl.substr (0, 6) == "rtp://"
      || mrl.substr (0, 7) == "http://" || mrl.substr (0, 8) == "https://")
    {
      isRemote = true;
    }

  if (provider == 0 && (fileExists (mrl) || isRemote))
    {
      provider = Ginga_Display->createContinuousMediaProvider (mrl);
      surface = createFrame ();
    }

  Thread::mutexUnlock (&(pMutex));
  clog << "AVPlayer::createProvider '" << mrl << "' all done" << endl;
}

IMediaProvider*
AVPlayer::createProviderT (void *ptr)
{
  AVPlayer *p = (AVPlayer *)ptr;

  p->createProvider ();

  return p->provider;
}

void
AVPlayer::finished ()
{
  clog << "AVPlayer::finished" << endl;
}

double
AVPlayer::getEndTime ()
{
  return scopeEndTime;
}

void
AVPlayer::initializeAudio (arg_unused (int numArgs), arg_unused (char *args[]))
{
}

void
AVPlayer::releaseAudio ()
{
}

void
AVPlayer::setSoundLevel (double level)
{
  // clog << "AVPlayer::setSoundLevel()" << endl;
  // lock()();
  this->soundLevel = level;
  if (provider != 0)
    {
      provider->setSoundLevel (soundLevel);
    }
  // unlock();
}

SDLSurface*
AVPlayer::createFrame ()
{
  // clog << "AVPlayer::createFrame()" << endl;

  Thread::mutexLock (&tMutex);
  if (surface != 0)
    {
      clog << "AVPlayer::createFrame Warning! surface != NULL";
      clog << endl;
      if (mainAV)
        {
          surface->setParentWindow (0);
        }
      delete surface;
    }

  surface = Ginga_Display->createSurface ();
  if (win != 0 && mainAV)
    {
      surface->setParentWindow (win);
    }

  Thread::mutexUnlock (&tMutex);

  return surface;
}

void
AVPlayer::getOriginalResolution (int *width, int *height)
{
  if (provider != 0)
    {
      provider->getOriginalResolution (height, width);
    }
}

double
AVPlayer::getTotalMediaTime ()
{
  if (provider != 0)
    {
      return provider->getTotalMediaTime ();
    }
  return 0;
}

int64_t
AVPlayer::getVPts ()
{
  if (provider == 0)
    {
      return 0;
    }

  return provider->getVPts ();
}

void
AVPlayer::timeShift (string direction)
{
  if (provider != 0)
    {
      if (direction == "forward")
        {
          provider->setMediaTime (provider->getMediaTime () + 10);
        }
      else if (direction == "backward")
        {
          provider->setMediaTime (provider->getMediaTime () - 10);
        }
    }
}

double
AVPlayer::getCurrentMediaTime ()
{
  if (provider == 0)
    {
      clog << "AVPlayer::getCurrentMediaTime returning -1";
      clog << " cause provider is NULL";
      clog << endl;
      return -1;
    }

  return provider->getMediaTime ();
}

double
AVPlayer::getMediaTime ()
{
  return getCurrentMediaTime ();
}

void
AVPlayer::setMediaTime (double pos)
{
  if (status == PLAY)
    {
      status = PAUSE;
      provider->setMediaTime (pos);
      status = PLAY;
      running = true;
      Thread::startThread ();
    }
  else if (provider != 0)
    {
      provider->setMediaTime (pos);
    }
}

void
AVPlayer::setStopTime (double pos)
{
  if (status == PLAY)
    {
      status = PAUSE;
      scopeEndTime = pos;
      status = PLAY;
      running = true;
      Thread::startThread ();
    }
  else
    {
      scopeEndTime = pos;
    }
}

double
AVPlayer::getStopTime ()
{
  return scopeEndTime;
}

void
AVPlayer::setScope (string scope, short type, double begin, double end,
                    double outTransDur)
{
  Player::setScope (scope, type, begin, end, outTransDur);
  if (type == TYPE_PRESENTATION)
    {
      if (scopeInitTime > 0)
        {
          setMediaTime (scopeInitTime);
        }

      if (scopeEndTime > 0)
        {
          setStopTime (scopeEndTime);
        }
    }
}

bool
AVPlayer::play ()
{
  if (provider == 0)
    {
      clog << "AVPlayer::play() can't play, provider is NULL" << endl;
      return false;
    }

  Player::play ();
  clog << "AVPlayer::play() calling provider play over" << endl;
  provider->playOver (surface);

  if (!running)
    {
      running = true;
      Thread::startThread ();
    }

  clog << "AVPlayer::play(" << mrl << ") all done!" << endl;
  return true;
}

void
AVPlayer::pause ()
{
  status = PAUSE;
  if (provider == 0)
    {
      return;
    }

  if (outputWindow != 0)
    {
      outputWindow->validate ();
    }
  provider->pause ();

  clog << "AVPlayer::pause(" << mrl << ") all done!" << endl;
}

void
AVPlayer::stop ()
{
  short previousStatus = status;

  Player::stop ();
  if (provider == 0)
    {
      return;
    }

  if (previousStatus != STOP)
    {
      provider->stop ();
    }

  clog << "AVPlayer::stop(" << mrl << ") all done!" << endl;
}

void
AVPlayer::resume ()
{
  setSoundLevel (soundLevel);

  Player::play ();
  provider->resume (surface);

  if (!running)
    {
      running = true;
      Thread::startThread ();
    }
}

string
AVPlayer::getPropertyValue (string const &name)
{
  if (name == "soundLevel")
    {
      return xstrbuild ("%d", (int) provider->getSoundLevel ());
    }

  return Player::getPropertyValue (name);
}

void
AVPlayer::setPropertyValue (const string &name, const string &value)
{
  string val = value;
  double fVal = 1.0;
  vector<string> *vals;

  if (name == "soundLevel")
    {
      if (isPercentualValue (val))
        {
          xstrassign (val, "%d", (int) (getPercentualValue (val) / 100));
        }

      if (val != "")
        {
          fVal = xstrtod (val);
        }
      setSoundLevel (fVal);
    }
  else if (mainAV)
    {
      if (name == "createWindow")
        {
          vals = split (val, ",");
          if (vals->size () == 4)
            {
              win = Ginga_Display->createWindow (xstrto_int ((*vals)[0]),
                                                 xstrto_int ((*vals)[1]),
                                                 xstrto_int ((*vals)[2]),
                                                 xstrto_int ((*vals)[3]), 1.0);

              int caps = win->getCap ("NOSTRUCTURE")
                         | win->getCap ("DOUBLEBUFFER");
              win->setCaps (caps);
              win->draw ();
            }

          delete vals;

          if (!running)
            {
              Thread::startThread ();
            }
        }
      else if (name == "bounds" && win != 0)
        {
          vals = split (val, ",");
          if (vals->size () == 4)
            {
              win->setBounds ( xstrto_int ((*vals)[0]),
                               xstrto_int ((*vals)[1]),
                               xstrto_int ((*vals)[2]),
                               xstrto_int ((*vals)[3]));
            }
          delete vals;
        }
      else if (name == "show" && win != 0)
        {
          win->show ();
        }
      else if (name == "hide" && win != 0)
        {
          win->hide ();
        }
    }

  Player::setPropertyValue (name, val);
}

void
AVPlayer::addListener (IPlayerListener *listener)
{
  Player::addListener (listener);
}

void
AVPlayer::release ()
{
  Ginga_Display->destroyContinuousMediaProvider ((IContinuousMediaProvider*)provider);
  provider = 0;
}

void
AVPlayer::setMrl (const char *mrl)
{
  this->mrl = mrl;
}

string
AVPlayer::getMrl ()
{
  return mrl;
}

bool
AVPlayer::isPlaying ()
{
  if (provider == 0)
    {
      return false;
    }

  if ((getCurrentMediaTime () <= 0 && status != PAUSE)
      || (getCurrentMediaTime () >= getStopTime () && getStopTime () > 0))
    {
      return false;
    }
  else
    {
      return true;
    }
}

bool
AVPlayer::setOutWindow (SDLWindow* windowId)
{
  if (mainAV && win == 0)
    {
      win = windowId;

      if (!running)
        {
          Thread::startThread ();
        }
      return true;
    }
  else
    {
      Player::setOutWindow (windowId);
    }
  return false;
}

void
AVPlayer::setAVPid (int aPid, int vPid)
{
  while (!buffered)
    {
      g_usleep (150000);
    }
  provider->setAVPid (aPid, vPid);
}

bool
AVPlayer::isRunning ()
{
  return running;
}

bool
AVPlayer::checkVideoResizeEvent ()
{
  bool hasEvent = false;

  if (mainAV)
    {
      provider->feedBuffers ();
    }
  else
    {
      g_usleep (150000);
    }
  hasEvent = provider->checkVideoResizeEvent (surface);
  setSoundLevel (this->soundLevel);
  if (hasEvent)
    {
      provider->playOver (surface);
    }

  return hasEvent;
}

void
AVPlayer::run ()
{
  double dur;
  double currentTime;
  double lastCurrentTime = -1;
  double timeRemain;
  double totalTime;

  clog << "AVPlayer::run" << endl;

  Thread::mutexLock (&tMutex);

  if (mainAV)
    {
      running = true;

      this->provider = Ginga_Display->createContinuousMediaProvider (mrl);

      this->surface = createFrame ();

      if (this->win != 0 && surface->getParentWindow () == 0)
        {
          surface->setParentWindow (win);
        }
      provider->playOver (surface);
      checkVideoResizeEvent ();
      buffered = true;

      // notifying GingaGUI

      // cmd means a command to an embeddor process
      // 0 means successful status message
      //
      cout << "cmd::0::tuned::?mAV?" << endl;
      waitForUnlockCondition ();
    }
  else if (isRemote)
    {
      waitForUnlockCondition ();
    }
  else
    {
      totalTime = getTotalMediaTime ();
      if (!isInfinity (scopeEndTime) && scopeEndTime > 0
          && scopeEndTime < totalTime)
        {
          dur = getStopTime ();
        }
      else
        {
          dur = totalTime;
        }

      clog << "AVPlayer::run total media time that will be considered ";
      clog << " is '" << dur << "'";
      clog << endl;

      if (isInfinity (dur))
        {
          clog << "AVPlayer::run duration is INF";
          clog << " => returning" << endl;

          Thread::mutexUnlock (&tMutex);
          return;
        }

      g_usleep (850000);
      currentTime = getCurrentMediaTime ();
      if (currentTime > dur)
        {
          currentTime = 0;
        }
      if (currentTime >= 0)
        {
          while (dur > (currentTime + 0.1))
            {
              clog << "AVPlayer::run dur = '" << dur;
              clog << "' curMediaTime = '";
              clog << currentTime << "' total = '";
              clog << getTotalMediaTime ();
              clog << "' for '" << mrl << "'" << endl;

              if (status != PLAY)
                {
                  clog << "AVPlayer::run status != play => exiting";
                  clog << endl;
                  break;
                }

              timeRemain = (dur - currentTime) * 1000;
              if (outTransTime > 0.0)
                {
                  timeRemain = outTransTime - (currentTime * 1000);
                }

              if (status != PLAY || !this->mSleep ((long int) timeRemain))
                {
                  clog << "AVPlayer::run can't sleep '" << timeRemain;
                  clog << "' => exiting" << endl;

                  if (status == PLAY && outTransTime > 0.0)
                    {
                      outTransTime = 0;
                      notifyPlayerListeners (PL_NOTIFY_OUTTRANS, "");
                    }
                  else
                    {
                      break;
                    }
                }
              else if (outTransTime > 0.0)
                {
                  clog << "AVPlayer::run notify transition at '";
                  clog << currentTime << "' (out transition time is '";
                  clog << outTransTime << "')" << endl;

                  outTransTime = 0;
                  notifyPlayerListeners (PL_NOTIFY_OUTTRANS, "");
                }

              currentTime = getCurrentMediaTime ();
              if (currentTime <= 0)
                {
                  clog << "AVPlayer::run currentTime = '" << currentTime;
                  clog << "' => exiting" << endl;
                  break;
                }

              if (xnumeq (lastCurrentTime, currentTime) && status != PAUSE)
                {
                  break;
                }
              lastCurrentTime = currentTime;
            }

          clog << "AVPlayer::run has exited from loop cause dur = '";
          clog << dur << "' and current time = '" << currentTime;
          clog << "'";
          clog << endl;
        }
      else
        {
          clog << "AVPlayer::run can't do nothing cause current time is";
          clog << " '" << currentTime << "'";
          clog << endl;
        }
    }

  clog << "AVPlayer::run(" << mrl << ") notifying ... " << endl;
  if (status != PAUSE)
    {
      presented = true;
    }

  if (status != STOP && status != PAUSE)
    {
      status = STOP;
      running = false;

      if (provider != 0)
        {
          provider->stop ();
        }

      clog << "AVPlayer::run(" << mrl << ") NOTIFY STOP" << endl;
      Thread::mutexUnlock (&tMutex);
      notifyPlayerListeners (PL_NOTIFY_STOP, "");
    }
  else
    {
      status = STOP;
      running = false;
      Thread::mutexUnlock (&tMutex);
    }

  clog << "AVPlayer::run(" << mrl << ") ALL DONE" << endl;
}

GINGA_PLAYER_END
