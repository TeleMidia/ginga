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

#ifndef GINGA_LUA_API_H
#define GINGA_LUA_API_H

#include "aux-ginga.h"
GINGA_BEGIN_DECLS
#include "aux-lua.h"
GINGA_END_DECLS

#include "Object.h"

GINGA_NAMESPACE_BEGIN

class Context;
class Document;
class Media;
class Switch;

/// The Lua interface to the internal model.
///
/// This class implements the Lua interface to the objects of the internal
/// model (Document, Object, Event, etc.).  We expose these C++ objects to
/// Lua using wrappers.  By _wrapper_ we mean a full Lua userdata consisting
/// of a single pointer, which points to the actual C++ object.  We call the
/// object associated with a Lua wrapper its _underlying object_.  We keep
/// the mapping between C++ objects and their wrappers in the
/// `LUA_REGISTRY`.  That is, for each C++ object `o` we set
/// `LUA_REGISTRY[o]=w`, where `w` is the Lua wrapper of object `o`.
///
class LuaAPI
{
  // Document --------------------------------------------------------------

public:

  /// Attaches Lua wrapper to \p doc.
  static void Document_attachWrapper (lua_State *L, Document *doc);

  /// Detaches Lua wrapper from \p doc.
  static void Document_detachWrapper (lua_State *L, Document *doc);

  /// Checks if the value at index \p i of stack is a Document wrapper.
  static Document *Document_check (lua_State *L, int i);

  //// Pushes the Lua wrapper of document onto stack.
  static void Document_push (lua_State *L, Document *doc);

  /// Calls a method of the Lua wrapper of the given document.
  static void Document_call (lua_State *L, Document *doc, const char *name,
                             int nargs, int nresults);

private:

  /// Registry key for the Document metatable.
  static const char *_DOCUMENT;

  /// The functions to load in the metatable of Document.
  static const struct luaL_Reg _Document_funcs[];

  static int __l_Document_gc (lua_State *L);

  static int _l_Document_getUnderlyingObject (lua_State *L);

  static int _l_Document_createObject (lua_State *L);

  static int _l_Document_createEvent (lua_State *L);

  // Object ----------------------------------------------------------------

public:

  /// Attaches Lua wrapper to \p obj.
  static void Object_attachWrapper (lua_State *L, Object *obj);

  /// Detaches Lua wrapper from \p obj.
  static void Object_detachWrapper (lua_State *L, Object *obj);

  /// Checks if the value at index \p i of stack is an Object wrapper.
  static Object *Object_check (lua_State *L, int i);

  /// Checks if the value at index \p i of stack is an Object::Type.
  static Object::Type Object_Type_check (lua_State *L, int i);

  /// Checks if the value at index \p i of stack is an Object::Type mask.
  static unsigned int Object_Type_Mask_check (lua_State *L, int i);

  /// Pushes the Lua wrapper of object onto stack.
  static void Object_push (lua_State *L, Object *obj);

  /// Pushes Object::Type (as string) onto stack.
  static void Object_Type_push (lua_State *L, Object::Type type);

  /// Pushes Object::Type bit-mask (as table) onto stack.
  static void Object_Type_Mask_push (lua_State *L, unsigned int mask);

private:

  /// Gets the registry key of the metatable of \p obj.
  static const char *_Object_getRegistryKey (Object *obj);

  /// The functions to load in the metatable of Object.
  static const struct luaL_Reg _Object_funcs[];

  /// Attaches Lua wrapper to \p obj.
  static void _Object_attachWrapper (lua_State *L, Object *obj);

  /// Detaches Lua wrapper from \p obj.
  static void _Object_detachWrapper (lua_State *L, Object *obj);

  static int _l_Object_getUnderlyingObject (lua_State *L);

  static int _l_Object_getType (lua_State *L);

  static int _l_Object_getDocument (lua_State *L);

  static int _l_Object_getParent (lua_State *L);

  static int _l_Object_getId (lua_State *L);

  static int _l_Object_getProperty (lua_State *L);

  static int _l_Object_setProperty (lua_State *L);

  static int _l_Object_getEvents (lua_State *L);

  static int _l_Object_getEvent (lua_State *L);

  // Composition -----------------------------------------------------------

public:

  /// Checks if the value at index \p i of stack is a Composition wrapper.
  static Composition *Composition_check (lua_State *L, int i);

  /// The functions to load in the metatable of Composition.
  static const struct luaL_Reg _Composition_funcs[];

private:

  // Context, switch, and media --------------------------------------------

public:

  /// Checks if the value at index \p i of stack is a Context wrapper.
  static Context *Context_check (lua_State *L, int i);

  /// Checks if the value at index \p i of stack is a Switch wrapper.
  static Switch *Switch_check (lua_State *L, int i);

  /// Checks if the value at index \p i of stack is a Media wrapper.
  static Media *Media_check (lua_State *L, int i);

private:

  /// Registry key for the Context metatable.
  static const char *_CONTEXT;

  /// Registry key for the Switch metatable.
  static const char *_SWITCH;

  /// Registry key for the Media metatable.
  static const char *_MEDIA;

  // Event -----------------------------------------------------------------

public:

  /// Attaches Lua wrapper to \p evt.
  static void Event_attachWrapper (lua_State *L, Event *evt);

  /// Detaches Lua wrapper from \p evt.
  static void Event_detachWrapper (lua_State *L, Event *evt);

  /// Checks if the value at index \p i of stack is an Event wrapper.
  static Event *Event_check (lua_State *L, int i);

  /// Checks if the value at index \p i of stack is an Event::Type.
  static Event::Type Event_Type_check (lua_State *L, int i);

  /// Checks if the value at index \p i of stack is an Event::State.
  static Event::State Event_State_check (lua_State *L, int i);

  //// Pushes the Lua wrapper of event onto stack.
  static void Event_push (lua_State *L, Event *doc);

  /// Pushes Event::Type (as string) onto stack.
  static void Event_Type_push (lua_State *L, Event::Type type);

  /// Pushes Event::State (as string) onto stack.
  static void Event_State_push (lua_State *L, Event::State state);

  /// Calls a method of the Lua wrapper of the given event.
  static void Event_call (lua_State *L, Event *doc, const char *name,
                          int nargs, int nresults);

private:

  /// Registry key for the Event metatable.
  static const char *_EVENT;

  /// The functions to load in the metatable of Event.
  static const struct luaL_Reg _Event_funcs[];

  static int _l_Event_getUnderlyingObject (lua_State *L);

  static int _l_Event_getType (lua_State *L);

  static int _l_Event_getObject (lua_State *L);

  static int _l_Event_getId (lua_State *L);

  static int _l_Event_getQualifiedId (lua_State *L);

  static int _l_Event_getState (lua_State *L);

  static int _l_Event_setState (lua_State *L);

  static int _l_Event_getBeginTime (lua_State *L);

  static int _l_Event_setBeginTime (lua_State *L);

  static int _l_Event_getEndTime (lua_State *L);

  static int _l_Event_setEndTime (lua_State *L);

  static int _l_Event_getLabel (lua_State *L);

  static int _l_Event_setLabel (lua_State *L);

  // Auxiliary -------------------------------------------------------------

public:

  /// Datatype for loadable Lua chunks.
  typedef struct
  {
    const char *name;
    const char *text;
    size_t len;
  } Chunk;

  // Chunk containing the code in LuaAPI_initMt.lua.
  static Chunk _initMt;
  static unsigned char initMt_lua[];
  static unsigned int initMt_lua_len;

  /// Chunk containing the code in LuaAPI_Document_initMt.lua.
  static Chunk _Document_initMt;
  static unsigned char Document_initMt_lua[];
  static unsigned int Document_initMt_lua_len;

  /// Chunk containing the code in LuaAPI_Object_initMt.lua.
  static Chunk _Object_initMt;
  static unsigned char Object_initMt_lua[];
  static unsigned int Object_initMt_lua_len;

  /// Chunk containing the code in LuaAPI_Composition_initMt.lua.
  static Chunk _Composition_initMt;
  static unsigned char Composition_initMt_lua[];
  static unsigned int Composition_initMt_lua_len;

  /// Chunk containing the code in LuaAPI_Event_initMt.lua.
  static Chunk _Event_initMt;
  static unsigned char Event_initMt_lua[];
  static unsigned int Event_initMt_lua_len;

private:

  /// Loads the metatable of a Lua wrapper (if not already loaded).
  ///
  /// @param L Lua state
  /// @param name The name for the metatable in LUA_REGISTRY.
  /// @param funcs A `NULL`-terminated array of luaL_Reg entries to install
  ///              or NULL (no entries to install).
  /// @param chunks A `NULL`-terminated array of Chunk or NULL (no chunks to
  ///              load).  If the array is given, each Lua chunk in it array
  ///              is run immediately after the metatable is loaded; each
  ///              receives the metatable as the first argument.
  /// @return \c true if the metatable was loaded in this call, or \c false
  ///         otherwise (metatable already loaded).
  ///
  static bool _loadLuaWrapperMt (lua_State *L, const char *name,
                                 const luaL_Reg *const funcs[],
                                 const Chunk *const chunks[]);

  /// Pops a value from the stack and sets it as the Lua wrapper of \p ptr.
  static void _attachLuaWrapper (lua_State *L, void *ptr);

  /// Detaches Lua wrapper from \p ptr.
  static void _detachLuaWrapper (lua_State *L, void *ptr);

  /// Pushes the Lua wrapper of \p ptr onto stack.
  static void _pushLuaWrapper (lua_State *L, void *ptr);

  /// Calls a method of the Lua wrapper of \p ptr.
  ///
  /// @param L Lua state.
  /// @param ptr The pointer whose wrapper is to be called.
  /// @param name The name of the method to call.
  /// @param nargs The number of arguments to the method.
  /// @param nresults The number of results of the method.
  ///
  static void _callLuaWrapper (lua_State *L, void *ptr, const char *name,
                               int nargs, int nresults);
};

GINGA_NAMESPACE_END

#endif // GINGA_LUA_API_H
