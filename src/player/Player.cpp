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
#include "ginga-color-table.h"
#include "Player.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

GINGA_PLAYER_BEGIN

Player::Player (const string &mrl)
{
  Thread::mutexInit (&listM, false);
  Thread::mutexInit (&lockedListM, false);

  this->mrl = mrl;
  this->window = NULL;
  this->notifying = false;
  this->presented = false;
  this->visible = true;
  this->status = SLEEPING;
  this->forcedNaturalEnd = false;
  this->scope = "";
  this->scopeType = -1;
  this->scopeInitTime = -1;
  this->scopeEndTime = -1;
  this->outTransTime = -1;

  //time attr
  this->initStartTime = 0;
  this->initPauseTime = 0;
  this->accTimePlaying = 0;
  this->accTimePaused = 0;

  //media attr
  this->texture = NULL; //media content
  this->borderWidth = 0;
  this->bgColor = {0, 0, 0, 0};
  this->borderColor = {0, 0, 0, 0};
  this->z = 0;
  this->alpha = 255;

  animator = new PlayerAnimator();

  Ginga_Display->registerPlayer(this);
}

Player::~Player ()
{
  Ginga_Display->unregisterPlayer(this);

  set<IPlayer *>::iterator i;

  this->status = SLEEPING;

  Thread::mutexLock (&listM);
  listeners.clear ();

  Thread::mutexLock (&lockedListM);
  lockedListeners.clear ();

  properties.clear ();

  Thread::mutexUnlock (&lockedListM);
  Thread::mutexUnlock (&listM);
  Thread::mutexDestroy (&lockedListM);
  Thread::mutexDestroy (&listM);
}

void
Player::setMrl (const string &mrl, bool visible)
{
  this->mrl = mrl;
  this->visible = visible;
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
Player::notifyPlayerListeners (short code,
                               const string &parameter,
                               short type,
                               const string &value)
{
  string p;
  string v;

  notifying = true;
  Thread::mutexLock (&listM);
  notifying = true;

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

  if (code == PL_NOTIFY_STOP)
    {
      presented = true;
    }

  ntsNotifyPlayerListeners (&listeners, code, parameter, type, value);
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
Player::setScope (const string &scope,
                  short type,
                  double initTime,
                  double endTime,
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
  this->initStartTime = (guint32) g_get_monotonic_time();
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
  vector<string> params;

  if (!value.length ())
    return;

  if (name == "bounds")
    {
      params = xstrsplit (value, ',');
      if (params.size () == 4)
        {
          this->rect.x = xstrtoint (params[0], 10);
          this->rect.y = xstrtoint (params[1], 10);
          this->rect.w = xstrtoint (params[2], 10);
          this->rect.h = xstrtoint (params[3], 10);
        }
    }
  else if (name == "location")
    {
      g_assert_not_reached ();
    }
  else if (name == "size")
    {
      g_assert_not_reached ();
    }
  else if (name == "left")
    {
      this->rect.x = xstrtoint (value, 10);
    }
  else if (name == "top")
    {
      this->rect.y = xstrtoint (value, 10);
    }
  else if (name == "width")
    {
      this->rect.w = xstrtoint (value, 10);
    }
  else if (name == "height")
    {
      this->rect.h = xstrtoint (value, 10);
    }
  else if(name == "background" || name == "backgroundColor"
          || name == "bgColor")
    {
      ginga_color_input_to_sdl_color(value, &this->bgColor);
    }
  else if (name == "transparency")
    {
      if(xstrtouint8 (value, 10) <= 0)
        this->alpha = 255;
      else
        this->alpha = (guint8)(255 - (((xstrtod (value)/100)*255)));
    }
  properties[name] = value;
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
  if( windowId!=NULL){
      this->rect = windowId->getRect();
      this->z = windowId->getZ();
  }

  this->window = windowId;
  return true;
}

PLAYER_STATUS
Player::getMediaStatus(){
   return this->status;
}

gint
Player::getZ(){
  return this->z;
}

void
Player::setAnimatorProperties(string dur, string name, string value)
{
  animator->addProperty(dur,name,value);
}

void
Player::redraw(SDL_Renderer* renderer)
{
  if(this->status == SLEEPING)
    return;

  animator->update(&this->rect,
                   &this->bgColor.r,
                   &this->bgColor.g,
                   &this->bgColor.b,
                   &this->alpha);

  if(this->window!=NULL)
    this->window->getBorder(&this->borderColor,&this->borderWidth);

  if (this->bgColor.a > 0)
    {  // background color
      SDLx_SetRenderDrawBlendMode (renderer, SDL_BLENDMODE_BLEND);
      SDLx_SetRenderDrawColor (renderer,
                               this->bgColor.r,
                               this->bgColor.g,
                               this->bgColor.b,
                               this->alpha);
      SDLx_RenderFillRect (renderer, &this->rect);
    }

  if (this->texture != NULL){
      SDLx_SetTextureAlphaMod (this->texture, this->alpha);
      SDLx_RenderCopy (renderer, this->texture, NULL, &this->rect);
    }

  if (this->borderWidth < 0)
    {
      this->borderWidth*=-1;

      SDLx_SetRenderDrawBlendMode (renderer, SDL_BLENDMODE_BLEND);
      SDLx_SetRenderDrawColor (renderer,
                               this->borderColor.r,
                               this->borderColor.g,
                               this->borderColor.b, 255);
      SDLx_RenderDrawRect (renderer, &this->rect);
    }
}

GINGA_PLAYER_END
