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
  deleting = true;

  destroyEvents ();

  pthread_mutex_lock (&stlMutex);
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

  pthread_mutex_unlock (&stlMutex);
  Thread::mutexDestroy (&stlMutex);

  pthread_mutex_lock (&compositeMutex);
  j = execObjList.begin ();
  while (j != execObjList.end ())
    {
      object = j->second;
      if (object != this && hasInstance (object, false))
        {
          object->removeParentObject (dataObject, (void *)this);
        }
      ++j;
    }
  execObjList.clear ();

  pthread_mutex_unlock (&compositeMutex);
  Thread::mutexDestroy (&compositeMutex);
}

void
NclCompositeExecutionObject::initializeCompositeExecutionObject (
    arg_unused (const string &id), Node *dataObject, arg_unused (NclCascadingDescriptor *descriptor))
{
  ContextNode *compositeNode;
  set<Link *> *compositionLinks;
  set<Link *>::iterator i;
  Entity *entity;

  typeSet.insert ("NclCompositeExecutionObject");
  Thread::mutexInit (&compositeMutex, true);
  Thread::mutexInit (&stlMutex, false);

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

  pthread_mutex_lock (&compositeMutex);
  parentDataObject = (Node *)(dataObject->getParentComposition ());

  if (parentDataObject != NULL)
    {
      i = execObjList.begin ();
      while (i != execObjList.end ())
        {
          object = i->second;
          if (object->getDataObject () == parentDataObject)
            {
              pthread_mutex_unlock (&compositeMutex);
              return (NclCompositeExecutionObject *)object;
            }
          ++i;
        }
    }
  pthread_mutex_unlock (&compositeMutex);

  return NULL;
}

void
NclCompositeExecutionObject::suspendLinkEvaluation (bool suspend)
{
  set<NclFormatterLink *>::iterator i;

  lockSTL ();
  i = links.begin ();
  while (i != links.end ())
    {
      (*i)->suspendLinkEvaluation (suspend);
      ++i;
    }
  unlockSTL ();
}

bool
NclCompositeExecutionObject::addExecutionObject (NclExecutionObject *obj)
{
  string objId;

  if (obj == NULL)
    {
      return false;
    }

  lockComposite ();
  objId = obj->getId ();
  if (execObjList.count (objId) != 0)
    {
      clog << "NclCompositeExecutionObject::addExecutionObject Warning! ";
      clog << "trying to add same obj twice: '" << objId << "'" << endl;
      unlockComposite ();
      return false;
    }

  execObjList[objId] = obj;
  unlockComposite ();

  obj->addParentObject ((void *)this, getDataObject ());
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

  lockComposite ();
  if (execObjList.empty ())
    {
      unlockComposite ();
      return NULL;
    }

  i = execObjList.find (id);
  if (i != execObjList.end ())
    {
      execObj = i->second;
      unlockComposite ();
      return execObj;
    }

  unlockComposite ();
  return NULL;
}

map<string, NclExecutionObject *> *
NclCompositeExecutionObject::getExecutionObjects ()
{
  map<string, NclExecutionObject *> *objs;

  lockComposite ();
  if (execObjList.empty ())
    {
      unlockComposite ();
      return NULL;
    }
  objs = new map<string, NclExecutionObject *> (execObjList);
  unlockComposite ();

  return objs;
}

int
NclCompositeExecutionObject::getNumExecutionObjects ()
{
  int size = 0;

  lockComposite ();
  size = (int) execObjList.size ();
  unlockComposite ();

  return size;
}

map<string, NclExecutionObject *> *
NclCompositeExecutionObject::recursivellyGetExecutionObjects ()
{
  map<string, NclExecutionObject *> *objects;
  NclExecutionObject *childObject;
  map<string, NclExecutionObject *> *grandChildrenObjects;
  map<string, NclExecutionObject *>::iterator i, j;

  objects = new map<string, NclExecutionObject *>;

  lockComposite ();
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

  unlockComposite ();
  return objects;
}

bool
NclCompositeExecutionObject::removeExecutionObject (NclExecutionObject *obj)
{
  map<string, NclExecutionObject *>::iterator i;

  lockComposite ();
  if (!deleting)
    {
      i = execObjList.find (obj->getId ());
      if (i != execObjList.end ())
        {
          execObjList.erase (i);
          unlockComposite ();
          return true;
        }
    }
  unlockComposite ();
  return false;
}

set<Link *> *
NclCompositeExecutionObject::getUncompiledLinks ()
{
  clog << "NclCompositeExecutionObject::getUncompiledLinks '" << getId ();
  clog << "' has '" << uncompiledLinks.size () << "' uncompiled links";
  clog << endl;

  lockSTL ();
  set<Link *> *uLinks = new set<Link *> (uncompiledLinks);
  unlockSTL ();

  return uLinks;
}

bool
NclCompositeExecutionObject::containsUncompiledLink (Link *dataLink)
{
  lockSTL ();
  if (uncompiledLinks.count (dataLink) != 0)
    {
      unlockSTL ();
      return true;
    }

  unlockSTL ();
  return false;
}

void
NclCompositeExecutionObject::removeLinkUncompiled (Link *ncmLink)
{
  set<Link *>::iterator i;

  lockSTL ();
  clog << "NclCompositeExecutionObject::removeLinkUncompiled '";
  clog << ncmLink->getId () << "'" << endl;
  i = uncompiledLinks.find (ncmLink);
  if (i != uncompiledLinks.end ())
    {
      uncompiledLinks.erase (i);
      unlockSTL ();
      return;
    }
  unlockSTL ();
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

  lockSTL ();
  links.insert (link);
  unlockSTL ();
}

void
NclCompositeExecutionObject::addNcmLink (Link *ncmLink)
{
  lockSTL ();
  if (uncompiledLinks.count (ncmLink) != 0)
    {
      clog << "NclCompositeExecutionObject::addNcmLink Warning! ";
      clog << "Trying to add same link twice" << endl;
    }
  uncompiledLinks.insert (ncmLink);
  unlockSTL ();
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

  lockSTL ();
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
              unlockSTL ();
              return;
            }
        }
    }
  unlockSTL ();
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

  lockSTL ();
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
  unlockSTL ();

  if (isRecursive)
    {
      lockComposite ();
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
      unlockComposite ();
    }
}

void
NclCompositeExecutionObject::setParentsAsListeners ()
{
  map<Node *, void *>::iterator i;

  lockSTL ();
  i = parentTable.begin ();
  while (i != parentTable.end ())
    {
      if (NclFormatterEvent::hasInstance (wholeContent, false))
        {
          wholeContent->addEventListener (
              (NclCompositeExecutionObject *)i->second);
        }
      ++i;
    }
  unlockSTL ();
}

void
NclCompositeExecutionObject::unsetParentsAsListeners ()
{
  map<Node *, void *>::iterator i;

  if (deleting)
    {
      return;
    }

  lockSTL ();
  i = parentTable.begin ();
  while (i != parentTable.end ())
    {
      if (deleting || !NclFormatterEvent::hasInstance (wholeContent, false))
        {
          return;
        }

      // unregister parent as a composite presentation listener
      wholeContent->removeEventListener (
          (NclCompositeExecutionObject *)i->second);

      ++i;
    }
  unlockSTL ();
}

void
NclCompositeExecutionObject::eventStateChanged (void *changedEvent,
                                                short transition,
                                                short previousState)
{
  NclFormatterEvent *event;
  set<NclFormatterEvent *>::iterator i;

  event = (NclFormatterEvent *)changedEvent;
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
          wholeContent->start ();
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
          wholeContent->abort ();
          unsetParentsAsListeners ();
        }
      break;

    case EventUtil::TR_STOPS:
      if (((NclPresentationEvent *)event)->getRepetitions () == 0)
        {
          lockComposite ();
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

          unlockComposite ();
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
          wholeContent->pause ();
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
          wholeContent->resume ();
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

  lockSTL ();

  evalLink = link;
  if (pendingLinks.count (evalLink) != 0)
    {
      linkNumber = pendingLinks[evalLink];
    }
  pendingLinks[evalLink] = linkNumber + 1;

  unlockSTL ();
}

void
NclCompositeExecutionObject::linkEvaluationFinished (
    NclFormatterCausalLink *link, bool start)
{
  int linkNumber;
  NclFormatterLink *finishedLink;
  map<NclFormatterLink *, int>::iterator i;

  lockSTL ();

  clog << "NclCompositeExecutionObject::linkEvaluationFinished(" << id;
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
                  unlockSTL ();
                  wholeContent->abort ();
                  if (deleting)
                    {
                      return;
                    }
                  unsetParentsAsListeners ();
                  lockSTL ();
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
  unlockSTL ();
}

bool
NclCompositeExecutionObject::setPropertyValue (NclAttributionEvent *event,
                                               const string &value)
{
  if (event->getAnchor ()->getPropertyName () == "visible")
    {
      if (value == "true")
        {
          visible = true;
        }
      else if (value == "false")
        {
          visible = false;
        }
      return false;
    }

  return NclExecutionObject::setPropertyValue (event, value);
}

void
NclCompositeExecutionObject::checkLinkConditions ()
{
  if (!running)
    {
      //Thread::startThread ();
    }
}

void
NclCompositeExecutionObject::run ()
{
  running = true;
  if (deleting || (runningEvents.empty () && pausedEvents.empty ()
                   && pendingLinks.empty ()))
    {
      clog << "NclCompositeExecutionObject::run ";
      clog << "I (" << id << ") am ending because of STOP of";
      clog << " the last running event (no pending links nor ";
      clog << "paused events)";
      clog << endl;

      if (wholeContent != NULL &&
          NclFormatterEvent::hasInstance (wholeContent, false))
        {
          wholeContent->stop ();
          unsetParentsAsListeners ();
        }
    }
  running = false;
}

void
NclCompositeExecutionObject::listRunningObjects ()
{
  map<string, NclExecutionObject *>::iterator i;
  vector<NclFormatterEvent *>::iterator j;
  vector<NclFormatterEvent *> *events;
  NclExecutionObject *object;
  NclFormatterEvent *event;

  clog << "NclCompositeExecutionObject::listRunningObjects for '";
  clog << id << "': ";
  i = execObjList.begin ();
  while (i != execObjList.end ())
    {
      object = i->second;
      events = object->getEvents ();
      if (events != NULL)
        {
          j = events->begin ();
          while (j != events->end ())
            {
              event = *j;
              if (event->getCurrentState () != EventUtil::ST_SLEEPING)
                {
                  clog << "'" << i->first << "', ";
                }
              ++j;
            }
          delete events;
          events = NULL;
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
  clog << id << "': ";

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

void
NclCompositeExecutionObject::lockComposite ()
{
  if (!deleting)
    {
      Thread::mutexLock (&compositeMutex);
    }
}

void
NclCompositeExecutionObject::unlockComposite ()
{
  if (!deleting)
    {
      Thread::mutexUnlock (&compositeMutex);
    }
}

void
NclCompositeExecutionObject::lockSTL ()
{
  if (!deleting)
    {
      Thread::mutexLock (&stlMutex);
    }
}

void
NclCompositeExecutionObject::unlockSTL ()
{
  if (!deleting)
    {
      Thread::mutexUnlock (&stlMutex);
    }
}

GINGA_FORMATTER_END
