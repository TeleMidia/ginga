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

#include "mb/Display.h"
using namespace ginga::mb;

GINGA_PRAGMA_DIAG_IGNORE (-Wfloat-conversion)

GINGA_NCL_BEGIN

void
LayoutRegion::setRect (SDL_Rect rect)
{
  this->rect = rect;
}

SDL_Rect
LayoutRegion::getRect (void)
{
  return this->rect;
}

void
LayoutRegion::setZ (int z, int zorder)
{
  this->z = z;
  this->zorder = zorder;
}

void
LayoutRegion::getZ (int *z, int *zorder)
{
  set_if_nonnull (z, this->z);
  set_if_nonnull (zorder, this->zorder);
}

void
LayoutRegion::dump ()
{
  g_debug ("%s at (%d,%d) size %dx%d z %d,%d",
           this->getId ().c_str (), this->rect.x, this->rect.y,
           this->rect.w, this->rect.h, this->z, this->zorder);
}

LayoutRegion::LayoutRegion (const string &id) : Entity (id)
{
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

  parent = NULL;

  // this->rect.x = 0;
  // this->rect.y = 0;
  // Ginga_Display->getSize (&this->rect.w, &this->rect.h);
  // this->z = 0;
  // this->zorder = 0;
}

LayoutRegion::~LayoutRegion ()
{
  LayoutRegion *region;
  map<string, LayoutRegion *>::iterator i;

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

  if (zIndex != NULL)
    {
      delete zIndex;
      zIndex = NULL;
    }
}

void
LayoutRegion::addRegion (LayoutRegion *region)
{
  vector<LayoutRegion *>::iterator i;
  string regId;

  regId = region->getId ();

  if (regions.count (regId) != 0)
    {
      return;
    }

  regions[regId] = region;
  region->setParent (this);
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

  vector<LayoutRegion *>::iterator it;
  for (it = childRegions->begin (); it != childRegions->end (); ++it)
    {
      childRegion = (*it)->cloneRegion ();
      cloneRegion->addRegion (childRegion);
    }
  delete childRegions;
  childRegions = NULL;

  return cloneRegion;
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

  i = regions.find (id);
  if (i == regions.end ())
    {
      if (id == getId ())
        {
          return this;
        }

      return NULL;
    }

  someRegion = i->second;

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

vector<LayoutRegion *> *
LayoutRegion::getRegions ()
{
  map<string, LayoutRegion *>::iterator i;
  vector<LayoutRegion *> *childRegions;
  childRegions = new vector<LayoutRegion *>;
  for (i = regions.begin (); i != regions.end (); ++i)
    {
      childRegions->push_back (i->second);
    }
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

  if (regions.count (region->getId ()) != 0)
    {
      i = regions.find (region->getId ());
      childRegion = i->second;
      childRegion->setParent (NULL);
      regions.erase (i);
      return true;
    }
  return false;
}

void
LayoutRegion::removeRegions ()
{
  map<string, LayoutRegion *>::iterator i;
  LayoutRegion *region;

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


void
LayoutRegion::setZIndex (int newZIndex)
{
  if (zIndex == NULL)
    {
      zIndex = new double;
    }
  *zIndex = newZIndex;
}

LayoutRegion *
LayoutRegion::getParent ()
{
  return parent;
}

void
LayoutRegion::setParent (LayoutRegion *parent)
{
  this->parent = parent;
}

int
LayoutRegion::getTopInPixels ()
{
  return this->rect.y;

# if 0
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
#endif
}

int
LayoutRegion::getBottomInPixels ()
{
  return this->rect.y + this->rect.h;

#if 0
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
#endif
}

int
LayoutRegion::getRightInPixels ()
{
  return this->rect.x + this->rect.w;

#if 0
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
#endif
}

int
LayoutRegion::getLeftInPixels ()
{
  return this->rect.x;

#if 0
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
#endif
}

int
LayoutRegion::getHeightInPixels ()
{
  return this->rect.h;

#if 0
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
#endif
}

int
LayoutRegion::getWidthInPixels ()
{
  return this->rect.w;

#if 0
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
#endif
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
  return this->rect.x;

#if 0
  if (parent != NULL)
    {
      return getLeftInPixels () + parent->getAbsoluteLeft ();
    }
  else
    {
      return getLeftInPixels ();
    }
#endif
}

int
LayoutRegion::getAbsoluteTop ()
{
  return this->rect.y;

#if 0
  if (parent != NULL)
    {
      return getTopInPixels () + parent->getAbsoluteTop ();
    }
  else
    {
      return getTopInPixels ();
    }
#endif
}

GINGA_NCL_END
