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

#include "ncl/ParserXercesC.h"
#include "Parser.h"
#include "player/PlayerText.h"

#if defined WITH_LUA && WITH_LUA
# include "ParserLua.h"         // for ncl-tab support
#endif

GINGA_NAMESPACE_BEGIN

// Option defaults.
static GingaOptions opts_defaults =
{
  800,                          // width
  600,                          // height
  false,                        // debug
  false,                        // experimental
  false,                        // opengl
  "",                           // background ("" == none)
};

// Option data.
typedef struct GingaOptionData
{
  GType type;                   // option type
  int offset;                   // offset in GingaOption struct
  void *func;                   // update function
} OptionTab;

#define OPTS_ENTRY(name,type,func)                              \
  {G_STRINGIFY (name),                                          \
      {(type), offsetof (GingaOptions, name),                   \
         pointerof (G_PASTE (Formatter::setOption, func))}}

// Option table.
static map<string, GingaOptionData> opts_table =
{
  OPTS_ENTRY (background,   G_TYPE_STRING,  Background),
  OPTS_ENTRY (debug,        G_TYPE_BOOLEAN, Debug),
  OPTS_ENTRY (experimental, G_TYPE_BOOLEAN, Experimental),
  OPTS_ENTRY (height,       G_TYPE_INT,     Size),
  OPTS_ENTRY (opengl,       G_TYPE_BOOLEAN, OpenGL),
  OPTS_ENTRY (width,        G_TYPE_INT,     Size),
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
  string id;
  NclContext *body;
  Event *evt;

  if (_state != GINGA_STATE_STOPPED)
    return false;               // nothing to do

  // Parse document.
  g_assert_null (_doc);
  w = _opts.width;
  h = _opts.height;

  if (!_opts.experimental)
    {
      _docLegacy = ParserXercesC::parse (file, w, h, errmsg);
      if (unlikely (_docLegacy == nullptr))
        return false;
      _doc = new Document ();
    }
  else
    {
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
      _docLegacy = nullptr;
    }

  g_assert_nonnull (_doc);
  _doc->setData ("formatter", (void *) this);

  Context *root = _doc->getRoot ();
  g_assert_nonnull (root);
  MediaSettings *settings = _doc->getSettings ();
  g_assert_nonnull (settings);

  // Initialize formatter variables.
  _docPath = file;
  _eos = false;
  _last_tick_total = 0;
  _last_tick_diff = 0;
  _last_tick_frameno = 0;

  // Run document.
  TRACE ("%s", file.c_str ());

  if (_docLegacy != nullptr)
    {
      id = _docLegacy->getId ();
      body = _docLegacy->getRoot ();
      g_assert_nonnull (body);

      root->addAlias (id);

      // Create settings node.
      NclMedia *settingsNode =  new NclMedia (_docLegacy,
                                              "__settings__", true);
      NclProperty *prop = new NclProperty (_docLegacy,
                                           "service.currentFocus");
      prop->setValue ("");
      settingsNode->addAnchor (prop);
      g_assert_nonnull (this->obtainExecutionObject (settingsNode->getId ()));
      evt = cast (Object *, settings)->getLambda ();
      g_assert_nonnull (evt);
      g_assert (evt->transition (Event::START));

      // Initialize settings object.
      list<NclNode *> *nodes = _docLegacy->getSettingsNodes ();
      for (auto node: *nodes)
        {
          NclMedia *content;

          content = (NclMedia *) node;
          if (content != settingsNode)
            settings->addAlias (content->getId ());

          for (auto anchor: *content->getAnchors ())
            {
              NclProperty *prop;
              string name;
              string value;

              if (!instanceof (NclProperty *, anchor))
                continue;           // nothing to do

              prop = cast (NclProperty *, anchor);
              name = prop->getName ();
              value = prop->getValue ();
              if (value == "")
                continue;           // nothing to do

              settings->setProperty (name, value, 0);
            }
        }
      delete nodes;

      // Start document.
      Object *obj = this->obtainExecutionObject (body->getId ());
      for (auto port: *body->getPorts ())
        {
          NclNode *target;
          NclAnchor *iface;
          Object *child;
          Event *e;

          port->getTarget (&target, &iface);
          child = this->obtainExecutionObject (target->getId ());
          g_assert_nonnull (child);

          if (!instanceof (NclArea *, iface))
            continue;       // nothing to do

          e = this->obtainEvent (child, Event::PRESENTATION, iface, "");
          g_assert_nonnull (e);
          cast (Context *, obj)->addPort (e);
        }
      for (auto link: *(body->getLinks ()))
        {
          auto ell = obtainFormatterLink (link);
          cast (Context *, obj)->addLink (ell.first, ell.second);
        }
      evt = obj->getLambda ();
      g_assert_nonnull (evt);
      if (_doc->evalAction (evt, Event::START) == 0)
        return false;
    }
  else
    {
      evt = root->getLambda ();
      g_assert_nonnull (evt);
      if (_doc->evalAction (evt, Event::START) == 0)
        return false;
    }

  // Refresh current focus.
  settings->updateCurrentFocus ("");

  _state = GINGA_STATE_PLAYING;
  return true;
}

bool
Formatter::start (const string &buf, size_t size, string *errmsg)
{
  Context *root;
  MediaSettings *settings;

  if (_state != GINGA_STATE_STOPPED)
    return false;               // nothing to do

  _doc = Parser::parseBuffer
    (buf.c_str (), size, _opts.width, _opts.height, errmsg);
  if (unlikely (_doc == nullptr))
    return false;

  g_assert_nonnull (_doc);
  _doc->setData ("formatter", (void *) this);

  // Initialize formatter variables.
  _docPath = "(buffer)";
  _eos = false;
  _last_tick_total = 0;
  _last_tick_diff = 0;
  _last_tick_frameno = 0;

  // Start root context (body).
  root = _doc->getRoot ();
  g_assert_nonnull (root);
  if (unlikely (_doc->evalAction (root->getLambda (), Event::START) == 0))
    return false;

  // Refresh current focus.
  settings = _doc->getSettings ();
  g_assert_nonnull (settings);
  settings->updateCurrentFocus ("");

  // Success.
  _state = GINGA_STATE_PLAYING;
  return true;
}

bool
Formatter::stop ()
{
  if (_state == GINGA_STATE_STOPPED)
    return false;               // nothing to do

  delete _doc;
  _state = GINGA_STATE_STOPPED;
  return true;
}

void
Formatter::resize (int width, int height)
{
  g_assert (width > 0 && height > 0);
  _opts.width = width;
  _opts.height = height;

  if (_state != GINGA_STATE_PLAYING)
    return;                     // nothing to do

  for (auto obj: *_doc->getObjects ())
    {
      if (!instanceof (Media *, obj))
        continue;
      obj->setProperty ("top", obj->getProperty ("top"));
      obj->setProperty ("left", obj->getProperty ("left"));
      obj->setProperty ("width", obj->getProperty ("width"));
      obj->setProperty ("height", obj->getProperty ("height"));
    }
}

void
Formatter::redraw (cairo_t *cr)
{
  GList *zlist;
  GList *l;

  if (_state != GINGA_STATE_PLAYING)
    return;                     // nothing to do

  if (_opts.opengl)  //clear screen opengl
    {
      GL::beginDraw ();
      GL::clear_scene (_opts.width, _opts.height);
    }
   else  //clear screen cairo
   {
      cairo_save (cr);
      cairo_set_source_rgba (cr,
                    0,
                    0,
                    0,
                    1.0);
      cairo_rectangle (cr, 0, 0, _opts.width, _opts.height);
      cairo_fill (cr);
      cairo_restore (cr);
   }

  if (_background.alpha > 0)
    {
      if (_opts.opengl)
        {
          GL::draw_quad (0, 0, _opts.width, _opts.height,
                         (float) _background.red,
                         (float) _background.green,
                         (float) _background.blue,
                         (float) _background.alpha);
        }
      else
        {
          cairo_save (cr);
          cairo_set_source_rgba (cr,
                                 _background.red,
                                 _background.green,
                                 _background.blue,
                                 _background.alpha);
          cairo_rectangle (cr, 0, 0, _opts.width, _opts.height);
          cairo_fill (cr);
          cairo_restore (cr);
        }
    }

  zlist = nullptr;
  for (auto &media: *_doc->getMedias ())
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
      static Color fg = {1., 1., 1., 1.};
      static Color bg = {0, 0, 0, 0};
      static Rect rect = {0, 0, 0, 0};
      string info;
      cairo_surface_t *debug;
      Rect ink;
      info = xstrbuild ("%s: #%lu %" GINGA_TIME_FORMAT " %.1ffps",
                        _docPath.c_str (),
                        _last_tick_frameno,
                        GINGA_TIME_ARGS (_last_tick_total),
                        1 * GINGA_SECOND / (double) _last_tick_diff);
      rect.width = _opts.width;
      rect.height = _opts.height;
      debug = PlayerText::renderSurface
        (info, "monospace", "", "bold", "9", fg, bg,
         rect, "center", "", true, &ink);
      ink = {0, 0, rect.width, ink.height - ink.y + 4};
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
#define _GINGA_CHECK_EOS(ginga)                                 \
  G_STMT_START                                                  \
  {                                                             \
    if ((ginga)->getEOS ())                                     \
      {                                                         \
        g_assert ((ginga)->_state == GINGA_STATE_PLAYING);      \
        (ginga)->setEOS (false);                                \
        g_assert ((ginga)->stop ());                            \
        g_assert ((ginga)->_state == GINGA_STATE_STOPPED);      \
      }                                                         \
  }                                                             \
  G_STMT_END

bool
Formatter::sendKey (const string &key, bool press)
{
  list<Object *> buf;

  _GINGA_CHECK_EOS (this);
  if (_state != GINGA_STATE_PLAYING)
    return false;               // nothing to do

  // IMPORTANT: When propagating a key to the objects, we cannot traverse
  // the object set directly, as the reception of a key may cause this set
  // to be modified.  We thus need to create a buffer with the objects that
  // should receive the key, i.e., those that are not sleeping, and then
  // propagate the key only to the objects in this buffer.
  for (auto obj: *_doc->getObjects ())
    if (!obj->isSleeping ())
      buf.push_back (obj);
  for (auto obj: buf)
    obj->sendKey (key, press);

  return true;
}

bool
Formatter::sendTick (uint64_t total, uint64_t diff, uint64_t frame)
{
  list<Object *> buf;

  // natural end of the document
  if (_doc->getRoot()->isOccurring () == false)
    this->setEOS(true);

    _GINGA_CHECK_EOS (this);

  if (_state != GINGA_STATE_PLAYING)
    return false;               // nothing to do

  _last_tick_total = total;
  _last_tick_diff = diff;
  _last_tick_frameno = frame;

  // IMPORTANT: The same warning about propagation that appear in
  // Formatter::sendKeyEvent() applies here.  The difference is that ticks
  // should only be propagated to objects that are occurring.
  for (auto obj: *_doc->getObjects ())
    if (obj->isOccurring ())
      buf.push_back (obj);
  for (auto obj: buf)
    obj->sendTick (total, diff, frame);

  return true;
}

const GingaOptions *
Formatter::getOptions ()
{
  return &_opts;
}

#define OPT_ERR_UNKNOWN(name)\
  ERROR ("unknown GingaOption '%s'", (name))
#define OPT_ERR_BAD_TYPE(name, typename)\
  ERROR ("GingaOption '%s' is of type '%s'", (name), (typename))

#define OPT_GETSET_DEFN(Name, Type, GType)                              \
  Type                                                                  \
  Formatter::getOption##Name (const string &name)                       \
  {                                                                     \
    GingaOptionData *opt;                                               \
    if (unlikely (!opts_table_index (name, &opt)))                      \
      OPT_ERR_UNKNOWN (name.c_str ());                                  \
    if (unlikely (opt->type != (GType)))                                \
      OPT_ERR_BAD_TYPE (name.c_str (), G_STRINGIFY (Type));             \
    return *((Type *)(((ptrdiff_t) &_opts) + opt->offset));             \
  }                                                                     \
  void                                                                  \
  Formatter::setOption##Name (const string &name, Type value)           \
  {                                                                     \
    GingaOptionData *opt;                                               \
    if (unlikely (!opts_table_index (name, &opt)))                      \
      OPT_ERR_UNKNOWN (name.c_str ());                                  \
    if (unlikely (opt->type != (GType)))                                \
      OPT_ERR_BAD_TYPE (name.c_str (), G_STRINGIFY (Type));             \
    *((Type *)(((ptrdiff_t) &_opts) + opt->offset)) = value;            \
    if (opt->func)                                                      \
      {                                                                 \
        ((void (*) (Formatter *, const string &, Type)) opt->func)      \
          (this, name, value);                                          \
      }                                                                 \
  }

OPT_GETSET_DEFN (Bool, bool, G_TYPE_BOOLEAN)
OPT_GETSET_DEFN (Int, int, G_TYPE_INT)
OPT_GETSET_DEFN (String, string, G_TYPE_STRING)


// Public: Internal API.

Formatter::Formatter (unused (int argc), unused (char **argv),
                      GingaOptions *opts) : Ginga (argc, argv, opts)
{
  const char *s;

  _state = GINGA_STATE_STOPPED;
  _opts = (opts) ? *opts : opts_defaults;
  _background = {0., 0., 0., 0.};

  _last_tick_total = 0;
  _last_tick_diff = 0;
  _last_tick_frameno = 0;
  _saved_G_MESSAGES_DEBUG = (s = g_getenv ("G_MESSAGES_DEBUG"))
    ? string (s) : "";

  _doc = nullptr;
  _docLegacy = nullptr;
  _docPath = "";
  _eos = false;

  // Initialize options.
  setOptionBackground (this, "background", _opts.background);
  setOptionDebug (this, "debug", _opts.debug);
  setOptionExperimental (this, "experimental", _opts.experimental);
  setOptionOpenGL (this, "opengl", _opts.opengl);
}

Formatter::~Formatter ()
{
  this->stop ();
}

Document *
Formatter::getDocument ()
{
  return _doc;
}

bool
Formatter::getEOS ()
{
  return _eos;
}

void
Formatter::setEOS (bool eos)
{
  _eos = eos;
}


// Public: Static.

void
Formatter::setOptionBackground (Formatter *self, const string &name,
                                string value)
{
  g_assert (name == "background");
  if (value == "")
    self->_background = {0.,0.,0.,0.};
  else
    self->_background = ginga::parse_color (value);
  TRACE ("%s:='%s'", name.c_str (), value.c_str ());
}

void
Formatter::setOptionDebug (Formatter *self, const string &name,
                           bool value)
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

void
Formatter::setOptionExperimental (unused (Formatter *self),
                                  const string &name, bool value)
{
  g_assert (name == "experimental");
  TRACE ("%s:=%s", name.c_str (), strbool (value));
}

void
Formatter::setOptionOpenGL (unused (Formatter *self),
                            const string &name, bool value)
{
  g_assert (name == "opengl");
#if !(defined WITH_OPENGL && WITH_OPENGL)
  if (unlikely (value))
    ERROR ("Not compiled with OpenGL support");
#endif
  TRACE ("%s:=%s", name.c_str (), strbool (value));
}

void
Formatter::setOptionSize (Formatter *self, const string &name,
                              int value)
{
  const GingaOptions *opts;
  g_assert (name == "width" || name == "height");
  opts = self->getOptions ();
  self->resize (opts->width, opts->height);
  TRACE ("%s:=%d", name.c_str (), value);
}


// Private.

Object *
Formatter::obtainExecutionObject (const string &id)
{
  NclNode *parentNode;
  Composition *parent;
  Object *object;

  if ((object = _doc->getObjectByIdOrAlias (id)) != nullptr)
    return object;              // already created

  NclEntity *entity = _docLegacy->getEntityById (id);
  g_assert_nonnull (entity);

  NclNode *node = cast (NclNode *, entity);
  g_assert_nonnull (node);

  // Get parent.
  parentNode = node->getParent ();
  if (parentNode == nullptr)
    {
      parent = nullptr;
    }
  else
    {
      parent = cast (Composition *, obtainExecutionObject
                     (parentNode->getId ()));
      g_assert_nonnull (parent);
      if ((object = _doc->getObjectByIdOrAlias (id)) != nullptr)
        return object;
    }

  // Solve refer, if needed.
  if (instanceof (NclMediaRefer *, node))
    {
      NclNode *target;

      TRACE ("solving refer %s", node->getId ().c_str ());
      target = node->derefer ();
      g_assert (!instanceof (NclMediaRefer *, target));
      object = obtainExecutionObject (target->derefer ()->getId ());
      object->addAlias (id);
      return object;
    }

  if (instanceof (NclSwitch *, node)) // switch
    {
      TRACE ("creating switch %s", node->getId ().c_str ());
      object = new Switch (id);
      if (parent != nullptr)
        parent->addChild (object);

      for (auto item: *cast (NclSwitch *, node)->getRules ())
        {
          Object *obj;

          g_assert_nonnull (item.first);
          g_assert_nonnull (item.second);

          obj = this->obtainExecutionObject (item.first->getId ());
          g_assert_nonnull (obj);
          cast (Switch *, object)->addRule (obj, item.second);
        }
      return object;
    }
  else if (instanceof (NclContext *, node)) // context
    {
      TRACE ("creating context %s", node->getId ().c_str ());
      object = new Context (id);
      if (parent != nullptr)
        parent->addChild (object);

      NclContext *ctx = cast (NclContext *, node);
      for (auto port: *ctx->getPorts ())
        {
          NclNode *target;
          NclAnchor *iface;
          Object *child;
          Event *e;

          port->getTarget (&target, &iface);
          child = this->obtainExecutionObject (target->getId ());
          g_assert_nonnull (child);

          if (!instanceof (NclArea *, iface))
            continue;       // nothing to do

          e = this->obtainEvent (child, Event::PRESENTATION, iface, "");
          g_assert_nonnull (e);
          cast (Context *, object)->addPort (e);
        }
      for (auto link: *(ctx->getLinks ()))
        {
          auto ell = obtainFormatterLink (link);
          cast (Context *, object)->addLink (ell.first, ell.second);
        }

      return object;
    }
  else if (instanceof (NclMedia *, node)) // media
    {
      TRACE ("creating media %s", node->getId ().c_str ());
      NclMedia *media;
      media = cast (NclMedia *, node);
      g_assert_nonnull (media);
      if (media->isSettings ())
        {
          object = new MediaSettings (id);
        }
      else
        {
          object = new Media (id, media->getMimeType (),
                              media->getSrc ());
        }

      // Initialize properties.
      for (auto anchor: *media->getAnchors ())
        {
          NclProperty *prop = cast (NclProperty *, anchor);
          if (prop != nullptr)
            object->setProperty (prop->getName (),
                                 prop->getValue ());
        }

      g_assert_nonnull (object);
      if (parent != nullptr)
        parent->addChild (object);
      return object;
    }
  else
    {
      g_assert_not_reached ();
    }
}

Event *
Formatter::obtainEvent (Object *obj, Event::Type type,
                        NclAnchor *anchor, const string &key)
{
  Event *event;

  if (type == Event::SELECTION)
    event = obj->getEvent (type, key);
  else
    event = obj->getEvent (type, anchor->getId ());

  if (event != nullptr)
    return event;

  g_assert (instanceof (Media *, obj));

  switch (type)
    {
    case Event::PRESENTATION:
      {
        NclArea *area = cast (NclArea *, anchor);
        g_assert_nonnull (area);
        obj->addPresentationEvent (anchor->getId (),
                                    area->getBegin (),
                                    area->getEnd ());
        event = obj->getPresentationEvent (anchor->getId ());
        g_assert_nonnull (event);
        break;
      }
    case Event::ATTRIBUTION:
      {
        NclProperty *property = cast (NclProperty *, anchor);
        g_assert_nonnull (property);
        obj->addAttributionEvent (property->getId ());
        event = obj->getAttributionEvent (property->getId ());
        g_assert_nonnull (event);
        event->setParameter ("value", property->getValue ());
        break;
      }
    case Event::SELECTION:
      {
        obj->addSelectionEvent (key);
        event = obj->getSelectionEvent (key);
        g_assert_nonnull (event);
        event->setParameter ("key", key);
        break;
      }
    default:
      g_assert_not_reached ();
    }

  g_assert_nonnull (event);
  return event;
}

Event *
Formatter::obtainFormatterEventFromBind (NclBind *bind)
{
  NclNode *node;
  Object *obj;
  NclAnchor *iface;
  Event::Type eventType;
  NclBind::RoleType roleType;
  string key = "";

  node = bind->getNode ();
  g_assert_nonnull (node);

  iface = bind->getInterface ();
  if (iface != nullptr && instanceof (NclPort *, iface))
    cast (NclPort *, iface)->getTarget (&node, nullptr);

  obj = obtainExecutionObject (node->getId ());
  g_assert_nonnull (obj);

  if (iface == nullptr)
    return obj->getLambda ();

  if (instanceof (NclComposition *, node)
      && instanceof (NclPort *, iface))
    {
      NclComposition *comp = cast (NclComposition *, node);
      NclPort *port = cast (NclPort *, iface);
      iface = comp->getMapInterface (port);
    }

  roleType = bind->getRoleType ();
  eventType = bind->getEventType ();

  if (eventType == Event::SELECTION
      && roleType == NclBind::CONDITION)
    {
      bind->getParameter ("key", &key);
    }

  return this->obtainEvent (obj, eventType, iface, key);
}

pair<list<Action>,list<Action>>
Formatter::obtainFormatterLink (NclLink *docLink)
{
  pair<list<Action>,list<Action>> link;

  g_assert_nonnull (docLink);
  for (auto bind: *docLink->getBinds ())
    {
      switch (bind->getRoleType ())
        {
        case NclBind::CONDITION:
          {
            Predicate *pred = bind->getPredicate ();
            if (pred != nullptr) // solve ghost binds in predicate
              {
                list<Predicate *> buf;

                pred = pred->clone ();
                g_assert_nonnull (pred);
                buf.push_back (pred);
                while (!buf.empty ())
                  {
                    Predicate *p = buf.back ();
                    buf.pop_back ();
                    switch (p->getType ())
                      {
                      case Predicate::FALSUM:
                      case Predicate::VERUM:
                        break;        // nothing to do
                      case Predicate::ATOM:
                        {
                          Predicate::Test test;
                          string left, right, ghost;
                          p->getTest (&left, &test, &right);
                          if (left[0] == '$')
                            {
                              ghost = docLink->getGhostBind
                                (left.substr (1, left.length () - 1));
                              if (ghost != "")
                                left = ghost;
                            }
                          if (right[0] == '$')
                            {
                              ghost = docLink->getGhostBind
                                (right.substr (1, right.length () - 1));
                              if (ghost != "")
                                right = ghost;
                            }
                          p->setTest (left, test, right);
                          break;
                        }
                      case Predicate::NEGATION:
                      case Predicate::CONJUNCTION:
                      case Predicate::DISJUNCTION:
                        for (auto child: *p->getChildren ())
                          buf.push_back (child);
                        break;
                      default:
                        g_assert_not_reached ();
                      }
                  }
              }

            Action cond;

            cond.event = this->obtainFormatterEventFromBind (bind);
            g_assert_nonnull (cond.event);
            cond.transition = bind->getTransition ();
            cond.predicate = pred;
            link.first.push_back (cond);
            break;
          }

        case NclBind::ACTION:
          {
            Action act;
            string value;

            act.event = this->obtainFormatterEventFromBind (bind);
            g_assert_nonnull (act.event);
            act.transition = bind->getTransition ();
            act.predicate = nullptr;
            bind->getParameter ("value", &act.value);
            link.second.push_back (act);
            break;
          }

          default:
            g_assert_not_reached ();
        }
    }

  return link;
}

GINGA_NAMESPACE_END
