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
#include "Document.h"
#include "LuaAPI.h"

#include "Context.h"
#include "Media.h"
#include "MediaSettings.h"
#include "Object.h"
#include "Switch.h"

GINGA_NAMESPACE_BEGIN

Document::Document ()
{
  Object *obj;

  _L = luaL_newstate ();
  g_assert_nonnull (_L);
  luaL_openlibs (_L);
  LuaAPI::_Document_attachWrapper (_L, this);

  _root = new Context (this, NULL, "__root__");
  _objects.insert (_root);
  _objectsById[_root->getId ()] = _root;

  _settings = NULL;
  obj = this->createObject (Object::MEDIA_SETTINGS, _root, "__settings__");
  _settings = cast (MediaSettings *, obj);
  g_assert_nonnull (_settings);
}

Document::~Document ()
{
  g_assert_nonnull (_root);
  delete _root;

  g_assert_nonnull (_L);
  LuaAPI::_Document_detachWrapper (_L, this);
  lua_close (_L);
}

lua_State *
Document::getLuaState ()
{
  return _L;
}

string
Document::toString ()
{
  return xstrbuild ("Document (%p)", this);
}

void
Document::getObjects (set<Object *> *objects, unsigned int mask)
{
  g_return_if_fail (objects != NULL);

  for (auto obj: _objects)
    if (obj->getType () & mask)
      objects->insert (obj);
}

Object *
Document::getObject (const string &id)
{
  auto it = _objectsById.find (id);
  if (it != _objectsById.end ())
    return it->second;          // found id

  for (auto obj : _objects)
    if (obj->hasAlias (id))
      return obj;               // found alias

  return NULL;                  // no match
}

Context *
Document::getRoot ()
{
  g_assert_nonnull (_root);
  return _root;
}

MediaSettings *
Document::getSettings ()
{
  g_assert_nonnull (_settings);
  return _settings;
}

Object *
Document::createObject (Object::Type type, Composition *parent,
                        const string &id)
{
  Object *obj;

  g_return_val_if_fail (parent != NULL, NULL);

  if (type == Object::MEDIA_SETTINGS && _settings != NULL)
    return NULL;                // settings already in document

  if (_objects.find (parent) == _objects.end ())
    return NULL;                // parent not in document

  if (this->getObject (id) != NULL)
    return NULL;                // id already in document

  obj = NULL;
  switch (type)
    {
    case Object::MEDIA:
      obj = new Media (this, parent, id);
      break;
    case Object::MEDIA_SETTINGS:
      obj = new MediaSettings (this, parent, id);
      break;
    case Object::CONTEXT:
      obj = new Context (this, parent, id);
      break;
    case Object::SWITCH:
      obj = new Switch (this, parent, id);
      break;
    default:
      g_assert_not_reached ();
    }
  g_assert_nonnull (obj);

  _objects.insert (obj);
  _objectsById[id] = obj;

  return obj;
}

// TODO --------------------------------------------------------------------

/**
 * @brief Evaluates action over document.
 */
int
Document::evalAction (Event *event, Event::Transition transition,
                      const string &value)
{
  Action act;
  act.event = event;
  g_assert_nonnull (event);
  act.transition = transition;
  act.predicate = NULL;
  act.value = value;
  return this->evalAction (act);
}

/**
 * @brief Evaluates action over Context.
 */
list<Action>
Document::evalActionInContext (Action act, Context *ctx)
{
  list<Action> stack;
  Event *evt;

  evt = act.event;
  g_assert_nonnull (evt);

  if (!ctx->getLinksStatus ())
    return stack;
  for (auto link : *ctx->getLinks ())
    {
      for (auto cond : link.first)
        {
          Predicate *pred;

          if (cond.event != evt || cond.transition != act.transition)
            continue;

          pred = cond.predicate;
          if (pred != NULL && !this->evalPredicate (pred))
            continue;

          // Success.
          auto acts = link.second;
          for (auto ri = acts.rbegin (); ri != acts.rend (); ++ri)
            {
              Action next_act = *(ri);
              string s;
              Time delay;

              if (!this->evalPropertyRef (next_act.delay, &s))
                {
                  s = next_act.delay;
                }

              delay = ginga::parse_time (s);

              if (delay == 0 || delay == GINGA_TIME_NONE)
                {
                  stack.push_back (*ri);
                }
              else
                {
                  Event *next_evt = next_act.event;
                  g_assert_nonnull (next_evt);
                  Object *next_obj = next_evt->getObject ();
                  g_assert_nonnull (next_obj);

                  ctx->addDelayedAction (next_evt, next_act.transition,
                                         next_act.value, delay);
                }
            }
        }
    }
  return stack;
}

/**
 * @brief Evaluates action over document.
 */
int
Document::evalAction (Action init)
{
  list<Action> stack;
  int n;

  stack.push_back (init);
  n = 0;

  while (!stack.empty ())
    {
      Action act;
      Event *evt;
      Object *obj;
      Composition *comp;
      Context *ctx_parent, *ctx_grandparent;

      act = stack.back ();
      stack.pop_back ();

      evt = act.event;
      g_assert_nonnull (evt);

      TRACE ("trigger stacked action: %s %s",
             Event::getTransitionAsString (act.transition).c_str (),
             act.event->getQualifiedId ().c_str ());

      evt->setParameter ("duration", act.duration);
      if (evt->getType () == Event::ATTRIBUTION)
        evt->setParameter ("value", act.value);

      if (!evt->transition (act.transition))
        continue;

      n++;
      obj = evt->getObject ();
      g_assert_nonnull (obj);
      comp = obj->getParent ();

      // If parent composition is a context
      if (comp != NULL &&
          instanceof (Context *, comp) && comp->isOccurring ())
        {
          ctx_parent = cast (Context *, comp);
          g_assert_nonnull (ctx_parent);

          // Trigger links in the parent context
          list<Action> ret = evalActionInContext (act, ctx_parent);
          stack.insert (stack.end (), ret.begin (), ret.end ());

          // If the event object is pointed by a port in the parent context,
          // trigger links in the its grantparent context ( and ancestors)
          comp = ctx_parent->getParent ();
          if (comp != NULL &&
              instanceof (Context *, comp) && comp->isOccurring ())
            {
              ctx_grandparent = cast (Context *, comp);
              for (auto port : *ctx_parent->getPorts ())
                {
                  if (port->getObject () == evt->getObject ())
                    {
                      list<Action> ret
                          = evalActionInContext (act, ctx_grandparent);
                      stack.insert (stack.end (), ret.begin (), ret.end ());
                    }
                }
            }
        }
      // If parent composition is a switch
      else if (comp != NULL && instanceof (Switch *, comp))
        {
          // Trigger the switchPort labelled action mapped to the switch's
          // media object.
          Switch *swtch = cast (Switch *, comp);
          for (const auto &swtchPort : *swtch->getSwitchPorts ())
            {
              for (const auto &mapped_evt : swtchPort.second)
                {
                  if (mapped_evt->getObject () == evt->getObject ()
                      && swtch->getParent () != NULL)
                    {
                      Event *label_evt = swtch->getEvent (
                          Event::PRESENTATION, swtchPort.first);
                      g_assert_nonnull (label_evt);

                      // Do the same action in the "equivalent" switchPort
                      Action label_act = act;
                      label_act.event = label_evt;
                      evalAction (label_act);
                    }
                }
            }
        }

      // If event object is a context, trigger the context itself
      if (instanceof (Context *, obj))
        {
          ctx_parent = cast (Context *, obj);
          g_assert_nonnull (ctx_parent);
          list<Action> ret = evalActionInContext (act, ctx_parent);
          stack.insert (stack.end (), ret.begin (), ret.end ());
        }
      // If have refer elements, trigger in the contexts
      else if (obj->getAliases ()->size ())
        {
          for (const auto &alias : *obj->getAliases ())
            {
              ctx_parent = cast (Context *, alias.second);
              if (ctx_parent)
                {
                  list<Action> ret = evalActionInContext (act, ctx_parent);
                  stack.insert (stack.end (), ret.begin (), ret.end ());
                }
            }
        }
    }
  return n;
}

bool
Document::evalPredicate (Predicate *pred)
{
  switch (pred->getType ())
    {
    case Predicate::FALSUM:
      TRACE ("false");
      return false;
      break;
    case Predicate::VERUM:
      TRACE ("true");
      return true;
      break;
    case Predicate::ATOM:
      {
        string left, right;
        Predicate::Test test;
        string msg_left, msg_test, msg_right;
        bool result;

        pred->getTest (&left, &test, &right);

        if (left[0] == '$')
          {
            msg_left = left;
            if (this->evalPropertyRef (left, &left))
              msg_left += " ('" + left + "')";
          }
        else
          {
            msg_left = "'" + left + "'";
          }

        if (right[0] == '$')
          {
            msg_right = right;
            if (this->evalPropertyRef (right, &right))
              msg_right += " ('" + right + "')";
          }
        else
          {
            msg_right = "'" + right + "'";
          }

        switch (test)
          {
          case Predicate::EQ:
            msg_test = "==";
            result = left == right;
            break;
          case Predicate::NE:
            msg_test = "!=";
            result = left != right;
            break;
          case Predicate::LT:
            msg_test = "<";
            result = left < right;
            break;
          case Predicate::LE:
            msg_test = "<=";
            result = left <= right;
            break;
          case Predicate::GT:
            msg_test = ">";
            result = left > right;
            break;
          case Predicate::GE:
            msg_test = ">=";
            result = left >= right;
            break;
          default:
            g_assert_not_reached ();
          }
        TRACE ("%s %s %s -> %s", msg_left.c_str (), msg_test.c_str (),
               msg_right.c_str (), strbool (result));
        return result;
      }
    case Predicate::NEGATION:
      g_assert_not_reached ();
      break;
    case Predicate::CONJUNCTION:
      {
        for (auto child : *pred->getChildren ())
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
    case Predicate::DISJUNCTION:
      {
        for (auto child : *pred->getChildren ())
          {
            if (this->evalPredicate (child))
              {
                TRACE ("or -> true");
                return true;
              }
          }
        TRACE ("or -> false");
        return false;
      }
      break;
    default:
      g_assert_not_reached ();
    }
  g_assert_not_reached ();
}

bool
Document::evalPropertyRef (const string &ref, string *result)
{
  size_t i;
  string id;
  string name;
  Object *object;

  if (ref[0] != '$' || (i = ref.find ('.')) == string::npos)
    return false;

  id = ref.substr (1, i - 1);
  name = ref.substr (i + 1);
  object = this->getObject (id);
  if (object == NULL)
    return false;

  tryset (result, object->getProperty (name));
  return true;
}

bool
Document::getData (const string &key, void **value)
{
  return _udata.getData (key, value);
}

bool
Document::setData (const string &key, void *value, UserDataCleanFunc fn)
{
  return _udata.setData (key, value, fn);
}

GINGA_NAMESPACE_END
