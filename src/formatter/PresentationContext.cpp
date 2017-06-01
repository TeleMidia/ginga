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
#include "PresentationContext.h"
#include "ctxmgmt/ContextManager.h"
#include "ctxmgmt/SystemInfo.h"

GINGA_FORMATTER_BEGIN

ContextManager *PresentationContext::contextManager = NULL;

PresentationContext::PresentationContext ()
{
  if (contextManager == NULL)
    {
      PresentationContext::contextManager = ContextManager::getInstance ();
    }

  initializeUserContext ();
  initializeSystemValues ();
}

PresentationContext::~PresentationContext ()
{
  contextTable.clear ();
}

void
PresentationContext::setPropertyValue (const string &property, const string &value)
{
  string oldValue = "";
  map<string, string>::iterator i;

  if ((property.length () >= 7 && property.substr (0, 7) == "system.")
      || (property.length () >= 5 && property.substr (0, 5) == "user."))
    {
      return;
    }

  clog << "PresentationContext::setPropertyValue propName = '";
  clog << property << "' propValue = '" << value << "'" << endl;
  i = contextTable.find (property);

  if (i != contextTable.end ())
    {
      oldValue = i->second;
    }

  contextTable[property] = value;
}

void
PresentationContext::incPropertyValue (const string &propertyName)
{
  string oldValue = "", newValue = "";
  map<string, string>::iterator i;

  i = contextTable.find (propertyName);
  if (i != contextTable.end ())
    {
      oldValue = i->second;
    }

  if (oldValue == "")
    {
      return;
    }

  xstrassign (newValue, "%d", (int) (xstrtod (oldValue) + 1));
  if ((newValue != "") && (newValue != oldValue))
    contextTable[propertyName] = newValue;
}

void
PresentationContext::decPropertyValue (const string &propertyName)
{
  string oldValue = "", newValue = "";
  map<string, string>::iterator i;

  i = contextTable.find (propertyName);
  if (i != contextTable.end ())
    {
      oldValue = i->second;
    }

  if (oldValue == "")
    {
      return;
    }

  xstrassign (newValue, "%d", (int) (xstrtod (oldValue) - 1));
  if ((newValue != "") && (newValue != oldValue))
    contextTable[propertyName] = newValue;
}

set<string> *
PresentationContext::getPropertyNames ()
{
  set<string> *propertyNames = new set<string>;
  map<string, string>::iterator i;
  for (i = contextTable.begin (); i != contextTable.end (); ++i)
    {
      propertyNames->insert (i->first);
    }

  return propertyNames;
}

string
PresentationContext::getPropertyValue (const string &attributeId)
{
  string propValue;

  if (contextTable.count (attributeId) == 0)
    {
      return "";
    }

  propValue = contextTable[attributeId];
  return propValue;
}

void
PresentationContext::initializeUserContext ()
{
  int currentUserId;
  map<string, string> *userProfile = NULL;

  currentUserId = PresentationContext::contextManager->getCurrentUserId ();
  userProfile
      = PresentationContext::contextManager->getUserProfile (currentUserId);

  if (userProfile == NULL)
    {
      PresentationContext::contextManager->addContextVar (
          currentUserId, "system.background-color", "000000");

      userProfile = PresentationContext::contextManager->getUserProfile (
          currentUserId);
    }

  contextTable.clear ();
  contextTable.insert (userProfile->begin (), userProfile->end ());

  delete userProfile;

  initializeUserInfo (currentUserId);
}

void
PresentationContext::initializeUserInfo (int currentUserId)
{
  GingaUser *user;

  user = PresentationContext::contextManager->getUser (currentUserId);
  if (user == NULL)
    return;                     // nothing to do

  xstrassign (contextTable["user.age"], "%d", user->getUserAge ());
  contextTable["user.location"] = user->getUserLocation ();
  contextTable["user.genre"] = user->getUserGenre ();
}

void
PresentationContext::initializeSystemValues ()
{
  SystemInfo *si;
  int w, h;

  // clog << "PresentationContext::initializeSystemValues " << endl;
  si = PresentationContext::contextManager->getSystemInfo ();

  contextTable["system.language"] = si->getSystemLanguage ();
  contextTable["system.caption"] = si->getCaptionLanguage ();
  contextTable["system.subtitle"] = si->getSubtitleLanguage ();
  xstrassign (contextTable["system.returnBitRate"], "%d", (int) si->getReturnBitRate ());

  si->getScreenSize (&w, &h);
  xstrassign (contextTable["system.screenSize"], "%d,%d", w, h);

  si->getScreenGraphicSize (&w, &h);
  xstrassign (contextTable["system.screenGraphicSize"], "%d,%d", w, h);
  contextTable["system.audioType"] = si->getAudioType ();
  xstrassign (contextTable["system.CPU"], "%d", (int) si->getCPUClock ());
  xstrassign (contextTable["system.memory"], "%d", (int) si->getMemorySize ());
  contextTable["system.operatingSystem"] = "";

  contextTable["system.devNumber(0)"] = "0";
  contextTable["system.devNumber(1)"] = "0";
  contextTable["system.devNumber(2)"] = "0";

  contextTable["system.classType(0)"] = "base";
  contextTable["system.classType(2)"] = "active";

  contextTable["system.info(0)"] = "0";
  contextTable["system.info(2)"] = "2";

  contextTable["system.classNumber"] = "3";

  if (contextTable.count ("default.focusBorderColor") == 0)
    {
      contextTable["DEFAULT_FOCUS_BORDER_COLOR"] = "blue";
    }

  if (contextTable.count ("default.selBorderColor") == 0)
    {
      contextTable["default.selBorderColor"] = "green";
    }

  if (contextTable.count ("default.focusBorderWidth") == 0)
    {
      contextTable["default.focusBorderWidth"] = "3";
    }

  if (contextTable.count ("default.focusBorderTransparency") == 0)
    {
      contextTable["default.focusBorderTransparency"] = "0";
    }
}

GINGA_FORMATTER_END
