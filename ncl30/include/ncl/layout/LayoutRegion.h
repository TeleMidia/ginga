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

#ifndef _LAYOUTREGION_H_
#define _LAYOUTREGION_H_

#include "../Entity.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "util/functions.h"
#include "util/Color.h"
using namespace ::br::pucrio::telemidia::util;

#include <pthread.h>
//#include <limits>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace layout {
	class LayoutRegion : public Entity {
		protected:
			string title;
			string outputMapRegionId;
			LayoutRegion* outputMapRegion;

			double left;
			bool leftPercentual;
			double top;
			bool topPercentual;
			double right;
			bool rightPercentual;
			double bottom;
			bool bottomPercentual;
			double width;
			bool widthPercentual;
			double height;
			bool heightPercentual;

			double* zIndex;
			map<string, LayoutRegion*> regions;
			vector<LayoutRegion*> sorted;
			LayoutRegion* parent;
			bool movable;
			bool resizable;
			bool decorated;
			int devClass;
			pthread_mutex_t mutex;

		public:
			LayoutRegion(string id);
			virtual ~LayoutRegion();
			virtual void setOutputMapRegion(LayoutRegion* outMapRegion);
			virtual LayoutRegion* getOutputMapRegion();
			virtual string getOutputMapRegionId();
			virtual void setDeviceClass(int deviceClass, string mapId);
			virtual int getDeviceClass();
			virtual void addRegion(LayoutRegion* region);
			virtual LayoutRegion* cloneRegion();

			virtual LayoutRegion* copyRegion();

			virtual int compareWidthSize(string w);
			virtual int compareHeightSize(string h);
			virtual double getBottom();
			virtual double getHeight();
			virtual double getLeft();
			virtual double getRight();
			virtual LayoutRegion* getRegion(string id);
			virtual LayoutRegion* getRegionRecursively(string id);
			void printRegionIdsRecursively();
			virtual vector<LayoutRegion*>* getRegions();
			virtual string getTitle();
			virtual double getTop();
			virtual double getWidth();
			virtual int getZIndex();
			virtual int getZIndexValue();
			virtual bool isBottomPercentual();
			virtual bool isHeightPercentual();
			virtual bool isLeftPercentual();
			virtual bool isRightPercentual();
			virtual bool isTopPercentual();
			virtual bool isWidthPercentual();
			virtual string toString();

		private:
			virtual bool removeRegion(LayoutRegion *region);

		public:
			virtual void removeRegions();

		private:
			virtual LayoutRegion* getDeviceLayout();
			virtual double getDeviceWidthInPixels();
			virtual double getDeviceHeightInPixels();

		public:
			virtual bool setBottom(double newBottom, bool isPercentual);
			virtual bool setTargetBottom(double newBottom, bool isPercentual);
			virtual bool setHeight(double newHeight, bool isPercentual);
			virtual bool setTargetHeight(double newHeight, bool isPercentual);
			virtual bool setLeft(double newLeft, bool isPercentual);
			virtual bool setTargetLeft(double newLeft, bool isPercentual);
			virtual bool setRight(double newRight, bool isPercentual);
			virtual bool setTargetRight(double newRight, bool isPercentual);
			virtual bool setTop(double newTop, bool isPercentual);
			virtual bool setTargetTop(double newTop, bool isPercentual);
		    virtual bool setWidth(double newWidth, bool isPercentual);
		    virtual bool setTargetWidth(double newWidth, bool isPercentual);

		    virtual void validateTarget();

		    virtual void setTitle(string newTitle);
		    virtual void setZIndex(int newZIndex);
		    virtual vector<LayoutRegion*>* getRegionsSortedByZIndex();
		    virtual vector<LayoutRegion*>* getRegionsOverRegion(
			        LayoutRegion* region);

		    virtual LayoutRegion* getParent();
		    virtual void setParent(LayoutRegion* parent);
		    virtual void refreshDeviceClassRegions();
		    virtual int getTopInPixels();
		    virtual int getBottomInPixels();
		    virtual int getRightInPixels();
		    virtual int getLeftInPixels();
		    virtual int getHeightInPixels();
		    virtual int getWidthInPixels();
			virtual bool isMovable();
			virtual bool isResizable();
			virtual bool isDecorated();
			virtual void setMovable(bool movable);
			virtual void setResizable(bool resizable);
			virtual void setDecorated(bool decorated);
			virtual void resetTop();
			virtual void resetBottom();
			virtual void resetRight();
			virtual void resetLeft();
			virtual void resetHeight();
			virtual void resetWidth();
			virtual void resetZIndex();
			virtual void resetDecorated();
			virtual void resetMovable();
			virtual void resetResizable();
			virtual int getAbsoluteLeft();
			virtual int getAbsoluteTop();
			virtual bool intersects(LayoutRegion* r);
			virtual bool intersects(int x, int y);

		private:
			double getPercentualValue(string value);
			bool isPercentualValue(string value);
			void lock();
			void unlock();
	};
}
}
}
}
}

#endif //_LAYOUTREGION_H_
