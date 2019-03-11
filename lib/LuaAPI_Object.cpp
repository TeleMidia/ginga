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

#include "LuaAPI.h"
#include "Object.h"

#include "Context.h"
#include "Media.h"
#include "Switch.h"

#define OBJECT_CONTEXT_STRING  "context"
#define OBJECT_SWITCH_STRING   "switch"
#define OBJECT_MEDIA_STRING    "media"

// Public.

void
LuaAPI::Object_attachWrapper (lua_State *L, Object *obj, Document *doc,
                              Object::Type type, const string &id)
{
  const char *name;

  g_return_if_fail (L != NULL);
  g_return_if_fail (obj != NULL);

  // Create Lua wrapper for object.
  switch (type)
    {
    case Object::CONTEXT:
      {
        static const struct luaL_Reg *const funcs[] =
          {
           _funcs,
           NULL
          };

        static const Chunk *const chunks[] =
          {
           &LuaAPI::_initMt,
           &LuaAPI::_Object_initMt,
           &LuaAPI::_Composition_initMt,
           &LuaAPI::_Context_initMt,
           &LuaAPI::_traceMt,
           NULL
          };

        Context **wrapper;

        name = LuaAPI::_CONTEXT;
        LuaAPI::_loadLuaWrapperMt (L, name, funcs, chunks);
        wrapper = (Context **) lua_newuserdata (L, sizeof (Context **));
        g_assert_nonnull (wrapper);
        *wrapper = (Context *) obj;
        g_assert_nonnull (*wrapper);
        break;
      }
    case Object::SWITCH:
      {
        static const struct luaL_Reg *const funcs[] =
          {
           _funcs,
           NULL
          };

        static const Chunk *const chunks[] =
          {
           &LuaAPI::_initMt,
           &LuaAPI::_Object_initMt,
           &LuaAPI::_Composition_initMt,
           &LuaAPI::_traceMt,
           NULL
          };

        Switch **wrapper;

        name = LuaAPI::_SWITCH;
        LuaAPI::_loadLuaWrapperMt (L, name, funcs, chunks);
        wrapper = (Switch **) lua_newuserdata (L, sizeof (Switch **));
        g_assert_nonnull (wrapper);
        *wrapper = (Switch *) obj;
        g_assert_nonnull (*wrapper);
        break;
      }
    case Object::MEDIA:
      {
        static const struct luaL_Reg *const funcs[] =
          {
           _funcs,
           NULL
          };

        static const Chunk *const chunks[] =
          {
           &LuaAPI::_initMt,
           &LuaAPI::_Object_initMt,
           &LuaAPI::_Media_initMt,
           &LuaAPI::_traceMt,
           NULL
          };

        Media **wrapper;

        name = LuaAPI::_MEDIA;
        LuaAPI::_loadLuaWrapperMt (L, name, funcs, chunks);
        wrapper = (Media **) lua_newuserdata (L, sizeof (Media **));
        g_assert_nonnull (wrapper);
        *wrapper = (Media *) obj;
        g_assert_nonnull (*wrapper);
        break;
      }
    default:
      g_assert_not_reached ();
    }

  luaL_setmetatable (L, name);

  // Set LUA_REGISTRY[obj]=wrapper.
  LuaAPI::_attachLuaWrapper (L, obj);

  // Call obj:_attachData().
  LuaAPI::Document_push (L, doc);
  LuaAPI::Object_Type_push (L, type);
  lua_pushstring (L, id.c_str ());
  LuaAPI::_callLuaWrapper (L, obj, "_attachData", 3, 0);
}

void
LuaAPI::Object_detachWrapper (lua_State *L, Object *obj)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (obj != NULL);

  // Call obj:_detachData().
  LuaAPI::_callLuaWrapper (L, obj, "_detachData", 0, 0);

  // Set LUA_REGISTRY[obj] = nil.
  LuaAPI::_detachLuaWrapper (L, obj);
}

Object *
LuaAPI::Object_check (lua_State *L, int i)
{
  const char *name;

  luaL_checktype (L, i, LUA_TUSERDATA);
  if (unlikely (!lua_getmetatable (L, i)))
    {
      goto fail;
    }

  lua_getfield (L, -1, "__name");
  name = lua_tostring (L, -1);
  if (unlikely (name == NULL))
    {
      lua_pop (L, 1);
      goto fail;
    }

  lua_pop (L, 2);
  if (g_str_equal (name, LuaAPI::_CONTEXT))
    return LuaAPI::Context_check (L, i);
  else if (g_str_equal (name, LuaAPI::_SWITCH))
    return LuaAPI::Switch_check (L, i);
  else if (g_str_equal (name, LuaAPI::_MEDIA))
    return LuaAPI::Media_check (L, i);

 fail:
  luaL_argcheck (L, 0, i, "Ginga.Object expected");
  return NULL;
}

Object::Type
LuaAPI::Object_Type_check (lua_State *L, int i)
{
  static const char *types[] = {OBJECT_CONTEXT_STRING,
                                OBJECT_SWITCH_STRING,
                                OBJECT_MEDIA_STRING,
                                NULL};

  g_return_val_if_fail (L != NULL, Object::CONTEXT);

  switch (luaL_checkoption (L, i, NULL, types))
    {
    case 0:
      return Object::CONTEXT;
    case 1:
      return Object::SWITCH;
    case 2:
      return Object::MEDIA;
    default:
      g_assert_not_reached ();
    }
}

void
LuaAPI::Object_push (lua_State *L, Object *obj)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (obj != NULL);

  LuaAPI::_pushLuaWrapper (L, obj);
}

void
LuaAPI::Object_Type_push (lua_State *L, Object::Type type)
{
  g_return_if_fail (L != NULL);

  switch (type)
    {
    case Object::CONTEXT:
      lua_pushliteral (L, OBJECT_CONTEXT_STRING);
      break;
    case Object::SWITCH:
      lua_pushliteral (L, OBJECT_SWITCH_STRING);
      break;
    case Object::MEDIA:
      lua_pushliteral (L, OBJECT_MEDIA_STRING);
      break;
    default:
      g_assert_not_reached ();
    }
}

void
LuaAPI::Object_call (lua_State *L, Object *obj, const char *name,
                     int nargs, int nresults)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (obj != NULL);
  g_return_if_fail (name != NULL);
  g_return_if_fail (nargs >= 0);
  g_return_if_fail (nresults >= 0);

  LuaAPI::_callLuaWrapper (L, obj, name, nargs, nresults);
}
