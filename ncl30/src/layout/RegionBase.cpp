/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

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

#include "ncl/layout/RegionBase.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace layout {
	RegionBase::RegionBase(string id, IDeviceLayout* deviceLayout) : Base(id) {
		this->deviceLayout      = deviceLayout;
		this->device            = "";
		this->deviceClass       = -1;
		this->outputMapRegionId = "";
		this->deviceRegion      = NULL;

		typeSet.insert("RegionBase");
	}

	RegionBase::~RegionBase() {
		map<string, LayoutRegion*>::iterator i;

		regions.clear();

		if (deviceRegion != NULL) {
			delete deviceRegion;
			deviceRegion = NULL;
		}

		if (deviceLayout != NULL) {
			// deviceLayout is deleted outside ncl30 scope
			deviceLayout = NULL;
		}
	}

	void RegionBase::createDeviceRegion() {
		IDeviceProperty* deviceProperty;

		deviceRegion = new LayoutRegion(device);

		deviceProperty = deviceLayout->getDeviceProperty(device);
		if (deviceProperty != NULL) {
			deviceRegion->setLeft(
					(double)(deviceProperty->getDeviceLeft()), false);

			deviceRegion->setTop(
					(double)(deviceProperty->getDeviceTop()), false);

			deviceRegion->setWidth(
					(double)(deviceProperty->getDeviceWidth()), false);

			deviceRegion->setHeight(
					(double)(deviceProperty->getDeviceHeight()), false);

		} else {
			clog << "RegionBase::createDeviceRegion '" << deviceClass << "'";
			clog << " Warning! Can't find device properties" << endl;

			deviceRegion->setTop(0, false);
			deviceRegion->setLeft(0, false);
			deviceRegion->setWidth(800, false);
			deviceRegion->setHeight(600, false);
		}
/*
		clog << endl << endl;
		clog << "RegionBase::createDeviceRegion '" << deviceClass << "'";
		clog << " and mapId '" << outputMapRegionId << "'" << endl;
		clog << endl << endl;*/

		deviceRegion->setDeviceClass(deviceClass, outputMapRegionId);
	}

	bool RegionBase::addRegion(LayoutRegion* region) {
		map<string, LayoutRegion*>::iterator i;
		string regId;

		regId = region->getId();
		/*clog << "RegionBase::addRegion Add region '" << regId;
		clog << "'" << endl;*/

		i = regions.find(regId);
		if (i != regions.end()) {
			clog << "RegionBase::addRegion Warning! Trying to add a region ";
			clog << "with id '" << regId << "' twice" << endl;
			return false;

		} else {
			deviceRegion->addRegion(region);
			region->setParent(deviceRegion);
			regions[regId] = region;
			return true;
		}
	}

	bool RegionBase::addBase(Base* base, string alias, string location)
		     throw(IllegalBaseTypeException*) {

		if (base->instanceOf("RegionBase")) {
			return Base::addBase(base, alias, location);
		}
		return false;
	}

	LayoutRegion* RegionBase::getRegionLocally(string regionId) {
		map<string, LayoutRegion*>::iterator childRegions;
		LayoutRegion *region;
		LayoutRegion *auxRegion;

		childRegions = regions.begin();
		while (childRegions != regions.end()) {
			region = childRegions->second;
			if (region->getId() == regionId) {
				return region;
			}
			auxRegion = region->getRegionRecursively(regionId);
			if (auxRegion != NULL) {
				return auxRegion;
			}
			++childRegions;
		}
		return NULL;
	}

	string RegionBase::getOutputMapRegionId() {
		return outputMapRegionId;
	}

	void RegionBase::setOutputMapRegion(LayoutRegion* outputMapRegion) {
		map<string, LayoutRegion*>::iterator i;

		for (i = regions.begin(); i != regions.end(); ++i) {
			i->second->setOutputMapRegion(outputMapRegion);
		}
	}

	LayoutRegion* RegionBase::getRegion(string regionId) {
		string::size_type index;
		string prefix, suffix;
		RegionBase* base;

		index = regionId.find_first_of("#");
		if (index == string::npos) {
			return getRegionLocally(regionId);
		}
		prefix = regionId.substr(0, index);
		index++;
		suffix = regionId.substr(index, regionId.length() - index);
		if (baseAliases.find(prefix) != baseAliases.end()) {
			base = (RegionBase*)(baseAliases[prefix]);
			return base->getRegion(suffix);

		} else if (baseLocations.find(prefix) != baseLocations.end()) {
			base = (RegionBase*)(baseLocations[prefix]);
			return base->getRegion(suffix);

		} else {
			return NULL;
		}
	}

	vector<LayoutRegion*>* RegionBase::getRegions() {
		map<string, LayoutRegion*>::iterator i;
		vector<LayoutRegion*>* childRegions;

		childRegions = new vector<LayoutRegion*>;
		for (i = regions.begin(); i != regions.end(); ++i) {
			childRegions->push_back(i->second);
		}

		return childRegions;
	}

	bool RegionBase::removeRegion(LayoutRegion* region) {
		map<string, LayoutRegion*>::iterator i;

		for (i = regions.begin(); i != regions.end(); ++i) {
			if (i->second == region) {
				regions.erase(i);
				return true;
			}
		}
		return false;
	}

	string RegionBase::getDevice() {
		return device;
	}

	void RegionBase::setDevice(string device, string regionId) {
		this->device = device;

		if (deviceRegion != NULL) {
			delete deviceRegion;
			deviceRegion = NULL;
		}

		if (device == "systemScreen(0)") {
			deviceClass = 0;

		} else if (device == "systemScreen(1)") {
			deviceClass = 1;

		} else if (device == "systemScreen(2)") {
			deviceClass = 2;
		}

		outputMapRegionId = regionId;
		createDeviceRegion();
	}

	int RegionBase::getDeviceClass() {
		return deviceClass;
	}

	void RegionBase::clear() {
		regions.clear();
		Base::clear();
	}
}
}
}
}
}
