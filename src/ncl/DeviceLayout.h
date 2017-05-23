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

#ifndef DEVICELAYOUT_H_
#define DEVICELAYOUT_H_

GINGA_NCL_BEGIN

class DeviceProperty
{
private:
  int width;
  int height;

public:
  DeviceProperty (int w, int h)
  {
    this->width = w;
    this->height = h;
  }
  virtual ~DeviceProperty () {}

public:

  void
  setDeviceWidth (int width)
  {
    this->width = width;
  }

  int
  getDeviceWidth ()
  {
    return this->width;
  }

  void
  setDeviceHeight (int height)
  {
    this->height = height;
  }

  int
  getDeviceHeight ()
  {
    return this->height;
  }
};

class DeviceLayout
{
private:
  map<string, DeviceProperty *> *devices;
  string layoutName;

public:
  DeviceLayout (const string &layoutName);
  virtual ~DeviceLayout ();
  string getLayoutName ();
  void addDevice (const string &name, int width, int height);
  DeviceProperty *getDeviceProperty (const string &name);
};

GINGA_NCL_END

#endif /*DEVICELAYOUT_H_*/
