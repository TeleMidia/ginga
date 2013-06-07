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

#include "dataprocessing/FilterManager.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
	FilterManager::FilterManager() {
		this->reading = false;
		Thread::mutexInit(&filterMutex, NULL);
	}

	FilterManager::~FilterManager() {
		map<short, SectionFilter*>::iterator i;
		map<int, SectionFilter*>::iterator j;
		map<string, map<unsigned int, ITransportSection*>*>::iterator k;
		map<unsigned int, ITransportSection*>::iterator l;

		Thread::mutexLock(&filterMutex);
		i = streamTypeFilters.begin();
		while (i != streamTypeFilters.end()) {
			delete i->second;
			++i;
		}
		streamTypeFilters.clear();

		j = pidFilters.begin();
		while (j != pidFilters.end()) {
			delete j->second;
			++j;
		}
		pidFilters.clear();

		k = sections.begin();
		while (k != sections.end()) {
			if (k->second != NULL) {
				l = k->second->begin();
				while (l != k->second->end()) {
					delete l->second;
					++l;
				}

				delete k->second;
			}
			++k;
		}
		sections.clear();

		Thread::mutexUnlock(&filterMutex);
		Thread::mutexDestroy(&filterMutex);
	}

	ITSFilter* FilterManager::createStreamTypeSectionFilter(
			short streamType, IDemuxer* demux, IFilterListener* listener) {

		SectionFilter* sf = NULL;
		int tId;

		Thread::mutexLock(&filterMutex);
		if (streamTypeFilters.count(streamType) == 0) {
			sf = new SectionFilter();
			sf->setListener(listener);

			streamTypeFilters[streamType] = sf;
			if ((demux->getCaps() & DPC_CAN_DEMUXBYHW) &&
					(demux->getCaps() & DPC_CAN_FILTERPID) &&
					(demux->getCaps() & DPC_CAN_FILTERTID)) {

				tId = IDemuxer::getTableIdFromStreamType(streamType);
				if (tId >= 0) {
					demux->addFilter(sf, -1, tId);
				}

			} else {
				demux->addStreamTypeFilter(streamType, sf);
			}
		}

		Thread::mutexUnlock(&filterMutex);

		return sf;
	}

	ITSFilter* FilterManager::createPidSectionFilter(
			int pid, IDemuxer* demux, IFilterListener* listener) {

		SectionFilter* sf = NULL;

		Thread::mutexLock(&filterMutex);
		if (pidFilters.count(pid) == 0) {
			sf = new SectionFilter();
			sf->setListener(listener);

			pidFilters[pid] = sf;

			if ((demux->getCaps() & DPC_CAN_DEMUXBYHW) &&
					(demux->getCaps() & DPC_CAN_FILTERPID) &&
					(demux->getCaps() & DPC_CAN_FILTERTID)) {

				demux->addFilter(sf, pid, -1);

			} else {
				demux->addPidFilter(pid, sf);
			}
		}

		Thread::mutexUnlock(&filterMutex);
		return sf;
	}

	void FilterManager::destroyFilter(IDemuxer* demux, short streamType) {
		SectionFilter* sf;
		map<short, SectionFilter*>::iterator i;

		clog << "FilterManager::destroyFilter type '";
		clog << streamType << "'" << endl;

		Thread::mutexLock(&filterMutex);
		i = streamTypeFilters.find(streamType);
		if (i != streamTypeFilters.end()) {
			sf = i->second;
			streamTypeFilters.erase(i);
			demux->removeFilter(sf);
		}

		Thread::mutexUnlock(&filterMutex);
	}

	void FilterManager::destroyFilter(IDemuxer* demux, ITSFilter* filter) {
		map<short, SectionFilter*>::iterator i;

		clog << "FilterManager::destroyFilter filter '";
		clog << filter << "'" << endl;

		Thread::mutexLock(&filterMutex);
		i = streamTypeFilters.begin();
		while (i != streamTypeFilters.end()) {
			if (filter == i->second) {
				streamTypeFilters.erase(i);
				break;
			}
		}

		demux->removeFilter(filter);

		Thread::mutexUnlock(&filterMutex);
	}

	//TODO: destroy pid filter

	bool FilterManager::processSection(ITransportSection* section) {
		FILE* sectionFd;
		int bytesSaved;
		string sectionName;
		string::size_type len;
		unsigned int sn, lsn; //sectionNumber and lastSectionNumber
		ITransportSection* sec;
		map<unsigned int, ITransportSection*>* secs;
		unsigned int i;
		string sectionDir;

		if (section == NULL) {
			clog << "FilterManager::processSection ";
			clog << "Warning! Section is NULL" << endl;
			return false;
		}

		if (section->getPayloadSize() <= 0 ||
			    section->getPayloadSize() > 4096) {

			clog << "FilterManager::processSection ";
			clog << "Warning! Wrong payloadSize" << endl;
			return false;
		}

		sectionName = section->getSectionName();
		unsigned found = sectionName.find_last_of("/\\");
		sectionDir = sectionName.substr(0,found);
		len = sectionName.length();
		if (sectionName == "") {
			clog << "FilterManager::processSection ";
			clog << "Warning! Empty section name" << endl;
			return false;

		} else if (!isDirectory((char*)sectionDir.c_str())) {

			clog << "FilterManager::processSection ";
			clog << "Warning! Wrong section name: ";
			clog << sectionName.c_str() << endl;
			return false;
		}

		Thread::mutexLock(&filterMutex);
		if (sections.count(sectionName) == 0) {
			clog << "FilterManager::processSection creating map for section '";
			clog << sectionName << "'" << endl;
			secs = new map<unsigned int, ITransportSection*>;
			sections[sectionName] = secs;

		} else {
			secs = sections[sectionName];
		}
		Thread::mutexUnlock(&filterMutex);

		sn = section->getSectionNumber();
		lsn = section->getLastSectionNumber();
		if (secs->count(sn) != 0) {
			clog << "FilterManager Warning! Adding section '";
			clog << sectionName;
			clog << "' in an existent ";
			clog << "position = '" << sn << "'";
			clog << endl;

			return false;

		} else {
			(*secs)[sn] = section;
		}

		// All sections received.
		if (secs->size() == (lsn + 1)) {
			sectionFd = fopen(sectionName.c_str(),
					 "wb");

			if (sectionFd <= 0) {
				clog << "FilterManager Warning! error open file ";
				clog << sectionName;
				clog << "." << endl;
				return false;
			}

			i = 0;
			while (i <= lsn) {
				if (secs->count(i) == 0) {
					clog << "FilterManager Warning! cant find pos '";
					clog << i << "'" << endl;
					fclose(sectionFd);
					return false;
				}

				sec = (*secs)[i];
				bytesSaved = fwrite(
						sec->getPayload(), 1, sec->getPayloadSize(), sectionFd);

				if (bytesSaved != (int)(sec->getPayloadSize())) {
					clog << "FilterManager Warning! bytesSaved = ";
					clog << bytesSaved << " dataSize = ";
					clog << sec->getPayloadSize() << endl;
				}

				i++;
				delete sec;
				sec = NULL;
			}
			fclose(sectionFd);

			addProcessedSection(sectionName);

			delete secs;
			secs = NULL;

			clog << "FilterManager::processSection all sections ";
			clog << "received and processed for '";
			clog << sectionName << "'" << endl;

			return true;
		}

		return false;
	}

	void FilterManager::addProcessedSection(string sectionName) {
		map<short, SectionFilter*>::iterator i;
		map<int, SectionFilter*>::iterator j;
		SectionFilter* sf;

		Thread::mutexLock(&filterMutex);
		i = streamTypeFilters.begin();
		while (i != streamTypeFilters.end()) {
			clog << "FilterManager::addProcessedSection '";
			clog << sectionName << "'" << endl;
			sf = i->second;
			sf->addProcessedSection(sectionName);
			++i;
		}

		j = pidFilters.begin();
		while (j != pidFilters.end()) {
			(j->second)->addProcessedSection(sectionName);
			++j;
		}

		if (sections.count(sectionName) != 0) {
			sections.erase(sections.find(sectionName));
		}
		Thread::mutexUnlock(&filterMutex);
		/*clog << "FilterManager::addProcessedSection '" << sectionName << "'";
		clog << " done!" << endl;*/
	}
}
}
}
}
}
}
