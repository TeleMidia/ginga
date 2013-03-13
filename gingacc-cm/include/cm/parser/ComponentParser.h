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

#ifndef _ComponentParser_H_
#define _ComponentParser_H_

#include "expat.h"

#include "IComponentParser.h"

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include <dlfcn.h>
#include <set>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace cm {
	class ComponentParser : public IComponentParser {
		private:
			map<string, IComponent*>* components;
			map<string, IComponent*>* symbols;
			map<string, set<string>*>* parentObjects;
			map<string, set<string>*>* dependenciesToAdd;

		public:
			ComponentParser();
			virtual ~ComponentParser();

			void parse(string xmlDocument);

		private:
			void releaseMaps();
			static void startElementHandler(
					void* data,
					const XML_Char* element, const XML_Char** attrs);

			static void stopElementHandler(void* data, const XML_Char* element);

			static void parseComponent(void* data, const XML_Char** attrs);
			void addComponent(string name, IComponent* component);

			static void parseDependency(void* data, const XML_Char** attrs);

			static void parseSymbol(void* data, const XML_Char** attrs);
			void addChild(string parent, string childObject);
			void addObject(string object, IComponent* component);

			static void parseLocation(void* data, const XML_Char** attrs);
			static void parseRepository(void* data, const XML_Char** attrs);

			IComponent* getComponent(string name);

		public:
			map<string, IComponent*>* getComponents();
			map<string, IComponent*>* getSymbols();
			map<string, set<string>*>* getParentObjects();
			map<string, set<string>*>* getUnsolvedDependencies();
			void solveDependencies();
	};
}
}
}
}
}
}

#endif //_ComponentParser_H_
