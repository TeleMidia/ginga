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
#include "FormatterScheduler.h"

GINGA_NAMESPACE_BEGIN


// Public.

FormatterScheduler::FormatterScheduler (Formatter *ginga)
{
  g_assert_nonnull (ginga);
  _ginga = ginga;
  _doc = nullptr;
  _settings = nullptr;
}

FormatterScheduler::~FormatterScheduler ()
{
  for (auto obj: _objects)
    delete obj;
}

bool
FormatterScheduler::run (NclDocument *doc)
{
  string id;
  NclContext *body;

  g_assert_nonnull (doc);
  _doc = doc;

  id = _doc->getId ();
  body = _doc->getRoot ();
  g_assert_nonnull (body);

  // Create dummy settings node.
  NclMedia *settings_node =  new NclMedia (_doc, "__settings__", true);
  body->addNode (settings_node);
  NclProperty *prop = new NclProperty (_doc, "service.currentFocus");
  prop->setValue ("");
  settings_node->addAnchor (prop);
  _settings = cast (FormatterMediaSettings *,
                    this->obtainExecutionObject (settings_node));
  g_assert_nonnull (_settings);

  // Create execution object for settings node and initialize it.
  vector<NclNode *> *nodes = _doc->getSettingsNodes ();
  for (auto node: *nodes)
    {
      NclMedia *content;

      content = (NclMedia *) node;
      if (content != settings_node)
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

          cast (FormatterObject *, _settings)->setProperty (name, value, 0);
        }
    }
  delete nodes;

  // Set global settings object.
  g_assert_nonnull (_settings->obtainLambda ());

  // Start document.
  FormatterObject *obj = this->obtainExecutionObject (body);
  FormatterEvent *evt = obj->obtainLambda ();
  g_assert_nonnull (evt);
  if (!evt->transition (NclEventStateTransition::START))
    return false;

  // Refresh current focus.
  _settings->updateCurrentFocus ("");

  // Success.
  return true;
}

FormatterMediaSettings *
FormatterScheduler::getSettings ()
{
  return _settings;
}

const set<FormatterObject *> *
FormatterScheduler::getObjects ()
{
  return &_objects;
}

FormatterObject *
FormatterScheduler::getObjectById (const string &id)
{
  for (auto obj: _objects)
    if (obj->getId () == id)
      return obj;
  return nullptr;
}

FormatterObject *
FormatterScheduler::getObjectByIdOrAlias (const string &id)
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
FormatterScheduler::getObjectPropertyByRef (const string &ref, string *result)
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

bool
FormatterScheduler::addObject (FormatterObject *obj)
{
  g_assert_nonnull (obj);
  if (_objects.find (obj) != _objects.end ()
      || getObjectByIdOrAlias (obj->getId ()) != nullptr)
    {
      return false;
    }
  _objects.insert (obj);
  return true;
}

static bool
cmpz (FormatterObject *a, FormatterObject *b)
{
  int z1, zo1, z2, zo2;

  g_assert_nonnull (a);
  g_assert_nonnull (b);

  z1 = zo1 = z2 = zo2 = 0;      // fixme
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

void
FormatterScheduler::redraw (cairo_t *cr)
{
  bool opengl;
  GingaColor background;
  int width, height;
  vector<FormatterObject *> objs;

  opengl = _ginga->getOptionBool ("opengl");
  background = ginga_parse_color (_ginga->getOptionString ("background"));
  width = _ginga->getOptionInt ("width");
  height = _ginga->getOptionInt ("height");

  if (opengl)
    {
      GL::beginDraw ();
      GL::clear_scene (width, height);
    }

  if (background.alpha > 0)
    {
      if (opengl)
        {
          GL::draw_quad (0, 0, width, height,
                         (float) background.red,
                         (float) background.green,
                         (float) background.blue,
                         (float) background.alpha);
        }
      else
        {
          cairo_save (cr);
          cairo_set_source_rgba (cr,
                                 background.red,
                                 background.green,
                                 background.blue,
                                 background.alpha);
          cairo_rectangle (cr, 0, 0, width, height);
          cairo_fill (cr);
          cairo_restore (cr);
        }
    }

  for (auto obj: _objects)      // fixme
    if (!instanceof (FormatterContext *, obj)
        && !instanceof (FormatterSwitch *, obj))
      objs.push_back (obj);
  std::sort (objs.begin (), objs.end (), cmpz);
  for (auto obj: objs)
    obj->redraw (cr);

  // if (_opts.debug)
  //   {
  //     static GingaColor fg = {1., 1., 1., 1.};
  //     static GingaColor bg = {0, 0, 0, 0};
  //     static GingaRect rect = {0, 0, 0, 0};
  //     string info;
  //     cairo_surface_t *debug;
  //     GingaRect ink;
  //     info = xstrbuild ("%s: #%lu %" GINGA_TIME_FORMAT " %.1ffps",
  //                       _ncl_file.c_str (),
  //                       _last_tick_frameno,
  //                       GINGA_TIME_ARGS (_last_tick_total),
  //                       1 * GINGA_SECOND / (double) _last_tick_diff);
  //     rect.width = _opts.width;
  //     rect.height = _opts.height;
  //     debug = PlayerText::renderSurface
  //       (info, "monospace", "", "bold", "9", fg, bg,
  //        rect, "center", "", true, &ink);
  //     ink = {0, 0, rect.width, ink.height - ink.y + 4};
  //     cairo_save (cr);
  //     cairo_set_source_rgba (cr, 1., 0., 0., .5);
  //     cairo_rectangle (cr, 0, 0, ink.width, ink.height);
  //     cairo_fill (cr);
  //     cairo_set_source_surface (cr, debug, 0, 0);
  //     cairo_paint (cr);
  //     cairo_restore (cr);
  //     cairo_surface_destroy (debug);
  //   }
}

void
FormatterScheduler::resize (int width, int height)
{
  g_assert (width == _ginga->getOptionInt ("width"));
  g_assert (height == _ginga->getOptionInt ("height"));
  for (auto obj: _objects)
    {
      obj->setProperty ("top", obj->getProperty ("top"));
      obj->setProperty ("left", obj->getProperty ("left"));
      obj->setProperty ("width", obj->getProperty ("width"));
      obj->setProperty ("height", obj->getProperty ("height"));
    }
}

void
FormatterScheduler::sendKeyEvent (const string &key, bool press)
{
  vector<FormatterObject *> buf;
  for (auto obj: _objects)
    if (instanceof (FormatterMediaSettings *, obj)
        || obj->isOccurring ())
      buf.push_back (obj);
  for (auto obj: buf)
    obj->sendKeyEvent (key, press);
}

void
FormatterScheduler::sendTickEvent (GingaTime total, GingaTime diff, GingaTime frame)
{
  vector<FormatterObject *> buf;

  for (auto obj: _objects)
    if (obj->isOccurring ())
      buf.push_back (obj);

  if (buf.empty ())
    {
      _ginga->setEOS (true);
      return;
    }

  for (auto obj: buf)
    {
      g_assert (!instanceof (FormatterMediaSettings *, obj));
      if (!obj->isOccurring ())
        continue;
      obj->sendTickEvent (total, diff, frame);
    }
  _settings->sendTickEvent (total, diff, frame);
}

FormatterObject *
FormatterScheduler::obtainExecutionObject (NclNode *node)
{
  string id;
  NclNode *parentNode;
  FormatterContext *parent;
  FormatterObject *object;

  id = node->getId ();
  g_assert (id != "");

  // Already created.
  if ((object = this->getObjectByIdOrAlias (id)) != nullptr)
    return object;

  // Get parent.
  parentNode = node->getParent ();
  if (parentNode == nullptr)
    {
      parent = nullptr;
    }
  else
    {
      parent = cast (FormatterContext *,
                     obtainExecutionObject (parentNode));
      g_assert_nonnull (parent);
      if ((object = this->getObjectByIdOrAlias (id)) != nullptr)
        return object;
    }

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

  if (instanceof (NclSwitch *, node))
    {
      TRACE ("creating switch %s", node->getId ().c_str ());
      object = new FormatterSwitch (_ginga, id, node);
      g_assert_nonnull
        (object->obtainEvent (NclEventType::PRESENTATION,
                              node->getLambda (), ""));
      goto done;
    }

  if (instanceof (NclContext *, node))
    {
      TRACE ("creating context %s", node->getId ().c_str ());
      object = new FormatterContext (_ginga, id, node);
      g_assert_nonnull
        (object->obtainEvent (NclEventType::PRESENTATION,
                              node->getLambda (), ""));

      g_assert_nonnull (object);
      if (parent != nullptr)
        object->initParent (parent);
      g_assert (this->addObject (object));

      NclContext *ctx = cast (NclContext *, node);
      for (auto link: *(ctx->getLinks ()))
        g_assert (cast (FormatterContext *, object)
                  ->addLink (obtainFormatterLink (link)));

      return object;
    }

  g_assert (instanceof (NclMedia *, node));
  TRACE ("creating media %s", node->getId ().c_str ());
  NclMedia *media;
  media = cast (NclMedia *, node);
  g_assert_nonnull (media);
  if (media->isSettings ())
    {
      object = new FormatterMediaSettings (_ginga, id, node);
      //_ruleAdapter->setSettings (object);
    }
  else
    {
      object = new FormatterObject (_ginga, id, node);
    }

 done:
  g_assert_nonnull (object);
  if (parent != nullptr)
    object->initParent (parent);
  g_assert (this->addObject (object));
  return object;
}


// Private.

FormatterEvent *
FormatterScheduler::obtainFormatterEventFromBind (NclBind *bind)
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
    return obj->obtainLambda ();

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
FormatterScheduler::obtainFormatterLink (NclLink *docLink)
{
  NclConnector *connector;
  FormatterLink *link;

  g_assert_nonnull (docLink);
  connector = cast (NclConnector *, docLink->getConnector ());
  g_assert_nonnull (connector);

  link = new FormatterLink (_ginga);

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

          cond = new FormatterCondition (pred, evt,
                                         connCond->getTransition ());
          g_assert (link->addCondition (cond));
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

              act->setDuration (dur);
              act->setValue (value);
            }
          g_assert (link->addAction (act));
        }
    }

  return link;
}

bool
FormatterScheduler::eval (FormatterPredicate *pred)
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
            if (!this->eval (child))
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

GINGA_NAMESPACE_END
