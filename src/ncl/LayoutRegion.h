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

#ifndef _LAYOUTREGION_H_
#define _LAYOUTREGION_H_

#include "Entity.h"

GINGA_NCL_BEGIN

class LayoutRegion : public Entity
{
protected:
  string title;
  string outputMapRegionId;
  LayoutRegion *outputMapRegion;

  double left;
  bool leftPercent;
  double top;
  bool topPercent;
  double right;
  bool rightPercent;
  double bottom;
  bool bottomPercent;
  double width;
  bool widthPercent;
  double height;
  bool heightPercent;

  double *zIndex;
  map<string, LayoutRegion *> regions;
  vector<LayoutRegion *> sorted;
  LayoutRegion *parent;
  bool movable;
  bool resizable;
  bool decorated;
  int devClass;
  pthread_mutex_t mutex;

public:
  LayoutRegion (const string &id);
  virtual ~LayoutRegion ();
  virtual void setOutputMapRegion (LayoutRegion *outMapRegion);
  virtual LayoutRegion *getOutputMapRegion ();
  virtual string getOutputMapRegionId ();
  virtual void setDeviceClass (int deviceClass, const string &mapId);
  virtual int getDeviceClass ();
  virtual void addRegion (LayoutRegion *region);
  virtual LayoutRegion *cloneRegion ();

  virtual LayoutRegion *copyRegion ();

  virtual int compareWidthSize (const string &w);
  virtual int compareHeightSize (const string &h);
  virtual double getBottom ();
  virtual double getHeight ();
  virtual double getLeft ();
  virtual double getRight ();
  virtual LayoutRegion *getRegion (const string &id);
  virtual LayoutRegion *getRegionRecursively (const string &id);
  void printRegionIdsRecursively ();
  virtual vector<LayoutRegion *> *getRegions ();
  virtual string getTitle ();
  virtual double getTop ();
  virtual double getWidth ();
  virtual int getZIndex ();
  virtual int getZIndexValue ();
  virtual bool isBottomPercent ();
  virtual bool isHeightPercent ();
  virtual bool isLeftPercent ();
  virtual bool isRightPercent ();
  virtual bool isTopPercent ();
  virtual bool isWidthPercent ();
  virtual string toString ();

private:
  virtual bool removeRegion (LayoutRegion *region);

public:
  virtual void removeRegions ();

private:
  virtual LayoutRegion *getDeviceLayout ();
  virtual double getDeviceWidthInPixels ();
  virtual double getDeviceHeightInPixels ();

public:
  virtual bool setBottom (double newBottom, bool isPercent);
  virtual bool setTargetBottom (double newBottom, bool isPercent);
  virtual bool setHeight (double newHeight, bool isPercent);
  virtual bool setTargetHeight (double newHeight, bool isPercent);
  virtual bool setLeft (double newLeft, bool isPercent);
  virtual bool setTargetLeft (double newLeft, bool isPercent);
  virtual bool setRight (double newRight, bool isPercent);
  virtual bool setTargetRight (double newRight, bool isPercent);
  virtual bool setTop (double newTop, bool isPercent);
  virtual bool setTargetTop (double newTop, bool isPercent);
  virtual bool setWidth (double newWidth, bool isPercent);
  virtual bool setTargetWidth (double newWidth, bool isPercent);

  virtual void validateTarget ();

  virtual void setTitle (const string &newTitle);
  virtual void setZIndex (int newZIndex);
  virtual vector<LayoutRegion *> *getRegionsSortedByZIndex ();
  virtual vector<LayoutRegion *> *
  getRegionsOverRegion (LayoutRegion *region);

  virtual LayoutRegion *getParent ();
  virtual void setParent (LayoutRegion *parent);
  virtual void refreshDeviceClassRegions ();
  virtual int getTopInPixels ();
  virtual int getBottomInPixels ();
  virtual int getRightInPixels ();
  virtual int getLeftInPixels ();
  virtual int getHeightInPixels ();
  virtual int getWidthInPixels ();
  virtual bool isMovable ();
  virtual bool isResizable ();
  virtual bool isDecorated ();
  virtual void setMovable (bool movable);
  virtual void setResizable (bool resizable);
  virtual void setDecorated (bool decorated);
  virtual void resetTop ();
  virtual void resetBottom ();
  virtual void resetRight ();
  virtual void resetLeft ();
  virtual void resetHeight ();
  virtual void resetWidth ();
  virtual void resetZIndex ();
  virtual void resetDecorated ();
  virtual void resetMovable ();
  virtual void resetResizable ();
  virtual int getAbsoluteLeft ();
  virtual int getAbsoluteTop ();
  virtual bool intersects (LayoutRegion *r);
  virtual bool intersects (int x, int y);

private:
  double getPercentValue (const string &value);
  bool isPercentValue (const string &value);
  void lock ();
  void unlock ();
};

GINGA_NCL_END

#endif //_LAYOUTREGION_H_
