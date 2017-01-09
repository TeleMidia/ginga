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

#include "gingancl/model/FormatterDeviceRegion.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace presentation {
	FormatterDeviceRegion::FormatterDeviceRegion(
		    string id) : LayoutRegion(id) {

		this->id = id;
		sortedRegions  = new vector<LayoutRegion*>;

		Thread::mutexInit(&rMutex, NULL);
		typeSet.insert("FormatterDeviceRegion");
	}

	void FormatterDeviceRegion::addRegion(LayoutRegion *region) {
		vector<LayoutRegion*>::iterator i;
		int zIndex;

		Thread::mutexLock(&rMutex);
		if (regionSet.count(region) == 0) {
			regionSet.insert(region);
			zIndex = region->getZIndexValue();

			i = sortedRegions->begin();
			while (i != sortedRegions->end()) {
				if (zIndex <= (*i)->getZIndexValue()) {
					break;
				}
				++i;
			}

			sortedRegions->insert(i, region);
			region->setParent(this);
		}
		Thread::mutexUnlock(&rMutex);
	}

	LayoutRegion *FormatterDeviceRegion::cloneRegion() {
		LayoutRegion *cloneRegion;
		vector<LayoutRegion*> *childRegions;

		cloneRegion = new FormatterDeviceRegion(id);

		cloneRegion->setTitle(getTitle());
		cloneRegion->setLeft(left, false);
		cloneRegion->setTop(top, false);
		cloneRegion->setWidth(width, false);
		cloneRegion->setHeight(height, false);
		cloneRegion->setDecorated(false);
		cloneRegion->setMovable(false);
		cloneRegion->setResizable(false);

		childRegions = getRegions();
		vector<LayoutRegion*>::iterator it;
		for (it = childRegions->begin(); it != childRegions->end(); ++it) {
			cloneRegion->addRegion(*it);
		}

		return cloneRegion;
	}

	int FormatterDeviceRegion::compareWidthSize(string w) {
		int newW;

		newW = atoi(w.c_str());
		if (newW == width) {
			return 0;

		} else if (newW > width) {
			return 1;

		} else {
			return -1;
		}
	}

	int FormatterDeviceRegion::compareHeightSize(string h) {
		int newH;

		newH = atoi(h.c_str());
		if (newH == height) {
			return 0;

		} else if (newH > height) {
			return 1;

		} else {
			return -1;
		}
	}

	short FormatterDeviceRegion::getBackgroundColor() {
		return -1;
	}

	double FormatterDeviceRegion::getBottom() {
		return NaN();
	}

	double FormatterDeviceRegion::getHeight() {
		return height;
	}

	double FormatterDeviceRegion::getLeft() {
		return left;
	}

	double FormatterDeviceRegion::getRight() {
		return NaN();
	}

	LayoutRegion* FormatterDeviceRegion::getRegion(string id) {
		int i, size;
		LayoutRegion *region;

		Thread::mutexLock(&rMutex);
		size = sortedRegions->size();
		for (i = 0; i < size; i++) {
			region = (LayoutRegion*)((*sortedRegions)[i]);
			if (region->getId() == id) {
				Thread::mutexUnlock(&rMutex);
				return region;
			}
		}

		Thread::mutexUnlock(&rMutex);
		return NULL;
	}

	LayoutRegion* FormatterDeviceRegion::getRegionRecursively(string id) {
		int i, size;
		LayoutRegion *region, *auxRegion;

		Thread::mutexLock(&rMutex);
		size = sortedRegions->size();
		for (i = 0; i < size; i++) {
			region = (LayoutRegion*)((*sortedRegions)[i]);
			if (region->getId() == id) {
				Thread::mutexUnlock(&rMutex);
				return region;
			}
			auxRegion = region->getRegionRecursively(id);
			if (auxRegion != NULL) {
				Thread::mutexUnlock(&rMutex);
				return auxRegion;
			}
		}

		Thread::mutexUnlock(&rMutex);
		return NULL;
	}

	vector<LayoutRegion*>* FormatterDeviceRegion::getRegions() {
		vector<LayoutRegion*>* regs;

		Thread::mutexLock(&rMutex);
		regs = new vector<LayoutRegion*>(*sortedRegions);
		Thread::mutexUnlock(&rMutex);

		return regs;
	}

	string FormatterDeviceRegion::getTitle() {
		return "";
	}

	double FormatterDeviceRegion::getTop() {
		return top;
	}

	double FormatterDeviceRegion::getWidth() {
		return width;
	}

	int FormatterDeviceRegion::getZIndex() {
		return -1;
	}

	int FormatterDeviceRegion::getZIndexValue() {
		int zIndex;

		zIndex = getZIndex();
		if (zIndex != -1) {
			return zIndex;

		} else {
			return 0;
		}
	}

	bool FormatterDeviceRegion::isBottomPercentual() {
		return false;
	}

	bool FormatterDeviceRegion::isHeightPercentual() {
		return false;
	}

	bool FormatterDeviceRegion::isLeftPercentual() {
		return false;
	}

	bool FormatterDeviceRegion::isRightPercentual() {
		return false;
	}

	bool FormatterDeviceRegion::isTopPercentual() {
		return false;
	}

	bool FormatterDeviceRegion::isWidthPercentual() {
		return false;
	}

	string FormatterDeviceRegion::toString() {
		string str;
		int i, size;
		LayoutRegion *region;

		str = "id: " + getId() + '\n';
		Thread::mutexLock(&rMutex);
		size = sortedRegions->size();
		Thread::mutexUnlock(&rMutex);
		for (i = 0; i < size; i++) {
			Thread::mutexLock(&rMutex);
			region = ((LayoutRegion*)(*sortedRegions)[i]);
			Thread::mutexUnlock(&rMutex);
			str = str + region->toString();
		}
		return str + '\n';
	}

	bool FormatterDeviceRegion::removeRegion(LayoutRegion *region) {
		vector<LayoutRegion*>::iterator it;

		Thread::mutexLock(&rMutex);
		if (regionSet.count(region) != 0) {
			for (it=sortedRegions->begin(); it!=sortedRegions->end(); ++it) {
				if (*it == region) {
					sortedRegions->erase(it);
					regionSet.erase(regionSet.find(region));
					Thread::mutexUnlock(&rMutex);
					return true;
				}
			}
		}

		Thread::mutexUnlock(&rMutex);
		return false;
	}

	void FormatterDeviceRegion::removeRegions() {
		Thread::mutexLock(&rMutex);
		sortedRegions->clear();
		regionSet.clear();
		Thread::mutexUnlock(&rMutex);
	}

	void FormatterDeviceRegion::setBackgroundColor(Color *newBackgroundColor) {

	}

	bool FormatterDeviceRegion::setBottom(double newBottom, bool isPercentual) {
		return false;
	}

	bool FormatterDeviceRegion::setHeight(double newHeight, bool isPercentual) {
		this->height = (int)newHeight;
		return true;
	}

	bool FormatterDeviceRegion::setLeft(double newLeft, bool isPercentual) {
		this->left = (int)newLeft;
		return true;
	}

	bool FormatterDeviceRegion::setRight(double newRight, bool isPercentual) {
		return false;
	}

	void FormatterDeviceRegion::setTitle(string newTitle) {

	}

	bool FormatterDeviceRegion::setTop(double newTop, bool isPercentual) {
		this->top = (int)newTop;
		return true;
	}

	bool FormatterDeviceRegion::setWidth(double newWidth, bool isPercentual) {
		this->width = (int)newWidth;
		return true;
	}

	void FormatterDeviceRegion::setZIndex(int newZIndex) {

	}

	vector<LayoutRegion*> *FormatterDeviceRegion::getRegionsSortedByZIndex() {
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

		return LayoutRegion::getRegionsSortedByZIndex();
	}

	vector<LayoutRegion*>* FormatterDeviceRegion::getRegionsOverRegion(
		    LayoutRegion* region) {

		vector<LayoutRegion*>* frontRegions;
		vector<LayoutRegion*>::iterator it;
		LayoutRegion* childRegion;
		int childZIndex, regionZIndex;

		regionZIndex = region->getZIndexValue();
		frontRegions = new vector<LayoutRegion*>;

		Thread::mutexLock(&rMutex);
		for (it = sortedRegions->begin(); it != sortedRegions->end(); ++it) {
			childRegion = *it;
			childZIndex = childRegion->getZIndexValue();
			if (childZIndex > regionZIndex &&
					region->intersects(childRegion)) {

				frontRegions->insert(frontRegions->begin(), childRegion);
			}
		}
		Thread::mutexUnlock(&rMutex);

		return frontRegions;
	}

	LayoutRegion* FormatterDeviceRegion::getParent() {
		return NULL;
	}

	void FormatterDeviceRegion::setParent(LayoutRegion *parent) {

	}

	int FormatterDeviceRegion::getTopInPixels() {
		return top;
	}

	int FormatterDeviceRegion::getBottomInPixels() {
		return top + height;
	}

	int FormatterDeviceRegion::getRightInPixels() {
		return left + width;
	}

	int FormatterDeviceRegion::getLeftInPixels() {
		return left;
	}

	int FormatterDeviceRegion::getHeightInPixels() {
		return height;
	}

	int FormatterDeviceRegion::getWidthInPixels() {
		return width;
	}

	bool FormatterDeviceRegion::isMovable() {
		return false;
	}

	bool FormatterDeviceRegion::isResizable() {
		return false;
	}

	bool FormatterDeviceRegion::isDecorated() {
		return false;
	}

	void FormatterDeviceRegion::setMovable(bool movable) {

	}

	void FormatterDeviceRegion::setResizable(bool resizable) {

	}

	void FormatterDeviceRegion::setDecorated(bool decorated) {

	}

	void FormatterDeviceRegion::resetTop() {

	}

	void FormatterDeviceRegion::resetBottom() {

	}

	void FormatterDeviceRegion::resetRight() {

	}

	void FormatterDeviceRegion::resetLeft() {

	}

	void FormatterDeviceRegion::resetHeight() {

	}

	void FormatterDeviceRegion::resetWidth() {

	}

	void FormatterDeviceRegion::resetZIndex() {

	}

	void FormatterDeviceRegion::resetDecorated() {

	}

	void FormatterDeviceRegion::resetMovable() {

	}

	void FormatterDeviceRegion::resetResizable() {

	}

	int FormatterDeviceRegion::getAbsoluteLeft() {
		return left;
	}

	int FormatterDeviceRegion::getAbsoluteTop() {
		return top;
	}

	void FormatterDeviceRegion::dispose() {
		removeRegions();
		//sortedRegions = NULL;
	}

	string FormatterDeviceRegion::getId() {
		return id;
	}

	void FormatterDeviceRegion::setId(string id) {
		this->id = id;
	}

	Entity *FormatterDeviceRegion::getDataEntity() {
		return NULL;
	}

	int FormatterDeviceRegion::compareTo(void *arg0) {
		return 0;
	}
}
}
}
}
}
}
}
