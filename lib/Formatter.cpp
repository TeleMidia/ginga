/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#include "aux-ginga.h"
#include "aux-gl.h"
#include "Formatter.h"

#include "FormatterContext.h"
#include "FormatterMedia.h"
#include "FormatterMediaSettings.h"
#include "FormatterObject.h"
#include "FormatterSwitch.h"

#include "ncl/ParserLibXML.h"
#include "ncl/ParserXercesC.h"
#include "player/PlayerText.h"

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
static bool
cmpz (FormatterMedia *a, FormatterMedia *b)
{
  int z1, zo1, z2, zo2;

  g_assert_nonnull (a);
  g_assert_nonnull (b);

  z1 = zo1 = z2 = zo2 = 0;
  a->getZ (&z1, &zo1);
  b->getZ (&z2, &zo2);

  if (z1 < z2)
    return true;
  if (z1 > z2)
    return false;
  if (zo1 < zo2)
    return true;
  if (zo1 > zo2)
    return false;
  return true;
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

  if (_state != GINGA_STATE_STOPPED)
    return false;               // nothing to do

  // Parse document.
  w = _opts.width;
  h = _opts.height;
  if (!_opts.experimental)
    _doc = ParserXercesC::parse (file, w, h, errmsg);
  else
    _doc = ParserLibXML::parseFile (file, w, h, errmsg);
  if (unlikely (_doc == nullptr))
    return false;

  // Initialize formatter variables.
  _docPath = file;
  _eos = false;
  _last_tick_total = 0;
  _last_tick_diff = 0;
  _last_tick_frameno = 0;
  g_assert_null (_settings);

  // Run document.
  TRACE ("%s", file.c_str ());

  id = _doc->getId ();
  body = _doc->getRoot ();
  g_assert_nonnull (body);

  // Create settings node.
  NclMedia *settingsNode =  new NclMedia (_doc, "__settings__", true);
  NclProperty *prop = new NclProperty (_doc, "service.currentFocus");
  prop->setValue ("");
  settingsNode->addAnchor (prop);
  g_assert_null (_settings);
  g_assert_nonnull (this->obtainExecutionObject (settingsNode));
  g_assert_nonnull (_settings);
  FormatterEvent *evt = cast (FormatterObject *, _settings)->getLambda ();
  g_assert_nonnull (evt);
  g_assert (evt->transition (NclEventStateTransition::START));

  // Initialize settings object.
  vector<NclNode *> *nodes = _doc->getSettingsNodes ();
  for (auto node: *nodes)
    {
      NclMedia *content;

      content = (NclMedia *) node;
      if (content != settingsNode)
        _settings->addAlias (content->getId ());

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

          _settings->setProperty (name, value, 0);
        }
    }
  delete nodes;

  // Start document.
  FormatterObject *obj = this->obtainExecutionObject (body);
  evt = obj->getLambda ();
  g_assert_nonnull (evt);
  if (this->evalAction (evt, NclEventStateTransition::START) == 0)
    return false;

  // Refresh current focus.
  _settings->updateCurrentFocus ("");

  _state = GINGA_STATE_PLAYING;
  return true;
}

bool
Formatter::stop ()
{
  if (_state == GINGA_STATE_STOPPED)
    return false;               // nothing to do

  for (auto obj: _objects)
    if (obj != _settings)
      delete obj;
  delete _settings;

  _objects.clear ();
  _mediaObjects.clear ();
  _settings = nullptr;
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

  for (auto obj: _objects)
    {
      if (!instanceof (FormatterMedia *, obj))
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
  vector<FormatterMedia *> medias;

  if (_state != GINGA_STATE_PLAYING)
    return;                     // nothing to do

  if (_opts.opengl)
    {
      GL::beginDraw ();
      GL::clear_scene (_opts.width, _opts.height);
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

  for (auto media: _mediaObjects)
    medias.push_back (cast (FormatterMedia *, media));
  std::sort (medias.begin (), medias.end (), cmpz);
  for (auto media: medias)
    media->redraw (cr);

  if (_opts.debug)
    {
      static GingaColor fg = {1., 1., 1., 1.};
      static GingaColor bg = {0, 0, 0, 0};
      static GingaRect rect = {0, 0, 0, 0};
      string info;
      cairo_surface_t *debug;
      GingaRect ink;
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
Formatter::sendKeyEvent (const string &key, bool press)
{
  _GINGA_CHECK_EOS (this);
  if (_state != GINGA_STATE_PLAYING)
    return false;               // nothing to do

  for (auto obj: this->getObjectsVector ())
    if (obj->isOccurring ())
      obj->sendKeyEvent (key, press);

  return true;
}

bool
Formatter::sendTickEvent (uint64_t total, uint64_t diff, uint64_t frame)
{
  vector<FormatterObject *> buf;

  _GINGA_CHECK_EOS (this);
  if (_state != GINGA_STATE_PLAYING)
    return false;               // nothing to do

  _last_tick_total = total;
  _last_tick_diff = diff;
  _last_tick_frameno = frame;

  buf = this->getObjectsVector ();
  if (buf.empty ())
    {
      this->setEOS (true);
      return true;
    }

  for (auto obj: buf)
    {
      if (!obj->isOccurring ())
        continue;

      obj->sendTickEvent (total, diff, frame);
    }

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
    Formatter::setOption##Name (const string &name, Type value)         \
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
  _docPath = "";
  _eos = false;
  _settings = nullptr;

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

const set<FormatterObject *> *
Formatter::getObjects ()
{
  return &_objects;
}

const set<FormatterMedia *> *
Formatter::getMediaObjects ()
{
  return &_mediaObjects;
}

FormatterMediaSettings *
Formatter::getSettings ()
{
  return _settings;
}

FormatterObject *
Formatter::getObjectById (const string &id)
{
  for (auto obj: _objects)
    if (obj->getId () == id)
      return obj;
  return nullptr;
}

FormatterObject *
Formatter::getObjectByIdOrAlias (const string &id)
{
  FormatterObject *obj;
  if ((obj = this->getObjectById (id)) != nullptr)
    return obj;
  for (auto obj: _objects)
    if (obj->hasAlias (id))
      return obj;
  return nullptr;
}

bool
Formatter::getObjectPropertyByRef (const string &ref,
                                   string *result)
{
  size_t i;
  string id;
  string name;
  FormatterObject *object;

  if (ref[0] != '$' || (i = ref.find ('.')) == string::npos)
    return false;

  id = ref.substr (1, i - 1);
  name = ref.substr (i + 1);
  object = this->getObjectByIdOrAlias (id);
  if (object == nullptr)
    return false;

  tryset (result, object->getProperty (name));
  return true;
}

void
Formatter::addObject (FormatterObject *obj)
{
  g_assert_nonnull (obj);
  if (_objects.find (obj) != _objects.end ()
      || getObjectByIdOrAlias (obj->getId ()) != nullptr)
    {
      return;
    }

  _objects.insert (obj);

  if (instanceof (FormatterMediaSettings *, obj))
    {
      g_assert_null (_settings);
      _settings = cast (FormatterMediaSettings *, obj);
      g_assert_nonnull (_settings);
    }
  else if (instanceof (FormatterMedia *, obj))
    {
      FormatterMedia *media = cast (FormatterMedia *, obj);
      g_assert_nonnull (media);
      _mediaObjects.insert (media);
    }
}

bool
Formatter::evalPredicate (FormatterPredicate *pred)
{
  switch (pred->getType ())
    {
    case PredicateType::FALSUM:
      TRACE ("false");
      break;
    case PredicateType::VERUM:
      TRACE ("true");
      break;
    case PredicateType::ATOM:
      {
        string left, right;
        PredicateTestType test;
        string msg_left, msg_test, msg_right;
        bool result;

        pred->getTest (&left, &test, &right);

        if (left[0] == '$')
          {
            msg_left = left;
            if (this->getObjectPropertyByRef (left, &left))
              msg_left += " ('" + left + "')";
          }
        else
          {
            msg_left = "'" + left + "'";
          }

        if (right[0] == '$')
          {
            msg_right = right;
            if (this->getObjectPropertyByRef (right, &right))
              msg_right += " ('" + right + "')";
          }
        else
          {
            msg_right = "'" + right + "'";
          }

        switch (test)
          {
          case PredicateTestType::EQ:
            msg_test = "==";
            result = left == right;
            break;
          case PredicateTestType::NE:
            msg_test = "!=";
            result = left != right;
            break;
          case PredicateTestType::LT:
            msg_test = "<";
            result = left < right;
            break;
          case PredicateTestType::LE:
            msg_test = "<=";
            result = left <= right;
            break;
          case PredicateTestType::GT:
            msg_test = ">";
            result = left > right;
            break;
          case PredicateTestType::GE:
            msg_test = ">=";
            result = left >= right;
            break;
          default:
            g_assert_not_reached ();
          }
        TRACE ("%s %s %s -> %s", msg_left.c_str (),
               msg_test.c_str (), msg_right.c_str (), strbool (result));
        return result;
      }
    case PredicateType::NEGATION:
      g_assert_not_reached ();
      break;
    case PredicateType::CONJUNCTION:
      {
        for (auto child: *pred->getChildren ())
          {
            if (!this->evalPredicate (child))
              {
                TRACE ("and -> false");
                return false;
              }
          }
        TRACE ("and -> true");
        return true;
      }
      break;
    case PredicateType::DISJUNCTION:
      g_assert_not_reached ();
      break;
    default:
      g_assert_not_reached ();
    }
  g_assert_not_reached ();
}

int
Formatter::evalAction (FormatterEvent *event,
                       NclEventStateTransition transition,
                       const string &value)
{
  FormatterAction *act;
  int result;

  act = new FormatterAction (event, transition);
  if (event->getType () == NclEventType::ATTRIBUTION)
    act->setParameter ("value", value);

  result = this->evalAction (act);
  delete act;

  return result;
}

int
Formatter::evalAction (FormatterAction *init)
{
  list<FormatterAction *> stack;
  int n;

  stack.push_back (init);
  n = 0;

  while (!stack.empty ())
    {
      FormatterAction *act;
      FormatterEvent *evt;
      FormatterObject *obj;
      FormatterComposition *comp;
      FormatterContext *ctx;
      bool done;

      act = stack.back ();
      stack.pop_back ();
      g_assert_nonnull (act);

      evt = act->getEvent ();
      g_assert_nonnull (evt);
      if (evt->getType () == NclEventType::ATTRIBUTION)
        {
          string dur;
          string value;
          if (act->getParameter ("duration", &dur))
            evt->setParameter ("duration", dur);
          if (act->getParameter ("value", &value))
            evt->setParameter ("value", value);
        }

      if (!evt->transition (act->getTransition ()))
        continue;

      n++;
      done = false;
      obj = evt->getObject ();
      g_assert_nonnull (obj);

      // Trigger links in parent context.
      comp = obj->getParent ();
      if (comp != nullptr
          && instanceof (FormatterContext *, comp)
          && comp->isOccurring ())
        {
          ctx = cast (FormatterContext *, comp);
          g_assert_nonnull (ctx);

        trigger:
          for (auto link: *ctx->getLinks ())
            {
              if (link->getDisabled ())
                continue;

              for (auto cond: *link->getConditions ())
                {
                  FormatterPredicate *pred;

                  if (cond->getEvent () != evt)
                    continue;

                  if (cond->getTransition () != act->getTransition ())
                    continue;

                  pred = cond->getPredicate ();
                  if (pred != nullptr && !this->evalPredicate (pred))
                    continue;

                  // Success.
                  auto acts = *link->getActions ();
                  std::list<FormatterAction *>::reverse_iterator rit
                    = acts.rbegin ();
                  for (; rit != acts.rend (); ++rit)
                    stack.push_back (*rit);
                }
            }
        }

      // Trigger links in context itself.
      if (!done && instanceof (FormatterContext *, obj))
        {
          ctx = cast (FormatterContext *, obj);
          g_assert_nonnull (ctx);
          done = true;
          goto trigger;
        }
    }
  return n;
}

FormatterObject *
Formatter::obtainExecutionObject (NclNode *node)
{
  string id;
  NclNode *parentNode;
  FormatterComposition *parent;
  FormatterObject *object;

  id = node->getId ();
  g_assert (id != "");

  if ((object = this->getObjectByIdOrAlias (id)) != nullptr)
    return object;              // already created

  // Get parent.
  parentNode = node->getParent ();
  if (parentNode == nullptr)
    {
      parent = nullptr;
    }
  else
    {
      parent = cast (FormatterComposition *,
                     obtainExecutionObject (parentNode));
      g_assert_nonnull (parent);
      if ((object = this->getObjectByIdOrAlias (id)) != nullptr)
        return object;
    }

  // Solve refer, if needed.
  if (instanceof (NclMediaRefer *, node))
    {
      NclNode *target;

      TRACE ("solving refer %s", node->getId ().c_str ());
      target = node->derefer ();
      g_assert (!instanceof (NclMediaRefer *, target));
      object = obtainExecutionObject (target->derefer ());
      object->addAlias (id);
      return object;
    }

  if (instanceof (NclSwitch *, node)) // switch
    {
      TRACE ("creating switch %s", node->getId ().c_str ());
      object = new FormatterSwitch (this, id);
      g_assert_nonnull (object);
      if (parent != nullptr)
        parent->addChild (object);
      this->addObject (object);

      for (auto item: *cast (NclSwitch *, node)->getRules ())
        {
          FormatterObject *obj;

          g_assert_nonnull (item.first);
          g_assert_nonnull (item.second);

          obj = this->obtainExecutionObject (item.first);
          g_assert_nonnull (obj);
          cast (FormatterSwitch *, object)->addRule (obj, item.second);
        }
      return object;
    }
  else if (instanceof (NclContext *, node)) // context
    {
      TRACE ("creating context %s", node->getId ().c_str ());
      object = new FormatterContext (this, id);
      g_assert_nonnull (object);
      if (parent != nullptr)
        parent->addChild (object);
      this->addObject (object);

      NclContext *ctx = cast (NclContext *, node);
      for (auto port: *ctx->getPorts ())
        {
          NclNode *target;
          NclAnchor *iface;
          FormatterObject *child;
          FormatterEvent *e;

          port->getTarget (&target, &iface);
          child = this->obtainExecutionObject (target);
          g_assert_nonnull (child);

          if (!instanceof (NclArea *, iface))
            continue;       // nothing to do

          e = child->obtainEvent
            (NclEventType::PRESENTATION, iface, "");
          g_assert_nonnull (e);
          cast (FormatterContext *, object)->addPort (e);
        }
      for (auto link: *(ctx->getLinks ()))
        cast (FormatterContext *, object)
          ->addLink (obtainFormatterLink (link));

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
          g_assert_null (_settings);
          object = new FormatterMediaSettings (this, id);
        }
      else
        {
          object = new FormatterMedia
            (this, id, media->getMimeType (), media->getSrc ());
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
      this->addObject (object);
      return object;
    }
  else
    {
      g_assert_not_reached ();
    }
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
    self->_background = ginga_parse_color (value);
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
  static int n = 0;
  g_assert (name == "opengl");
  if (unlikely (n++ > 0))
    ERROR ("Cannot change to 'opengl' on-the-fly");
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

vector<FormatterObject *>
Formatter::getObjectsVector ()
{
  vector<FormatterObject *> buf;
  for (auto obj: _objects)
    buf.push_back (obj);
  return buf;
}

FormatterEvent *
Formatter::obtainFormatterEventFromBind (NclBind *bind)
{
  NclNode *node;
  FormatterObject *obj;
  NclAnchor *iface;
  NclRole *role;
  NclEventType type;
  string key = "";

  node = bind->getNode ();
  g_assert_nonnull (node);

  iface = bind->getInterface ();
  if (iface != nullptr && instanceof (NclPort *, iface))
    cast (NclPort *, iface)->getTarget (&node, nullptr);

  obj = obtainExecutionObject (node);
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

  role = bind->getRole ();
  g_assert_nonnull (role);

  type = role->getEventType ();
  if (type == NclEventType::SELECTION)
    {
      NclCondition *cond = cast (NclCondition *, role);
      if (cond != nullptr)
        {
          key = cond->getKey ();
          if (key[0] == '$')
            key = bind->getParameter
              (key.substr (1, key.length () - 1));
        }
    }

  return obj->obtainEvent (type, iface, key);
}

FormatterLink *
Formatter::obtainFormatterLink (NclLink *docLink)
{
  NclConnector *connector;
  FormatterLink *link;

  g_assert_nonnull (docLink);
  connector = cast (NclConnector *, docLink->getConnector ());
  g_assert_nonnull (connector);

  link = new FormatterLink ();
  for (auto connCond: *connector->getConditions ())
    {
      FormatterPredicate *pred = connCond->getPredicate ();
      if (pred != nullptr)      // solve ghost binds in predicate
        {
          vector <FormatterPredicate *> buf;

          pred = pred->clone ();
          g_assert_nonnull (pred);

          buf.push_back (pred);
          while (!buf.empty ())
            {
              FormatterPredicate *p = buf.back ();
              buf.pop_back ();
              switch (p->getType ())
                {
                case PredicateType::FALSUM:
                case PredicateType::VERUM:
                  break;        // nothing to do
                case PredicateType::ATOM:
                  {
                    PredicateTestType test;
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
                case PredicateType::NEGATION:
                case PredicateType::CONJUNCTION:
                case PredicateType::DISJUNCTION:
                  for (auto child: *p->getChildren ())
                    buf.push_back (child);
                  break;
                default:
                  g_assert_not_reached ();
                }
            }
        }
      for (auto bind: docLink->getBinds (connCond))
        {
          FormatterEvent *evt;
          FormatterCondition *cond;

          evt = this->obtainFormatterEventFromBind (bind);
          g_assert_nonnull (evt);

          cond = new FormatterCondition
            (pred, evt, connCond->getTransition ());
          link->addCondition (cond);
        }
    }

  for (auto connAct: *connector->getActions ())
    {
      for (auto bind: docLink->getBinds (connAct))
        {
          FormatterEvent *evt;
          FormatterAction *act;

          evt = this->obtainFormatterEventFromBind (bind);
          g_assert_nonnull (evt);

          act = new FormatterAction (evt, connAct->getTransition ());
          if (evt->getType () == NclEventType::ATTRIBUTION)
            {
              string dur;
              string value;

              dur = connAct->getDuration ();
              if (dur[0] == '$')
                dur = bind->getParameter
                  (dur.substr (1, dur.length () - 1));

              value = connAct->getValue ();
              if (value[0] == '$')
                value = bind->getParameter
                  (value.substr (1, value.length () - 1));

              act->setParameter ("duration", dur);
              act->setParameter ("value", value);
            }
          link->addAction (act);
        }
    }

  return link;
}

GINGA_NAMESPACE_END
