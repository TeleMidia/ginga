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

#include "ginga.h"
#include "NclCompositeExecutionObject.h"

GINGA_FORMATTER_BEGIN

NclCompositeExecutionObject::NclCompositeExecutionObject (
    const string &id, Node *dataObject, bool handling,
    INclLinkActionListener *seListener)
    : NclExecutionObject (id, dataObject, handling, seListener)
{
  initializeCompositeExecutionObject (id, dataObject, NULL);
}

NclCompositeExecutionObject::NclCompositeExecutionObject (
    const string &id, Node *dataObject, NclCascadingDescriptor *descriptor,
    bool handling, INclLinkActionListener *seListener)
    : NclExecutionObject (id, dataObject, descriptor, handling, seListener)
{
  initializeCompositeExecutionObject (id, dataObject, descriptor);
}

NclCompositeExecutionObject::~NclCompositeExecutionObject ()
{
  NclExecutionObject *object;
  NclFormatterLink *link;
  set<NclFormatterLink *>::iterator i;
  map<string, NclExecutionObject *>::iterator j;

  removeInstance (this);
  _isDeleting = true;

  destroyEvents ();

  _runningEvents.clear ();
  _pausedEvents.clear ();
  _pendingLinks.clear ();

  i = _links.begin ();
  while (i != _links.end ())
    {
      link = *i;
      if (link != NULL)
        {
          delete link;
          link = NULL;
        }
      ++i;
    }
  _links.clear ();
  _uncompiledLinks.clear ();

  j = _execObjList.begin ();
  while (j != _execObjList.end ())
    {
      object = j->second;
      if (object != this && hasInstance (object, false))
        {
          object->removeParentObject (_dataObject, this);
        }
      ++j;
    }
  _execObjList.clear ();
}

void
NclCompositeExecutionObject::initializeCompositeExecutionObject (
    arg_unused (const string &_id),
    Node *dataObject,
    arg_unused (NclCascadingDescriptor *_descriptor))
{
  ContextNode *compositeNode;
  set<Link *> *compositionLinks;
  set<Link *>::iterator i;
  Entity *entity;

  _typeSet.insert ("NclCompositeExecutionObject");

  _execObjList.clear ();
  _links.clear ();
  _uncompiledLinks.clear ();
  _runningEvents.clear ();
  _pausedEvents.clear ();
  _pendingLinks.clear ();

  entity = dataObject->getDataEntity ();

  if (entity != NULL && entity->instanceOf ("ContextNode"))
    {
      compositeNode = (ContextNode *)entity;
      compositionLinks = compositeNode->getLinks ();
      if (compositionLinks != NULL)
        {
          i = compositionLinks->begin ();
          while (i != compositionLinks->end ())
            {
              _uncompiledLinks.insert (*i);
              ++i;
            }
        }
    }
}

NclCompositeExecutionObject *
NclCompositeExecutionObject::getParentFromDataObject (Node *dataObject)
{
  NclExecutionObject *object;
  Node *parentDataObject;
  map<string, NclExecutionObject *>::iterator i;

  parentDataObject = (Node *)(dataObject->getParentComposition ());

  if (parentDataObject != NULL)
    {
      i = _execObjList.begin ();
      while (i != _execObjList.end ())
        {
          object = i->second;
          if (object->getDataObject () == parentDataObject)
            {
              return (NclCompositeExecutionObject *)object;
            }
          ++i;
        }
    }
  return NULL;
}

void
NclCompositeExecutionObject::suspendLinkEvaluation (bool suspend)
{
  set<NclFormatterLink *>::iterator i;

  i = _links.begin ();
  while (i != _links.end ())
    {
      (*i)->suspendLinkEvaluation (suspend);
      ++i;
    }
}

bool
NclCompositeExecutionObject::addExecutionObject (NclExecutionObject *obj)
{
  string objId;

  if (obj == NULL)
    {
      return false;
    }

  objId = obj->getId ();
  if (_execObjList.count (objId) != 0)
    {
      WARNING ("Trying to add the same obj twice: '%s'.", objId.c_str ());
      return false;
    }

  _execObjList[objId] = obj;

  obj->addParentObject (this, getDataObject ());
  return true;
}

bool
NclCompositeExecutionObject::containsExecutionObject (const string &execObjId)
{
  if (getExecutionObject (execObjId) != NULL)
    {
      return true;
    }
  else
    {
      return false;
    }
}

NclExecutionObject *
NclCompositeExecutionObject::getExecutionObject (const string &id)
{
  map<string, NclExecutionObject *>::iterator i;
  NclExecutionObject *execObj;

  if (_execObjList.empty ())
    {
      return NULL;
    }

  i = _execObjList.find (id);
  if (i != _execObjList.end ())
    {
      execObj = i->second;
      return execObj;
    }

  return NULL;
}

map<string, NclExecutionObject *> *
NclCompositeExecutionObject::getExecutionObjects ()
{
  map<string, NclExecutionObject *> *objs;

  if (_execObjList.empty ())
    {
      return NULL;
    }
  objs = new map<string, NclExecutionObject *> (_execObjList);

  return objs;
}

bool
NclCompositeExecutionObject::removeExecutionObject (NclExecutionObject *obj)
{
  map<string, NclExecutionObject *>::iterator i;

  if (!_isDeleting)
    {
      i = _execObjList.find (obj->getId ());
      if (i != _execObjList.end ())
        {
          _execObjList.erase (i);
          return true;
        }
    }
  return false;
}

set<Link *> *
NclCompositeExecutionObject::getUncompiledLinks ()
{
  clog << "NclCompositeExecutionObject::getUncompiledLinks '" << getId ();
  clog << "' has '" << _uncompiledLinks.size () << "' uncompiled links";
  clog << endl;

  set<Link *> *uLinks = new set<Link *> (_uncompiledLinks);
  return uLinks;
}

bool
NclCompositeExecutionObject::containsUncompiledLink (Link *dataLink)
{
  if (_uncompiledLinks.count (dataLink) != 0)
    return true;
  return false;
}

void
NclCompositeExecutionObject::removeLinkUncompiled (Link *ncmLink)
{
  set<Link *>::iterator i;

  clog << "NclCompositeExecutionObject::removeLinkUncompiled '";
  clog << ncmLink->getId () << "'" << endl;
  i = _uncompiledLinks.find (ncmLink);
  if (i != _uncompiledLinks.end ())
    {
      _uncompiledLinks.erase (i);
      return;
    }
}

void
NclCompositeExecutionObject::setLinkCompiled (NclFormatterLink *link)
{
  if (link == NULL)
    {
      clog << "NclCompositeExecutionObject::setLinkCompiled Warning! ";
      clog << "trying to compile a NULL link" << endl;
      return;
    }

  Link *compiledLink;
  compiledLink = link->getNcmLink ();

  if (compiledLink == NULL)
    {
      clog << "NclCompositeExecutionObject::setLinkCompiled Warning! ";
      clog << "formatterLink has returned a NULL ncmLink" << endl;
      return;
    }

  _links.insert (link);
}

void
NclCompositeExecutionObject::setParentsAsListeners ()
{
  map<Node *, NclCompositeExecutionObject *>::iterator i;

  i = _parentTable.begin ();
  while (i != _parentTable.end ())
    {
      if (NclFormatterEvent::hasInstance (_wholeContent, false))
        {
          _wholeContent->addEventListener (i->second);
        }
      ++i;
    }
}

void
NclCompositeExecutionObject::unsetParentsAsListeners ()
{
  map<Node *,NclCompositeExecutionObject *>::iterator i;

  if (_isDeleting)
    {
      return;
    }

  i = _parentTable.begin ();
  while (i != _parentTable.end ())
    {
      if (_isDeleting || !NclFormatterEvent::hasInstance (_wholeContent, false))
        {
          return;
        }

      // unregister parent as a composite presentation listener
      _wholeContent->removeEventListener (
          (NclCompositeExecutionObject *)i->second);

      ++i;
    }
}

void
NclCompositeExecutionObject::eventStateChanged (NclFormatterEvent *event,
                                                short transition,
                                                short previousState)
{
  set<NclFormatterEvent *>::iterator i;

  if (!(event->instanceOf ("NclPresentationEvent"))
      || !NclFormatterEvent::hasInstance (event, false))
    {
      return;
    }

  switch (transition)
    {
    case EventUtil::TR_STARTS:
      if (_runningEvents.empty () && _pausedEvents.empty ())
        {
          setParentsAsListeners ();
          _wholeContent->start ();
        }

      _runningEvents.insert (event);
      break;

    case EventUtil::TR_ABORTS:
      lastTransition = transition;
      if (previousState == EventUtil::ST_OCCURRING)
        {
          i = _runningEvents.find (event);
          if (i != _runningEvents.end ())
            {
              _runningEvents.erase (i);
            }
        }
      else if (previousState == EventUtil::ST_PAUSED)
        {
          i = _pausedEvents.find (event);
          if (i != _pausedEvents.end ())
            {
              _pausedEvents.erase (i);
            }
        }

      if (_runningEvents.empty () && _pausedEvents.empty ()
          && _pendingLinks.empty ())
        {
          _wholeContent->abort ();
          unsetParentsAsListeners ();
        }
      break;

    case EventUtil::TR_STOPS:
      if (((NclPresentationEvent *)event)->getRepetitions () == 0)
        {
          lastTransition = transition;
          if (previousState == EventUtil::ST_OCCURRING)
            {
              i = _runningEvents.find (event);
              if (i != _runningEvents.end ())
                {
                  _runningEvents.erase (i);
                }
            }
          else if (previousState == EventUtil::ST_PAUSED)
            {
              i = _pausedEvents.find (event);
              if (i != _pausedEvents.end ())
                {
                  _pausedEvents.erase (i);
                }
            }

          if (_runningEvents.size () < 2)
            {
              listRunningObjects ();
            }

          if (_runningEvents.size () < 2 && _pausedEvents.empty ()
              && !_pendingLinks.empty ())
            {
              listPendingLinks ();
            }

          if (_runningEvents.empty () && _pausedEvents.empty ()
              && _pendingLinks.empty ())
            {
              checkLinkConditions ();
            }
        }
      break;

    case EventUtil::TR_PAUSES:
      i = _runningEvents.find (event);
      if (i != _runningEvents.end ())
        {
          _runningEvents.erase (i);
        }

      _pausedEvents.insert (event);
      if (_runningEvents.empty ())
        {
          _wholeContent->pause ();
        }
      break;

    case EventUtil::TR_RESUMES:
      i = _pausedEvents.find (event);
      if (i != _pausedEvents.end ())
        {
          _pausedEvents.erase (i);
        }

      _runningEvents.insert (event);
      if (_runningEvents.size () == 1)
        {
          _wholeContent->resume ();
        }
      break;

    default:
      break;
    }
}

void
NclCompositeExecutionObject::linkEvaluationStarted (
    NclFormatterCausalLink *link)
{
  int linkNumber = 0;
  NclFormatterLink *evalLink;

  evalLink = link;
  if (_pendingLinks.count (evalLink) != 0)
    {
      linkNumber = _pendingLinks[evalLink];
    }
  _pendingLinks[evalLink] = linkNumber + 1;
}

void
NclCompositeExecutionObject::linkEvaluationFinished (
    NclFormatterCausalLink *link, bool start)
{
  int linkNumber;
  NclFormatterLink *finishedLink;
  map<NclFormatterLink *, int>::iterator i;

  clog << "NclCompositeExecutionObject::linkEvaluationFinished(" << _id;
  clog << ") '";
  clog << link->getNcmLink ()->getId () << "'" << endl;

  finishedLink = link;
  i = _pendingLinks.find (finishedLink);
  if (i != _pendingLinks.end ())
    {
      linkNumber = i->second;
      if (linkNumber == 1)
        {
          _pendingLinks.erase (i);
          if (_runningEvents.empty () && _pausedEvents.empty ()
              && _pendingLinks.empty ())
            {
              if (lastTransition == EventUtil::TR_STOPS)
                {
                  checkLinkConditions ();
                }
              else if (!start)
                {
                  _wholeContent->abort ();
                  if (_isDeleting)
                    {
                      return;
                    }
                  unsetParentsAsListeners ();
                }
              else
                {
                  // if nothing starts the composition may
                  // stay locked as occurring
                  clog << "NclCompositeExecutionObject::";
                  clog << "linkEvaluationFinished ";
                  clog << "if nothing starts the composition may ";
                  clog << "stay locked as occurring";
                  clog << endl;
                }
            }
        }
      else
        {
          _pendingLinks[finishedLink] = linkNumber - 1;
        }
    }
}

bool
NclCompositeExecutionObject::setProperty (NclAttributionEvent *event,
                                          const string &value)
{
  if (event->getAnchor ()->getName () == "visible")
    {
      if (value == "true")
        {
          _visible = true;
        }
      else if (value == "false")
        {
          _visible = false;
        }
      return false;
    }

  return NclExecutionObject::setProperty (event, value);
}

void
NclCompositeExecutionObject::checkLinkConditions ()
{
  if (_isDeleting || (_runningEvents.empty () && _pausedEvents.empty ()
                   && _pendingLinks.empty ()))
    {
      clog << "NclCompositeExecutionObject::run ";
      clog << "I (" << _id << ") am ending because of STOP of";
      clog << " the last running event (no pending links nor ";
      clog << "paused events)";
      clog << endl;

      if (_wholeContent != NULL &&
          NclFormatterEvent::hasInstance (_wholeContent, false))
        {
          _wholeContent->stop ();
          unsetParentsAsListeners ();
        }
    }
}

void
NclCompositeExecutionObject::listRunningObjects ()
{
  map<string, NclExecutionObject *>::iterator i;
  vector<NclFormatterEvent *>::iterator j;
  NclExecutionObject *object;
  NclFormatterEvent *event;

  clog << "NclCompositeExecutionObject::listRunningObjects for '";
  clog << _id << "': ";
  i = _execObjList.begin ();
  while (i != _execObjList.end ())
    {
      object = i->second;
      vector<NclFormatterEvent *> events = object->getEvents ();
      j = events.begin ();
      while (j != events.end ())
        {
          event = *j;
          if (event->getCurrentState () != EventUtil::ST_SLEEPING)
            {
              clog << "'" << i->first << "', ";
            }
          ++j;
        }
      ++i;
    }
  clog << " runingEvents->size = '" << _runningEvents.size () << "'";
  clog << endl;
}

void
NclCompositeExecutionObject::listPendingLinks ()
{
  map<NclFormatterLink *, int>::iterator i;
  Link *ncmLink;

  clog << "NclCompositeExecutionObject::listPendingLinks for '";
  clog << _id << "': ";

  i = _pendingLinks.begin ();
  while (i != _pendingLinks.end ())
    {
      ncmLink = i->first->getNcmLink ();

      if (Entity::hasInstance (ncmLink, false))
        {
          clog << "'" << ncmLink->getId () << "', ";
        }
      ++i;
    }

  clog << " pendingLinks.size = '" << _pendingLinks.size () << "'";
  clog << endl;
}

GINGA_FORMATTER_END
