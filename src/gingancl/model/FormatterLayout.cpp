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

#include "FormatterLayout.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace presentation {
	FormatterLayout::FormatterLayout(
			GingaScreenID screenId, int x, int y, int w, int h) {

		myScreen      = screenId;
		deviceRegion  = NULL;

		typeSet.insert("FormatterLayout");
		Thread::mutexInit(&mutex, NULL);
		createDeviceRegion(x, y, w, h);
	}

	FormatterLayout::~FormatterLayout() {
		clear();
		lock();
		sortedRegions.clear();
		regionMap.clear();
		regionZIndex.clear();
		objectMap.clear();
		flChilds.clear();
		unlock();
	}

	GingaScreenID FormatterLayout::getScreenID() {
		return myScreen;
	}

	LayoutRegion* FormatterLayout::getDeviceRegion() {
		return deviceRegion;
	}

	void FormatterLayout::addChild(string objectId, FormatterLayout* child) {
		flChilds[objectId] = child;
	}

	FormatterLayout* FormatterLayout::getChild(string objectId) {
		map<string, FormatterLayout*>::iterator i;

		i = flChilds.find(objectId);
		if (i != flChilds.end()) {
			return i->second;
		}

		return NULL;
	}

	string FormatterLayout::getBitMapRegionId() {
		if (deviceRegion != NULL) {
			return deviceRegion->getOutputMapRegionId();
		}
		return "";
	}

	LayoutRegion* FormatterLayout::getNcmRegion(string regionId) {
		if (deviceRegion != NULL) {
			return deviceRegion->getRegionRecursively(regionId);

		} else {
			clog << "FormatterLayout::getNcmRegion device region is NULL";
			clog << endl;
		}
		return NULL;
	}

	void FormatterLayout::printRegionMap() {
		map<string, set<FormatterRegion*>*>::iterator i;

		cout << "FormatterLayout::printRegionMap: | '";

		i = regionMap.begin();
		while (i != regionMap.end()) {
			cout << i->first << "' ";
			++i;
		}
		cout << "|" << endl;
	}

	void FormatterLayout::printObjectMap() {
		map<FormatterRegion*, ExecutionObject*>::iterator i;

		cout << "FormatterLayout::printObjectMap: | '";

		i = objectMap.begin();
		while (i != objectMap.end()) {
			cout << i->second->getId() << "' ";
			++i;
		}
		cout << "|" << endl;
	}

	void FormatterLayout::createDeviceRegion(int x, int y, int w, int h) {
		deviceRegion = new FormatterDeviceRegion("defaultScreenFormatter");
		deviceRegion->setLeft(x, false);
		deviceRegion->setTop(y, false);
		deviceRegion->setWidth(w, false);
		deviceRegion->setHeight(h, false);
	}

	ExecutionObject* FormatterLayout::getObject(int x, int y) {
		map<string, set<FormatterRegion*>*>::iterator i;

		FormatterRegion* formRegion = NULL;
		set<FormatterRegion*>* formRegions = NULL;
		set<FormatterRegion*>::iterator j;

		map<FormatterRegion*, ExecutionObject*>::iterator k;

		LayoutRegion* currentRegion = NULL;
		LayoutRegion* region        = NULL;
		ExecutionObject* object     = NULL;

		lock();
		i = regionMap.begin();
		while (i != regionMap.end()) {
			formRegions = i->second;
			j = formRegions->begin();
			while (j != formRegions->end()) {
				formRegion = *j;
				k = objectMap.find(formRegion);
				if (formRegion->intersects(x, y) && k != objectMap.end()) {
					if (object == NULL) {
						region = k->first->getLayoutRegion();
						object = k->second;

					} else {
						currentRegion = formRegion->getLayoutRegion();
						if (currentRegion != NULL && region != NULL) {
							if (currentRegion->getZIndex() >
									region->getZIndex()) {

								region = currentRegion;
								object = k->second;
							}
						}
					}
				}
				++j;
			}
			++i;
		}
		unlock();
		return object;
	}

	/*bool FormatterLayout::getScreenShot(IWindow* region) {
		map<string, set<FormatterRegion*>*>::iterator i;
		set<FormatterRegion*>::iterator j;
		vector<string>::iterator k;
		map<FormatterRegion*, ExecutionObject*>::iterator l;
		map<string, FormatterLayout*>::iterator m;

		set<FormatterRegion*>* formRegions = NULL;
		FormatterRegion* formatterRegion;
		IWindow* formWindow = NULL;

		lock();
		k = sortedRegions.begin();
		while (k != sortedRegions.end()) {
			i = regionMap.find(*k);
			if (i != regionMap.end()) {
				formRegions = i->second;
				j = formRegions->begin();
				while (j != formRegions->end()) {
					formatterRegion = (*j);
					formWindow = formatterRegion->getODContentPane();
					if (formWindow != NULL) {
						if (childs != NULL) {
							l = objectMap.find(formatterRegion);
							if (l != objectMap.end()) {
								m = childs->find(l->second->getId());
								if (m != childs->end()) {
									m->second->getScreenShot(formWindow);
									formWindow->validate();
									unlock();
									return false;
								}
							}
						}
						region->blit(formWindow);
					}
					++j;
				}
			}
			++k;
		}

		unlock();
		return true;
	}*/

	void FormatterLayout::getSortedIds(vector<GingaWindowID>* sortedIds) {
		map<string, set<FormatterRegion*>*>::iterator i;
		set<FormatterRegion*>::iterator j;
		vector<string>::iterator k;
		map<FormatterRegion*, ExecutionObject*>::iterator l;
		map<string, FormatterLayout*>::iterator m;

		set<FormatterRegion*>* formRegions = NULL;
		FormatterRegion* formatterRegion;
		GingaWindowID outputId;

		lock();
		k = sortedRegions.begin();
		while (k != sortedRegions.end()) {
			i = regionMap.find(*k);
			if (i != regionMap.end()) {
				formRegions = i->second;
				j = formRegions->begin();
				while (j != formRegions->end()) {
					formatterRegion = (*j);
					outputId = formatterRegion->getOutputId();
					if (outputId != 0) {
						sortedIds->push_back(outputId);
						l = objectMap.find(formatterRegion);
						if (l != objectMap.end()) {
							m = flChilds.find(l->second->getId());
							if (m != flChilds.end()) {
								m->second->getSortedIds(sortedIds);
							}
						}
					}
					++j;
				}
			}
			++k;
		}
		unlock();
	}

	GingaWindowID FormatterLayout::prepareFormatterRegion(
			ExecutionObject* object,
			GingaSurfaceID renderedSurface,
			string plan) {

		CascadingDescriptor* descriptor;
		FormatterRegion* region;

		LayoutRegion *layoutRegion, *parent, *grandParent;
		string regionId, mapId;

		vector<LayoutRegion*>* childs;
		vector<LayoutRegion*>::iterator i;

		int devClass, zIndex;
		float cvtZIndex;
		GingaWindowID windowId = 0;

		if (object == NULL || object->getDescriptor() == NULL ||
			    object->getDescriptor()->getFormatterRegion() == NULL) {

			clog << "FormatterLayout::prepareFormatterRegion returns -1";
			clog << endl;
			return windowId;
		}

		clog << "FormatterLayout::prepareFormatterRegion for '";
		clog << object->getId() << "'";
		clog << endl;

		descriptor   = object->getDescriptor();
		region       = descriptor->getFormatterRegion();
		layoutRegion = region->getOriginalRegion();

		//every presented object has a region root
		//the formatter device region
		parent = layoutRegion;
		grandParent = layoutRegion->getParent();
		if (grandParent != NULL) {
			while (grandParent->getParent() != NULL) {
				parent = grandParent;
				grandParent = grandParent->getParent();
			}
		}

		if (grandParent != deviceRegion && grandParent != NULL) {
			childs = grandParent->getRegions();
			i = childs->begin();
			while (i != childs->end()) {
				deviceRegion->addRegion(*i);
				(*i)->setParent(deviceRegion);
				++i;
			}
			delete childs;

			devClass = grandParent->getDeviceClass();
			if (devClass >= 0) {
				mapId = grandParent->getOutputMapRegionId();
				deviceRegion->setDeviceClass(devClass, mapId);
			}

			deviceRegion->addRegion(parent);
			parent->setParent(deviceRegion);
		}

		regionId = layoutRegion->getId();
		zIndex   = layoutRegion->getZIndex();

		windowId = addRegionOnMaps(
				object, region, renderedSurface, regionId, zIndex, plan,
				&cvtZIndex);

		clog << "FormatterLayout::prepareFormatterRegion deviceRegion class ";
		clog << "is '" << deviceRegion->getDeviceClass() << "'";
		clog << endl;

		return windowId;
	}

	float FormatterLayout::refreshZIndex(
			FormatterRegion* region,
			string layoutRegionId,
			int zIndex,
			string plan,
			GingaSurfaceID renderedSurface) {

		ExecutionObject* object;
		float cvtZIndex;

		object = removeFormatterRegionFromMaps(layoutRegionId, region);

		addRegionOnMaps(
				object,
				region,
				renderedSurface,
				layoutRegionId,
				zIndex,
				plan,
				&cvtZIndex);

		return cvtZIndex;
	}

	void FormatterLayout::sortRegion(
			string regionId, float cvtIndex, string plan) {

		vector<string>::iterator i;
		map<string, float>::iterator j;

		i = sortedRegions.begin();
		while (i != sortedRegions.end()) {
			j = regionZIndex.find(*i);
			if (j != regionZIndex.end()) {
				if (cvtIndex <= j->second) {
					break;
				}
			}
			++i;
		}

		sortedRegions.insert(i, regionId);
	}

	float FormatterLayout::convertZIndex(int zIndex, string plan) {
		float planPower = 2.0;

		if (plan == "background") {
			planPower = 0.0;

		} else if (plan == "video") {
			planPower = 1.0;
		}

		if (zIndex > 256) {
			zIndex = 256;
		}

		return (planPower + ((float)zIndex / 1000));
	}

	void FormatterLayout::showObject(ExecutionObject* object) {
		FormatterRegion* region;
		map<int, set<string>*>::iterator i;

		if (object == NULL || object->getDescriptor() == NULL ||
			    object->getDescriptor()->getFormatterRegion() == NULL) {

			return;
		}

		region = object->getDescriptor()->getFormatterRegion();
		region->showContent();
	}

	void FormatterLayout::hideObject(ExecutionObject* object) {
		FormatterRegion* region = NULL;
		LayoutRegion* layoutRegion;
		CascadingDescriptor* descriptor;
		string regionId;

		descriptor = object->getDescriptor();
		if (descriptor != NULL) {
			region = object->getDescriptor()->getFormatterRegion();
		}

		if (region == NULL) {
			if (descriptor != NULL) {
				descriptor->setFormatterLayout(this);
			}
			return;
		}

		region->hideContent();
		layoutRegion = region->getLayoutRegion();
		regionId = layoutRegion->getId();

		//removeFormatterRegionFromMaps(regionId, region);
	}

	GingaWindowID FormatterLayout::addRegionOnMaps(
			ExecutionObject* object, FormatterRegion* region,
			GingaSurfaceID renderedSurface,
			string layoutRegionId, int zIndex, string plan, float* cvtZIndex) {

		float convertedZIndex;
		set<FormatterRegion*>* formRegions;
		GingaWindowID windowId;

		lock();
		convertedZIndex = convertZIndex(zIndex, plan);
		if (regionMap.count(layoutRegionId) == 0) {
			formRegions                  = new set<FormatterRegion*>;
			regionMap[layoutRegionId]    = formRegions;
			regionZIndex[layoutRegionId] = convertedZIndex;

		} else {
			formRegions = regionMap[layoutRegionId];
		}

		formRegions->insert(region);
		windowId = region->getOutputId();
		if (windowId == 0) {
			windowId = region->prepareOutputDisplay(
					renderedSurface, convertedZIndex);
		}
		sortRegion(layoutRegionId, convertedZIndex, plan);

		objectMap[region] = object;

		*cvtZIndex = convertedZIndex;
		unlock();

		return windowId;
	}

	ExecutionObject* FormatterLayout::removeFormatterRegionFromMaps(
			string layoutRegionId, FormatterRegion* formatterRegion) {

		set<FormatterRegion*>* formRegions;
		set<FormatterRegion*>::iterator i;
		map<string, set<FormatterRegion*>*>::iterator j;
		map<FormatterRegion*, ExecutionObject*>::iterator k;
		map<string, float>::iterator l;
		vector<string>::iterator m;

		ExecutionObject* myObject = NULL;

		lock();
		if (regionMap.count(layoutRegionId) != 0) {
			formRegions = regionMap[layoutRegionId];
			i = formRegions->find(formatterRegion);
			if (i != formRegions->end()) {
				formRegions->erase(i);
				if (formRegions->empty()) {
					j = regionMap.find(layoutRegionId);
					if (j != regionMap.end()) {
						regionMap.erase(j);
						delete formRegions;

						l = regionZIndex.find(layoutRegionId);
						if (l != regionZIndex.end()) {
							regionZIndex.erase(l);
						}

						m = sortedRegions.begin();
						while (m != sortedRegions.end()) {
							if (layoutRegionId == *m) {
								m = sortedRegions.erase(m);

							} else {
								++m;
							}
						}
					}
				}

				k = objectMap.find(formatterRegion);
				if (k != objectMap.end()) {
					myObject = k->second;
					objectMap.erase(k);
				}
			}
		}
		unlock();

		return myObject;
	}

	set<FormatterRegion*>* FormatterLayout::
		    getFormatterRegionsFromNcmRegion(string regionId) {

		set<FormatterRegion*>* regions;

		lock();
		if (regionMap.count(regionId) == 0) {
			unlock();
			return NULL;
		}

		regions = new set<FormatterRegion*>(*(regionMap[regionId]));
		unlock();
		return regions;
	}

	void FormatterLayout::clear() {
		map<string, set<FormatterRegion*>*>::iterator i;
		set<FormatterRegion*>::iterator j;

		lock();
		i = regionMap.begin();
		while (i != regionMap.end()) {
			j = i->second->begin();
			while (j != i->second->end()) {
				delete *j;
				++j;
			}

			i->second->clear();
			delete i->second;

			++i;
		}

		regionMap.clear();

		regionZIndex.clear();
		objectMap.clear();
		sortedRegions.clear();
		unlock();

		deviceRegion->removeRegions();
	}

	void FormatterLayout::lock() {
		Thread::mutexLock(&mutex);
	}

	void FormatterLayout::unlock() {
		Thread::mutexUnlock(&mutex);
	}
}
}
}
}
}
}
}
