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
#include "LayoutRegion.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wfloat-conversion)

GINGA_NCL_BEGIN

LayoutRegion::LayoutRegion (const string &id) : Entity (id)
{
  outputMapRegionId = "";
  outputMapRegion = NULL;

  top = (double)NAN;
  topPercent = false;

  left = (double)NAN;
  leftPercent = false;

  bottom = (double)NAN;
  bottomPercent = false;

  right = (double)NAN;
  rightPercent = false;

  width = (double)NAN;
  widthPercent = false;

  height = (double)NAN;
  heightPercent = false;

  zIndex = NULL;

  devClass = -1;

  parent = NULL;
  pthread_mutex_init (&mutex, NULL);
}

LayoutRegion::~LayoutRegion ()
{
  LayoutRegion *region;
  map<string, LayoutRegion *>::iterator i;

  lock ();
  Entity::hasInstance (this, true);

  if (parent != NULL)
    {
      parent->removeRegion (this);
      parent = NULL;
    }

  i = regions.begin ();
  while (i != regions.end ())
    {
      region = i->second;

      if (Entity::hasInstance (region, false))
        {
          region->setParent (NULL);
          delete region;
        }

      ++i;
    }

  regions.clear ();
  sorted.clear ();

  if (zIndex != NULL)
    {
      delete zIndex;
      zIndex = NULL;
    }

  unlock ();
  pthread_mutex_destroy (&mutex);
}

void
LayoutRegion::addRegion (LayoutRegion *region)
{
  int zIndexVal;
  vector<LayoutRegion *>::iterator i;
  string regId;

  regId = region->getId ();

  lock ();
  if (regions.count (regId) != 0)
    {
      unlock ();
      return;
    }

  regions[regId] = region;
  region->setParent (this);

  zIndexVal = region->getZIndexValue ();
  i = sorted.begin ();
  while (i != sorted.end ())
    {
      if (zIndexVal <= (*i)->getZIndexValue ())
        {
          break;
        }
      ++i;
    }

  sorted.insert (i, region);
  unlock ();
}

LayoutRegion *
LayoutRegion::cloneRegion ()
{
  LayoutRegion *cloneRegion;
  vector<LayoutRegion *> *childRegions;
  LayoutRegion *childRegion;

  cloneRegion = new LayoutRegion (getId ());

  if (getParent () != NULL)
    {
      cloneRegion->setParent (getParent ());
    }

  if (!isnan (getBottom ()))
    {
      cloneRegion->setBottom (getBottom (), isBottomPercent ());
    }

  if (!isnan (getLeft ()))
    {
      cloneRegion->setLeft (getLeft (), isLeftPercent ());
    }

  if (!isnan (getTop ()))
    {
      cloneRegion->setTop (getTop (), isTopPercent ());
    }

  if (!isnan (getRight ()))
    {
      cloneRegion->setRight (getRight (), isRightPercent ());
    }

  if (!isnan (getWidth ()))
    {
      cloneRegion->setWidth (getWidth (), isWidthPercent ());
    }

  if (!isnan (getHeight ()))
    {
      cloneRegion->setHeight (getHeight (), isHeightPercent ());
    }

  cloneRegion->setZIndex (getZIndex ());

  childRegions = getRegions ();
  if (childRegions == NULL)
    {
      delete cloneRegion;
      return NULL;
    }

  lock ();
  vector<LayoutRegion *>::iterator it;
  for (it = childRegions->begin (); it != childRegions->end (); ++it)
    {
      childRegion = (*it)->cloneRegion ();
      cloneRegion->addRegion (childRegion);
    }
  unlock ();
  delete childRegions;
  childRegions = NULL;

  return cloneRegion;
}

LayoutRegion *
LayoutRegion::copyRegion ()
{
  LayoutRegion *cloneRegion;

  cloneRegion = new LayoutRegion (getId ());
  if (!isnan (getBottom ()))
    {
      cloneRegion->setBottom (getBottomInPixels (), false);
    }

  if (!isnan (getLeft ()))
    {
      cloneRegion->setLeft (getAbsoluteLeft (), false);
    }

  if (!isnan (getTop ()))
    {
      cloneRegion->setTop (getAbsoluteTop (), false);
    }

  if (!isnan (getRight ()))
    {
      cloneRegion->setRight (getRightInPixels (), false);
    }

  if (!isnan (getWidth ()))
    {
      cloneRegion->setWidth (getWidthInPixels (), false);
    }

  if (!isnan (getHeight ()))
    {
      cloneRegion->setHeight (getHeightInPixels (), false);
    }

  cloneRegion->setZIndex (getZIndex ());

  cloneRegion->setParent (getDeviceLayout ());

  return cloneRegion;
}

int
LayoutRegion::compareWidthSize (const string &w)
{
  int oldW;
  int newW;

  oldW = getWidthInPixels ();
  if (xstrispercent (w))
    {
      newW = (int)((getParent ()->getWidthInPixels ()
                    * getPercentValue (w))
                   / 100);
    }
  else
    {
      newW = xstrtoint (w.c_str (), 10);
    }

  if (newW == oldW)
    {
      return 0;
    }
  else if (newW > oldW)
    {
      return 1;
    }
  else
    {
      return -1;
    }
}

int
LayoutRegion::compareHeightSize (const string &h)
{
  int oldH;
  int newH;

  oldH = getHeightInPixels ();
  if (xstrispercent (h))
    {
      newH = (int)((getParent ()->getHeightInPixels ()
                    * getPercentValue (h))
                   / 100);
    }
  else
    {
      newH = xstrtoint (h.c_str (), 10);
    }
  if (newH == oldH)
    {
      return 0;
    }
  else if (newH > oldH)
    {
      return 1;
    }
  else
    {
      return -1;
    }
}

double
LayoutRegion::getBottom ()
{
  return bottom;
}

double
LayoutRegion::getHeight ()
{
  return height;
}

double
LayoutRegion::getLeft ()
{
  return left;
}

double
LayoutRegion::getRight ()
{
  return right;
}

LayoutRegion *
LayoutRegion::getRegion (const string &id)
{
  map<string, LayoutRegion *>::iterator i;
  LayoutRegion *someRegion;

  lock ();
  i = regions.find (id);
  if (i == regions.end ())
    {
      unlock ();

      if (id == getId ())
        {
          return this;
        }

      return NULL;
    }

  someRegion = i->second;
  unlock ();

  return someRegion;
}

LayoutRegion *
LayoutRegion::getRegionRecursively (const string &id)
{
  map<string, LayoutRegion *>::iterator i;
  LayoutRegion *region;
  LayoutRegion *auxRegion;

  region = getRegion (id);
  if (region != NULL)
    {
      return region;
    }

  i = regions.begin ();
  while (i != regions.end ())
    {
      region = i->second;

      auxRegion = region->getRegionRecursively (id);
      if (auxRegion != NULL)
        {
          return auxRegion;
        }

      ++i;
    }
  return NULL;
}

void
LayoutRegion::printRegionIdsRecursively ()
{
  map<string, LayoutRegion *>::iterator i;

  cout << "Region '" << getId () << "' has: ";

  i = regions.begin ();
  while (i != regions.end ())
    {
      cout << "'" << i->first << "' ";

      ++i;
    }

  cout << endl;

  i = regions.begin ();
  while (i != regions.end ())
    {
      i->second->printRegionIdsRecursively ();
      ++i;
    }
}

vector<LayoutRegion *> *
LayoutRegion::getRegions ()
{
  map<string, LayoutRegion *>::iterator i;
  vector<LayoutRegion *> *childRegions;
  lock ();
  childRegions = new vector<LayoutRegion *>;
  for (i = regions.begin (); i != regions.end (); ++i)
    {
      childRegions->push_back (i->second);
    }
  unlock ();
  return childRegions;
}

double
LayoutRegion::getTop ()
{
  return top;
}

double
LayoutRegion::getWidth ()
{
  return width;
}

int
LayoutRegion::getZIndex ()
{
  if (zIndex != NULL)
    {
      return (int)*zIndex;
    }
  else
    {
      return 0;
    }
}

int
LayoutRegion::getZIndexValue ()
{
  if (zIndex != NULL)
    {
      return (int)*zIndex;
    }
  else
    {
      return 0;
    }
}

bool
LayoutRegion::isBottomPercent ()
{
  return bottomPercent;
}

bool
LayoutRegion::isHeightPercent ()
{
  return heightPercent;
}

bool
LayoutRegion::isLeftPercent ()
{
  return leftPercent;
}

bool
LayoutRegion::isRightPercent ()
{
  return rightPercent;
}

bool
LayoutRegion::isTopPercent ()
{
  return topPercent;
}

bool
LayoutRegion::isWidthPercent ()
{
  return widthPercent;
}

bool
LayoutRegion::removeRegion (LayoutRegion *region)
{
  map<string, LayoutRegion *>::iterator i;
  LayoutRegion *childRegion;

  lock ();
  if (regions.count (region->getId ()) != 0)
    {
      i = regions.find (region->getId ());
      childRegion = i->second;
      childRegion->setParent (NULL);
      regions.erase (i);
      unlock ();
      return true;
    }
  unlock ();
  return false;
}

void
LayoutRegion::removeRegions ()
{
  map<string, LayoutRegion *>::iterator i;
  LayoutRegion *region;

  lock ();
  i = regions.begin ();
  while (i != regions.end ())
    {
      region = i->second;
      if (region != this && Entity::hasInstance (region, false))
        {
          region->setParent (NULL);
          delete region;
        }
      ++i;
    }
  regions.clear ();
  unlock ();
}

LayoutRegion *
LayoutRegion::getDeviceLayout ()
{
  LayoutRegion *device;

  if (parent == NULL)
    {
      device = this;
    }
  else
    {
      device = parent;
    }

  while (device->getParent () != NULL)
    {
      device = device->getParent ();
    }

  return device;
}

double
LayoutRegion::getDeviceWidthInPixels ()
{
  LayoutRegion *device = getDeviceLayout ();

  clog << "LayoutRegion::getDeviceWidthInPixels: '";
  clog << device->getWidthInPixels () << "'" << endl;

  return device->getWidthInPixels ();
}

double
LayoutRegion::getDeviceHeightInPixels ()
{
  LayoutRegion *device = getDeviceLayout ();

  clog << "LayoutRegion::getDeviceHeightInPixels: '";
  clog << device->getHeightInPixels () << "'" << endl;

  return device->getHeightInPixels ();
}

bool
LayoutRegion::setBottom (double newBottom, bool isPercent)
{
  if (newBottom < 0 || isnan (newBottom))
    {
      clog << "LayoutRegion::setBottom Warning! Trying ";
      clog << "to set an invalid bottom value: " << newBottom << endl;
      return false;
    }

  bottom = newBottom;
  bottomPercent = isPercent;
  if (parent != NULL)
    {
      if ((getTopInPixels () + getHeightInPixels ())
          > parent->getHeightInPixels ())
        {
          // since the region will stay outside the parent edges, the
          // bottom is set to the maximum value allowed
          if (isPercent)
            {
              bottom = 1.0;
            }
          else
            {
              bottom = parent->getHeightInPixels ();
            }
          return false;
        }
    }

  return true;
}

bool
LayoutRegion::setTargetBottom (double newBottom, bool isPercent)
{
  double tBottom;
  double deviceHeight;
  double currentHeight;

  if (newBottom < 0)
    {
      clog << "LayoutRegion::setTargetBottom Warning! Trying ";
      clog << "to set an invalid bottom value: " << newBottom << endl;
      return false;
    }

  deviceHeight = getDeviceHeightInPixels ();
  if (isPercent)
    {
      // tBottom = (newBottom * getHeightInPixels()) / 100;
      tBottom = (newBottom * deviceHeight) / 100;
    }
  else
    {
      tBottom = newBottom;
    }

  currentHeight = getHeightInPixels ();

  if (tBottom + currentHeight > deviceHeight)
    {
      tBottom = deviceHeight - currentHeight;
    }

  bottom = tBottom;
  bottomPercent = false;

  setTargetTop (deviceHeight - (tBottom + currentHeight), false);

  return true;
}

bool
LayoutRegion::setHeight (double newHeight, bool isPercent)
{
  if (newHeight < 0 || isnan (newHeight))
    {
      clog << "LayoutRegion::setHeight Warning! Trying ";
      clog << "to set an invalid height value: " << newHeight << endl;
      return false;
    }

  height = newHeight;
  heightPercent = isPercent;

  if (parent != NULL)
    {
      if ((getTopInPixels () + getHeightInPixels ())
          > parent->getHeightInPixels ())
        {
          // since the region will stay outside the parent edges, the
          // top is set to the maximum value allowed
          if (isPercent)
            {
              height = (parent->getHeightInPixels () - getTopInPixels ())
                       / parent->getHeightInPixels ();
            }
          else
            {
              height = parent->getHeightInPixels () - getTopInPixels ();
            }

          return false;
        }
    }

  return true;
}

bool
LayoutRegion::setTargetHeight (double newHeight, bool isPercent)
{
  double tHeight;

  if (newHeight < 0)
    {
      clog << "LayoutRegion::setTargetHeight Warning! Trying ";
      clog << "to set an invalid height value: " << newHeight << endl;
      return false;
    }

  if (isPercent)
    {
      // tHeight = (newHeight * getHeightInPixels()) / 100;
      tHeight = (newHeight * getDeviceHeightInPixels ()) / 100;
    }
  else
    {
      tHeight = newHeight;
    }

  height = tHeight;
  heightPercent = false;

  return true;
}

bool
LayoutRegion::setLeft (double newLeft, bool isPercent)
{
  if ((newLeft < 0) || (isnan (newLeft)))
    {
      clog << "LayoutRegion::setLeft Warning! Trying ";
      clog << "to set an invalid left value: " << newLeft << endl;
      return false;
    }

  this->left = newLeft;
  leftPercent = isPercent;

  if (parent != NULL)
    {
      if ((getLeftInPixels () + getWidthInPixels ())
          > parent->getWidthInPixels ())
        {
          // since the region will stay outside the parent edges, the
          // left is set to the maximum value allowed
          if (isPercent)
            {
              left = (double)((parent->getWidthInPixels ()
                               - getWidthInPixels ())
                              / parent->getWidthInPixels ());
            }
          else
            {
              left = (double)(parent->getWidthInPixels ()
                              - getWidthInPixels ());
            }

          return false;
        }
    }

  return true;
}

bool
LayoutRegion::setTargetLeft (double newLeft, bool isPercent)
{
  double tLeft;

  if (newLeft < 0)
    {
      clog << "LayoutRegion::setTargetLeft Warning! Trying ";
      clog << "to set an invalid left value: " << newLeft << endl;
      return false;
    }

  if (isPercent)
    {
      // tLeft = (newLeft * getWidthInPixels()) / 100;
      tLeft = (newLeft * getDeviceWidthInPixels ()) / 100;
    }
  else
    {
      tLeft = newLeft;
    }

  left = tLeft;
  leftPercent = false;

  return true;
}

bool
LayoutRegion::setRight (double newRight, bool isPercent)
{
  if (newRight < 0 || isnan (newRight))
    {
      clog << "LayoutRegion::setRight Warning! Trying ";
      clog << "to set an invalid right value: " << newRight << endl;
      return false;
    }

  right = newRight;
  rightPercent = isPercent;

  if (parent != NULL)
    {
      if ((getLeftInPixels () + getWidthInPixels ())
          > parent->getWidthInPixels ())
        {
          // since the region will stay outside the parent edges, the
          // right is set to the maximum value allowed
          if (isPercent)
            {
              right = 1.0;
            }
          else
            {
              right = parent->getWidthInPixels ();
            }
          return false;
        }
    }

  return true;
}

bool
LayoutRegion::setTargetRight (double newRight, bool isPercent)
{
  double tRight;
  double deviceWidth;
  double currentWidth;

  if (newRight < 0)
    {
      clog << "LayoutRegion::setTargetRight Warning! Trying ";
      clog << "to set an invalid right value: " << newRight << endl;
      return false;
    }

  deviceWidth = getDeviceWidthInPixels ();
  if (isPercent)
    {
      // tRight = (newRight * getWidthInPixels()) / 100;
      tRight = (newRight * deviceWidth) / 100;
    }
  else
    {
      tRight = newRight;
    }

  currentWidth = getWidthInPixels ();

  if (tRight + currentWidth > deviceWidth)
    {
      tRight = deviceWidth - currentWidth;
    }

  right = tRight;
  rightPercent = false;

  setTargetLeft (deviceWidth - (tRight + currentWidth), false);

  return true;
}

bool
LayoutRegion::setTop (double newTop, bool isPercent)
{
  if (newTop < 0 || isnan (newTop))
    {
      clog << "LayoutRegion::setTop Warning! Trying ";
      clog << "to set an invalid top value: " << newTop << endl;
      return false;
    }

  top = newTop;
  topPercent = isPercent;

  if (parent != NULL)
    {
      if (((double)(getTopInPixels ()) + (double)(getHeightInPixels ()))
          > (double)(parent->getHeightInPixels ()))
        {
          // since the region will stay outside the parent edges, the
          // top is set to the maximum value allowed
          if (isPercent)
            {
              top = (double)((double)(parent->getHeightInPixels ()
                                      - getHeightInPixels ())
                             / (double)(parent->getHeightInPixels ()));
            }
          else
            {
              top = (double)((double)parent->getHeightInPixels ()
                             - (double)getHeightInPixels ());
            }
          return false;
        }
    }
  return true;
}

bool
LayoutRegion::setTargetTop (double newTop, bool isPercent)
{
  double tTop;

  if (newTop < 0 || isnan (newTop))
    {
      clog << "LayoutRegion::setTop Warning! Trying ";
      clog << "to set an invalid top value: " << newTop << endl;
      return false;
    }

  if (isPercent)
    {
      // tTop = (newTop * getHeightInPixels()) / 100;
      tTop = (newTop * getDeviceHeightInPixels ()) / 100;
    }
  else
    {
      tTop = newTop;
    }

  top = tTop;
  topPercent = false;

  return true;
}

bool
LayoutRegion::setWidth (double newWidth, bool isPercent)
{
  if (newWidth < 0 || isnan (newWidth))
    {
      clog << "LayoutRegion::setWidth Warning! Trying ";
      clog << "to set an invalid width value: " << newWidth;
      clog << endl;
      return false;
    }

  width = newWidth;
  widthPercent = isPercent;

  if (parent != NULL)
    {
      if ((getLeftInPixels () + getWidthInPixels ())
          > parent->getWidthInPixels ())
        {
          // since the region will stay outside the parent edges, the
          // top is set to the maximum value allowed
          if (isPercent)
            {
              width = ((double)(parent->getWidthInPixels ())
                       - (double)getLeftInPixels ())
                      / (double)(parent->getWidthInPixels ());
            }
          else
            {
              width = parent->getWidthInPixels () - getLeftInPixels ();
            }
          return false;
        }
    }
  return true;
}

bool
LayoutRegion::setTargetWidth (double newWidth, bool isPercent)
{
  double tWidth;

  if (newWidth <= 0)
    {
      clog << "LayoutRegion::setTargetWidth Warning! Trying ";
      clog << "to set an invalid width value: " << newWidth;
      clog << endl;
      return false;
    }

  if (isPercent)
    {
      // tWidth = (newWidth * getWidthInPixels()) / 100;
      tWidth = (newWidth * getDeviceWidthInPixels ()) / 100;
    }
  else
    {
      tWidth = newWidth;
    }

  width = tWidth;
  widthPercent = false;

  return true;
}

void
LayoutRegion::setZIndex (int newZIndex)
{
  if (zIndex == NULL)
    {
      zIndex = new double;
    }
  *zIndex = newZIndex;
}

vector<LayoutRegion *> *
LayoutRegion::getRegionsSortedByZIndex ()
{
  vector<LayoutRegion *> *sortedRegions;
  lock ();
  sortedRegions = new vector<LayoutRegion *> (sorted);
  unlock ();
  return sortedRegions;
}

vector<LayoutRegion *> *
LayoutRegion::getRegionsOverRegion (LayoutRegion *region)
{
  vector<LayoutRegion *> *allRegions;
  vector<LayoutRegion *>::iterator i;
  vector<LayoutRegion *> *frontRegions;
  LayoutRegion *childRegion;

  frontRegions = new vector<LayoutRegion *>;
  allRegions = getRegionsSortedByZIndex ();

  i = allRegions->begin ();
  while (i != allRegions->end ())
    {
      childRegion = *i;
      if (childRegion->getZIndexValue () > region->getZIndexValue ())
        {
          frontRegions->insert (frontRegions->begin (), childRegion);
        }
      ++i;
    }
  delete allRegions;
  allRegions = NULL;

  return frontRegions;
}

LayoutRegion *
LayoutRegion::getParent ()
{
  return parent;
}

void
LayoutRegion::setDeviceClass (int deviceClass, const string &mapId)
{
  bool changed = false;

  if (deviceClass != this->devClass && deviceClass >= 0)
    {
      this->devClass = deviceClass;
      changed = true;
    }

  if (outputMapRegionId == "")
    {
      this->outputMapRegionId = mapId;
      changed = true;
    }

  if (changed)
    {
      refreshDeviceClassRegions ();
    }
}

int
LayoutRegion::getDeviceClass ()
{
  return devClass;
}

void
LayoutRegion::setOutputMapRegion (LayoutRegion *outMapRegion)
{
  this->outputMapRegion = outMapRegion;
}

LayoutRegion *
LayoutRegion::getOutputMapRegion ()
{
  return outputMapRegion;
}

string
LayoutRegion::getOutputMapRegionId ()
{
  return outputMapRegionId;
}

void
LayoutRegion::setParent (LayoutRegion *parent)
{
  int dClass;
  string mapId;

  lock ();
  this->parent = parent;

  if (parent != NULL)
    {
      if (parent->getOutputMapRegion () == NULL && outputMapRegion != NULL)
        {
          parent->setOutputMapRegion (outputMapRegion);
        }

      dClass = parent->getDeviceClass ();
      mapId = parent->getOutputMapRegionId ();
      if (dClass >= 0)
        {
          setDeviceClass (dClass, mapId);
        }
    }
  unlock ();
}

void
LayoutRegion::refreshDeviceClassRegions ()
{
  map<string, LayoutRegion *>::iterator i;

  i = regions.begin ();
  while (i != regions.end ())
    {
      if (i->second != this)
        {
          i->second->setDeviceClass (devClass, outputMapRegionId);
        }
      ++i;
    }
}

int
LayoutRegion::getTopInPixels ()
{
  double b, h;

  if (!isnan (top))
    {
      // top was defined
      if (isTopPercent () && parent != NULL)
        {
          return (int)((top * parent->getHeightInPixels ()) / 100);
        }
      else
        {
          return (int)top;
        }
    }
  else if ((!isnan (height)) && (!isnan (bottom)))
    {
      // top is based on height and bottom
      if (isHeightPercent () && parent != NULL)
        {
          h = (int)((height * parent->getHeightInPixels ()) / 100);
        }
      else
        {
          h = height;
        }

      if (parent != NULL)
        {
          if (isBottomPercent ())
            {
              b = (int)(((100 - bottom) * parent->getHeightInPixels ())
                        / 100);
            }
          else
            {
              b = parent->getHeightInPixels () - bottom;
            }
        }
      else
        {
          b = h;
        }

      return (int)(b - h);
    }
  else
    {
      // default value
      return 0;
    }
}

int
LayoutRegion::getBottomInPixels ()
{
  double bottomInPixels;
  double t, h;

  bottomInPixels = (double)NAN;

  if (!isnan (height) && !isnan (top))
    {
      // bottom is based on height and top, independent
      // of bottom definition
      if (isTopPercent () && parent != NULL)
        {
          t = (int)(((double)top * parent->getHeightInPixels ()) / 100);
        }
      else
        {
          t = top;
        }

      if (isHeightPercent () && parent != NULL)
        {
          h = (int)(((double)height * parent->getHeightInPixels ()) / 100);
        }
      else
        {
          h = height;
        }
      bottomInPixels = (int)(t + h);
    }
  else if (!isnan (bottom))
    {
      // bottom is based on its own value
      if (isBottomPercent () && parent != NULL)
        {
          bottomInPixels
              = (int)(((100 - bottom) * parent->getHeightInPixels ())
                      / 100);
        }
      else
        {
          bottomInPixels = (int)(getHeightInPixels () - bottom);
        }
    }
  else
    {
      bottomInPixels = (int)(getTopInPixels () + getHeightInPixels ());
    }

  return (int)bottomInPixels;
}

int
LayoutRegion::getRightInPixels ()
{
  double l, w;

  if (!isnan (width) && !isnan (left))
    {
      // right is based on width and left, independent of right
      // definition
      if (isLeftPercent () && parent != NULL)
        {
          l = (int)(((double)left * parent->getWidthInPixels ()) / 100);
        }
      else
        {
          l = left;
        }

      if (isWidthPercent () && parent != NULL)
        {
          w = (int)(((double)width * parent->getWidthInPixels ()) / 100);
        }
      else
        {
          w = width;
        }
      return (int)(l + w);
    }
  else if (!isnan (right))
    {
      // right is based on its own value
      if (isRightPercent () && parent != NULL)
        {
          return (int)(((100 - right) * parent->getWidthInPixels ()) / 100);
        }
      else
        {
          return (int)(getWidthInPixels () - right);
        }
    }
  else
    {
      return (int)(getLeftInPixels () + getWidthInPixels ());
    }
  return 0;
}

int
LayoutRegion::getLeftInPixels ()
{
  double r, w;

  if (!isnan (left))
    {
      // left was defined
      if (isLeftPercent () && parent != NULL)
        {
          return (int)((left * parent->getWidthInPixels ()) / 100);
        }
      else
        {
          return (int)left;
        }
    }
  else if (!isnan (width) && !isnan (right))
    {
      // left is based on width and right
      if (isWidthPercent () && parent != NULL)
        {
          w = (int)((width * parent->getWidthInPixels ()) / 100);
        }
      else
        {
          w = (int)width;
        }

      if (parent != NULL)
        {
          if (isRightPercent ())
            {
              r = (int)(((100 - right) * parent->getWidthInPixels ())
                        / 100);
            }
          else
            {
              r = parent->getWidthInPixels () - (int)right;
            }
        }
      else
        {
          r = w;
        }

      return (int)(r - w);
    }
  else
    {
      // default value
      return 0;
    }
}

int
LayoutRegion::getHeightInPixels ()
{
  int t = 0;
  int b = 0;

  if (!isnan (getHeight ()))
    {
      if (isHeightPercent () && parent != NULL)
        {
          return (int)((getHeight () * parent->getHeightInPixels ()) / 100);
        }
      else
        {
          return (int)getHeight ();
        }
    }
  else
    {
      t = 0;

      if (parent != NULL)
        {
          b = parent->getHeightInPixels ();
        }

      if (!isnan (getTop ()))
        {
          // height is based on top and bottom, independent of
          // height definition
          if (isTopPercent () && parent != NULL)
            {
              t = (int)((getTop () * parent->getHeightInPixels ()) / 100);
            }
          else
            {
              t = (int)getTop ();
            }
        }

      if (!isnan (getBottom ()) && parent != NULL)
        {
          if (isBottomPercent ())
            {
              b = (int)(((100 - getBottom ())
                         * parent->getHeightInPixels ())
                        / 100);
            }
          else
            {
              b = parent->getHeightInPixels () - (int)getBottom ();
            }
        }

      if (b >= t)
        {
          return (int)(b - t);
        }
      else
        {
          return 0;
        }
    }
}

int
LayoutRegion::getWidthInPixels ()
{
  int l = 0;
  int r = 0;

  if (!(isnan (getWidth ())))
    {
      if (isWidthPercent () && parent != NULL)
        {
          return (int)((width * parent->getWidthInPixels ()) / 100);
        }
      else
        {
          return (int)getWidth ();
        }
    }
  else
    {
      l = 0;

      if (parent != NULL)
        {
          r = parent->getWidthInPixels ();
        }

      if (!(isnan (getLeft ())))
        {
          if (isLeftPercent () && parent != NULL)
            {
              l = (int)((getLeft () * parent->getWidthInPixels ()) / 100);
            }
          else
            {
              l = (int)getLeft ();
            }
        }

      if (!(isnan (getRight ())) && parent != NULL)
        {
          if (isRightPercent ())
            {
              r = (int)(((100 - getRight ()) * parent->getWidthInPixels ())
                        / 100);
            }
          else
            {
              r = parent->getWidthInPixels () - (int)getRight ();
            }
        }

      if (r >= l)
        {
          return (int)(r - l);
        }
      else
        {
          return 0;
        }
    }
  return 0;
}

void
LayoutRegion::resetTop ()
{
  top = (double)NAN;
}

void
LayoutRegion::resetBottom ()
{
  bottom = (double)NAN;
}

void
LayoutRegion::resetRight ()
{
  right = (double)NAN;
}

void
LayoutRegion::resetLeft ()
{
  left = (double)NAN;
}

void
LayoutRegion::resetHeight ()
{
  height = (double)NAN;
}

void
LayoutRegion::resetWidth ()
{
  width = (double)NAN;
}

void
LayoutRegion::resetZIndex ()
{
  if (zIndex != NULL)
    {
      delete zIndex;
      zIndex = NULL;
    }
}

int
LayoutRegion::getAbsoluteLeft ()
{
  if (parent != NULL)
    {
      return getLeftInPixels () + parent->getAbsoluteLeft ();
    }
  else
    {
      return getLeftInPixels ();
    }
}

int
LayoutRegion::getAbsoluteTop ()
{
  if (parent != NULL)
    {
      return getTopInPixels () + parent->getAbsoluteTop ();
    }
  else
    {
      return getTopInPixels ();
    }
}

double
LayoutRegion::getPercentValue (const string &value)
{
  string actualValue;
  double floatValue;

  actualValue = value.substr (0, value.length () - 1);
  floatValue = xstrtod (actualValue);
  if (floatValue < 0)
    floatValue = 0;
  return floatValue;
}

void
LayoutRegion::lock ()
{
  pthread_mutex_lock (&mutex);
}

void
LayoutRegion::unlock ()
{
  pthread_mutex_unlock (&mutex);
}

GINGA_NCL_END
