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
 * @brief The NCL document.
 *
 * Run-time representation of an NCL document.
 * The Document maintains the NCL object tree.
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
   * @see Document::getRoot(), Document::getSettingsObject().
   */
  Document ();

  /**
   * @brief Destroys document and all its objects.
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
   * @brief Gets the set of objects in document whose type match \p mask.
   * @param[out] objects The set of matched objects.
   * @param mask A bitmask of or-ed Object::Type values.
   */
  void getObjects (set<Object *> *objects, uint mask=(uint) -1);

  /**
   * @brief Gets the object in document with the given id or alias.
   * @param id The id or alias to match.
   * @return The matched object or \c NULL (no such object).
   */
  Object *getObjectById (const string &id);

  /**
   * @brief Gets the root Context of document.
   * @return The root Context of document.
   */
  Context *getRoot ();

  /**
   * @brief Gets the MediaSettings of document.
   * @return The MediaSettings of document.
   */
  MediaSettings *getSettings ();

  /**
   * @brief Adds object to document.
   * @param object The object to add.
   * @return \c true if successful, or \c false otherwise.
   */
  bool addObject (Object *object);

  /**
   * @brief Creates and adds a new object to document.
   * @param type The type of the new object.
   * @param parent The parent object to add the new object to.
   * @param id The id of the new object (must not occur in document).
   * @return The newly created object if successful, or \c NULL otherwise.
   */
  Object *createObject (Object::Type type, Composition *parent,
                        const string &id);

  // TODO ------------------------------------------------------------------

  int evalAction (Event *, Event::Transition, const string &value = "");

  int evalAction (Action);

  bool evalPredicate (Predicate *);

  bool evalPropertyRef (const string &, string *);

  bool getData (const string &, void **);

  bool setData (const string &, void *, UserDataCleanFunc fn = nullptr);

private:
  /// The Lua state associate with this document.
  lua_State *_L;

  /// The set of all objects in this document.
  set<Object *> _objects;

  /// A map with all objects in this document indexed by id.
  map<string, Object *> _objectsById;

  /// The root Context (body) of this document.
  Context *_root;

  /// The settings object (MediaSettings) of this document.
  MediaSettings *_settings;

  /// User data attached to this document.
  UserData _udata;

  // TODO ------------------------------------------------------------------
  list<Action> evalActionInContext (Action, Context *);
};

GINGA_NAMESPACE_END

#endif // GINGA_DOCUMENT_H
