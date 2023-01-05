/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef OBJECT_H
#define OBJECT_H

#include "Event.h"

namespace ginga {

class Document;
class Composition;
class MediaSettings;

class Object
{
public:
  explicit Object (const string &);
  virtual ~Object ();

  string getId ();

  Document *getDocument ();
  void initDocument (Document *);

  Composition *getParent ();
  void initParent (Composition *);

  virtual string getObjectTypeAsString () = 0;
  virtual string toString ();

  const list<pair<string, Composition *> > *getAliases ();
  bool hasAlias (const string &);
  void addAlias (const string &, Composition * = nullptr);

  const set<Event *> *getEvents ();
  Event *getEvent (Event::Type, const string &);
  Event *getAttributionEvent (const string &);
  void addAttributionEvent (const string &);
  Event *getPresentationEvent (const string &);
  Event *getPresentationEventByLabel (const string &);
  void addPresentationEvent (const string &, Time, Time);
  void addPresentationEvent (const string &, const string &);
  Event *getSelectionEvent (const string &);
  void addSelectionEvent (const string &);
  Event *getLookAtEvent (const string &);
  void addLookAtEvent (const string &);

  Event *getLambda ();
  bool isOccurring ();
  bool isPaused ();
  bool isSleeping ();

  virtual string getProperty (const string &);
  virtual void setProperty (const string &, const string &, Time dur = 0);

  const list<pair<Action, Time> > *getDelayedActions ();
  void addDelayedAction (Event *, Event::Transition,
                         const string &value = "", Time delay = 0);

  virtual void sendKey (const string &, bool);
  virtual void sendTick (Time, Time, Time);

  Time getTime ();

  /**
   * @brief Initiates event transition.
   *
   * This function is called by Event::transition() immediately before
   * transitioning \p evt.  If the transition can go on, the function
   * returns \c true.  Otherwise, if the transition must be cancelled, e.g.,
   * due to some error, the function returns false.
   *
   * @param evt Event to be transitioned.
   * @param transition The desired transition.
   * @return \c true if successful, or \c false otherwise (cancel
   * transition).
   */
  virtual bool beforeTransition (Event *, Event::Transition) = 0;

  /**
   * @brief Finishes event transition.
   *
   * This function is called by Event::transition() immediately after
   * transitioning \p evt.  If the transition can finish successfully, the
   * function returns \c true.  Otherwise, if the transition must be
   * reverted, e.g., due to some error, the function returns false.
   *
   * @param evt Event that was transitioned.
   * @param transition The transition.
   * @return \c true if successful, or \c false otherwise (cancel
   * transition).
   */
  virtual bool afterTransition (Event *, Event::Transition) = 0;

protected:
  string _id;                                  // id
  Document *_doc;                              // parent document
  Composition *_parent;                        // parent object
  list<pair<string, Composition *> > _aliases; // aliases
  Time _time;                                  // playback time
  map<string, string> _properties;             // property map
  Event *_lambda;                              // lambda event
  set<Event *> _events;                        // all events
  list<pair<Action, Time> > _delayed;          // delayed actions

  virtual void doStart ();
  virtual void doStop ();
};

}

#endif // OBJECT_H
