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

#ifndef FORMATTERDEVICEREGION_H_
#define FORMATTERDEVICEREGION_H_

#include "util/Color.h"
using namespace ::br::pucrio::telemidia::util;

#include "ncl/Entity.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "ncl/layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include <pthread.h>
#include <set>
#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace presentation {
  class FormatterDeviceRegion : public LayoutRegion {
	private:
		string id;
		int top, left, width, height;
		vector<LayoutRegion*>* sortedRegions;
		set<LayoutRegion*> regionSet;
		pthread_mutex_t rMutex;

	public:
		FormatterDeviceRegion(string id);
		virtual ~FormatterDeviceRegion(){};
		void addRegion(LayoutRegion *region);
		LayoutRegion *cloneRegion();
		int compareWidthSize(string w);
		int compareHeightSize(string h);
		short getBackgroundColor();
		double getBottom();
		double getHeight();
		double getLeft();
		double getRight();
		LayoutRegion *getRegion(string id);
		LayoutRegion *getRegionRecursively(string id);
		vector<LayoutRegion*> *getRegions();
		string getTitle();
		double getTop();
		double getWidth();
		int getZIndex();
		int getZIndexValue();
		bool isBottomPercentual();
		bool isHeightPercentual();
		bool isLeftPercentual();
		bool isRightPercentual();
		bool isTopPercentual();
		bool isWidthPercentual();
		string toString();
		bool removeRegion(LayoutRegion *region);
		void removeRegions();
		void setBackgroundColor(Color *newBackgroundColor);
		bool setBottom(double newBottom, bool isPercentual);
		bool setHeight(double newHeight, bool isPercentual);
		bool setLeft(double newLeft, bool isPercentual);
		bool setRight(double newRight, bool isPercentual);
		void setTitle(string newTitle);
		bool setTop(double newTop, bool isPercentual);
		bool setWidth(double newWidth, bool isPercentual);
		void setZIndex(int newZIndex);
		vector<LayoutRegion*> *getRegionsSortedByZIndex();
		vector<LayoutRegion*> *getRegionsOverRegion(
			    LayoutRegion *region);

		LayoutRegion *getParent();
		void setParent(LayoutRegion *parent);
		int getTopInPixels();
		int getBottomInPixels();
		int getRightInPixels();
		int getLeftInPixels();
		int getHeightInPixels();
		int getWidthInPixels();
		bool isMovable();
		bool isResizable();
		bool isDecorated();
		void setMovable(bool movable);
		void setResizable(bool resizable);
		void setDecorated(bool decorated);
		void resetTop();
		void resetBottom();
		void resetRight();
		void resetLeft();
		void resetHeight();
		void resetWidth();
		void resetZIndex();
		void resetDecorated();
		void resetMovable();
		void resetResizable();
		int getAbsoluteLeft();
		int getAbsoluteTop();

		void dispose();
		string getId();
		void setId(string id);
		Entity *getDataEntity();
		int compareTo(void *arg0);
  };
}
}
}
}
}
}
}

#endif /*FORMATTERDEVICEREGION_H_*/
