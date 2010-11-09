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

#ifndef _ComponentDescription_H_
#define _ComponentDescription_H_

#include "IComponentDescription.h"

#include <fstream>
#include <iostream>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace cm {
	class ComponentDescription : public IComponentDescription {
		private:
			map<string, IComponent*>* components;
			string location;
			string name;
			ofstream* file;

		public:
			ComponentDescription(map<string, IComponent*>* comps);
			virtual ~ComponentDescription();

			void setLocation(string location);
			void setDescriptionName(string name);
			void describe();

		private:
			bool createFile();
			void describeBegin();
			void describeComponents();
			void describeComponent(IComponent* component);
			void describeEndOfComponent();
			void describeDependencies(IComponent* component);
			void describeDependency(IComponent* component);
			void describeSymbols(IComponent* component);
			void describeSymbol(string obj, string creator, string destroyer);
			void describeLocation(IComponent* component);
			void describeRepositories(IComponent* component);
			void describeRepository(string repositoryUri);
			void describeEnd();
			void closeFile();
	};
}
}
}
}
}
}

#endif //_ComponentDescription_H_
