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
#include "Converter.h"

GINGA_FORMATTER_BEGIN


// Public.

Scheduler::Scheduler (GingaInternal *ginga)
{
  g_assert_nonnull (ginga);
  _ginga = ginga;
  _converter = nullptr;
  _doc = nullptr;
  _settings = nullptr;
}

Scheduler::~Scheduler ()
{
  for (auto obj: _objects)
    delete obj;
  _objects.clear ();

  if (_converter != nullptr)
    delete _converter;
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

  // Create converter.
  _converter = new Converter (_ginga, new RuleAdapter ());

  // Create execution object for settings node and initialize it.
  ExecutionObjectSettings *settings = nullptr;
  vector<Node *> *nodes = _doc->getSettingsNodes ();
  for (auto node: *nodes)
    {
      Media *content;

      if (settings == nullptr)
        {
          ExecutionObject *obj;
          obj = _converter->obtainExecutionObject (node);
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

  // Start document.
  ExecutionObject *obj = _converter->obtainExecutionObject (body);
  NclEvent *evt = obj->getLambda (EventType::PRESENTATION);
  NclAction *act = new NclAction (evt, EventStateTransition::START);
  runAction (evt, act);
  delete act;

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
    if (instanceof (ExecutionObjectSettings *, obj) || obj->isOccurring ())
      buf.push_back (obj);
  for (auto obj: buf)
    obj->sendKeyEvent (key, press);
}

void
Scheduler::sendTickEvent (GingaTime total, GingaTime diff, GingaTime frame)
{
  vector<ExecutionObject *> buf;
  for (auto obj: _objects)
    if (obj->isOccurring ())
      buf.push_back (obj);
  for (auto obj: buf)
    {
      g_assert (!instanceof (ExecutionObjectSettings *, obj));
      obj->sendTickEvent (total, diff, frame);
    }
  _settings->sendTickEvent (total, diff, frame);
}

void
Scheduler::scheduleAction (NclAction *action)
{
  runAction (action->getEvent (), action);
}


// Private.

void
Scheduler::runAction (NclEvent *event, NclAction *action)
{
  ExecutionObject *obj;
  string name;

  obj = event->getObject ();
  g_assert_nonnull (obj);

  name = EventUtil::getEventStateTransitionAsString
    (action->getEventStateTransition ());

  TRACE ("running %s over %s",
         name.c_str (), obj->getId ().c_str ());

  if (event->getType () == EventType::SELECTION)
    {
      event->transition (EventStateTransition::START);
      event->transition (EventStateTransition::STOP);
      delete action;
      return;
    }

  if (instanceof (ExecutionObjectSwitch *, obj))
    {
      g_assert_not_reached ();
      return;
    }

  if (instanceof (ExecutionObjectContext *, obj))
    {
      this->runActionOverComposition
        ((ExecutionObjectContext *) obj, action);
      return;
    }

  if (event->getType () == EventType::ATTRIBUTION)
    {
      Property *property;
      string name;
      string value;
      GingaTime dur;

      g_assert (action->getEventStateTransition ()
                == EventStateTransition::START);

      if (event->getState () != EventState::SLEEPING)
        return;                 // nothing to do

      property = cast (Property *, event->getAnchor ());
      g_assert_nonnull (property);

      name = property->getName ();
      value = action->getValue ();
      if (value[0] == '$')
        this->getObjectPropertyByRef (value, &value);

      string s;
      s = action->getDuration ();
      if (s[0] == '$')
        this->getObjectPropertyByRef (s, &s);
      dur = ginga_parse_time (s);

      if (event->transition (EventStateTransition::START))
        {
          obj->setProperty (name, value, dur);
          // TODO: Wrap this in a closure to be called at the end of
          // animation.
          event->transition (EventStateTransition::STOP);
        }
      return;
    }

  switch (action->getEventStateTransition ())
    {
    case EventStateTransition::START:
      obj->prepare (event);
      g_assert (obj->start ());
      break;
    case EventStateTransition::STOP:
      obj->stop ();
      break;
    case EventStateTransition::PAUSE:
      g_assert (obj->pause ());
      break;
    case EventStateTransition::RESUME:
      g_assert (obj->resume ());
      break;
    case EventStateTransition::ABORT:
      g_assert (obj->abort ());
      break;
    default:
      g_assert_not_reached ();
    }
}

void
Scheduler::runActionOverComposition (ExecutionObjectContext *ctxObj,
                                     NclAction *action)
{
  NclEvent *event;
  EventType type;
  EventStateTransition acttype;

  Node *node;
  Entity *entity;
  Composition *compNode;

  event = action->getEvent ();
  g_assert_nonnull (event);

  type = event->getType ();

  if (type == EventType::ATTRIBUTION)
    {
      ERROR_NOT_IMPLEMENTED
        ("context property attribution is not supported");
    }

  if (type == EventType::SELECTION)
    {
      WARNING ("trying to select composition '%s'",
               ctxObj->getId ().c_str ());
      return;
    }

  node = ctxObj->getNode ();
  g_assert_nonnull (node);

  entity = cast (Entity *, node);
  g_assert_nonnull (entity);

  compNode = cast (Composition *, entity);
  g_assert_nonnull (compNode);

  acttype = action->getEventStateTransition ();
  if (acttype == EventStateTransition::START) // start all ports
    {
      for (auto port: *compNode->getPorts ())
        {
          Node *node;
          Anchor *iface;
          ExecutionObject *child;
          NclEvent *evt;

          port->getTarget (&node, &iface);
          child = _converter->obtainExecutionObject (node);
          g_assert_nonnull (child);

          if (!instanceof (Area *, iface))
            continue;           // nothing to do

          evt = child->obtainEvent (EventType::PRESENTATION, iface, "");
          g_assert_nonnull (evt);
          g_assert (evt->getType () == EventType::PRESENTATION);

          runAction (evt, action);

        }
    }
  else if (acttype == EventStateTransition::STOP) // stop all children
    {
      //ctxObj->suspendLinkEvaluation (true);
      for (auto child: *ctxObj->getChildren ())
        {
          NclEvent *evt;
          evt = child->getLambda (EventType::PRESENTATION);
          if (evt == nullptr)
            continue;
          runAction (evt, action);
        }
      //ctxObj->suspendLinkEvaluation (false);
    }
  else if (acttype == EventStateTransition::ABORT)
    {
      ERROR_NOT_IMPLEMENTED ("action 'abort' is not supported");
    }
  else if (acttype == EventStateTransition::PAUSE)
    {
      ERROR_NOT_IMPLEMENTED ("action 'pause' is not supported");
    }
  else if (acttype == EventStateTransition::RESUME)
    {
      ERROR_NOT_IMPLEMENTED ("action 'resume' is not supported");
    }
  else
    {
      g_assert_not_reached ();
    }
}

GINGA_FORMATTER_END
