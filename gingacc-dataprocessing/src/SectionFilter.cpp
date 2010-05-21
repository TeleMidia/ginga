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

#include "../include/SectionFilter.h"

#ifndef abs
#define abs(a) ((a) < 0 ? (-a) : (a))
#endif

#include "../config.h"

#if HAVE_COMPSUPPORT
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#include "../../gingacc-tsparser/include/TransportSection.h"
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {

#if HAVE_COMPSUPPORT
	static IComponentManager* cm = IComponentManager::getCMInstance();
#endif

	SectionFilter::SectionFilter() {
		this->processedSections = new set<string>;
		this->listener          = NULL;
		this->hFilteredSections = new map<int, ITransportSection*>;
		this->lastPid           = -1;

		pthread_mutex_init(&stlMutex, NULL);
	}

	SectionFilter::~SectionFilter() {
		map<int, ITransportSection*>::iterator i;
		map<unsigned int, SectionHandler*>::iterator j;

		cout << "SectionFilter::~SectionFilter" << endl;
		pthread_mutex_lock(&stlMutex);

		if (processedSections != NULL) {
			delete processedSections;
			processedSections = NULL;
		}

		if (hFilteredSections != NULL) {
			i = hFilteredSections->begin();
			while (i != hFilteredSections->end()) {
				delete i->second;
				++i;
			}

			delete hFilteredSections;
			hFilteredSections = NULL;
		}

		/*Clear the allocated structs */
		j = sectionPidSelector.begin();
		while (j != sectionPidSelector.end()) {
			//TODO - verify if needs to delete the internal allocated memory
			delete j->second;
			++j;
		}

		pthread_mutex_unlock(&stlMutex);
		pthread_mutex_destroy(&stlMutex);
	}

	void SectionFilter::setListener(IFilterListener* listener) {
		this->listener = listener;
	}
	
	// Set manipulation functions.

	bool SectionFilter::checkProcessedSections(string sectionName) {
		bool checked;

		pthread_mutex_lock(&stlMutex);
		if (processedSections->count(sectionName) == 0) {
			checked = false;

		} else {
			checked = true;
		}
		pthread_mutex_unlock(&stlMutex);

		return checked;
	}

	void SectionFilter::addProcessedSection(string sectionName) {
		pthread_mutex_lock(&stlMutex);
		if (processedSections != NULL) {
			processedSections->insert(sectionName);
		}
		pthread_mutex_unlock(&stlMutex);
	}

	void SectionFilter::removeProcessedSection(string sectionName) {
		set<string>::iterator i;

		pthread_mutex_lock(&stlMutex);
		i = processedSections->find(sectionName);
		if (i != processedSections->end()) {
			processedSections->erase(i);
		}
		pthread_mutex_unlock(&stlMutex);
	}

	void SectionFilter::clearProcessedSections() {
		pthread_mutex_lock(&stlMutex);
		processedSections->clear();
		pthread_mutex_unlock(&stlMutex);
	}

	bool SectionFilter::checkSectionVersion(ITransportSection* section) {
		unsigned int tableId;
		string sectionName;

		tableId = section->getTableId();
		if (section->getVersionNumber() > 0) {
			if (tableId == 0x42 || tableId == 0x4E || tableId == 0xC8) {
				sectionName = "epg/data/" +
						itos(section->getESId()) +
					    itos(section->getTableId()) +
					    itos(section->getExtensionId()) +
					    itos(section->getVersionNumber() - 1);

			} else {
				sectionName = "carousel/modules/" +
						itos(section->getESId()) +
					    itos(section->getTableId()) +
					    itos(section->getExtensionId()) +
					    itos(section->getVersionNumber() - 1);
			}

			if (checkProcessedSections(sectionName)) {
				// there is an old version
				// TODO: do something
				return true;
			}
		}
		return false;
	}

	// Delivers consolidated section packets to listener.  Invalid
	// packets are ignored.
	void SectionFilter::receiveTSPacket(ITSPacket* pack) {
		unsigned int counter;
		unsigned int packPID;
		int last;
		SectionHandler* handler;

		packPID = pack->getPid();
		handler = getSectionHandler(packPID);
		if (handler == NULL) {
			handler          = new SectionHandler;
			handler->section = NULL;

			resetHandler(handler);
			sectionPidSelector[packPID] = handler;
		}

		/*if (pack->getStartIndicator()) {
			cout << "SectionFilter::receiveTSPacket number '" << recvPack;
			cout << "'" << endl;
		}*/
		handler->recvPack++;

		counter = pack->getContinuityCounter();
		last    = handler->lastContinuityCounter;

		//updates the Continuity
		handler->lastContinuityCounter = counter;

		// Check if the TS has a beginning of a new section.
		if (pack->getStartIndicator()) {
			// Consolidates previous section.
			if (handler->section != NULL) {
				/*cout << "SectionFilter::receiveTSPacket ";
				cout << "Consolidates previous section.";
				cout << endl;*/
				verifyAndAddData(pack);
			}

			// Create a new section.
			if (!verifyAndCreateSection(pack)) {
				/*cout << "SectionFilter::receiveTSPacket - Failed to create ";
				cout << "Section, perhaps header is not complete yet!" << endl;
				*/
			}

		// Continuation of a previous section.
		} else {
			if (handler->section == NULL) {
				/* Tries to create a continuation of section
				 * which the header was not ready at the moment */
				if (!verifyAndCreateSection(pack)) {
					/*cout << "SectionFilter::receiveTSPacket - Receive a ";
					cout << "continuation but failed to create ";
					cout << "Section, perhaps header is not complete yet!";
					cout << endl;*/
				}

			} else {
				bool isValidCounter = false;

				if (pack->getAdaptationFieldControl() == 2 ||
						pack->getAdaptationFieldControl() == 0) {

					if (last == counter) {
						isValidCounter = true;
					}

				} else if ((last == 15 && counter == 0) ||
						(counter - last == 1)) {

					isValidCounter = true;
				}

				if (isValidCounter) {
					/* The section is OK */
					verifyAndAddData(pack);

				} else { // Discontinuity, ignore section.
					cout << "SectionFilter::receiveTSPacket: ";
					cout << "Discontinuity, last = '" << last << "'";
					cout << " counter = '" << counter << "'";
					cout << " ignoring section.";
					cout << endl;

					ignore(pack->getPid());
				}
			}
		}
	}

	void SectionFilter::receiveSection(
			char* buf, int len, IFrontendFilter* filter) {

		//cout << "SectionFilter::receiveSection '" << len << "'" << endl;
		ITransportSection* filteredSection = NULL;
		map<int, ITransportSection*>::iterator i;
		int pid;

		if (listener == NULL) {
			cout << "SectionFilter::receiveSection Warning!";
			cout << " NULL listener" << endl;
			return;
		}

		pid     = filter->getPid();
		lastPid = pid;

		i = hFilteredSections->find(pid);
		if (i == hFilteredSections->end()) {
#if HAVE_COMPSUPPORT
			filteredSection = ((TSSectionCreator*)(cm->getObject(
					"TransportSection")))(buf, len);

#else
			filteredSection = new TransportSection(buf, len);
#endif
			filteredSection->setESId(pid);
			(*hFilteredSections)[pid] = filteredSection;
			i = hFilteredSections->find(pid);

		} else {
			filteredSection = i->second;
			filteredSection->addData(buf, len);
		}

		if (filteredSection->isConsolidated()) {
			if (i != hFilteredSections->end()) {
				hFilteredSections->erase(i);
			}

			if (!checkProcessedSections(filteredSection->getSectionName())) {
				listener->receiveSection(filteredSection);

			} else {
				delete filteredSection;
			}

			filteredSection = NULL;
		}
	}

	void SectionFilter::receivePes(
			char* buf, int len, IFrontendFilter* filter) {

		cout << "SectionFilter::receivePes" << endl;
	}

	SectionHandler* SectionFilter::getSectionHandler(unsigned int pid) {
		if (sectionPidSelector.count(pid) != 0) {
			return sectionPidSelector[pid];
		}

		return NULL;
	}

	// Process and frees SECTION.
	void SectionFilter::process(ITransportSection* section, unsigned int pid) {
		SectionHandler* handler;

		handler = getSectionHandler(pid);
		if (handler != NULL && listener != NULL) {
			if (!checkProcessedSections(handler->section->getSectionName())) {
				listener->receiveSection(handler->section);
			}
		}

		resetHandler(handler);
	}

	void SectionFilter::resetHandler(SectionHandler* handler) {
		handler->section               = NULL;
		handler->lastContinuityCounter = -1;
		handler->headerSize            = 0;
		handler->recvPack              = 0;

		memset(
				handler->sectionHeader,
				0,
				ARRAY_SIZE(handler->sectionHeader));
	}

	// Ignore SECTION.
	void SectionFilter::ignore(unsigned int pid) {
		map<unsigned int, SectionHandler*>::iterator i;

		i = sectionPidSelector.find(pid);
		if (i != sectionPidSelector.end()) {
			resetHandler(i->second);
			if (i->second->section != NULL) {
				delete i->second->section;
			}
		}
	}

	void SectionFilter::verifyAndAddData(ITSPacket* pack) {
		char data[184];
		SectionHandler* handler;
		/* Get the freespace in Section */
		unsigned int freespace;

		handler = getSectionHandler(pack->getPid());
		if (handler == NULL) {
			return;
		}

		freespace = handler->section->getSectionLength() + 3 -
				handler->section->getCurrentSize();

		/* If the freeSpace is bigger than payLoadSize then
		* add just the payloadSize      */
		if (freespace > pack->getPayloadSize()) {
			freespace = pack->getPayloadSize();
		}
		memset(data, 0, sizeof(data));

		pack->getPayload(data);
		if (freespace > 0) {
			handler->section->addData(data, freespace);

		} else {
			_debug("Trying to add 0 bytes in the section\n");
		}

		/*if (pack->getPid() == 0x384) {
			printf("NEW ====================%d\n", pack->getPayloadSize());
			for (int j = 0; j < pack->getPayloadSize(); j++) {
				printf("%02hhX ", (char) data[j]);
			}
			printf("\n====================\n");
		}*/

		if (handler->section->isConsolidated()) {
			process(handler->section, pack->getPid());
		}
	}

	bool SectionFilter::verifyAndCreateSection(ITSPacket* pack) {
		unsigned int offset;
		unsigned int diff;
		char data[184];
		char* buffer;
		SectionHandler* handler;

		handler = getSectionHandler(pack->getPid());
		if (handler == NULL) {
			return false;
		}

		offset = pack->getPointerField();

		/* What is the real payload */
		diff   = pack->getPayloadSize() - offset;

		pack->getPayload(data);
		/* The payload has only a part of the header */
		if (diff < (ARRAY_SIZE(handler->sectionHeader) - handler->headerSize)) {
			/*cout << "SectionFilter::verifyAndCreateSection ";
			cout << "Creating Section header, currentSize is '";
			cout << handler->headerSize << " and dataSize is '";
			cout << diff << "'";
			cout << endl;*/

			memcpy(
					(void*)&handler->sectionHeader[handler->headerSize],
					(void*)&data[offset],
					diff);

			handler->headerSize            = handler->headerSize + diff;
			handler->lastContinuityCounter = pack->getContinuityCounter();
			return false;

		/* Needs to copy the header */
		} else if (handler->headerSize > 0) {
			/*cout << "Pointer field: " << pack->getPointerField();
			cout << " PayloadSize: " << pack->getPayloadSize() << endl;
			cout << "Appending Section header '" << handler->headerSize << "'";
			cout << " to data '" << diff << "'" << endl;*/

			/* Creates the new data buffer */
			buffer = new char[handler->headerSize + diff];

			/* Copies the header to buffer */
			memcpy(
					(void*)&buffer[0],
					(void*)&handler->sectionHeader, handler->headerSize);

			/* Copies the payload to buffer */
			memcpy(
					(void*)&buffer[handler->headerSize],
					(void*)&data[offset],
					diff);

			/* Creates the new section */
#if HAVE_COMPSUPPORT
			handler->section = ((TSSectionCreator*)(cm->getObject(
					"TransportSection")))(buffer, diff + handler->headerSize);

#else
			handler->section = new TransportSection(
					buffer, diff + handler->headerSize);
#endif

			delete buffer;
			buffer = NULL;

		/* The Header is ready */
		} else if (pack->getStartIndicator()) {
			/*cout << "SectionFilter::verifyAndCreateSection PUSI = 1";
			cout << " for PID = '" << pack->getPid() << "'";
			cout << "Header is ready already!" << endl;*/

#if HAVE_COMPSUPPORT
			handler->section = ((TSSectionCreator*)(cm->getObject(
					"TransportSection")))(&data[offset], diff);

#else
			handler->section = new TransportSection(
					&data[offset], diff);
#endif

		} else {
			/*cout << "SectionFilter::verifyAndCreateSection ";
			cout << "nothing to do current header size is '";
			cout << handler->headerSize << "' current section address is '";
			cout << handler->section << "' TS packet: ";
			pack->print();
			cout << endl;*/
		}

		return setSectionParameters(pack);
	}

	bool SectionFilter::setSectionParameters(ITSPacket* pack) {
		SectionHandler* handler;

		handler = getSectionHandler(pack->getPid());
		/* First of all verifies if the currentSection is OK */
		if (handler == NULL || handler->section == NULL) {
			return false;
		}

		/* Verifies if the TransportSection has been created! */
		if (!(handler->section->isConstructionFailed())) {
			_error("SectionFilter::receiveTSPacket failed to create Section!\n");
			ignore(pack->getPid());
			return false;
		}

		handler->section->setESId(pack->getPid());

		_debug(
				"SectionFilter::setSectionParameters "
				"Section %s created with secNUm=%d, lasSec=%d and secLen=%d\n",
				handler->section->getSectionName().c_str(),
				handler->section->getSectionNumber(),
				handler->section->getLastSectionNumber(),
				handler->section->getSectionLength());

		if (handler->section->isConsolidated()) {
			process(handler->section, pack->getPid());

		} else {
			handler->lastContinuityCounter = pack->getContinuityCounter();
		}

		//handler->headerSize = 0;
		//memset(handler->sectionHeader, 0, ARRAY_SIZE(handler->sectionHeader));
		return true;
	}
}
}
}
}
}
}
