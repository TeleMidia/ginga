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

#ifndef FORMATTERLAYOUT_H_
#define FORMATTERLAYOUT_H_

#include "mb/interface/ISurface.h"
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

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace presentation {
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
				ISurface* renderedSurface,
				string plan);

		float refreshZIndex(
				FormatterRegion* region,
				string layoutRegionId,
				int zIndex,
				string plan,
				ISurface* renderedSurface);

	private:
		void sortRegion(string regionId, float cvtIndex, string plan);
		float convertZIndex(int zIndex, string plan);

	public:
		void showObject(ExecutionObject* object);
		void hideObject(ExecutionObject* object);

	private:
		GingaWindowID addRegionOnMaps(
				ExecutionObject* object, FormatterRegion* region,
				ISurface* renderedSurface,
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
}
}
}
}
}
}
}

#endif /*FORMATTERLAYOUT_H_*/
