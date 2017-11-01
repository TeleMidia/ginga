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
#include "Scheduler.h"

GINGA_FORMATTER_BEGIN


// Public.

Scheduler::Scheduler (GingaInternal *ginga)
{
  g_assert_nonnull (ginga);
  _ginga = ginga;
  _doc = nullptr;
  _settings = nullptr;
}

Scheduler::~Scheduler ()
{
  for (auto obj: _objects)
    delete obj;
}

bool
Scheduler::run (NclDocument *doc)
{
  string id;
  Context *body;

  g_assert_nonnull (doc);
  _doc = doc;

  id = _doc->getId ();
  body = _doc->getRoot ();
  g_assert_nonnull (body);

  // Insert dummy settings node.
  Media *dummy =  new Media (_doc, "__settings__", true);
  body->addNode (dummy);
  Property *prop = new Property (_doc, "service.currentFocus");
  prop->setValue ("");
  dummy->addAnchor (prop);

  // Create execution object for settings node and initialize it.
  ExecutionObjectSettings *settings = nullptr;
  vector<Node *> *nodes = _doc->getSettingsNodes ();
  for (auto node: *nodes)
    {
      Media *content;

      if (settings == nullptr)
        {
          ExecutionObject *obj;
          obj = this->obtainExecutionObject (node);
          g_assert_nonnull (obj);
          settings = cast (ExecutionObjectSettings *, obj);
          g_assert_nonnull (settings);
        }

      content = (Media *) node;
      for (auto anchor: *content->getAnchors ())
        {
          Property *prop;
          string name;
          string value;

          if (!instanceof (Property *, anchor))
            continue;           // nothing to do

          prop = cast (Property *, anchor);
          name = prop->getName ();
          value = prop->getValue ();
          if (value == "")
            continue;           // nothing to do

          cast (ExecutionObject *, settings)->setProperty (name, value, 0);
        }
    }
  delete nodes;

  // Set global settings object.
  g_assert_nonnull (settings);
  _settings = settings;
  g_assert_nonnull (_settings->obtainEvent
                    (EventType::PRESENTATION,
                     _settings->getNode ()->getLambda (), ""));

  // Start document.
  ExecutionObject *obj = this->obtainExecutionObject (body);
  NclEvent *evt = obj->getLambda (EventType::PRESENTATION);
  if (!evt->transition (EventStateTransition::START))
    return false;

  // Refresh current focus.
  settings->updateCurrentFocus ("");

  // Success.
  return true;
}

ExecutionObjectSettings *
Scheduler::getSettings ()
{
  return _settings;
}

const set<ExecutionObject *> *
Scheduler::getObjects ()
{
  return &_objects;
}

ExecutionObject *
Scheduler::getObjectById (const string &id)
{
  for (auto obj: _objects)
    if (obj->getId () == id)
      return obj;
  return nullptr;
}

ExecutionObject *
Scheduler::getObjectByIdOrAlias (const string &id)
{
  ExecutionObject *obj;
  if ((obj = this->getObjectById (id)) != nullptr)
    return obj;
  for (auto obj: _objects)
    if (obj->hasAlias (id))
      return obj;
  return nullptr;
}

bool
Scheduler::getObjectPropertyByRef (const string &ref, string *result)
{
  size_t i;
  string id;
  string name;
  ExecutionObject *object;

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
Scheduler::addObject (ExecutionObject *obj)
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
cmpz (ExecutionObject *a, ExecutionObject *b)
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
Scheduler::redraw (cairo_t *cr)
{
  bool opengl;
  GingaColor background;
  int width, height;
  vector<ExecutionObject *> objs;

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
    if (!instanceof (ExecutionObjectContext *, obj)
        && !instanceof (ExecutionObjectSwitch *, obj))
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
  //     debug = TextPlayer::renderSurface
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
Scheduler::resize (int width, int height)
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
Scheduler::sendKeyEvent (const string &key, bool press)
{
  vector<ExecutionObject *> buf;
  for (auto obj: _objects)
    if (instanceof (ExecutionObjectSettings *, obj)
        || obj->getLambdaState () == EventState::OCCURRING)
      buf.push_back (obj);
  for (auto obj: buf)
    obj->sendKeyEvent (key, press);
}

void
Scheduler::sendTickEvent (GingaTime total, GingaTime diff, GingaTime frame)
{
  vector<ExecutionObject *> buf;

  for (auto obj: _objects)
    if (obj->getLambdaState () == EventState::OCCURRING)
      buf.push_back (obj);

  if (buf.empty ())
    {
      _ginga->setEOS (true);
      return;
    }

  for (auto obj: buf)
    {
      g_assert (!instanceof (ExecutionObjectSettings *, obj));
      if (obj->getLambdaState () != EventState::OCCURRING)
        continue;
      obj->sendTickEvent (total, diff, frame);
    }
  _settings->sendTickEvent (total, diff, frame);
}

ExecutionObject *
Scheduler::obtainExecutionObject (Node *node)
{
  string id;
  Node *parentNode;
  ExecutionObjectContext *parent;
  ExecutionObject *object;

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
      parent = cast (ExecutionObjectContext *,
                     obtainExecutionObject (parentNode));
      g_assert_nonnull (parent);
      if ((object = this->getObjectByIdOrAlias (id)) != nullptr)
        return object;
    }

  if (instanceof (Refer *, node))
    {
      Node *target;

      TRACE ("solving refer %s", node->getId ().c_str ());
      target = node->derefer ();
      g_assert (!instanceof (Refer *, target));
      object = obtainExecutionObject (target->derefer ());
      object->addAlias (id);
      return object;
    }

  if (instanceof (Switch *, node))
    {
      TRACE ("creating switch %s", node->getId ().c_str ());
      object = new ExecutionObjectSwitch (_ginga, id, node);
      g_assert_nonnull
        (object->obtainEvent (EventType::PRESENTATION,
                              node->getLambda (), ""));
      goto done;
    }

  if (instanceof (Context *, node))
    {
      TRACE ("creating context %s", node->getId ().c_str ());
      object = new ExecutionObjectContext (_ginga, id, node);
      g_assert_nonnull
        (object->obtainEvent (EventType::PRESENTATION,
                              node->getLambda (), ""));

      g_assert_nonnull (object);
      if (parent != nullptr)
        object->initParent (parent);
      g_assert (this->addObject (object));

      Context *ctx = cast (Context *, node);
      for (auto link: *(ctx->getLinks ()))
        g_assert (cast (ExecutionObjectContext *, object)
                  ->addLink (obtainNclLink (link)));

      return object;
    }

  g_assert (instanceof (Media *, node));
  TRACE ("creating media %s", node->getId ().c_str ());
  Media *media;
  media = cast (Media *, node);
  g_assert_nonnull (media);
  if (media->isSettings ())
    {
      object = new ExecutionObjectSettings (_ginga, id, node);
      //_ruleAdapter->setSettings (object);
    }
  else
    {
      object = new ExecutionObject (_ginga, id, node);
    }

 done:
  g_assert_nonnull (object);
  if (parent != nullptr)
    object->initParent (parent);
  g_assert (this->addObject (object));
  return object;
}


// Private.

NclEvent *
Scheduler::obtainNclEventFromBind (Bind *bind)
{
  Node *node;
  ExecutionObject *obj;
  Anchor *iface;
  Role *role;
  EventType type;
  string key = "";

  node = bind->getNode ();
  g_assert_nonnull (node);

  iface = bind->getInterface ();
  if (iface != nullptr
      && instanceof (Port *, iface)
      && !(instanceof (SwitchPort *, iface)))
    {
      cast (Port *, iface)->getTarget (&node, nullptr);
    }

  obj = obtainExecutionObject (node);
  g_assert_nonnull (obj);

  if (iface == nullptr)
    return obj->getLambda (EventType::PRESENTATION);

  if (instanceof (Composition *, node)
      && instanceof (Port *, iface))
    {
      Composition *comp = cast (Composition *, node);
      Port *port = cast (Port *, iface);
      iface = comp->getMapInterface (port);
    }

  role = bind->getRole ();
  g_assert_nonnull (role);

  type = role->getEventType ();
  if (type == EventType::SELECTION)
    {
      Condition *cond = cast (Condition *, role);
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

NclLink *
Scheduler::obtainNclLink (Link *docLink)
{
  Connector *connector;
  NclLink *link;

  g_assert_nonnull (docLink);
  connector = cast (Connector *, docLink->getConnector ());
  g_assert_nonnull (connector);

  link = new NclLink (_ginga);

  for (auto connCond: *connector->getConditions ())
    {
      for (auto bind: docLink->getBinds (connCond))
        {
          NclEvent *evt;
          NclCondition *cond;

          evt = this->obtainNclEventFromBind (bind);
          g_assert_nonnull (evt);

          cond = new NclCondition (evt, connCond ->getTransition ());
          g_assert (link->addCondition (cond));
        }
    }

  for (auto connAct: *connector->getActions ())
    {
      for (auto bind: docLink->getBinds (connAct))
        {
          NclEvent *evt;
          NclAction *act;

          evt = this->obtainNclEventFromBind (bind);
          g_assert_nonnull (evt);

          act = new NclAction (evt, connAct->getTransition ());
          if (evt->getType () == EventType::ATTRIBUTION)
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

GINGA_FORMATTER_END
