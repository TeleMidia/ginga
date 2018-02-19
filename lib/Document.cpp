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

#include "Context.h"
#include "Media.h"
#include "MediaSettings.h"
#include "Object.h"
#include "Switch.h"

GINGA_NAMESPACE_BEGIN

/**
 * @brief Creates a new document.
 *
 * This function creates a new NCL document containing a root context (the
 * document body) with id "__root__" and a child settings media object with
 * id "__settings__".
 *
 * @return New #Document.
 */
Document::Document ()
{
  MediaSettings *obj;

  _root = new Context ("__root__");
  _settings = nullptr;
  g_assert (this->addObject (_root));

  obj = new MediaSettings ("__settings__");
  _root->addChild (obj);
  _settings = obj;
}

/**
 * @brief Destroys document.
 *
 * This function destroys the document and all its child objects.
 */
Document::~Document ()
{
  delete _root;
}

/**
 * @brief Gets document objects.
 * @return The set of objects in document.
 */
const set<Object *> *
Document::getObjects ()
{
  return &_objects;
}

/**
 * @brief Gets document object by id.
 * @param id Object id.
 * @return The object if successful, or null otherwise.
 */
Object *
Document::getObjectById (const string &id)
{
  auto it = _objectsById.find (id);
  if (it == _objectsById.end ())
    return nullptr;
  return it->second;
}

/**
 * @brief Gets document object by id or alias.
 * @param id Object id or alias.
 * @return The object if successful, or null otherwise.
 */
Object *
Document::getObjectByIdOrAlias (const string &id)
{
  Object *obj;
  if ((obj = this->getObjectById (id)) != nullptr)
    return obj;
  for (auto obj : _objects)
    if (obj->hasAlias (id))
      return obj;
  return nullptr;
}

/**
 * @brief Adds object to document.
 *
 * This function assumes that \p obj is not in another document.
 *
 * @param obj The object to add.
 * @return \c true if successful, or \c false otherwise (object already in
 * document).
 */
bool
Document::addObject (Object *obj)
{
  g_assert_nonnull (obj);

  if (_objects.find (obj) != _objects.end ()
      || getObjectByIdOrAlias (obj->getId ()) != nullptr)
    {
      return false; // already in document
    }

  // Settings can only be added once.
  if (unlikely (_settings != nullptr && instanceof (MediaSettings *, obj)))
    g_assert_false (instanceof (MediaSettings *, obj));

  obj->initDocument (this);
  _objects.insert (obj);
  _objectsById[obj->getId ()] = obj;

  if (instanceof (Media *, obj))
    {
      Media *media = cast (Media *, obj);
      g_assert_nonnull (media);
      _medias.insert (media);
    }
  else if (instanceof (Context *, obj))
    {
      Context *ctx = cast (Context *, obj);
      g_assert_nonnull (ctx);
      _contexts.insert (ctx);
    }
  else if (instanceof (Switch *, obj))
    {
      Switch *swtch = cast (Switch *, obj);
      g_assert_nonnull (swtch);
      _switches.insert (swtch);
    }
  else
    {
      g_assert_not_reached ();
    }
  return true;
}

/**
 * @brief Gets document's root object.
 * @return The root object.
 */
Context *
Document::getRoot ()
{
  g_assert_nonnull (_root);
  return _root;
}

/**
 * @brief Gets document's settings object.
 * @return The settings object.
 */
MediaSettings *
Document::getSettings ()
{
  g_assert_nonnull (_settings);
  return _settings;
}

const set<Media *> *
Document::getMedias ()
{
  return &_medias;
}

const set<Context *> *
Document::getContexts ()
{
  return &_contexts;
}

const set<Switch *> *
Document::getSwitches ()
{
  return &_switches;
}

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
  act.predicate = nullptr;
  act.value = value;
  return this->evalAction (act);
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
      Context *ctx;
      bool done;

      act = stack.back ();
      stack.pop_back ();

      evt = act.event;
      g_assert_nonnull (evt);

      evt->setParameter ("duration", act.duration);
      if (evt->getType () == Event::ATTRIBUTION)
        evt->setParameter ("value", act.value);

      if (!evt->transition (act.transition))
        continue;

      n++;
      done = false;
      obj = evt->getObject ();
      g_assert_nonnull (obj);

      // Trigger links in parent context.
      comp = obj->getParent ();
      if (comp != nullptr &&
          instanceof (Context *, comp) && comp->isOccurring ())
        {
          ctx = cast (Context *, comp);
          g_assert_nonnull (ctx);

        trigger:
          if (ctx->getLinksStatus ())
            {
              for (auto link : *ctx->getLinks ())
                {
                  for (auto cond : link.first)
                    {
                      Predicate *pred;

                      if (cond.event != evt
                          || cond.transition != act.transition)
                        continue;

                      pred = cond.predicate;
                      if (pred != nullptr && !this->evalPredicate (pred))
                        continue;

                      // Success.
                      auto acts = link.second;
                      for (auto ri = acts.rbegin (); ri != acts.rend ();
                           ++ri)
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

                              comp->addDelayedAction (
                                  next_evt, next_act.transition,
                                  next_act.value, delay);
                            }
                        }
                    }
                }
            }
        }

      // Trigger links in context itself.
      if (!done && instanceof (Context *, obj))
        {
          ctx = cast (Context *, obj);
          g_assert_nonnull (ctx);
          done = true;
          goto trigger;
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
      break;
    case Predicate::VERUM:
      TRACE ("true");
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
      g_assert_not_reached ();
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
  object = this->getObjectByIdOrAlias (id);
  if (object == nullptr)
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
