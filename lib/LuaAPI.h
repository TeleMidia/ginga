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

/**
 * The Lua interface to the internal model.
 *
 * This class implements the Lua interface to the objects of the internal
 * model (Document, Object, Event, etc.).  We expose these C++ objects to
 * Lua using wrappers.  By _wrapper_ we mean a full Lua userdata consisting
 * of a single pointer, which points to the actual C++ object.  We call the
 * object associated with a Lua wrapper its _underlying object_.  We keep
 * the mapping between C++ objects and their wrappers in the `LUA_REGISTRY`.
 * That is, for each C++ object `o` we set `LUA_REGISTRY[o]=w`, where `w` is
 * the Lua wrapper of object `o`.
 */
class LuaAPI
{
public:

  /**
   * Attaches Lua wrapper to \p doc.
   */
  static void _Document_attachWrapper (lua_State *L, Document *doc);

  /**
   * Detaches Lua wrapper from \p doc.
   */
  static void _Document_detachWrapper (lua_State *L, Document *doc);

  /**
   * Attaches Lua wrapper to \p ctx.
   */
  static void _Context_attachWrapper (lua_State *L, Context *ctx);

  /**
   * Detaches Lua wrapper from \p ctx.
   */
  static void _Context_detachWrapper (lua_State *L, Context *ctx);

  /**
   * Attaches Lua wrapper to \p swtch.
   */
  static void _Switch_attachWrapper (lua_State *L, Switch *swtch);

  /**
   * Detaches Lua wrapper from \p swtch.
   */
  static void _Switch_detachWrapper (lua_State *L, Switch *swtch);

  /**
   * Attaches Lua wrapper to \p media.
   */
  static void _Media_attachWrapper (lua_State *L, Media *media);

  /**
   * Detaches Lua wrapper from \p media.
   */
  static void _Media_detachWrapper (lua_State *L, Media *media);

  /**
   * Attaches Lua wrapper to \p evt.
   */
  static void _Event_attachWrapper (lua_State *L, Event *evt);

  /**
   * Detaches Lua wrapper from \p evt.
   */
  static void _Event_detachWrapper (lua_State *L, Event *evt);

private:

  // Document --------------------------------------------------------------

  /**
   * Registry key for the Document metatable.
   */
  static const char *DOCUMENT;

  /**
   * Lua code to run when loading Document metatable.
   */
  static unsigned char Document_initMt_lua[];

  /**
   * Length in bytes of LuaAPI::Document_initMt_lua.
   */
  static unsigned int Document_initMt_lua_len;

  /**
   * Checks if the value at index \p i of stack is a Document wrapper.
   */
  static Document *_Document_check (lua_State *L, int i);

  static int __l_Document_toString (lua_State *L);

  static int __l_Document_getUnderlyingObject (lua_State *L);

  static int l_Document_getObject (lua_State *L);

  static int l_Document_getObjects (lua_State *L);

  static int l_Document_getRoot (lua_State *L);

  static int l_Document_getSettings (lua_State *L);

  static int l_Document_createObject (lua_State *L);

  // Object ----------------------------------------------------------------

  /**
   * Lua code to run when loading Object metatable.
   */
  static unsigned char Object_initMt_lua[];

  /**
   * Length in bytes of LuaAPI::Object_initMt_lua.
   */
  static unsigned int Object_initMt_lua_len;

  /**
   * Array of Object::Type names; used in luaL_checkoption() calls.
   */
  static const char *const _Object_optTypes[];

  /**
   * Gets the Object::Type associated with index \p i of
   * LuaAPI::_Object_optTypes.
   */
  static Object::Type _Object_getOptIndexType (int i);

  /**
   * Gets the registry key of the metatable of \p obj.
   */
  static const char *_Object_getRegistryKey (Object *obj);

  /**
   * Attaches Lua wrapper to \p obj.
   */
  static void _Object_attachWrapper (lua_State *L, Object *obj);

  /**
   * Detaches Lua wrapper from \p obj.
   */
  static void _Object_detachWrapper (lua_State *L, Object *obj);

  /**
   * Checks if the value at index \p i of stack is an Object wrapper.
   */
  static Object *_Object_check (lua_State *L, int i);

  static int __l_Object_toString (lua_State *L);

  static int __l_Object_getUnderlyingObject (lua_State *L);

  static int l_Object_getType (lua_State *L);

  static int l_Object_getDocument (lua_State *L);

  static int l_Object_getParent (lua_State *L);

  static int l_Object_getId (lua_State *L);

  static int l_Object_getProperty (lua_State *L);

  static int l_Object_setProperty (lua_State *L);

  static int l_Object_getEvents (lua_State *L);

  static int l_Object_getEvent (lua_State *L);

  // Context ---------------------------------------------------------------

  /**
   * Registry key for the Context metatable.
   */
  static const char *CONTEXT;

  /**
   * Checks if the value at index \p i of stack is a Context wrapper.
   */
  static Context *_Context_check (lua_State *L, int i);

  // Switch:

  /**
   * Registry key for the Switch metatable.
   */
  static const char *SWITCH;

  /**
   * Checks if the value at index \p i of stack is a Switch wrapper.
   */
  static Switch *_Switch_check (lua_State *L, int i);

  // Media -----------------------------------------------------------------

  /**
   * Registry key for the Media metatable.
   */
  static const char *MEDIA;

  /**
   * Checks if the value at index \p i of stack is a Media wrapper.
   */
  static Media *_Media_check (lua_State *L, int i);

  // Event -----------------------------------------------------------------

  /**
   * Registry key for the Event metatable.
   */
  static const char *EVENT;

  /**
   * Lua code to run when loading Event metatable.
   */
  static unsigned char Event_initMt_lua[];

  /**
   * Length in bytes of LuaAPI::Event_initMt_lua.
   */
  static unsigned int Event_initMt_lua_len;

  /**
   * Array of Event::Type names; used in luaL_checkoption() calls.
   */
  static const char *const _Event_optTypes[];

  /**
   * Gets the Event::Type associated with index \p i of
   * LuaAPI::_Event_optTypes.
   */
  static Event::Type _Event_getOptIndexType (int i);

  /**
   * Checks if the value at index \p i of stack is an Event wrapper.
   */
  static Event *_Event_check (lua_State *L, int i);

  static int __l_Event_toString (lua_State *L);

  static int __l_Event_getUnderlyingObject (lua_State *L);

  static int l_Event_getType (lua_State *L);

  static int l_Event_getObject (lua_State *L);

  static int l_Event_getId (lua_State *L);

  static int l_Event_getQualifiedId (lua_State *L);

  static int l_Event_getState (lua_State *L);

  // Auxiliary -------------------------------------------------------------

  /**
   * Loads the metatable of a Lua wrapper (if not already loaded).
   *
   * @param L Lua state
   * @param funcs The functions to install in the metatable.
   * @param name The name for the metatable in LUA_REGISTRY.
   * @param chunk A Lua chunk to run immediately after the metatable is
   *              loaded, or NULL (no chunk).  If given, \p chunk is loaded
   *              and called with the newly created metatable as first
   *              argument.
   * @param len Then length of \p chunk in bytes.
   */
  static void
  loadLuaWrapperMt (lua_State *L, const luaL_Reg *funcs, const char *name,
                    const char *chunk, size_t len)
  {
    luaL_getmetatable (L, name);
    if (!lua_isnil (L, -1))
      {
        lua_pop (L, 1);
        return;                 // nothing to do
      }

    lua_pop (L, 1);
    //luaL_newmetatable (L, name);
    luax_newmetatable (L, name);
    luaL_setfuncs (L, funcs, 0);

    if (chunk == NULL)
      {
        lua_pop (L, 1);
        return;
      }

    // Load and call chunk.
    if (unlikely (luaL_loadbuffer (L, chunk, len, name) != LUA_OK))
      {
        luax_dump_stack (L);
        ERROR ("%s", lua_tostring (L, -1));
      }
    lua_insert (L, -2);
    if (unlikely (lua_pcall (L, 1, 0, 0) != LUA_OK))
      {
        luax_dump_stack (L);
        ERROR ("%s", lua_tostring (L, -1));
      }
  }

  /**
   * Pops a value from the stack and sets it as the Lua wrapper of \p ptr.
   */
  static void
  attachLuaWrapper (lua_State *L, void *ptr)
  {
    lua_pushvalue (L, LUA_REGISTRYINDEX);
    lua_insert (L, -2);
    lua_rawsetp (L, -2, ptr);
    lua_pop (L, 1);
  }

  /**
   * Detaches Lua wrapper from \p ptr.
   */
  static void
  detachLuaWrapper (lua_State *L, void *ptr)
  {
    lua_pushnil (L);
    attachLuaWrapper (L, ptr);
  }

  /**
   * Pushes the Lua wrapper of \p ptr onto stack.
   */
  static void
  pushLuaWrapper (lua_State *L, void *ptr)
  {
    lua_pushvalue (L, LUA_REGISTRYINDEX);
    lua_rawgetp (L, -1, ptr);
    lua_remove (L, -2);
  }

  /**
   * Calls a method of the Lua wrapper of \p ptr.
   *
   * @param L Lua state.
   * @param ptr The pointer whose wrapper is to be called.
   * @param name The name of the method to call.
   * @param nargs The number of arguments to the method.
   * @param nresults The number of results of the method.
   */
  static void
  callLuaWrapper (lua_State *L, void *ptr, const char *name,
                  int nargs, int nresults)
  {
    LuaAPI::pushLuaWrapper (L, ptr);
    g_assert (luaL_getmetafield (L, -1, name) != LUA_TNIL);

    lua_insert (L, (-nargs) -2);
    lua_insert (L, (-nargs) -1);

    if (unlikely (lua_pcall (L, nargs + 1, nresults, 0) != LUA_OK))
      {
        luax_dump_stack (L);
        ERROR ("%s", lua_tostring (L, -1));
      }
  }
};

GINGA_NAMESPACE_END

#endif // GINGA_LUA_API_H
