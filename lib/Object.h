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

GINGA_NAMESPACE_BEGIN

class Document;
class Composition;
class MediaSettings;

/**
 * @brief Object in an NCL document.
 *
 * Run-time representation of an NCL object.
 *
 * @see Media, MediaSettings, Context, Switch.
 */
class Object
{
public:

  /**
   * @brief Possible concrete types for NCL objects.
   */
  enum Type
  {
    MEDIA          = 1 << 1,    ///< Media object.
    MEDIA_SETTINGS = 1 << 2,    ///< MediaSettings object.
    CONTEXT        = 1 << 3,    ///< Context object.
    SWITCH         = 1 << 4     ///< Switch object.
  };

  /**
   * @brief Converts Object::Type to a human-readable string.
   * @param type The Object::Type to convert.
   * @return The resulting string.
   */
  static string getTypeAsString (Object::Type type);

  /**
   * @brief Creates a new object.
   * @param doc The container document.
   * @param parent The parent object to add the new object to
   *        or \p NULL (no parent).
   * @param id The id of the new object (must not occur in \p doc).
   *
   * The newly created object has a single presentation event: the lambda
   * event, called <tt>@lambda</tt>.
   *
   * @see Object::createObject().
   */
  Object (Document *doc, Composition *parent, const string &id);

  /**
   * @brief Destroys the object and all its events.
   */
  virtual ~Object ();

  /**
   * @brief Gets a string representation of object.
   * @return A string representation of object.
   */
  virtual string toString ();

  /**
   * @brief Gets the type of object.
   * @return The type of object.
   */
  virtual Object::Type getType () = 0;

  /**
   * @brief Gets the container document of object.
   * @return The container document of object.
   */
  Document *getDocument ();

  /**
   * @brief Gets the parent of object.
   * @return The parent object or \c NULL (no parent).
   */
  Composition *getParent ();

  /**
   * @brief Gets the id of object.
   * @return The id of object.
   */
  string getId ();

  /**
   * @brief Gets the list of aliases of object.
   * @return The list of aliases of object.
   */
  const list<pair<string, Composition *> > *getAliases ();

  /**
   * @brief Tests whether object has alias.
   * @param alias The alias to test.
   * @return \c true if successful, or \c false otherwise.
   */
  bool hasAlias (const string &alias);

  /**
   * @brief Adds alias to object.
   * @param alias The alias to add.
   * @param comp The composition where this alias occur or \p NULL (none).
   */
  void addAlias (const string &alias, Composition *comp);

  /**
   * @brief Gets the current value of an object property.
   * @param name Property name.
   * @return Current value.
   */
  virtual string getProperty (const string &name);

  /**
   * @brief Sets the current value of an object property.
   * @param name Property name.
   * @param value Value to set.
   * @param duration The duration of the set action.
   */
  virtual void setProperty (const string &name, const string &value,
                            Time duration = 0);

  /**
   * @brief Gets the set of events in object.
   * @return The set of events in object.
   */
  const set<Event *> *getEvents ();

  // TODO ------------------------------------------------------------------

  Event *getEvent (Event::Type, const string &);
  Event *getAttributionEvent (const string &);
  void addAttributionEvent (const string &);
  Event *getPresentationEvent (const string &);
  Event *getPresentationEventByLabel (const string &);
  void addPresentationEvent (const string &, Time, Time);
  void addPresentationEvent (const string &, const string &);
  Event *getSelectionEvent (const string &);
  void addSelectionEvent (const string &);

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

  /// Container document.
  Document *_doc;

  /// Lua state associated with the container document.
  lua_State *_L;

  /// Parent object.
  Composition *_parent;

  /// Object id.
  string _id;

  /// Aliases of this object and the compositions where they occur.
  list<pair<string, Composition *> > _aliases;

  /// Total playback time.
  Time _time;

  /// Property map.
  map<string, string> _properties;

  /// The lambda (presentation) event.
  Event *_lambda;

  /// All events, including lambda.
  set<Event *> _events;

  /// Delayed actions.
  list<pair<Action, Time> > _delayed;

  virtual void doStart ();
  virtual void doStop ();
};

GINGA_NAMESPACE_END

#endif // OBJECT_H
