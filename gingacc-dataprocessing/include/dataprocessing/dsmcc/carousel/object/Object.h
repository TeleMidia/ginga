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

#ifndef OBJECT_H_
#define OBJECT_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "../Binding.h"

#include <vector>
#include <iostream>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	class Object {
		protected:
			unsigned int carouselId;
			unsigned int moduleId;
			unsigned int key;
			string kind;
			string name;

			//bindings (srg and dir only)
			vector<Binding*>* bindings;

			//data (fil only)
			char* data;
			unsigned int dataSize;

		public:
			Object();
			void setCarouselId(unsigned int objectCarouselId);
			void setModuleId(unsigned int objectModuleId);
			void setKey(unsigned int objectKey);
			void setKind(string objectKind);
			void addBinding(Binding* binding);
			void setData(char* data);
			void setDataSize(unsigned int size);

			string getObjectId();
			unsigned int getCarouselId();
			unsigned int getModuleId();
			unsigned int getKey();
			string getKind();
			vector<Binding*>* getBindings();
			char* getData();
			unsigned int getDataSize();
	};
}
}
}
}
}
}
}

#endif /*OBJECT_H_*/
