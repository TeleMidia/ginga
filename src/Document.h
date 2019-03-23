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

#ifndef GINGA_DOCUMENT_H
#define GINGA_DOCUMENT_H

#include "Object.h"

GINGA_NAMESPACE_BEGIN

class Context;
class Media;
class Switch;

/// Run-time representation of an NCL document.
///
/// The Document maintains the NCL object graph and acts a top-level
/// interface to this graph.
///
class Document
{
public:

  /// Creates a new document.
  ///
  /// The newly created document has a Context, called `__root__`, and a
  /// single Media object, called `__settings__`.
  ///
  /// @param L The Lua state to associate this document to.
  ///
  Document (lua_State *L);

  /// Destroys document and all its objects.
  ~Document ();

  /// Gets the Lua state associated with document.
  lua_State *getLuaState ();

  /// Gets the set of objects in document.
  void getObjects (set<Object *> *objects);

  /// Gets the object in document with the given id.
  ///
  /// @param id The id to match.
  /// @return The matched object or \c NULL (no such object).
  ///
  Object *getObject (const string &id);

  /// Gets the root Context of document.
  Context *getRoot ();

  /// Gets the settings Media object of document.
  Media *getSettings ();

  /// Creates a new object and adds it to document.
  Object *createObject (Object::Type type, const string &id);

  /// Gets the set of state machines in document.
  void getStateMachines (set<StateMachine *> *machines);

  /// Gets the state machine in document with the given qualified id.
  StateMachine *getStateMachine (const string &id);

  /// Creates a new state machine in object.
  StateMachine *createStateMachine (StateMachine::Type type,
                                    Object *obj, const string &smId);

  /// Creates a new state machine in object.
  StateMachine *createStateMachine (StateMachine::Type type,
                                    const string &objId,
                                    const string &smId);

  /// Creates a new state machine as specified by the qualified id.
  StateMachine *createStateMachine (const string &qualId);

  /// Gets the playback time of document.
  lua_Integer getTime ();

  /// Advances the playback time of document by the specified amount.
  void advanceTime (lua_Integer dt);

  /// Sends key press/release.
  void sendKey (const string &key, bool press);

  /// Draws the current frame of document into cairo context.
  void draw (cairo_t *cr);

private:

  /// The Lua state associate with this document.
  lua_State *_L;

  // TODO ------------------------------------------------------------------

public:
  int evalAction (StateMachine *, StateMachine::Transition, const string &value = "");

  int evalAction (Action);

  bool evalPredicate (Predicate *);

  bool evalPropertyRef (const string &, string *);

  bool getData (const string &, void **);

  bool setData (const string &, void *, UserDataCleanFunc fn = nullptr);


private:

  /// User data attached to this document.
  UserData _udata;

  // TODO ------------------------------------------------------------------
  list<Action> evalActionInContext (Action, Context *);
};

GINGA_NAMESPACE_END

#endif // GINGA_DOCUMENT_H
