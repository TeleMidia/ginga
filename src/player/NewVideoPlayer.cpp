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
#include "NewVideoPlayer.h"
#include "mb/Display.h"

#define TRACE() g_debug ("%s",G_STRFUNC);

#include "util/functions.h"
using namespace ::ginga::util;

GINGA_PLAYER_BEGIN

NewVideoPlayer::NewVideoPlayer (const string &mrl) : Thread (), Player (mrl)
{
	TRACE ();
}

NewVideoPlayer::~NewVideoPlayer ()
{
	TRACE ();
}

SDLSurface*
NewVideoPlayer::getSurface ()
{
	TRACE ();
  return NULL;
}

void
NewVideoPlayer::finished ()
{
	TRACE ();
}

double
NewVideoPlayer::getEndTime ()
{
	TRACE ();
  return 0;
}

void
NewVideoPlayer::initializeAudio (arg_unused (int numArgs), arg_unused (char *args[]))
{
	TRACE ();
}

void
NewVideoPlayer::releaseAudio ()
{
	g_debug ("%s", G_STRLOC);
}

void
NewVideoPlayer::getOriginalResolution (arg_unused(int *width), arg_unused(int *height))
{
	g_debug ("%s", G_STRLOC);
}

double
NewVideoPlayer::getTotalMediaTime ()
{
	g_debug ("%s", G_STRLOC);
  return 0;
}

int64_t
NewVideoPlayer::getVPts ()
{
	g_debug ("%s", G_STRLOC);
  return 0;
}

void
NewVideoPlayer::timeShift (arg_unused(const string &direction))
{
	TRACE ();
}

double
NewVideoPlayer::getMediaTime ()
{
	TRACE ();
  return 0;
}

void
NewVideoPlayer::setMediaTime (arg_unused(double pos))
{
	TRACE ();
}

void
NewVideoPlayer::setStopTime (arg_unused(double pos))
{
	TRACE ();
}

double
NewVideoPlayer::getStopTime ()
{
	TRACE ();
  return 0;
}

void
NewVideoPlayer::setScope (arg_unused(const string &scope), arg_unused(short type), arg_unused(double begin), arg_unused(double end),arg_unused(double outTransDur))
{
	TRACE ();
}

bool
NewVideoPlayer::play ()
{
	TRACE ();
	return true;
}

void
NewVideoPlayer::pause ()
{
	TRACE ();
}

void
NewVideoPlayer::stop ()
{
	TRACE ();
}

void
NewVideoPlayer::resume ()
{
	TRACE ();
}

string
NewVideoPlayer::getPropertyValue (arg_unused(const string &name))
{
	TRACE ();
  return NULL;
}

void
NewVideoPlayer::setPropertyValue (arg_unused(const string &name), arg_unused(const string &value))
{
	TRACE ();
}

void
NewVideoPlayer::addListener (IPlayerListener *listener)
{
  (void) listener;
	TRACE ();
}

void
NewVideoPlayer::release ()
{
	TRACE ();
}

string
NewVideoPlayer::getMrl ()
{
	TRACE ();
  return NULL;
}

bool
NewVideoPlayer::isPlaying ()
{
	return true;
}

bool
NewVideoPlayer::setOutWindow (arg_unused(SDLWindow* windowId))
{
	return true;
}

void
NewVideoPlayer::setAVPid (arg_unused(int aPid), arg_unused(int vPid))
{
	TRACE ();
}

bool
NewVideoPlayer::isRunning ()
{
	TRACE ();
	return true;
}

void
NewVideoPlayer::run ()
{
}

GINGA_PLAYER_END
