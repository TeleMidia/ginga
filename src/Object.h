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

#include "StateMachine.h"

GINGA_NAMESPACE_BEGIN

class Document;
class Composition;
class MediaSettings;

/// Object in an NCL document.
class Object
{
public:

  /// Types of NCL objects.
  enum Type
  {
     CONTEXT,                   ///< Context object.
     SWITCH,                    ///< Switch object.
     MEDIA,                     ///< Media object.
  };

  /// Creates a new object.
  ///
  /// The newly created object has a single presentation state machine: the
  /// lambda state machine, called `@lambda`.
  ///
  /// @param doc The container document.
  /// @param id The id of the new object (must not occur in \p doc).
  /// @return The newly created object.
  ///
  Object (Document *doc, const string &id);

  /// Destroys the object and all its state machines.
  virtual ~Object ();

  /// Tests whether this document is a composition.
  bool isComposition ();

  /// Gets the container document of object.
  Document *getDocument ();

  /// Gets the type of object.
  Object::Type getType ();

  /// Gets the id of object.
  string getId ();

  /// Gets the set of parents of object.
  void getParents (set<Composition *> *parents);

  /// Gets the set of state machines in object.
  void getStateMachines (set<StateMachine *> *machines);

  /// Gets the state machine in object with the given type and id.
  ///
  /// @param type The type to match
  /// @param id The id to match.
  /// @return The matched state machine or \c NULL (no such state machine).
  ///
  StateMachine *getStateMachine (StateMachine::Type type, const string &id);

  /// Gets the lambda presentation state machine of object.
  StateMachine *getLambda ();

  /// Creates a new state machine and adds it to object.
  ///
  /// @param type The type of the new state machine.
  /// @param id the id of the new state machine (must no occur in \p
  ///        object).
  /// @return The newly created state machine if successful, or \c NULL
  ///         otherwise.
  ///
  /// @see Document::createStateMachine().
  ///
  StateMachine *createStateMachine (StateMachine::Type type,
                                    const string &id);

  /// Gets the playback time of object.
  lua_Integer getTime ();

  /// Sets the playback time of object.
  void setTime (lua_Integer time);

  /// Gets the value of an object property.
  virtual bool getProperty (const string &name, GValue *value);

  /// Gets the value of a boolean property.
  bool getPropertyBool (const string &name, bool *value);

  /// Gets the value of an integer property.
  bool getPropertyInteger (const string &name, lua_Integer *value);

  /// Gets the value of a number property.
  bool getPropertyNumber (const string &name, lua_Number *value);

  /// Gets the value of a string property.
  bool getPropertyString (const string &name, string *value);

  /// Sets the value of an object property.
  virtual void setProperty (const string &name, const GValue *value);

  /// Sets the value of a boolean property.
  void setPropertyBool (const string &name, bool value);

  /// Sets the value of an integer property.
  void setPropertyInteger (const string &name, lua_Integer value);

  /// Sets the value of a number property.
  void setPropertyNumber (const string &name, lua_Number value);

  /// Sets the value of a string property.
  void setPropertyString (const string &name, const string &value);

  /// Unsets an object property.
  void unsetProperty (const string &name);

protected:

  /// The Lua state associated with this object.
  lua_State *_L;
};

GINGA_NAMESPACE_END

#endif // GINGA_OBJECT_H
