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
		this->processedSections     = new set<string>;
		this->lastContinuityCounter = -1;
		this->currentSection        = NULL;
		this->listener              = NULL;
		this->currentHeaderSize     = 0;
		this->recvPack              = 1;

		pthread_mutex_init(&stlMutex, NULL);

		memset(sectionHeader, 0, ARRAY_SIZE(sectionHeader));
	}

	SectionFilter::~SectionFilter() {
		cout << "SectionFilter::~SectionFilter" << endl;
		pthread_mutex_lock(&stlMutex);

		if (processedSections != NULL) {
			delete processedSections;
			processedSections = NULL;
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
		int last;

		/*if (pack->getStartIndicator()) {
			cout << "SectionFilter::receiveTSPacket number '" << recvPack;
			cout << "'" << endl;
		}*/
		recvPack++;

		counter               = pack->getContinuityCounter();
		last                  = lastContinuityCounter;
		lastContinuityCounter = counter; //updates the Continuity

		// Check if the TS has a beginning of a new section.
		if (pack->getStartIndicator()) {
			// Consolidates previous section.
			if (currentSection != NULL) {
				/*cout << "SectionFilter::receiveTSPacket ";
				cout << "Consolidates previous section.";
				cout << endl;*/
				verifyAndAddData(pack);
			}

			// Create a new section.
			if (!verifyAndCreateSection(pack)) {
				cout << "SectionFilter::receiveTSPacket - Failed to create ";
				cout << "Section, perhaps header is not complete yet!" << endl;
			}

		// Continuation of a previous section.
		} else {
			if (currentSection == NULL) {
				/* Tries to create a continuation of section
				 * which the header was not ready at the moment */
				if (!verifyAndCreateSection(pack)) {
					/*cout << "SectionFilter::receiveTSPacket - Receive a ";
					cout << "continuation but Failed to create ";
					cout << "Section, perhaps header is not complete yet!";
					cout << endl;*/
				}
			}

			/* Verifies Continuity */
			if (last >= 0 || (last == 15 && counter == 0) ||
					(abs(last - counter) == 1)) {

				/* The section is OK */
				verifyAndAddData(pack);

			} else { // Discontinuity, ignore section.
				/*cout << "SectionFilter::receiveTSPacket: ";
				cout << "Discontinuity, ignoring section...";
				cout << endl;*/

				ignore(currentSection);
				return;
			}
		}
	}

	void SectionFilter::receiveSection(
			char* buf, int len, IFrontendFilter* filter) {

		//cout << "SectionFilter::receiveSection '" << len << "'" << endl;

		if (listener == NULL) {
			cout << "SectionFilter::receiveSection Warning!";
			cout << " NULL listener" << endl;
			return;
		}

		if (currentSection == NULL) {
#if HAVE_COMPSUPPORT
			currentSection = ((TSSectionCreator*)(cm->getObject(
					"TransportSection")))(buf, len);

#else
			currentSection = new TransportSection(buf, len);
#endif
			currentSection->setESId(filter->getPid());

		} else {
			currentSection->addData(buf, len);
		}

		if (currentSection->isConsolidated()) {
			if (!checkProcessedSections(currentSection->getSectionName())) {
				listener->receiveSection(currentSection);

			} else {
				delete currentSection;
			}

			currentSection = NULL;
		}
	}

	void SectionFilter::receivePes(
			char* buf, int len, IFrontendFilter* filter) {

		cout << "SectionFilter::receivePes" << endl;
	}

	// Process and frees SECTION.
	void SectionFilter::process(ITransportSection* section) {
		if (!checkProcessedSections(currentSection->getSectionName())
				&& listener != NULL) {

			listener->receiveSection(currentSection);

		} else {
			/*_debug(
					"SectionFilter::process"
					"Section %s already processed! ... \n",
					currentSection->getSectionName().c_str());*/
		}

		currentSection = NULL;
		delete currentSection;
		this->lastContinuityCounter = -1;
		this->currentHeaderSize = 0;
	}

	// Ignore SECTION.
	void SectionFilter::ignore(ITransportSection* section) {
		delete currentSection;
		currentSection = NULL;
		this->lastContinuityCounter = -1;
		this->currentHeaderSize = 0;
	}

	void SectionFilter::verifyAndAddData(ITSPacket* pack){
		char data[184];
		/* Get the freespace in Section */
		unsigned int freespace;

		freespace = (
				currentSection->getSectionLength() +
				3 -
				currentSection->getCurrentSize());

		/* If the freeSpace is bigger than payLoadSize then
		 * add just the payloadSize	 */
		if (freespace > pack->getPayloadSize()) {
			freespace = pack->getPayloadSize();
		}

		pack->getPayload(data);
		if (freespace > 0) {
			currentSection->addData(data, freespace);

		} else {
			_debug("Trying to add 0 bytes in the section\n");
		}

		if (currentSection->isConsolidated()) {
			process(currentSection);
		}
	}

	bool SectionFilter::verifyAndCreateSection(ITSPacket* pack){
		unsigned int offset;
		unsigned int diff;
		char data[184];
		char* buffer;

		offset = pack->getPointerField();

		/* What is the real payload */
		diff   = pack->getPayloadSize() - offset;

		pack->getPayload(data);
		if (diff < (ARRAY_SIZE(sectionHeader) - currentHeaderSize)) {

			cout << "SectionFilter::verifyAndCreateSection ";
			cout << "Creating Section header, currentSize is '";
			cout << currentHeaderSize << "and dataSize is '";
			cout << diff << "'";
			cout << endl;

			memcpy(
					(void*)&sectionHeader[currentHeaderSize],
					(void*)&data[offset], diff);

			currentHeaderSize     = currentHeaderSize + diff;
			lastContinuityCounter = pack->getContinuityCounter();
			return false;

		/* Needs to copy the header */
		} else if (currentHeaderSize > 0) {
			cout << "Appending Section header '" << currentHeaderSize << "'";
			cout << " to data '" << diff << "'" << endl;

			/* Creates the new data buffer */
			buffer = new char[currentHeaderSize + diff];

			/* Copies the header to buffer */
			memcpy(
					(void*)&buffer[0],
					(void*)&sectionHeader, currentHeaderSize);

			/* Copies the payload to buffer */
			memcpy(
					(void*)&buffer[currentHeaderSize],
					(void*)&data[offset], diff);

			/* Creates the new section */
#if HAVE_COMPSUPPORT
			currentSection = ((TSSectionCreator*)(cm->getObject(
					"TransportSection")))(buffer, diff + currentHeaderSize);

#else
			currentSection = new TransportSection(
					buffer, diff + currentHeaderSize);
#endif

			delete buffer;
			buffer = NULL;

		/* The Header is ready */
		} else if (pack->getStartIndicator()) {
#if HAVE_COMPSUPPORT
			currentSection = ((TSSectionCreator*)(cm->getObject(
					"TransportSection")))(&data[offset], diff);

#else
			currentSection = new TransportSection(
					&data[offset], diff);
#endif

		} else {
			/*cout << "SectionFilter::verifyAndCreateSection ";
			cout << "nothing to do";
			cout << "current header size is '";
			cout << currentHeaderSize << "' current section address is '";
			cout << currentSection << "' TS packet: ";
			pack->print();
			cout << endl;*/
		}

		return setSectionParameters(pack);
	}

	bool SectionFilter::setSectionParameters(ITSPacket* pack){
		/* First of all verifies if the currentSection is OK */
		if (!currentSection) {
			return false;
		}

		/* Verifies if the TransportSection has been created! */
		if (!(currentSection->isConstructionFailed())) {
			//_error("SectionFilter::receiveTSPacket Failed to create Section!\n");
			ignore(currentSection);
			return false;
		}

		currentSection->setESId(pack->getPid());

		/*_debug(
				"SectionFilter::setSectionParameters "
				"Section %s created with secNUm=%d, lasSec=%d and secLen=%d\n",
				currentSection->getSectionName().c_str(),
				currentSection->getSectionNumber(),
				currentSection->getLastSectionNumber(),
				currentSection->getSectionLength());*/

		if (currentSection->isConsolidated()) {
			process(currentSection);

		} else {
			this->lastContinuityCounter = pack->getContinuityCounter();
		}

		currentHeaderSize = 0;
		memset(sectionHeader, 0, ARRAY_SIZE(sectionHeader));
		return true;
	}

}
}
}
}
}
}
