/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#ifndef FORMATTERREGION_H_
#define FORMATTERREGION_H_

#include "util/Color.h"
#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "ncl/layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl/descriptor/Descriptor.h"
#include "ncl/descriptor/DescriptorUtil.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl/transition/Transition.h"
#include "ncl/transition/TransitionUtil.h"
using namespace ::br::pucrio::telemidia::ncl::transition;

#include "mb/IInputManager.h"
#include "mb/ILocalScreenManager.h"
#include "mb/interface/IWindow.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "FocusSourceManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation::focus;

#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace presentation {
  class FormatterRegion {
	private:
		void* layoutManager; //FormatterLayout
		void* descriptor; //CascadingDescriptor

		string objectId;
		LayoutRegion* ncmRegion;
		LayoutRegion* originalRegion;
		bool externHandler;
		IWindow* outputDisplay;

		ISurface* renderedSurface;

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
			    ISurface* component);

		void sliceComponent(
			    int width,
			    int height,
			    int prefWidth,
			    int prefHeight,
			    ISurface* component);

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
				ISurface* renderedSurface, float cvtIndex);

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
}
}
}
}
}
}
}

typedef struct {
	::br::pucrio::telemidia::ginga::ncl::model::presentation::FormatterRegion* fr;
	Transition* t;
} TransInfo;

#endif /*FORMATTERREGION_H_*/
