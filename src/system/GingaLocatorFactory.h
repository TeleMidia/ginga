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

#ifndef GINGALOCATORFACTORY_H_
#define GINGALOCATORFACTORY_H_

#include "util/functions.h"
using namespace ::ginga::util;

#include "Thread.h"

GINGA_SYSTEM_BEGIN

class GingaLocatorFactory
{
private:
  static GingaLocatorFactory *_instance;
  map<string, string> *iorClients;
  map<string, string> *iorProviders;
  map<string, string> *iorNames;

  bool isWaiting;
  pthread_mutex_t flagMutexSignal;
  pthread_cond_t flagCondSignal;

  GingaLocatorFactory ();
  ~GingaLocatorFactory ();

public:
  void release ();
  static GingaLocatorFactory *getInstance ();
  void createLocator (const string &providerUri, const string &ior);
  string getLocation (const string &ior);
  string getName (const string &ior);
  void addClientLocation (const string &ior, const string &clientUri, const string &name);
  void waitNewLocatorCondition ();
  bool newLocatorConditionSatisfied ();
};

GINGA_SYSTEM_END

#endif /*GINGALOCATORFACTORY_H_*/
