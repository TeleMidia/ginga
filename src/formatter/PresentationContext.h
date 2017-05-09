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

#ifndef _PRESENTATIONCONTEXT_H_
#define _PRESENTATIONCONTEXT_H_

#include "util/Observable.h"

#include "util/functions.h"
using namespace ::ginga::util;

#include "ctxmgmt/ContextManager.h"
#include "ctxmgmt/SystemInfo.h"
using namespace ::ginga::ctxmgmt;

#include "ginga.h"

#include "system/Thread.h"
using namespace ::ginga::system;

#include "ContextBase.h"

#include "ctxmgmt/IContextListener.h"
using namespace ::ginga::ctxmgmt;

#include "math.h"

GINGA_FORMATTER_BEGIN

class PresentationContext : public Observable, public IContextListener
{
private:
  map<string, string> contextTable;
  static ContextManager *contextManager;
  IContextListener *globalVarListener;
  pthread_mutex_t attrMutex;
  void *devListener;

public:
  PresentationContext ();
  virtual ~PresentationContext ();

  void setPropertyValue (const string &propertyName, const string &value);
  void incPropertyValue (const string &propertyName);
  void decPropertyValue (const string &propertyName);
  set<string> *getPropertyNames ();
  string getPropertyValue (const string &attributeId);

private:
  void initializeUserContext ();
  void initializeUserInfo (int currentUserId);
  void initializeSystemValues ();

public:
  void save ();
  void setGlobalVarListener (IContextListener *listener);
  void receiveGlobalAttribution (const string &pName, const string &value);
};

GINGA_FORMATTER_END

#endif //_PRESENTATIONCONTEXT_H_
