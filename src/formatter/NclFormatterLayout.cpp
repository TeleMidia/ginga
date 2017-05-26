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
#include "NclFormatterLayout.h"

GINGA_FORMATTER_BEGIN

NclFormatterLayout::NclFormatterLayout (int w, int h)
{
  region = NULL;

  Thread::mutexInit (&mutex, false);
  createRegion (w, h);
}

NclFormatterLayout::~NclFormatterLayout ()
{
  clear ();
  lock ();
  regionMap.clear ();
  regionZIndex.clear ();
  objectMap.clear ();
  unlock ();
}

LayoutRegion *
NclFormatterLayout::getRegion ()
{
  return this->region;
}

void
NclFormatterLayout::createRegion (int w, int h)
{
  region = new LayoutRegion ("defaultScreenFormatter");
  region->setTop (0, false);
  region->setLeft (0, false);
  region->setWidth (w, false);
  region->setHeight (h, false);
}

SDLWindow*
NclFormatterLayout::prepareFormatterRegion (NclExecutionObject *object,
                                            string plan)
{
  NclCascadingDescriptor *descriptor;
  NclFormatterRegion *fregion;

  LayoutRegion *layoutRegion, *parent, *grandParent;
  string regionId, mapId;

  vector<LayoutRegion *> *childs;
  vector<LayoutRegion *>::iterator i;

  int zIndex;
  double cvtZIndex;
  SDLWindow* windowId = 0;

  if (object == NULL || object->getDescriptor () == NULL
      || object->getDescriptor ()->getFormatterRegion () == NULL)
    {
      clog << "NclFormatterLayout::prepareFormatterRegion returns -1";
      clog << endl;
      return windowId;
    }

  clog << "NclFormatterLayout::prepareFormatterRegion for '";
  clog << object->getId () << "'";
  clog << endl;

  descriptor = object->getDescriptor ();
  fregion = descriptor->getFormatterRegion ();
  layoutRegion = fregion->getOriginalRegion ();

  // every presented object has a region root
  // the formatter region
  parent = layoutRegion;
  grandParent = layoutRegion->getParent ();
  if (grandParent != NULL)
    {
      while (grandParent->getParent () != NULL)
        {
          parent = grandParent;
          grandParent = grandParent->getParent ();
        }
    }

  if (grandParent != region && grandParent != NULL)
    {
      childs = grandParent->getRegions ();
      i = childs->begin ();
      while (i != childs->end ())
        {
          region->addRegion (*i);
          (*i)->setParent (region);
          ++i;
        }
      delete childs;

      region->addRegion (parent);
      parent->setParent (region);
    }

  regionId = layoutRegion->getId ();
  zIndex = layoutRegion->getZIndex ();

  windowId = addRegionOnMaps (object, fregion, regionId,
                              zIndex, plan, &cvtZIndex);

  return windowId;
}

double
NclFormatterLayout::refreshZIndex (NclFormatterRegion *region,
                                   const string &layoutRegionId, int zIndex,
                                   const string &plan)
{
  NclExecutionObject *object;
  double cvtZIndex;

  object = removeFormatterRegionFromMaps (layoutRegionId, region);

  addRegionOnMaps (object, region, layoutRegionId, zIndex,
                   plan, &cvtZIndex);

  return cvtZIndex;
}

double
NclFormatterLayout::convertZIndex (int zIndex, const string &plan)
{
  double planPower = 2.0;

  if (plan == "background")
    {
      planPower = 0.0;
    }
  else if (plan == "video")
    {
      planPower = 1.0;
    }

  if (zIndex > 256)
    {
      zIndex = 256;
    }

  return (planPower + ((double)zIndex / 1000));
}

void
NclFormatterLayout::showObject (NclExecutionObject *object)
{
  NclFormatterRegion *region;
  map<int, set<string> *>::iterator i;

  if (object == NULL || object->getDescriptor () == NULL
      || object->getDescriptor ()->getFormatterRegion () == NULL)
    {
      return;
    }

  region = object->getDescriptor ()->getFormatterRegion ();
  region->showContent ();
}

void
NclFormatterLayout::hideObject (NclExecutionObject *object)
{
  NclFormatterRegion *region = NULL;
  LayoutRegion *layoutRegion;
  NclCascadingDescriptor *descriptor;
  string regionId;

  descriptor = object->getDescriptor ();
  if (descriptor != NULL)
    {
      region = object->getDescriptor ()->getFormatterRegion ();
    }

  if (region == NULL)
    {
      if (descriptor != NULL)
        {
          descriptor->setFormatterLayout (this);
        }
      return;
    }

  region->hideContent ();
  layoutRegion = region->getLayoutRegion ();
  regionId = layoutRegion->getId ();

  // removeFormatterRegionFromMaps(regionId, region);
}

SDLWindow*
NclFormatterLayout::addRegionOnMaps (NclExecutionObject *object,
                                     NclFormatterRegion *region,
                                     string layoutRegionId, int zIndex,
                                     string plan, double *cvtZIndex)
{
  double convertedZIndex;
  set<NclFormatterRegion *> *formRegions;
  SDLWindow* windowId;

  lock ();
  convertedZIndex = convertZIndex (zIndex, plan);
  if (regionMap.count (layoutRegionId) == 0)
    {
      formRegions = new set<NclFormatterRegion *>;
      regionMap[layoutRegionId] = formRegions;
      regionZIndex[layoutRegionId] = convertedZIndex;
    }
  else
    {
      formRegions = regionMap[layoutRegionId];
    }

  formRegions->insert (region);
  windowId = region->getOutputId ();
  if (windowId == 0)
    {
      windowId
          = region->prepareOutputDisplay (convertedZIndex);
    }
  objectMap[region] = object;

  *cvtZIndex = convertedZIndex;
  unlock ();

  return windowId;
}

NclExecutionObject *
NclFormatterLayout::removeFormatterRegionFromMaps (
    const string &layoutRegionId, NclFormatterRegion *formatterRegion)
{
  set<NclFormatterRegion *> *formRegions;
  set<NclFormatterRegion *>::iterator i;
  map<string, set<NclFormatterRegion *> *>::iterator j;
  map<NclFormatterRegion *, NclExecutionObject *>::iterator k;
  map<string, double>::iterator l;
  vector<string>::iterator m;

  NclExecutionObject *myObject = NULL;

  lock ();
  if (regionMap.count (layoutRegionId) != 0)
    {
      formRegions = regionMap[layoutRegionId];
      i = formRegions->find (formatterRegion);
      if (i != formRegions->end ())
        {
          formRegions->erase (i);
          if (formRegions->empty ())
            {
              j = regionMap.find (layoutRegionId);
              if (j != regionMap.end ())
                {
                  regionMap.erase (j);
                  delete formRegions;

                  l = regionZIndex.find (layoutRegionId);
                  if (l != regionZIndex.end ())
                    {
                      regionZIndex.erase (l);
                    }

                }
            }

          k = objectMap.find (formatterRegion);
          if (k != objectMap.end ())
            {
              myObject = k->second;
              objectMap.erase (k);
            }
        }
    }
  unlock ();

  return myObject;
}

void
NclFormatterLayout::clear ()
{
  map<string, set<NclFormatterRegion *> *>::iterator i;
  set<NclFormatterRegion *>::iterator j;

  lock ();
  i = regionMap.begin ();
  while (i != regionMap.end ())
    {
      j = i->second->begin ();
      while (j != i->second->end ())
        {
          delete *j;
          ++j;
        }

      i->second->clear ();
      delete i->second;

      ++i;
    }

  regionMap.clear ();

  regionZIndex.clear ();
  objectMap.clear ();
  unlock ();

  region->removeRegions ();
}

void
NclFormatterLayout::lock ()
{
  Thread::mutexLock (&mutex);
}

void
NclFormatterLayout::unlock ()
{
  Thread::mutexUnlock (&mutex);
}

GINGA_FORMATTER_END
