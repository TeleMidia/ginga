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

#ifndef FORMATTERREGION_H_
#define FORMATTERREGION_H_

#include "util/Color.h"
#include "util/functions.h"
using namespace ::ginga::util;

#include "ncl/LayoutRegion.h"
using namespace ::ginga::ncl;

#include "ncl/Descriptor.h"
#include "ncl/DescriptorUtil.h"
using namespace ::ginga::ncl;

#include "ncl/Transition.h"
#include "ncl/TransitionUtil.h"
using namespace ::ginga::ncl;

#include "mb/InputManager.h"
#include "mb/LocalScreenManager.h"
#include "mb/SDLWindow.h"
using namespace ::ginga::mb;

#include "FocusSourceManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation::focus;

#include <vector>
#include <string>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_PRESENTATION_BEGIN

  class FormatterRegion {
	private:
		void* layoutManager; //FormatterLayout
		void* descriptor; //CascadingDescriptor

		string objectId;
		LayoutRegion* ncmRegion;
		LayoutRegion* originalRegion;
		bool externHandler;
		GingaWindowID outputDisplay;

		GingaSurfaceID renderedSurface;

		bool imVisible;

		short focusState;
		Color* focusBorderColor;
		int focusBorderWidth;
		string focusComponentSrc;
		Color* selBorderColor;
		string selComponentSrc;
		int selBorderWidth;

		int zIndex;

		Color* bgColor;
		float transparency;
		short fit;
		short scroll;
		Color* chromaKey;
		string transitionIn;
		string transitionOut;
		bool abortTransitionIn;
		bool abortTransitionOut;
		pthread_mutex_t mutex;
		pthread_mutex_t mutexT;
		pthread_mutex_t mutexFI;

		string focusIndex;
		string moveUp;
		string moveDown;
		string moveLeft;
		string moveRight;

		string plan;

	public:
		static const short UNSELECTED = 0;
		static const short FOCUSED = 1;
		static const short SELECTED = 2;

		FormatterRegion(
			    string objectId, void* descriptor, void* layoutManager);

		virtual ~FormatterRegion();

	private:
		void initializeNCMRegion();

	public:
		void setRenderedSurface(GingaSurfaceID iSur);

		void setZIndex(int zIndex);
		int getZIndex();

		void setPlan(string plan);
		string getPlan();

		void setFocusIndex(string focusIndex);
		string getFocusIndex();

		void setMoveUp(string moveUp);
		string getMoveUp();

		void setMoveDown(string moveDown);
		string getMoveDown();

		void setMoveLeft(string moveLeft);
		string getMoveLeft();

		void setMoveRight(string moveRight);
		string getMoveRight();

		void setFocusBorderColor(Color* focusBorderColor);
		Color* getFocusBorderColor();

		void setFocusBorderWidth(int focusBorderWidth);
		int getFocusBorderWidth();

		void setFocusComponentSrc(string focusComponentSrc);
		string getFocusComponentSrc();

		void setSelBorderColor(Color* selBorderColor);
		Color* getSelBorderColor();

		void setSelBorderWidth(int selBorderWidth);
		int getSelBorderWidth();

		void setSelComponentSrc(string selComponentSrc);
		string getSelComponentSrc();

		void setFocusInfo(
			    Color* focusBorderColor,
			    int focusBorderWidth,
			    string focusComponentSrc,
			    Color* selBorderColor,
			    int selBorderWidth,
			    string selComponentSrc);

		void* getLayoutManager();
		GingaWindowID getOutputId();

	private:
		void meetComponent(
			    int width,
			    int height,
			    int prefWidth,
			    int prefHeight,
			    GingaSurfaceID component);

		void sliceComponent(
			    int width,
			    int height,
			    int prefWidth,
			    int prefHeight,
			    GingaSurfaceID component);

		void updateCurrentComponentSize();

	public:
		void updateRegionBounds();

	private:
		void sizeRegion();

	public:
		bool intersects(int x, int y);
		LayoutRegion* getLayoutRegion();
		LayoutRegion* getOriginalRegion();

		GingaWindowID prepareOutputDisplay(
				GingaSurfaceID renderedSurface, float cvtIndex);

		void showContent();
		void hideContent();

		void performOutTrans();
		double getOutTransDur();

		void setRegionVisibility(bool visible);

	private:
		void disposeOutputDisplay();

	public:
		void toFront();

	private:
		void bringChildrenToFront(LayoutRegion* parentRegion);
		void traverseFormatterRegions(
				LayoutRegion *region, LayoutRegion* baseRegion);

		void bringHideWindowToFront(
				LayoutRegion* baseRegion, LayoutRegion* hideRegion);

		void bringSiblingToFront(FormatterRegion *region);

	public:
		void setGhostRegion(bool ghost);
		bool isVisible();
		short getFocusState();
		bool setSelection(bool selOn);
		void setFocus(bool focusOn);
		void unselect();

		Color* getBackgroundColor();

		float getTransparency();
		void setTransparency(string strTrans);
		void setTransparency(float transparency);
		void setBackgroundColor(string color);
		void setBackgroundColor(Color *color);
		void setChromaKey(string value);
		void setRgbChromaKey(string value);
		void setFit(string value);
		void setFit(short value);
		void setScroll(string value);
		void setScroll(short value);

	private:
		void barWipe(Transition* transition, bool isShowEffect);
		static void* barWipeT(void* ptr);
		void fade(Transition* transition, bool isShowEffect);
		static void* fadeT(void* ptr);
		void lock();
		void unlock();
		void lockTransition();
		void unlockTransition();
		void lockFocusInfo();
		void unlockFocusInfo();
  };

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_PRESENTATION_END
typedef struct {
	::br::pucrio::telemidia::ginga::ncl::model::presentation::FormatterRegion* fr;
	Transition* t;
} TransInfo;

#endif /*FORMATTERREGION_H_*/
