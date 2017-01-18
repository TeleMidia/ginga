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

#ifndef _ANIMATIONCONTROLLER_H_
#define _ANIMATIONCONTROLLER_H_

#include "model/AttributionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "ncl/Animation.h"
using namespace ::ginga::ncl;

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "adapters/FormatterPlayerAdapter.h"
#include "adapters/PlayerAdapterManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include "model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ANIMATION_BEGIN

	class AnimationController : public Thread {
		private:
			PlayerAdapterManager* pManager;
			FormatterPlayerAdapter* player; //useful if media is playing
			ExecutionObject* execObj; 		//useful if the object isn't playing
											//or it's a Ginga Settings Node.

			AttributionEvent* event; 		//attribution event

			//useful when the animation is over a region
			LayoutRegion* initRegion;
			LayoutRegion* targetRegion;

			string propName;			//name of the property (can be a group
										//of properties like bounds, location,
										//etc.)

			bool isExecObjAnim;

			//animation parameters
			vector <string>* params;	//parameters passe by the author
			vector <string> propertySingleNames;
			vector <double> initValues;
			vector <string> strTargetValues;
			vector <double> targetValues;

			double initTime;
			double duration;

			int stepSize; //this would be a vector

		private:
			AnimationController(
					ExecutionObject* execObj,
					PlayerAdapterManager* pManager,
					FormatterPlayerAdapter* player,
					AttributionEvent* event,
				    string value,
				    Animation* anim);

			virtual ~AnimationController();

		public:
			static void startAnimation(
					ExecutionObject* execObj,
					PlayerAdapterManager* pManager,
					FormatterPlayerAdapter* player,
					AttributionEvent* event,
				    string value,
				    Animation* anim);

		private:
			string previousValues;

			/**
			 * This method will execute the animation.
			 *
			 * WARNING: this method calls "delete this" when the animation is
			 * done. So, the user of this class should never call delete to the
			 * pointer that creates the animation. Additionally, this object
			 * always has to be instantiate as a pointer, never as a variable
			 * (when the destructor is called automatically when the scope
			 * finishes).
			 */
			void run();

			/**
			 * Load the current values from the properties will be animated in
			 * the object to the initValues vector.
			 */
			bool loadInitValues();

			/**
			 * Based on initValues and the parameters passed by the author
			 * this function calculate and load the target values with the
			 * final value for each property that must be changed
			 */
			bool loadTargetValues();

			bool animeStep();

			/**
			 * Return the value of the the target from a single property
			 */
			double getSinglePropertyTarget(int i);

			/**
			 * The targetRegion variable is used to calculate the value of
			 * dimension and position the region will be when the
			 * animation ends. First of all, this targetRegion will be a copy
			 * of the initial region, so this function is responsible to, based
			 * on initial region and the values in targetValues vector,
			 * update the targetRegion to have the final parameters.
			 */
			bool updateTargetRegion();

			/**
			 * The property can be a single name that groups a lot of single
			 * properties like location, size, bound, etc. So, this function
			 * is responsible to separate this property name and the parameters
			 * passed by the user in single property names loading the vectors:
			 * propertySingleNames and strTargetValues
			 */
			bool ungroupProperty();

			bool isExecutionObjectProperty(string name);

	};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ANIMATION_END
#endif // _ANIMATIONCONTROLLER_H_
