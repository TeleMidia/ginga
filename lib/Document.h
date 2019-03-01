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

/**
 * @brief Run-time representation of an NCL document.
 *
 * The Document is a run-time representation of an NCL document.  It holds
 * and acts as a top-level interface to an NCL object tree.
 */
class Document
{
public:

  /**
   * @brief Creates a new document.
   *
   * The newly created document has a root Context, called
   * <tt>__root__</tt>, which contains a single MediaSettings object, called
   * <tt>__settings__</tt>.
   *
   * @return The newly created document.
   *
   * @see Document::getRoot()
   * @see Document::getSettingsObject()
   */
  Document ();

  /**
   * @brief Destroys document.
   *
   * Destroys document and all of its objects.
   */
  virtual ~Document ();

  /**
   * @brief Gets the Lua state associated with document.
   * @return The Lua state associated with document.
   */
  lua_State *getLuaState ();

  /**
   * @brief Gets a string representation of document.
   * @return A string representation of document.
   */
  string toString ();

  /**
   * @brief Gets the set of objects in document.
   * @return The set of objects in document.
   */
  const set<Object *> *getObjects ();

  /**
   * @brief Gets an object from document by its id.
   * @param id Object id.
   * @return Object or \c NULL (no such object).
   */
  Object *getObjectById (const string &id);

  /**
   * @brief Gets an object from document by its id or alias.
   * @param id Object id or alias.
   * @return Object or \c NULL (no such object).
   */
  Object *getObjectByIdOrAlias (const string &id);

  /**
   * @brief Adds an object to document.
   *
   * @param obj Object to add.
   * @return \c true if successful, or \c false otherwise (object already in
   * document).
   *
   * @warning \p obj must not be in another document.
   */
  bool addObject (Object *obj);

  /**
   * @brief Gets the root Context of document.
   * @return The root Context of document.
   */
  Context *getRoot ();

  /**
   * @brief Gets the MediaSettings of document.
   * @return The MediaSettings of document.
   */
  MediaSettings *getSettingsObject ();

  /**
   * @brief Gets the set of Media objects in document.
   * @param[out] medias The set of Media objects in document.
   */
  void getMediaObjects (set <Media *> *medias);

  /**
   * @brief Gets the set of Context objects in document.
   * @param[out] contexts The set of Context objects in document.
   */
  void getContextObjects (set <Context *> *contexts);

  /**
   * @brief Gets the set of Switch objects in document.
   * @param[out] switches The set of Switch objects in document.
   */
  void getSwitchObjects (set <Switch *> *switches);

  /**
   * @brief Creates and adds a new Media to document.
   * @param parent The parent object to add the new Media to.
   * @param id The id of the new Media (must not occur in document).
   * @return The newly created Media if successful, or \c NULL otherwise.
   */
  Media *createMedia (Composition *parent,
                      const string &id);

  // TODO

  // Context *createContext (Composition *parent, const string &id);
  // Switch *createSwitch (Composition *parent, const string &id);

  int evalAction (Event *, Event::Transition, const string &value = "");

  int evalAction (Action);

  bool evalPredicate (Predicate *);

  bool evalPropertyRef (const string &, string *);

  bool getData (const string &, void **);

  bool setData (const string &, void *, UserDataCleanFunc fn = nullptr);

private:
  list<Action> evalActionInContext (Action, Context *);

  /// Associated Lua state.
  lua_State *_L;

  /// Object set.
  set<Object *> _objects;

  /// Object map (indexed by id).
  map<string, Object *> _objectsById;

  /// The document's root Context (body).
  Context *_root;

  /// The document's settings node (MediaSettings object).
  MediaSettings *_settings;

  /// Attached user data.
  UserData _udata;
};

GINGA_NAMESPACE_END

#endif // GINGA_DOCUMENT_H
