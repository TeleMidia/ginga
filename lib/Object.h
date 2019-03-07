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

#ifndef GINGA_OBJECT_H
#define GINGA_OBJECT_H

#include "Event.h"

GINGA_NAMESPACE_BEGIN

class Document;
class Composition;
class MediaSettings;

/// Object in an NCL document.
///
/// Run-time representation of an NCL object.
///
/// @see Context, Switch, Media, MediaSettings.
///
class Object
{
public:

  /// Possible concrete types for NCL objects.
  enum Type
  {
     CONTEXT = 1 << 1,   ///< Context object.
     SWITCH  = 1 << 2,   ///< Switch object.
     MEDIA   = 1 << 3,   ///< Media object.
  };

  /// Creates a new object.
  ///
  /// The newly created object has a single presentation event: the lambda
  /// event, called `@lambda`.
  ///
  /// @param doc The container document.
  /// @param parent The parent object to add the new object to or \p NULL
  ///               (no parent).
  /// @param id The id of the new object (must not occur in \p doc).
  /// @return The newly created object.
  ///
  /// @warning Do not call the constructors of Object subclasses directly.
  /// Use Document::createObject() to create objects.
  ///
  Object (Document *doc, Composition *parent, const string &id);

  /// Destroys the object and all its events.
  virtual ~Object ();

  /// Gets the type of object.
  virtual Object::Type getType () = 0;

  /// Gets the container document of object.
  Document *getDocument ();

  /// Gets the parent of object.
  ///
  /// @return The parent object or \c NULL (no parent).
  ///
  Composition *getParent ();

  /// Gets the id of object.
  string getId ();

  /// Gets the list of aliases of object.
  const list<pair<string, Composition *> > *getAliases ();

  /// Tests whether object has the given alias.
  bool hasAlias (const string &alias);

  /// Adds alias to object.
  ///
  /// @param alias The alias to add.
  /// @param comp The composition where this alias occur or \p NULL (none).
  ///
  void addAlias (const string &alias, Composition *comp);

  /// Gets the current value of an object property.
  virtual string getProperty (const string &name);

  /// Sets the current value of an object property.
  virtual void setProperty (const string &name, const string &value,
                            Time duration = 0);

  /// Gets the set of events in object whose type match \p mask.
  ///
  /// @param[out] events The set of matched events.
  /// @param mask A bitmask of or-ed Event::Type values.
  ///
  void getEvents (set <Event *> *events,
                  unsigned int mask=(unsigned int) -1);

  /// Gets the event in object with the given type and id.
  ///
  /// @param type The type to match
  /// @param id The id to match.
  /// @return The matched event or \c NULL (no such event).
  ///
  Event *getEvent (Event::Type type, const string &id);

  /// Creates a new event and adds it to object.
  ///
  /// @param type The type of the new event.
  /// @param id the id of the new event (must no occur in \p object).
  /// @return The newly created event if successful, or \c NULL otherwise.
  ///
  /// @see Document::createEvent().
  ///
  Event *createEvent (Event::Type type, const string &id);

  // TODO ------------------------------------------------------------------

  Event *getLambda ();
  bool isOccurring ();
  bool isPaused ();
  bool isSleeping ();

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
  virtual bool beforeTransition (Event *evt,
                                 Event::Transition transition) = 0;

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
  virtual bool afterTransition (Event *evt,
                                Event::Transition transition) = 0;

protected:

  /// The container document of this object.
  Document *_doc;

  /// The Lua state associated with the container document.
  lua_State *_L;

  /// The parent of this object.
  Composition *_parent;

  /// The id of this object.
  string _id;

  /// The aliases of this object and the compositions where they occur.
  list<pair<string, Composition *> > _aliases;

  /// A map with the properties of this object indexed by name.
  map<string, string> _properties;

  /// The set of all events in this object (including lambda).
  set<Event *> _events;

  /// The lambda (presentation) event of this object.
  Event *_lambda;

  /// The playback time of this object.
  Time _time;

  /// Initializes the set of events of this object.
  void _initEvents ();

  /// Finalizes the set of events of this object.
  void _finiEvents ();

  // TODO ------------------------------------------------------------------

  /// Delayed actions.
  list<pair<Action, Time> > _delayed;

  virtual void doStart ();
  virtual void doStop ();
};

GINGA_NAMESPACE_END

#endif // GINGA_OBJECT_H
