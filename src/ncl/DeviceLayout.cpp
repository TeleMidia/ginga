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

#include "config.h"
#include "ncl/DeviceLayout.h"

GINGA_NCL_BEGIN

	DeviceLayout::DeviceLayout(string layoutName) {
		devices          = new map<string, IDeviceProperty*>;
		this->layoutName = layoutName;
	}

	DeviceLayout::~DeviceLayout() {
		map<string, IDeviceProperty*>::iterator i;

		i = devices->begin();
		while (i != devices->end()) {
			delete i->second;
			++i;
		}
		delete devices;
	}

	string DeviceLayout::getLayoutName() {
		return layoutName;
	}

	void DeviceLayout::addDevice(
			string name, int x, int y, int width, int height) {

		IDeviceProperty* device;

		if (devices->count(name) == 0) {
			device = new DeviceProperty(x, y, width, height);
			(*devices)[name] = device;

		} else {
			device = (*devices)[name];

			device->setDeviceLeft(x);
			device->setDeviceTop(y);
			device->setDeviceWidth(width);
			device->setDeviceHeight(height);
		}
	}

	IDeviceProperty* DeviceLayout::getDeviceProperty(string name) {
		if (devices->count(name) != 0) {
			return (*devices)[name];

		} else {
			return NULL;
		}
	}

GINGA_NCL_END
