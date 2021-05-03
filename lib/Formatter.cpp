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

#include "Context.h"
#include "Media.h"
#include "MediaSettings.h"
#include "Object.h"
#include "Switch.h"

#include "Parser.h"
#include "PlayerText.h"
#include "WebServices.h"

/**
 * @file Formatter.cpp
 * @brief The Formatter class.
 */

#if defined WITH_LUA && WITH_LUA
#include "ParserLua.h" // for ncl-ltab support
#endif

GINGA_NAMESPACE_BEGIN

// Option data.
typedef struct GingaOptionData
{
  GType type; // option type
  int offset; // offset in GingaOption struct
  void *func; // update function
} OptionTab;

#define OPTS_ENTRY(name, type, func)                                       \
  {                                                                        \
    G_STRINGIFY (name),                                                    \
    {                                                                      \
      (type), offsetof (GingaOptions, name),                               \
          pointerof (G_PASTE (Formatter::setOption, func))                 \
    }                                                                      \
  }

// Option table.
static map<string, GingaOptionData> opts_table = {
  OPTS_ENTRY (background, G_TYPE_STRING, Background),
  OPTS_ENTRY (debug, G_TYPE_BOOLEAN, Debug),
  OPTS_ENTRY (experimental, G_TYPE_BOOLEAN, Experimental),
  OPTS_ENTRY (height, G_TYPE_INT, Size),
  OPTS_ENTRY (opengl, G_TYPE_BOOLEAN, OpenGL),
  OPTS_ENTRY (width, G_TYPE_INT, Size),
};

// Indexes option table.
static bool
opts_table_index (const string &key, GingaOptionData **result)
{
  map<string, GingaOptionData>::iterator it;
  if ((it = opts_table.find (key)) == opts_table.end ())
    return false;
  tryset (result, &it->second);
  return true;
}

// Compares the z-index and z-order of two media objects.
static int
zcmp (Media *a, Media *b)
{
  int z1, zo1, z2, zo2;

  g_assert_nonnull (a);
  g_assert_nonnull (b);

  z1 = zo1 = z2 = zo2 = 0;
  a->getZ (&z1, &zo1);
  b->getZ (&z2, &zo2);

  if (z1 < z2)
    return -1;
  if (z1 > z2)
    return 1;
  if (zo1 < zo2)
    return -1;
  if (zo1 > zo2)
    return 1;
  return 0;
}

// Public: External API.

GingaState
Formatter::getState ()
{
  return _state;
}

bool
Formatter::start (const string &file, string *errmsg)
{
  int w, h;
  Event *evt;

  // This must be the first check.
  if (_state != GINGA_STATE_STOPPED)
    return false;

  if (_opts.webservices)
    this->_webservices->start ();

  // Parse document.
  g_assert_null (_doc);
  w = _opts.width;
  h = _opts.height;
  _doc = nullptr;

#if defined WITH_LUA && WITH_LUA
  if (xstrhassuffix (file, ".lua"))
    {
      _doc = ParserLua::parseFile (file, errmsg);
      if (unlikely (_doc == nullptr))
        return false;
    }
#endif

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

  // Initialize formatter variables.
  _docPath = file;
  _eos = false;
  _lastTickTotal = 0;
  _lastTickDiff = 0;
  _lastTickFrameNo = 0;

  // Run document.
  TRACE ("%s", file.c_str ());
  evt = root->getLambda ();
  g_assert_nonnull (evt);
  if (_doc->evalAction (evt, Event::START) == 0)
    return false;

  // Start settings.
  evt = _doc->getSettings ()->getLambda ();
  g_assert_nonnull (evt);
  g_assert (evt->transition (Event::START));

  // Sets formatter state.
  _state = GINGA_STATE_PLAYING;

  // start webservices
  if (_opts.webservices)
    _webservices->start ();

  return true;
}

bool
Formatter::stop ()
{
  // This must be the first check.
  if (_state == GINGA_STATE_STOPPED)
    return false;

  delete _doc;
  _doc = nullptr;

  _state = GINGA_STATE_STOPPED;
  return true;
}

void
Formatter::resize (int width, int height)
{
  g_assert (width > 0 && height > 0);
  _opts.width = width;
  _opts.height = height;

  // This must be the first check.
  if (_state != GINGA_STATE_PLAYING)
    return;

  // Resize each media object in document.
  for (auto media : *_doc->getMedias ())
    {
#define UPDATE_IF_HAVE(PROP)                                               \
  {                                                                        \
    string s = media->getProperty (PROP);                                  \
    if (s != "")                                                           \
      media->setProperty (PROP, s);                                        \
  }

      UPDATE_IF_HAVE ("top");
      UPDATE_IF_HAVE ("left");
      UPDATE_IF_HAVE ("width");
      UPDATE_IF_HAVE ("height");
      UPDATE_IF_HAVE ("bottom");
      UPDATE_IF_HAVE ("right");
    }
}

void
Formatter::redraw (cairo_t *cr)
{
  GList *zlist;
  GList *l;

  // This must be the first check.
  if (_state != GINGA_STATE_PLAYING)
    return;

  if (_opts.opengl)
    {
      GL::beginDraw ();
      GL::clear_scene (_opts.width, _opts.height);
    }
  else
    {
      cairo_save (cr);
      cairo_set_source_rgba (cr, 0, 0, 0, 1.0);
      cairo_rectangle (cr, 0, 0, _opts.width, _opts.height);
      cairo_fill (cr);
      cairo_restore (cr);
    }

  if (_background.alpha > 0)
    {
      if (_opts.opengl)
        {
          GL::draw_quad (0, 0, _opts.width, _opts.height,
                         (float) _background.red, (float) _background.green,
                         (float) _background.blue,
                         (float) _background.alpha);
        }
      else
        {
          cairo_save (cr);
          cairo_set_source_rgba (cr, _background.red, _background.green,
                                 _background.blue, _background.alpha);
          cairo_rectangle (cr, 0, 0, _opts.width, _opts.height);
          cairo_fill (cr);
          cairo_restore (cr);
        }
    }

  zlist = nullptr;
  for (auto &media : *_doc->getMedias ())
    zlist = g_list_insert_sorted (zlist, media, (GCompareFunc) zcmp);

  l = zlist;
  while (l != NULL)
    {
      GList *next = l->next;
      ((Media *) l->data)->redraw (cr);
      zlist = g_list_delete_link (zlist, l);
      l = next;
    }
  g_assert_null (zlist);

  if (_opts.debug)
    {
      static Color fg = { 1., 1., 1., 1. };
      static Color bg = { 0, 0, 0, 0 };
      static Rect rect = { 0, 0, 0, 0 };
      string info;
      cairo_surface_t *debug;
      Rect ink;
      info = xstrbuild ("%s: #%lu %" GINGA_TIME_FORMAT " %.1ffps",
                        _docPath.c_str (), _lastTickFrameNo,
                        GINGA_TIME_ARGS (_lastTickTotal),
                        1 * GINGA_SECOND / (double) _lastTickDiff);
      rect.width = _opts.width;
      rect.height = _opts.height;
      debug = PlayerText::renderSurface (info, "monospace", "", "bold", "9",
                                         fg, bg, rect, "center", "", true,
                                         &ink);
      ink = { 0, 0, rect.width, ink.height - ink.y + 4 };
      cairo_save (cr);
      cairo_set_source_rgba (cr, 1., 0., 0., .5);
      cairo_rectangle (cr, 0, 0, ink.width, ink.height);
      cairo_fill (cr);
      cairo_set_source_surface (cr, debug, 0, 0);
      cairo_paint (cr);
      cairo_restore (cr);
      cairo_surface_destroy (debug);
    }
}

// Stops formatter if EOS has been seen.
#define _GINGA_CHECK_EOS(ginga)                                            \
  G_STMT_START                                                             \
  {                                                                        \
    Context *root;                                                         \
    root = _doc->getRoot ();                                               \
    g_assert_nonnull (root);                                               \
    if (root->isSleeping ())                                               \
      {                                                                    \
        (ginga)->setEOS (true);                                            \
      }                                                                    \
    if ((ginga)->getEOS ())                                                \
      {                                                                    \
        g_assert ((ginga)->_state == GINGA_STATE_PLAYING);                 \
        (ginga)->setEOS (false);                                           \
        g_assert ((ginga)->stop ());                                       \
        g_assert ((ginga)->_state == GINGA_STATE_STOPPED);                 \
      }                                                                    \
  }                                                                        \
  G_STMT_END

bool
Formatter::sendKey (const string &key, bool press)
{
  list<Object *> buf;

  // This must be the first check.
  if (_state != GINGA_STATE_PLAYING)
    return false;
  _GINGA_CHECK_EOS (this);
  if (_state != GINGA_STATE_PLAYING)
    return false;

  // IMPORTANT: When propagating a key to the objects, we cannot traverse
  // the object set directly, as the reception of a key may cause this set
  // to be modified.  We thus need to create a buffer with the objects that
  // should receive the key, i.e., those that are not sleeping, and then
  // propagate the key only to the objects in this buffer.
  for (auto obj : *_doc->getObjects ())
    if (!obj->isSleeping ())
      buf.push_back (obj);
  for (auto obj : buf)
    obj->sendKey (key, press);

  return true;
}

bool
Formatter::sendTick (uint64_t total, uint64_t diff, uint64_t frame)
{
  list<Object *> buf;

  // This must be the first check.
  if (_state != GINGA_STATE_PLAYING)
    return false;
  _GINGA_CHECK_EOS (this);
  if (_state != GINGA_STATE_PLAYING)
    return false;

  _lastTickTotal = total;
  _lastTickDiff = diff;
  _lastTickFrameNo = frame;

  // IMPORTANT: The same warning about propagation that appear in
  // Formatter::sendKeyEvent() applies here.  The difference is that ticks
  // should only be propagated to objects that are occurring.
  for (auto obj : *_doc->getObjects ())
    if (obj->isOccurring () || obj->isPreparing ())
      buf.push_back (obj);
  for (auto obj : buf)
    obj->sendTick (total, diff, frame);

  return true;
}

const GingaOptions *
Formatter::getOptions ()
{
  return &_opts;
}

#define OPT_ERR_UNKNOWN(name) ERROR ("unknown GingaOption '%s'", (name))
#define OPT_ERR_BAD_TYPE(name, typename)                                   \
  ERROR ("GingaOption '%s' is of type '%s'", (name), (typename))

#define OPT_GETSET_DEFN(Name, Type, GType)                                 \
  Type Formatter::getOption##Name (const string &name)                     \
  {                                                                        \
    GingaOptionData *opt;                                                  \
    if (unlikely (!opts_table_index (name, &opt)))                         \
      OPT_ERR_UNKNOWN (name.c_str ());                                     \
    if (unlikely (opt->type != (GType)))                                   \
      OPT_ERR_BAD_TYPE (name.c_str (), G_STRINGIFY (Type));                \
    return *((Type *) (((ptrdiff_t) &_opts) + opt->offset));               \
  }                                                                        \
  void Formatter::setOption##Name (const string &name, Type value)         \
  {                                                                        \
    GingaOptionData *opt;                                                  \
    if (unlikely (!opts_table_index (name, &opt)))                         \
      OPT_ERR_UNKNOWN (name.c_str ());                                     \
    if (unlikely (opt->type != (GType)))                                   \
      OPT_ERR_BAD_TYPE (name.c_str (), G_STRINGIFY (Type));                \
    *((Type *) (((ptrdiff_t) &_opts) + opt->offset)) = value;              \
    if (opt->func)                                                         \
      {                                                                    \
        ((void (*) (Formatter *, const string &, Type)) opt->func) (       \
            this, name, value);                                            \
      }                                                                    \
  }

OPT_GETSET_DEFN (Bool, bool, G_TYPE_BOOLEAN)
OPT_GETSET_DEFN (Int, int, G_TYPE_INT)
OPT_GETSET_DEFN (String, string, G_TYPE_STRING)

// Public: Internal API.

/**
 * @brief Creates a new Formatter.
 * @param opts Options to initialize the formatter with.
 * @return New #Formatter.
 */
Formatter::Formatter (const GingaOptions *opts) : Ginga (opts)
{
  const char *s;

  _state = GINGA_STATE_STOPPED;
  if (opts)
    _opts = *opts;
  else
    {
      // defaults.
      _opts.width = 800;
      _opts.height = 600;
      _opts.debug = false;
      _opts.webservices = false;
      _opts.opengl = false;
      _opts.experimental = false;
    };
  _background = { 0., 0., 0., 0. };

  _lastTickTotal = 0;
  _lastTickDiff = 0;
  _lastTickFrameNo = 0;
  _saved_G_MESSAGES_DEBUG
      = (s = g_getenv ("G_MESSAGES_DEBUG")) ? string (s) : "";

  _doc = nullptr;
  _webservices = new WebServices (this);
  _docPath = "";
  _eos = false;

  // Initialize options.
  setOptionBackground (this, "background", _opts.background);
  setOptionDebug (this, "debug", _opts.debug);
  setOptionExperimental (this, "experimental", _opts.experimental);
  setOptionOpenGL (this, "opengl", _opts.opengl);
}

/**
 * @brief Destroys formatter.
 */
Formatter::~Formatter ()
{
  this->stop ();
}

/**
 * @brief Gets current document.
 * @return Current document or null (no current document).
 */
Document *
Formatter::getDocument ()
{
  return _doc;
}

/**
 * @brief Gets WebServices.
 * @return WebServices or null (no current document).
 */
WebServices *
Formatter::getWebServices ()
{
  return _webservices;
}

/**
 * @brief Gets EOS flag.
 * @return EOS flag.
 */
bool
Formatter::getEOS ()
{
  return _eos;
}

/**
 * @brief Sets EOS flag.
 * @param eos Flag value.
 */
void
Formatter::setEOS (bool eos)
{
  _eos = eos;
}

// Public: Static.

/**
 * @brief Sets background option of the given Formatter.
 * @param self Formatter.
 * @param name Must be the string "background".
 * @param value Color value.
 */
void
Formatter::setOptionBackground (Formatter *self, const string &name,
                                string value)
{
  g_assert (name == "background");
  if (value == "")
    self->_background = { 0., 0., 0., 0. };
  else
    self->_background = ginga::parse_color (value);
  TRACE ("%s:='%s'", name.c_str (), value.c_str ());
}

/**
 * @brief Sets debug option of the given Formatter.
 * @param self Formatter.
 * @param name Must be the string "debug".
 * @param value Debug flag value.
 */
void
Formatter::setOptionDebug (Formatter *self, const string &name, bool value)
{
  g_assert (name == "debug");
  if (value)
    {
      const char *curr = g_getenv ("G_MESSAGES_DEBUG");
      if (curr != nullptr)
        self->_saved_G_MESSAGES_DEBUG = string (curr);
      g_assert (g_setenv ("G_MESSAGES_DEBUG", "all", true));
    }
  else
    {
      g_assert (g_setenv ("G_MESSAGES_DEBUG",
                          self->_saved_G_MESSAGES_DEBUG.c_str (), true));
    }
  TRACE ("%s:=%s", name.c_str (), strbool (value));
}

/**
 * @brief Sets WebServices option option of the given Formatter.
 * @param self Formatter.
 * @param name Must be the string "webservies".
 * @param value webservies flag value.
 */
void
Formatter::setOptionWebServices (Formatter *self, const string &name,
                                 bool value)
{
  g_assert (name == "webservices");
  if (value)
    self->_webservices->start ();
  else
    self->_webservices->stop ();
  TRACE ("%s:=%s", name.c_str (), strbool (value));
}

/**
 * @brief Sets the experimental option of the given Formatter.
 * @param self Formatter.
 * @param name Must be the string "experimental".
 * @param value Experimental flag value.
 */
void
Formatter::setOptionExperimental (unused (Formatter *self),
                                  const string &name, bool value)
{
  g_assert (name == "experimental");
  TRACE ("%s:=%s", name.c_str (), strbool (value));
}

/**
 * @brief Sets the OpenGL option of the given Formatter.
 * @param self Formatter.
 * @param name Must be the string "opengl".
 * @param value OpenGL flag value.
 */
void
Formatter::setOptionOpenGL (unused (Formatter *self), const string &name,
                            bool value)
{
  g_assert (name == "opengl");
#if !(defined WITH_OPENGL && WITH_OPENGL)
  if (unlikely (value))
    ERROR ("Not compiled with OpenGL support");
#endif
  TRACE ("%s:=%s", name.c_str (), strbool (value));
}

/**
 * @brief Sets the width or height options of the given Formatter.
 * @param self Formatter.
 * @param name Must be either the string "width" or the string "height".
 * @param value Width or height value in pixels.
 */
void
Formatter::setOptionSize (Formatter *self, const string &name, int value)
{
  const GingaOptions *opts;
  g_assert (name == "width" || name == "height");
  opts = self->getOptions ();
  self->resize (opts->width, opts->height);
  TRACE ("%s:=%d", name.c_str (), value);
}

GINGA_NAMESPACE_END
