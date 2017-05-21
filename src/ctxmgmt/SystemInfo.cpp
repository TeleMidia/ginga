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
#include "SystemInfo.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

GINGA_CTXMGMT_BEGIN

SystemInfo::SystemInfo ()
{
  initializeClockSpeed ();
  sysTable = NULL;
}

SystemInfo::~SystemInfo () {}

void
SystemInfo::initializeClockSpeed ()
{
  string line = "";
  clockSpeed = 0;
}

string
SystemInfo::getValue (const string &attribute)
{
  map<string, string>::iterator i;

  if (sysTable == NULL)
    goto fail;

  i = sysTable->find (attribute);
  if (i != sysTable->end ())
    return i->second;

fail:
  return "";
}

void
SystemInfo::setSystemTable (map<string, string> *t)
{
  if (this->sysTable != NULL)
    delete this->sysTable;
  this->sysTable = t;
}

string
SystemInfo::getSystemLanguage ()
{
  string value = getValue ("system.language");
  return (value != "") ? value : "por";
}

string
SystemInfo::getCaptionLanguage ()
{
  string value = getValue ("system.caption");
  return (value != "") ? value : "por";
}

string
SystemInfo::getSubtitleLanguage ()
{
  string value = getValue ("system.subtitle");
  return (value != "") ? value : "por";
}

double
SystemInfo::getReturnBitRate ()
{
  string value = getValue ("system.returnBitRate");
  return (value != "") ? xstrtod (value) : 0.;
}

void
SystemInfo::getScreenSize (int *width, int *height)
{
  Ginga_Display->getSize (width, height);
}

void
SystemInfo::getScreenGraphicSize (int *width, int *height)
{
  Ginga_Display->getSize (width, height);
}

string
SystemInfo::getAudioType ()
{
  string value = getValue ("system.audioType");
  return (value != "") ? value : "stereo";
}

double
SystemInfo::getCPUClock ()
{
  return clockSpeed;
}

double
SystemInfo::getMemorySize ()
{
  return 0;
}

string
SystemInfo::getJavaConfiguration ()
{
  string value = getValue ("system.javaConfiguration");
  return (value != "") ? value : "0";
}

string
SystemInfo::getJavaProfile ()
{
  string value = getValue ("system.javaProfile");
  return (value != "") ? value : "0";
}

string
SystemInfo::getLuaVersion ()
{
  return string ("5.1");
}

GINGA_CTXMGMT_END
