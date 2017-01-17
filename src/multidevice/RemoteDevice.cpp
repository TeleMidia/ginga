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
#include "multidevice/RemoteDevice.h"

GINGA_MULTIDEVICE_BEGIN

	RemoteDevice::RemoteDevice(unsigned int address, int deviceClass) {
		this->address = address;
		this->deviceClass = deviceClass;
		width  = 0;
		height = 0;
	}

	RemoteDevice::~RemoteDevice() {

	}

	unsigned int RemoteDevice::getDeviceAddress() {
		return address;
	}

	int RemoteDevice::getDeviceClass() {
		return deviceClass;
	}

	void RemoteDevice::setDeviceResolution(int width, int height) {
		this->width = width;
		this->height = height;
	}

	void RemoteDevice::getDeviceResolution(int* width, int* height) {
		*width = this->width;
		*height = this->height;
	}

GINGA_MULTIDEVICE_END
