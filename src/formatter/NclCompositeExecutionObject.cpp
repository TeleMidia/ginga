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

  runningEvents.clear ();
  pausedEvents.clear ();
  pendingLinks.clear ();

  i = links.begin ();
  while (i != links.end ())
    {
      link = *i;
      if (link != NULL)
        {
          delete link;
          link = NULL;
        }
      ++i;
    }
  links.clear ();
  uncompiledLinks.clear ();

  j = execObjList.begin ();
  while (j != execObjList.end ())
    {
      object = j->second;
      if (object != this && hasInstance (object, false))
        {
          object->removeParentObject (_dataObject, this);
        }
      ++j;
    }
  execObjList.clear ();
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

  execObjList.clear ();
  links.clear ();
  uncompiledLinks.clear ();
  runningEvents.clear ();
  pausedEvents.clear ();
  pendingLinks.clear ();

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
              uncompiledLinks.insert (*i);
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
      i = execObjList.begin ();
      while (i != execObjList.end ())
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

  i = links.begin ();
  while (i != links.end ())
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
  if (execObjList.count (objId) != 0)
    {
      WARNING ("Trying to add the same obj twice: '%s'.", objId.c_str ());
      return false;
    }

  execObjList[objId] = obj;

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

  if (execObjList.empty ())
    {
      return NULL;
    }

  i = execObjList.find (id);
  if (i != execObjList.end ())
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

  if (execObjList.empty ())
    {
      return NULL;
    }
  objs = new map<string, NclExecutionObject *> (execObjList);

  return objs;
}

int
NclCompositeExecutionObject::getNumExecutionObjects ()
{
  return (int) execObjList.size ();
}

map<string, NclExecutionObject *> *
NclCompositeExecutionObject::recursivellyGetExecutionObjects ()
{
  map<string, NclExecutionObject *> *objects;
  NclExecutionObject *childObject;
  map<string, NclExecutionObject *> *grandChildrenObjects;
  map<string, NclExecutionObject *>::iterator i, j;

  objects = new map<string, NclExecutionObject *>;

  i = execObjList.begin ();
  while (i != execObjList.end ())
    {
      childObject = i->second;
      (*objects)[i->first] = childObject;
      if (childObject->instanceOf ("NclCompositeExecutionObject"))
        {
          grandChildrenObjects
              = ((NclCompositeExecutionObject *)childObject)
                    ->recursivellyGetExecutionObjects ();

          j = grandChildrenObjects->begin ();
          while (j != grandChildrenObjects->end ())
            {
              (*objects)[j->first] = j->second;
              ++j;
            }
          delete grandChildrenObjects;
          grandChildrenObjects = NULL;
        }
      ++i;
    }

  return objects;
}

bool
NclCompositeExecutionObject::removeExecutionObject (NclExecutionObject *obj)
{
  map<string, NclExecutionObject *>::iterator i;

  if (!_isDeleting)
    {
      i = execObjList.find (obj->getId ());
      if (i != execObjList.end ())
        {
          execObjList.erase (i);
          return true;
        }
    }
  return false;
}

set<Link *> *
NclCompositeExecutionObject::getUncompiledLinks ()
{
  clog << "NclCompositeExecutionObject::getUncompiledLinks '" << getId ();
  clog << "' has '" << uncompiledLinks.size () << "' uncompiled links";
  clog << endl;

  set<Link *> *uLinks = new set<Link *> (uncompiledLinks);
  return uLinks;
}

bool
NclCompositeExecutionObject::containsUncompiledLink (Link *dataLink)
{
  if (uncompiledLinks.count (dataLink) != 0)
    return true;
  return false;
}

void
NclCompositeExecutionObject::removeLinkUncompiled (Link *ncmLink)
{
  set<Link *>::iterator i;

  clog << "NclCompositeExecutionObject::removeLinkUncompiled '";
  clog << ncmLink->getId () << "'" << endl;
  i = uncompiledLinks.find (ncmLink);
  if (i != uncompiledLinks.end ())
    {
      uncompiledLinks.erase (i);
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

  links.insert (link);
}

void
NclCompositeExecutionObject::addNcmLink (Link *ncmLink)
{
  if (uncompiledLinks.count (ncmLink) != 0)
    {
      clog << "NclCompositeExecutionObject::addNcmLink Warning! ";
      clog << "Trying to add same link twice" << endl;
    }
  uncompiledLinks.insert (ncmLink);
}

void
NclCompositeExecutionObject::removeNcmLink (Link *ncmLink)
{
  bool contains;
  NclFormatterLink *link;
  Link *compiledNcmLink;
  set<Link *>::iterator i;
  set<NclFormatterLink *>::iterator j;

  contains = false;

  clog << "NclCompositeExecutionObject::removeNcmLink("
       << ncmLink->getId ();
  clog << "'" << endl;

  i = uncompiledLinks.find (ncmLink);
  if (i != uncompiledLinks.end ())
    {
      contains = true;
      uncompiledLinks.erase (i);
    }

  if (!contains)
    {
      for (j = links.begin (); j != links.end (); ++j)
        {
          link = *j;
          compiledNcmLink = link->getNcmLink ();
          if (ncmLink == compiledNcmLink)
            {
              links.erase (j);
              delete link;
              link = NULL;
              return;
            }
        }
    }
}

void
NclCompositeExecutionObject::setAllLinksAsUncompiled (bool isRecursive)
{
  NclExecutionObject *childObject;
  map<string, NclExecutionObject *>::iterator i;

  NclFormatterLink *link;
  Link *ncmLink;
  set<NclFormatterLink *>::iterator j;

  clog << "NclCompositeExecutionObject::setAllLinksAsUncompiled for '";
  clog << getId () << "'" << endl;

  j = links.begin ();
  while (j != links.end ())
    {
      link = *j;
      ncmLink = link->getNcmLink ();
      if (uncompiledLinks.count (ncmLink) != 0)
        {
          clog << "NclCompositeExecutionObject::setAllLinksAsUncompiled ";
          clog << "Warning! Trying to add same link twice" << endl;
        }
      uncompiledLinks.insert (ncmLink);
      delete link;
      link = NULL;
      ++j;
    }
  links.clear ();

  if (isRecursive)
    {
      i = execObjList.begin ();
      while (i != execObjList.end ())
        {
          childObject = i->second;
          if (childObject->instanceOf ("NclCompositeExecutionObject"))
            {
              assert (childObject != this);
              ((NclCompositeExecutionObject *)childObject)
                  ->setAllLinksAsUncompiled (isRecursive);
            }
          ++i;
        }
    }
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
      if (runningEvents.empty () && pausedEvents.empty ())
        {
          setParentsAsListeners ();
          _wholeContent->start ();
        }

      runningEvents.insert (event);
      break;

    case EventUtil::TR_ABORTS:
      lastTransition = transition;
      if (previousState == EventUtil::ST_OCCURRING)
        {
          i = runningEvents.find (event);
          if (i != runningEvents.end ())
            {
              runningEvents.erase (i);
            }
        }
      else if (previousState == EventUtil::ST_PAUSED)
        {
          i = pausedEvents.find (event);
          if (i != pausedEvents.end ())
            {
              pausedEvents.erase (i);
            }
        }

      if (runningEvents.empty () && pausedEvents.empty ()
          && pendingLinks.empty ())
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
              i = runningEvents.find (event);
              if (i != runningEvents.end ())
                {
                  runningEvents.erase (i);
                }
            }
          else if (previousState == EventUtil::ST_PAUSED)
            {
              i = pausedEvents.find (event);
              if (i != pausedEvents.end ())
                {
                  pausedEvents.erase (i);
                }
            }

          if (runningEvents.size () < 2)
            {
              listRunningObjects ();
            }

          if (runningEvents.size () < 2 && pausedEvents.empty ()
              && !pendingLinks.empty ())
            {
              listPendingLinks ();
            }

          if (runningEvents.empty () && pausedEvents.empty ()
              && pendingLinks.empty ())
            {
              checkLinkConditions ();
            }
        }
      break;

    case EventUtil::TR_PAUSES:
      i = runningEvents.find (event);
      if (i != runningEvents.end ())
        {
          runningEvents.erase (i);
        }

      pausedEvents.insert (event);
      if (runningEvents.empty ())
        {
          _wholeContent->pause ();
        }
      break;

    case EventUtil::TR_RESUMES:
      i = pausedEvents.find (event);
      if (i != pausedEvents.end ())
        {
          pausedEvents.erase (i);
        }

      runningEvents.insert (event);
      if (runningEvents.size () == 1)
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
  if (pendingLinks.count (evalLink) != 0)
    {
      linkNumber = pendingLinks[evalLink];
    }
  pendingLinks[evalLink] = linkNumber + 1;
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
  i = pendingLinks.find (finishedLink);
  if (i != pendingLinks.end ())
    {
      linkNumber = i->second;
      if (linkNumber == 1)
        {
          pendingLinks.erase (i);
          if (runningEvents.empty () && pausedEvents.empty ()
              && pendingLinks.empty ())
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
          pendingLinks[finishedLink] = linkNumber - 1;
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
  if (_isDeleting || (runningEvents.empty () && pausedEvents.empty ()
                   && pendingLinks.empty ()))
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
  i = execObjList.begin ();
  while (i != execObjList.end ())
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
  clog << " runingEvents->size = '" << runningEvents.size () << "'";
  clog << endl;
}

void
NclCompositeExecutionObject::listPendingLinks ()
{
  map<NclFormatterLink *, int>::iterator i;
  Link *ncmLink;

  clog << "NclCompositeExecutionObject::listPendingLinks for '";
  clog << _id << "': ";

  i = pendingLinks.begin ();
  while (i != pendingLinks.end ())
    {
      ncmLink = i->first->getNcmLink ();

      if (Entity::hasInstance (ncmLink, false))
        {
          clog << "'" << ncmLink->getId () << "', ";
        }
      ++i;
    }

  clog << " pendingLinks.size = '" << pendingLinks.size () << "'";
  clog << endl;
}

GINGA_FORMATTER_END
