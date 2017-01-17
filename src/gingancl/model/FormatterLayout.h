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

#ifndef FORMATTERLAYOUT_H_
#define FORMATTERLAYOUT_H_

#include "mb/SDLSurface.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "ncl/layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "CascadingDescriptor.h"
#include "ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "FormatterRegion.h"
#include "FormatterDeviceRegion.h"

#include <map>
#include <string>
#include <set>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_PRESENTATION_BEGIN

  class FormatterLayout {
	protected:
		set<string> typeSet;

	private:
		GingaScreenID myScreen;
		map<string, FormatterLayout*> flChilds;
		LayoutRegion* deviceRegion;
		vector<string> sortedRegions;
		map<string, float> regionZIndex;
		map<string, set<FormatterRegion*>*> regionMap;
		map<FormatterRegion*, ExecutionObject*> objectMap;
		pthread_mutex_t mutex;

	public:
		FormatterLayout(GingaScreenID screenId, int x, int y, int w, int h);
		virtual ~FormatterLayout();

		GingaScreenID getScreenID();
		LayoutRegion* getDeviceRegion();
		void addChild(string objectId, FormatterLayout* child);
		FormatterLayout* getChild(string objectId);
		string getBitMapRegionId();
		LayoutRegion* getNcmRegion(string regionId);

	private:
		void printRegionMap();
		void printObjectMap();

	public:
		void getSortedIds(vector<GingaWindowID>* sortedIds);

	private:
		void createDeviceRegion(int x, int y, int w, int h);

	public:
		ExecutionObject* getObject(int x, int y);
		GingaWindowID prepareFormatterRegion(
				ExecutionObject* object,
				GingaSurfaceID renderedSurface,
				string plan);

		float refreshZIndex(
				FormatterRegion* region,
				string layoutRegionId,
				int zIndex,
				string plan,
				GingaSurfaceID renderedSurface);

	private:
		void sortRegion(string regionId, float cvtIndex, string plan);
		float convertZIndex(int zIndex, string plan);

	public:
		void showObject(ExecutionObject* object);
		void hideObject(ExecutionObject* object);

	private:
		GingaWindowID addRegionOnMaps(
				ExecutionObject* object, FormatterRegion* region,
				GingaSurfaceID renderedSurface,
				string layoutRegionId, int zIndex, string plan,
				float* cvtZIndex);

		ExecutionObject* removeFormatterRegionFromMaps(
				string layoutRegionId, FormatterRegion* formatterRegion);

	public:
		set<FormatterRegion*>* getFormatterRegionsFromNcmRegion(
			    string regionId);

		void clear();

	private:
		void lock();
		void unlock();
   };

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_PRESENTATION_END
#endif /*FORMATTERLAYOUT_H_*/
