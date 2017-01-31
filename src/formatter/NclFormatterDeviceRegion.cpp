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
#include "NclFormatterDeviceRegion.h"

GINGA_FORMATTER_BEGIN

NclFormatterDeviceRegion::NclFormatterDeviceRegion (const string &id)
    : LayoutRegion (id)
{
  this->id = id;
  sortedRegions = new vector<LayoutRegion *>;

  Thread::mutexInit (&rMutex, false);
  typeSet.insert ("NclFormatterDeviceRegion");
}

void
NclFormatterDeviceRegion::addRegion (LayoutRegion *region)
{
  vector<LayoutRegion *>::iterator i;
  int zIndex;

  Thread::mutexLock (&rMutex);
  if (regionSet.count (region) == 0)
    {
      regionSet.insert (region);
      zIndex = region->getZIndexValue ();

      i = sortedRegions->begin ();
      while (i != sortedRegions->end ())
        {
          if (zIndex <= (*i)->getZIndexValue ())
            {
              break;
            }
          ++i;
        }

      sortedRegions->insert (i, region);
      region->setParent (this);
    }
  Thread::mutexUnlock (&rMutex);
}

LayoutRegion *
NclFormatterDeviceRegion::cloneRegion ()
{
  LayoutRegion *cloneRegion;
  vector<LayoutRegion *> *childRegions;

  cloneRegion = new NclFormatterDeviceRegion (id);

  cloneRegion->setTitle (getTitle ());
  cloneRegion->setLeft (left, false);
  cloneRegion->setTop (top, false);
  cloneRegion->setWidth (width, false);
  cloneRegion->setHeight (height, false);
  cloneRegion->setDecorated (false);
  cloneRegion->setMovable (false);
  cloneRegion->setResizable (false);

  childRegions = getRegions ();
  vector<LayoutRegion *>::iterator it;
  for (it = childRegions->begin (); it != childRegions->end (); ++it)
    {
      cloneRegion->addRegion (*it);
    }

  return cloneRegion;
}

int
NclFormatterDeviceRegion::compareWidthSize (const string &w)
{
  int newW;

  newW = atoi (w.c_str ());
  if (newW == width)
    {
      return 0;
    }
  else if (newW > width)
    {
      return 1;
    }
  else
    {
      return -1;
    }
}

int
NclFormatterDeviceRegion::compareHeightSize (const string &h)
{
  int newH;

  newH = atoi (h.c_str ());
  if (newH == height)
    {
      return 0;
    }
  else if (newH > height)
    {
      return 1;
    }
  else
    {
      return -1;
    }
}

short
NclFormatterDeviceRegion::getBackgroundColor ()
{
  return -1;
}

double
NclFormatterDeviceRegion::getBottom ()
{
  return NaN ();
}

double
NclFormatterDeviceRegion::getHeight ()
{
  return height;
}

double
NclFormatterDeviceRegion::getLeft ()
{
  return left;
}

double
NclFormatterDeviceRegion::getRight ()
{
  return NaN ();
}

LayoutRegion *
NclFormatterDeviceRegion::getRegion (const string &id)
{
  int i, size;
  LayoutRegion *region;

  Thread::mutexLock (&rMutex);
  size = (int) sortedRegions->size ();
  for (i = 0; i < size; i++)
    {
      region = (LayoutRegion *)((*sortedRegions)[i]);
      if (region->getId () == id)
        {
          Thread::mutexUnlock (&rMutex);
          return region;
        }
    }

  Thread::mutexUnlock (&rMutex);
  return NULL;
}

LayoutRegion *
NclFormatterDeviceRegion::getRegionRecursively (const string &id)
{
  int i, size;
  LayoutRegion *region, *auxRegion;

  Thread::mutexLock (&rMutex);
  size = (int) sortedRegions->size ();
  for (i = 0; i < size; i++)
    {
      region = (LayoutRegion *)((*sortedRegions)[i]);
      if (region->getId () == id)
        {
          Thread::mutexUnlock (&rMutex);
          return region;
        }
      auxRegion = region->getRegionRecursively (id);
      if (auxRegion != NULL)
        {
          Thread::mutexUnlock (&rMutex);
          return auxRegion;
        }
    }

  Thread::mutexUnlock (&rMutex);
  return NULL;
}

vector<LayoutRegion *> *
NclFormatterDeviceRegion::getRegions ()
{
  vector<LayoutRegion *> *regs;

  Thread::mutexLock (&rMutex);
  regs = new vector<LayoutRegion *> (*sortedRegions);
  Thread::mutexUnlock (&rMutex);

  return regs;
}

string
NclFormatterDeviceRegion::getTitle ()
{
  return "";
}

double
NclFormatterDeviceRegion::getTop ()
{
  return top;
}

double
NclFormatterDeviceRegion::getWidth ()
{
  return width;
}

int
NclFormatterDeviceRegion::getZIndex ()
{
  return -1;
}

int
NclFormatterDeviceRegion::getZIndexValue ()
{
  int zIndex;

  zIndex = getZIndex ();
  if (zIndex != -1)
    {
      return zIndex;
    }
  else
    {
      return 0;
    }
}

bool
NclFormatterDeviceRegion::isBottomPercentual ()
{
  return false;
}

bool
NclFormatterDeviceRegion::isHeightPercentual ()
{
  return false;
}

bool
NclFormatterDeviceRegion::isLeftPercentual ()
{
  return false;
}

bool
NclFormatterDeviceRegion::isRightPercentual ()
{
  return false;
}

bool
NclFormatterDeviceRegion::isTopPercentual ()
{
  return false;
}

bool
NclFormatterDeviceRegion::isWidthPercentual ()
{
  return false;
}

string
NclFormatterDeviceRegion::toString ()
{
  string str;
  int i, size;
  LayoutRegion *region;

  str = "id: " + getId () + '\n';
  Thread::mutexLock (&rMutex);
  size = (int) sortedRegions->size ();
  Thread::mutexUnlock (&rMutex);
  for (i = 0; i < size; i++)
    {
      Thread::mutexLock (&rMutex);
      region = ((LayoutRegion *)(*sortedRegions)[i]);
      Thread::mutexUnlock (&rMutex);
      str = str + region->toString ();
    }
  return str + '\n';
}

bool
NclFormatterDeviceRegion::removeRegion (LayoutRegion *region)
{
  vector<LayoutRegion *>::iterator it;

  Thread::mutexLock (&rMutex);
  if (regionSet.count (region) != 0)
    {
      for (it = sortedRegions->begin (); it != sortedRegions->end (); ++it)
        {
          if (*it == region)
            {
              sortedRegions->erase (it);
              regionSet.erase (regionSet.find (region));
              Thread::mutexUnlock (&rMutex);
              return true;
            }
        }
    }

  Thread::mutexUnlock (&rMutex);
  return false;
}

void
NclFormatterDeviceRegion::removeRegions ()
{
  Thread::mutexLock (&rMutex);
  sortedRegions->clear ();
  regionSet.clear ();
  Thread::mutexUnlock (&rMutex);
}

void
NclFormatterDeviceRegion::setBackgroundColor (arg_unused (Color *newBackgroundColor))
{
}

bool
NclFormatterDeviceRegion::setBottom (arg_unused (double newBottom), arg_unused (bool isPercentual))
{
  return false;
}

bool
NclFormatterDeviceRegion::setHeight (double newHeight, arg_unused (bool isPercentual))
{
  this->height = (int) newHeight;
  return true;
}

bool
NclFormatterDeviceRegion::setLeft (double newLeft, arg_unused (bool isPercentual))
{
  this->left = (int) newLeft;
  return true;
}

bool
NclFormatterDeviceRegion::setRight (arg_unused (double newRight), arg_unused (bool isPercentual))
{
  return false;
}

void
NclFormatterDeviceRegion::setTitle (arg_unused (const string &newTitle))
{
}

bool
NclFormatterDeviceRegion::setTop (double newTop, arg_unused (bool isPercentual))
{
  this->top = (int)newTop;
  return true;
}

bool
NclFormatterDeviceRegion::setWidth (double newWidth, arg_unused (bool isPercentual))
{
  this->width = (int)newWidth;
  return true;
}

void
NclFormatterDeviceRegion::setZIndex (arg_unused (int newZIndex))
{
}

vector<LayoutRegion *> *
NclFormatterDeviceRegion::getRegionsSortedByZIndex ()
{
  /*vector<LayoutRegion*>* sortedRegions;
  vector<LayoutRegion*>::iterator componentRegions;
  LayoutRegion* ncmRegion;
  LayoutRegion* auxRegion;
  vector<LayoutRegion*>::iterator i;
  int j, size, zIndexValue;

  sortedRegions = new vector<LayoutRegion*>;

  componentRegions = sortedRegions->begin();
  while (componentRegions != sortedRegions->end()) {
          ncmRegion = (*componentRegions);
          zIndexValue = ncmRegion->getZIndexValue();

          size = sortedRegions->size();
          i = sortedRegions->begin();
          for (j = 0; j < size; j++) {
                  auxRegion = *i;
                  if (i == sortedRegions->end() ||
                                  zIndexValue <= (auxRegion)->
                              getZIndexValue()) {
                          break;
                  }
                  ++i;
          }
          sortedRegions->insert(i, ncmRegion);
          ++componentRegions;
  }

  Thread::mutexLock(&rMutex);
  sortedRegions = new vector<LayoutRegion*>(*sortedRegions);
  Thread::mutexUnlock(&rMutex);

  return sortedRegions;*/

  return LayoutRegion::getRegionsSortedByZIndex ();
}

vector<LayoutRegion *> *
NclFormatterDeviceRegion::getRegionsOverRegion (LayoutRegion *region)
{
  vector<LayoutRegion *> *frontRegions;
  vector<LayoutRegion *>::iterator it;
  LayoutRegion *childRegion;
  int childZIndex, regionZIndex;

  regionZIndex = region->getZIndexValue ();
  frontRegions = new vector<LayoutRegion *>;

  Thread::mutexLock (&rMutex);
  for (it = sortedRegions->begin (); it != sortedRegions->end (); ++it)
    {
      childRegion = *it;
      childZIndex = childRegion->getZIndexValue ();
      if (childZIndex > regionZIndex && region->intersects (childRegion))
        {
          frontRegions->insert (frontRegions->begin (), childRegion);
        }
    }
  Thread::mutexUnlock (&rMutex);

  return frontRegions;
}

LayoutRegion *
NclFormatterDeviceRegion::getParent ()
{
  return NULL;
}

void
NclFormatterDeviceRegion::setParent (arg_unused (LayoutRegion *parent))
{
}

int
NclFormatterDeviceRegion::getTopInPixels ()
{
  return top;
}

int
NclFormatterDeviceRegion::getBottomInPixels ()
{
  return top + height;
}

int
NclFormatterDeviceRegion::getRightInPixels ()
{
  return left + width;
}

int
NclFormatterDeviceRegion::getLeftInPixels ()
{
  return left;
}

int
NclFormatterDeviceRegion::getHeightInPixels ()
{
  return height;
}

int
NclFormatterDeviceRegion::getWidthInPixels ()
{
  return width;
}

bool
NclFormatterDeviceRegion::isMovable ()
{
  return false;
}

bool
NclFormatterDeviceRegion::isResizable ()
{
  return false;
}

bool
NclFormatterDeviceRegion::isDecorated ()
{
  return false;
}

void
NclFormatterDeviceRegion::setMovable (arg_unused (bool movable))
{
}

void
NclFormatterDeviceRegion::setResizable (arg_unused (bool resizable))
{
}

void
NclFormatterDeviceRegion::setDecorated (arg_unused (bool decorated))
{
}

void
NclFormatterDeviceRegion::resetTop ()
{
}

void
NclFormatterDeviceRegion::resetBottom ()
{
}

void
NclFormatterDeviceRegion::resetRight ()
{
}

void
NclFormatterDeviceRegion::resetLeft ()
{
}

void
NclFormatterDeviceRegion::resetHeight ()
{
}

void
NclFormatterDeviceRegion::resetWidth ()
{
}

void
NclFormatterDeviceRegion::resetZIndex ()
{
}

void
NclFormatterDeviceRegion::resetDecorated ()
{
}

void
NclFormatterDeviceRegion::resetMovable ()
{
}

void
NclFormatterDeviceRegion::resetResizable ()
{
}

int
NclFormatterDeviceRegion::getAbsoluteLeft ()
{
  return left;
}

int
NclFormatterDeviceRegion::getAbsoluteTop ()
{
  return top;
}

void
NclFormatterDeviceRegion::dispose ()
{
  removeRegions ();
  // sortedRegions = NULL;
}

string
NclFormatterDeviceRegion::getId ()
{
  return id;
}

void
NclFormatterDeviceRegion::setId (const string &id)
{
  this->id = id;
}

Entity *
NclFormatterDeviceRegion::getDataEntity ()
{
  return NULL;
}

int
NclFormatterDeviceRegion::compareTo (arg_unused (void *arg0))
{
  return 0;
}

GINGA_FORMATTER_END
