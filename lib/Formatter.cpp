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

#include "aux-ginga.h"
#include "aux-gl.h"
#include "Formatter.h"

#include "LuaAPI.h"
#include "Context.h"
#include "Media.h"
#include "MediaSettings.h"
#include "Object.h"
#include "Switch.h"

#include "Parser.h"
#include "PlayerText.h"

GINGA_NAMESPACE_BEGIN

// Public: External API.

lua_State *
Formatter::getLuaState ()
{
  g_assert_nonnull (_doc);
  return _doc->getLuaState ();
}

bool
Formatter::start (const string &file, int w, int h, string *errmsg)
{
  lua_State *L;
  Event *evt;
  map<string, string> params;

  // Parse document.
  g_assert_null (_doc);
  _doc = nullptr;

  // if (xstrhassuffix (file, ".lua"))
  //   {
  //     _doc = ParserLua::parseFile (file, errmsg);
  //     if (unlikely (_doc == nullptr))
  //       return false;
  //   }

  if (_doc == nullptr)
    _doc = Parser::parseFile (file, w, h, errmsg);
  if (unlikely (_doc == nullptr))
    return false;

  g_assert_nonnull (_doc);
  _doc->setData ("formatter", (void *) this);

  Context *root = _doc->getRoot ();
  g_assert_nonnull (root);
  MediaSettings *settings = _doc->getSettings ();
  g_assert_nonnull (settings);

  settings->setPropertyInteger ("width", w);
  settings->setPropertyInteger ("height", h);

  // Initialize formatter variables.
  _docPath = file;

  // Run document.
  TRACE ("%s", file.c_str ());
  evt = root->getLambda ();
  g_assert_nonnull (evt);
  if (_doc->evalAction (evt, Event::START) == 0)
    return false;

  // Start settings.
  evt = settings->getLambda ();
  g_assert_nonnull (evt);
  g_assert (evt->transition (Event::START, params));

  // Set global _D in Lua state.
  L = this->getLuaState ();
  g_assert_nonnull (L);
  LuaAPI::Document_push (L, _doc);
  lua_setglobal (L, "_D");

  return true;
}

void
Formatter::resize (int width, int height)
{
  g_assert_nonnull (_doc);
  g_assert (width > 0 && height > 0);
  _doc->getSettings ()->setPropertyInteger ("width", width);
  _doc->getSettings ()->setPropertyInteger ("height", height);
}

bool
Formatter::sendKey (const string &key, bool press)
{
  set<Object *> objects;
  list<Object *> buf;

  g_assert_nonnull (_doc);

  // IMPORTANT: When propagating a key to the objects, we cannot traverse
  // the object set directly, as the reception of a key may cause the state
  // of some objects in this set to be modified.  We thus need to create a
  // buffer with the objects that should receive the key, i.e., those that
  // are not sleeping, and then propagate the key only to the objects in
  // this buffer.

  _doc->getObjects (&objects);
  for (auto obj: objects)
    if (!obj->isSleeping ())
      buf.push_back (obj);
  for (auto obj: buf)
    {
      if (obj->getType () == Object::MEDIA)
        {
          Media *media = cast (Media *, obj);
          media->sendKey (key, press);
        }
    }

  return true;
}

string
Formatter::debug_getDocPath ()
{
  return _docPath;
}

// Public: Internal API.

Formatter::Formatter () : Ginga ()
{
  _doc = NULL;
  _docPath = "";
}

Formatter::~Formatter ()
{
}

void *
Formatter::getDocument ()
{
  return _doc;
}

GINGA_NAMESPACE_END
