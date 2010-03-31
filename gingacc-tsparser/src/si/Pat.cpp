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

#include "../../include/Pat.h"

#include "../../include/IDemuxer.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
	Pat::Pat() : TransportSection() {
		programs = new map<unsigned int, Pmt*>;
		pat = new map<unsigned int, unsigned int>;
		unprocessedPmts = new vector<unsigned int>;
		currentPid = 150;
		currentCarouselId = 15;
		carouselComponentTag = 0x21;
	}

	unsigned int Pat::getNextPid() {
		currentPid++;
		while (hasPid(currentPid)) {
			currentPid++;
		}

		return currentPid;
	}

	unsigned int Pat::getNextCarouselId() {
		currentCarouselId++;
		return currentCarouselId;
	}

	unsigned int Pat::getCarouselComponentTag() {
		return carouselComponentTag;
	}

	bool Pat::hasProgramNumber(unsigned int programNumber) {
		map<unsigned int, unsigned int>::iterator i;
		i = pat->begin();
		while (i != pat->end()) {
			if (i->second == programNumber) {
				return true;
			}
			++i;
		}
		return false;
	}

	bool Pat::hasPid(unsigned int pid) {
		map<unsigned int, Pmt*>::iterator i;
		Pmt* pmt;

		i = programs->begin();
		while (i != programs->end()) {
			pmt = i->second;
			if (pmt->hasPid(pid)) {
				return true;
			}
			++i;
		}
		return false;
	}

	Pmt* Pat::getPmtByProgramNumber(unsigned int programNumber) {
		map<unsigned int, unsigned int>::iterator i;

		i = pat->begin();
		while (i != pat->end()) {
			if (i->second == programNumber) {
				if (programs->count(i->first) != 0) {
					return (*programs)[i->first];
				}
			}
			++i;
		}
		return NULL;
	}

	unsigned int Pat::getTSId() {
		return this->idExtention;
	}

	void Pat::setTSId(unsigned int newTSId) {
		this->idExtention = newTSId;
	}

	unsigned int Pat::getNumOfPrograms() {
		return programs->size();
	}

	void Pat::addProgram(unsigned int pid, unsigned int programNumber) {
		if (pat->count(pid) != 0) {

			cout << "Pat::addProgram Warning! Trying to override an existent";
			cout << " program. Pid = '" << pid << "'" << endl;

		} else {
			(*pat)[pid] = programNumber;
		}
	}

	void Pat::addPmt(Pmt* program) {
		if (program == NULL) {
			return;
		}

		vector<unsigned int>::iterator i;
		for (i = unprocessedPmts->begin(); i != unprocessedPmts->end(); ++i) {
			if (*i == program->getPid()) {
				unprocessedPmts->erase(i);
				break;
			}
		}

		if (programs->count(program->getPid() != 0)) {

			cout << "Pat::addPmt Warning! Trying to override an existent";
			cout << " program. Pid = '" << program->getPid() << "'";
			cout << endl;

		} else {
			(*programs)[program->getPid()] = program;
		}
	}

	void Pat::replacePmt(unsigned int pid, Pmt* newPmt) {
		Pmt* oldPmt;
		if (programs->count(pid) != 0) {
			oldPmt = (*programs)[pid];
			delete oldPmt;
			(*programs)[pid] = newPmt;
		}
	}

	unsigned int Pat::getFirstProgramNumber() {
		if (pat->empty()) {
			return 0;
		}
		return pat->begin()->second;
	}

	unsigned int Pat::getProgramNumberByPid(unsigned int pid) {
		if (pat->count(pid) != 0) {
			return (*pat)[pid];
		}
		return 0;
	}

	short Pat::getStreamType(unsigned int pid) {
		if (programs->empty()) {
			return 0;
		}

		map<unsigned int, Pmt*>::iterator i;

		Pmt* program = NULL;
		i = programs->begin();
		while (i != programs->end()) {
			program = i->second;
			if (program->hasPid(pid)) {
				break;
			}

			program = NULL;
			++i;
		}

		if (program == NULL) {
			return 0;
		}

		return program->getStreamTypeValue(pid);
	}

	bool Pat::isSectionType(unsigned int pid) {
		if (pid == 0x00 || pid == 0x01 || pid == 0x03 ||
			    programs->count(pid) != 0) {

			return true;
		}

		vector<unsigned int>::iterator it;
		for (it = unprocessedPmts->begin();
			    it != unprocessedPmts->end(); ++it) {

			if (*it == pid) {
				return true;
			}
		}

		map<unsigned int, Pmt*>::iterator i;

		Pmt* program = NULL;
		i = programs->begin();
		while (i != programs->end()) {
			program = i->second;
			if (program->hasPid(pid)) {
				break;
			}

			program = NULL;
			++i;
		}

		if (program == NULL) {
			return false;
		}

		return program->isSectionType(pid);
	}

	bool Pat::processSectionPayload() {
		if (tableId != 0 || !isConsolidated()) {
			return false;
		}

		char sectionPayload[sectionLength - 9];
		memcpy((void*)(&sectionPayload[0]), getPayload(), sectionLength - 9);

		unsigned int n;
		unsigned int i;
		unsigned int programNumber;
		unsigned int pid;
		n = (unsigned int)((sectionLength - 9)/4);

		for (i = 0; i < n; i++) {
			programNumber = ((sectionPayload[i*4] & 0xFF) << 8) |
				    (sectionPayload[((i*4) + 1)] & 0xFF);

			pid = ((sectionPayload[((i*4) + 2)] & 0x1F) << 8) |
				    (sectionPayload[((i*4) + 3)] & 0xFF);

			if (pid != (unsigned int)NIT_PID) { /* Ignores NIT_PIDs */
				(*pat)[pid] = programNumber;
				unprocessedPmts->push_back(pid);
			}
		}
		return true;
	}

	bool Pat::hasUnprocessedPmt() {
		if (unprocessedPmts->empty()) {
			return false;
		}
		return true;
	}

	vector<unsigned int>* Pat::getUnprocessedPmtPids() {
		return unprocessedPmts;
	}

	map<unsigned int, Pmt*>* Pat::getProgramsInfo() {
		return this->programs;
	}

	void Pat::checkConsistency() {
		map<unsigned int, unsigned int>::iterator i;

		i = pat->begin();
		while (i != pat->end()) {
			if (programs->count(i->first) == 0) {
				pat->erase(i);
				if (pat->empty()) {
					return;
				}
				i = pat->begin();

			} else {
				++i;
			}
		}
	}

	unsigned int Pat::getDefaultProgramPid() {
		Pmt* pmt;

		if (programs->empty()) {
			return 0;
		}

		pmt = programs->begin()->second;
		return pmt->getPid();
	}

	unsigned int Pat::getDefaultMainVideoPid() {
		Pmt* pmt;

		if (programs->empty()) {
			return 0;
		}

		pmt = programs->begin()->second;
		return pmt->getDefaultMainVideoPid();
	}

	unsigned int Pat::getDefaultMainAudioPid() {
		Pmt* pmt;

		if (programs->empty()) {
			return 0;
		}

		pmt = programs->begin()->second;
		return pmt->getDefaultMainAudioPid();
	}

	void Pat::print() {
		cout << "Pat::print" << endl;
		cout << "TS id = " << idExtention << endl;
		cout << "programs:" << endl;
		map<unsigned int, unsigned int>::iterator i;
		for (i = pat->begin(); i != pat->end(); ++i) {
			cout << "programNumber '" << hex << i->second << "' ";
			cout << " has pid = '" << hex << i->first << "'" << endl;
		}
	}
}
}
}
}
}
}
}
