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

#include "cm/assembler/ComponentDescription.h"

#include <unistd.h>

using namespace ::br::pucrio::telemidia::ginga::core::cm;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace cm {
	ComponentDescription::ComponentDescription(
					map<string, IComponent*>* comps) {

		this->components = new map<string, IComponent*>(*comps);
		this->location = "";
		this->name = "";
		this->file = NULL;
	}

	ComponentDescription::~ComponentDescription() {
		delete components;
	}

	void ComponentDescription::setLocation(string location) {
		this->location = location;
	}

	void ComponentDescription::setDescriptionName(string name) {
		this->name = name;
	}

	void ComponentDescription::describe() {
		if (createFile()) {
			describeBegin();
			describeComponents();
			describeEnd();
			closeFile();
		}
	}

	bool ComponentDescription::createFile() {
		string currentFile = location + name;
		if (unlink(currentFile.c_str()) != 0) {
			cerr << "ComponentDescription::createFile warning: can't unlink '";
			cerr << currentFile << "'" << endl;
		}

		file = new ofstream(currentFile.c_str(), ios::out | ios::binary);
		if (!file) {
			cerr << "ComponentDescription::createFile warning: can't create '";
			cerr << currentFile << "'" << endl;
			return false;

		} else {
			file->seekp(ofstream::end);
			if (file->tellp() > 2) {
				cerr << "ComponentDescription::createFile warning: opening ";
				cerr << " existent file '" << currentFile << "' pos = '";
				cerr << file->tellp() << "'" << endl;
				return false;
			}
			file->seekp(0);
		}

		return true;
	}

	void ComponentDescription::describeBegin() {
		*file << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl;
		*file << "<middleware>" << endl;
	}

	void ComponentDescription::describeComponents() {
		map<string, IComponent*>::iterator i;
		IComponent* comp;

		i = components->begin();
		while (i != components->end()) {
			comp = i->second;
			describeComponent(comp);
			describeDependencies(comp);
			describeSymbols(comp);
			describeLocation(comp);
			describeRepositories(comp);
			describeEndOfComponent();
			++i;
		}
	}

	void ComponentDescription::describeComponent(IComponent* c) {
		//<component name="xxx" version="xxx" type="xx">
		*file << "  <component name=\"" << c->getName() << "\" version=\"";
		*file << c->getVersion() << "\" type=\"" << c->getType() << "\">";
		*file << endl;
	}


	void ComponentDescription::describeEndOfComponent() {
		*file << "  </component>" << endl << endl;
	}

	void ComponentDescription::describeDependencies(IComponent* c) {
		set<IComponent*>* deps;
		set<IComponent*>::iterator i;

		deps = c->getDependencies();
		i = deps->begin();
		while (i != deps->end()) {
			describeDependency(*i);
			++i;
		}
	}

	void ComponentDescription::describeDependency(IComponent* c) {
		//<dependency componentName="xxx" version="xxx"/>
		*file << "    <dependency componentName=\"" << c->getName();
		*file << "\" version=\"" << c->getVersion() << "\"/>";
		*file << endl;
	}

	void ComponentDescription::describeSymbols(IComponent* c) {
		map<string, string>* creas;
		map<string, string>* dests;
		map<string, string>::iterator i, j;
		string destroyer;

		creas = c->getCreatorSymbols();
		dests = c->getDestroyerSymbols();

		i = creas->begin();
		while (i != creas->end()) {
			j = dests->find(i->first);
			if (j != dests->end()) {
				describeSymbol(i->first, i->second, j->second);
			} else {
				describeSymbol(i->first, i->second, "");
			}
			++i;
		}
	}

	void ComponentDescription::describeSymbol(string o, string c, string d) {
		//<symbol object="X" creator="createX" destroyer="destroyX"/>
		*file << "    <symbol object=\"" << o << "\" creator=\"" << c;
		*file << "\" destroyer=\"" << d << "\"/>";
		*file << endl;
	}

	void ComponentDescription::describeLocation(IComponent* c) {
		//<location type="local" uri="libdir"/>
		*file << "    <location type=\"" << c->getStrLocationType() << "\"";
		*file << " uri=\"" << c->getLocation() << "\"/>";
		*file << endl;
	}

	void ComponentDescription::describeRepositories(IComponent* c) {
		set<string>* reps;
		set<string>::iterator i;

		reps = c->getRepositories();
		i = reps->begin();
		while (i != reps->end()) {
			describeRepository(*i);
			++i;
		}
	}

	void ComponentDescription::describeRepository(string r) {
		//<repository uri="http://www.xxx.zu/abc.so"/>
		*file << "    <repository uri=\"" << r << "\"/>";
		*file << endl;
	}

	void ComponentDescription::describeEnd() {
		*file << "</middleware>" << endl;
	}

	void ComponentDescription::closeFile() {
		file->close();
		delete file;
		file = NULL;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::cm::IComponentDescription*
		createComponentDescription(map<string, IComponent*>* comps) {

	return new ::br::pucrio::telemidia::ginga::core::cm::
			ComponentDescription(comps);
}

extern "C" void destroyComponentDescription(
		::br::pucrio::telemidia::ginga::core::cm::IComponentDescription* cd) {

	delete cd;
}
