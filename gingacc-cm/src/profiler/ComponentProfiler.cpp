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

#include "cm/profiler/ComponentProfiler.h"

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace cm {
	ComponentProfiler::ComponentProfiler(string processName) {
		this->cm          = IComponentManager::getCMInstance();
		this->compDesc    = cm->copyComponentDescription();
		this->auxFile     = SystemCompat::getTemporaryDir() + "/_compProf.txt";
		this->processName = processName;
	}

	ComponentProfiler::~ComponentProfiler() {
		delete compDesc;

#ifndef _WIN32
		string strCmd = "rm -f " + auxFile;
		::system(strCmd.c_str());
#endif //!_WIN32
	}

	void ComponentProfiler::process() {
		map<string, IComponent*>::iterator i;
		map<string, string>* symbols;
		map<string, string>::iterator j;
		IComponent* component;
		double elapsedTime;

		i = compDesc->begin();
		while (i != compDesc->end()) {
			component = i->second;
			elapsedTime = getCurrentTimeMillis();
			cout << "Name: " << component->getName() << endl;
			symbols = component->getCreatorSymbols();
			j = symbols->begin();
			while (j != symbols->end()) {
				cout << "Symbols[" << j->first << "]=" << j->second << endl;
				++j;
			}
			elapsedTime = getCurrentTimeMillis() - elapsedTime;
			++i;
		}
	}

	void ComponentProfiler::updateDescription() {

	}

	bool ComponentProfiler::getFootprint(int* cpu, int* mem) {
		bool gotIt = false;
		string value, pidNum, strCmd;

#ifndef _WIN32
		//process pid
		pidNum = getPIdFromPName(processName);

		//cpu from process pid
		strCmd = "ps -p " + pidNum + " -o pcpu > " + auxFile;
		value  = getValue(strCmd);
		*cpu   = stof(value);

		//mem from process pid
		strCmd = "cat /proc/" + pidNum + "/status | grep VmLib > " + auxFile;
		value  = getValue(strCmd);
		*mem   = stof(value);

#endif //!_WIN32

		return gotIt;
	}

	string ComponentProfiler::getPIdFromPName(string name) {

		string pidNum = "";
		string strCmd;

#ifndef _WIN32

		strCmd = "pgrep " + name + " > "  + auxFile;
		pidNum = getValue(strCmd);

#endif //!_WIN32

		return pidNum;
	}

	string ComponentProfiler::getValue(string command) {
		ifstream pidFile;
		string value = "";

		::system(command.c_str());
		pidFile.open(auxFile.c_str(), ifstream::in);
		if (pidFile.is_open()) {
			if (pidFile.good()) {
				pidFile >> value;
			}
		}

		return value;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::cm::IComponentProfiler*
		createComponentProfiler(string processName) {

	return new ::br::pucrio::telemidia::ginga::core::cm::ComponentProfiler(
			processName);
}

extern "C" void destroyComponentProfiler(
		::br::pucrio::telemidia::ginga::core::cm::IComponentProfiler* cp) {

	delete cp;
}
