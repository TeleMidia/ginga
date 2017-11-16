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

#ifndef OBJECT_H
#define OBJECT_H

#include "Event.h"

GINGA_NAMESPACE_BEGIN

class Document;
class Composition;
class MediaSettings;

class Object
{
public:
  Object (const string &);
  virtual ~Object ();

  string getId ();

  Document *getDocument ();
  void initDocument (Document *);

  Composition *getParent ();
  void initParent (Composition *);

  const vector <string> *getAliases ();
  bool hasAlias (const string &);
  void addAlias (const string &);

  Event *getEvent (Event::Type, const string &);
  Event *getAttributionEvent (const string &);
  void addAttributionEvent (const string &);
  Event *getPresentationEvent (const string &);
  void addPresentationEvent (const string &, Time, Time);
  Event *getSelectionEvent (const string &);
  void addSelectionEvent (const string &);

  Event *getLambda ();
  bool isOccurring ();
  bool isPaused ();
  bool isSleeping ();

  virtual string getProperty (const string &);
  virtual void setProperty (const string &, const string &, Time dur=0);

  bool getData (const string &, void **);
  bool setData (const string &, void *);

  list<pair<Action, Time>> *getDelayedActions ();
  void addDelayedAction (Event *, Event::Transition,
                         const string &value="", Time delay=0);

  virtual void sendKeyEvent (const string &, bool);
  virtual void sendTickEvent (Time, Time, Time);

  virtual bool startTransition (Event *, Event::Transition) = 0;
  virtual void endTransition (Event *, Event::Transition) = 0;

protected:
  string _id;                      // id
  Document *_doc;                  // parent document
  Composition *_parent;            // parent object
  vector<string> _aliases;         // aliases

  Time _time;                      // playback time
  map<string, string> _properties; // property map
  map<string, void *> _userdata;   // userdata map

  Event *_lambda;                    // lambda event
  set<Event *> _events;              // all events
  list<pair<Action, Time>> _delayed; // delayed actions

  virtual void doStart ();
  virtual void doStop ();
};

GINGA_NAMESPACE_END

#endif // OBJECT_H
