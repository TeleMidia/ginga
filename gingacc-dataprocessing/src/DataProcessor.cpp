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

#include "../include/DataProcessor.h"
#include "../include/dsmcc/StreamEvent.h"

//TODO: clean this mess
#include "../include/dsmcc/npt/DSMCCSectionPayload.h"


namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
	DataProcessor::DataProcessor() : Thread() {
		eventListeners  = new map<string, set<IStreamEventListener*>*>;
		objectListeners = new set<IObjectListener*>;
		epgListeners    = new set<IEPGListener*>;
		filterManager   = new FilterManager();
		processedIds    = new set<unsigned int>;

		processors      = new map<unsigned int, MessageProcessor*>;
		nptProcessor    = NULL;
		sdl             = NULL;

		sections        = new vector<ITransportSection*>;

		removeOCFilter  = false;
		demux           = NULL;
		start();

		pthread_mutex_init(&mutex, NULL);

		mkdir("carousel", 0777);
		mkdir("carousel/modules", 0777);

		mkdir("epg", 0777);
		mkdir("epg/data", 0777);
	}

	DataProcessor::~DataProcessor() {
		running = false;

		if (eventListeners != NULL) {
			delete eventListeners;
			eventListeners = NULL;
		}

		if (objectListeners != NULL) {
			delete objectListeners;
			objectListeners = NULL;
		}

		if (epgListeners != NULL) {
			delete epgListeners;
			epgListeners = NULL;
		}

		if (filterManager != NULL) {
			delete filterManager;
			filterManager = NULL;
		}

		if (processors != NULL) {
			delete processors;
			processors = NULL;
		}

		if (processedIds != NULL) {
			delete processedIds;
			processedIds = NULL;
		}

		if (nptProcessor != NULL) {
			delete nptProcessor;
			nptProcessor = NULL;
		}

		if (sections != NULL) {
			delete sections;
			sections = NULL;
		}

		pthread_mutex_destroy(&mutex);
	}

	void DataProcessor::serviceDomainMounted(
			map<string, string>* names, map<string, string>* paths) {

		if (removeOCFilter && demux != NULL) {
			cout << "DataProcessor::run requesting ";
			cout << "OC filter destroy" << endl;
			filterManager->destroyFilter(demux, STREAM_TYPE_DSMCC_TYPE_B);
		}

		if (sdl != NULL) {
			sdl->serviceDomainMounted(names, paths);
		}
	}

	void DataProcessor::removeOCFilterAfterMount(bool removeIt) {
		removeOCFilter = removeIt;
	}

	void DataProcessor::setSTCProvider(ISTCProvider* stcProvider) {
		nptProcessor = new NPTProcessor(stcProvider);
	}

	ITimeBaseProvider* DataProcessor::getNPTProvider() {
		return nptProcessor;
	}

	void DataProcessor::createStreamTypeSectionFilter(
			short streamType, IDemuxer* demux) {

		cout << "DataProcessor::createStreamTypeSectionFilter '";
		cout << streamType << "'" << endl;
		this->demux = demux;
		filterManager->createStreamTypeSectionFilter(streamType, demux, this);
	}

	void DataProcessor::createPidSectionFilter(int pid, IDemuxer* demux) {
		cout << "DataProcessor::createPidSectionFilter '";
		cout << pid << "'" << endl;

		this->demux = demux;
		filterManager->createPidSectionFilter(pid, demux, this);
	}

	void DataProcessor::addEPGListener(IEPGListener* listener) {
		cout << "DataProcessor::addEPGListener" << endl;
		epgListeners->insert(listener);
	}

	void DataProcessor::addSEListener(
			string eventType, IStreamEventListener* listener) {

		map<string, set<IStreamEventListener*>*>::iterator i;
		set<IStreamEventListener*>* listeners;

		cout << "DataProcessor::addSEListener" << endl;
		i = eventListeners->find(eventType);
		if (i != eventListeners->end()) {
			listeners = i->second;
			if (listeners == NULL)
				listeners = new set<IStreamEventListener*>;

			listeners->insert(listener);

		} else {
			listeners = new set<IStreamEventListener*>;
			listeners->insert(listener);
			(*eventListeners)[eventType] = listeners;
		}
	}

	void DataProcessor::removeSEListener(
			string eventType, IStreamEventListener* listener) {

		map<string, set<IStreamEventListener*>*>::iterator i;
		set<IStreamEventListener*>::iterator j;
		set<IStreamEventListener*>* listeners;

		i = eventListeners->find(eventType);
		if (i != eventListeners->end()) {
			listeners = i->second;
			if (listeners != NULL) {
				j = listeners->find(listener);
				if (j != listeners->end()) {
					listeners->erase(j);
					return;
				}
			}
		}
	}

	void DataProcessor::setServiceDomainListener(
			IServiceDomainListener* listener) {

		sdl = listener;
	}

	void DataProcessor::addObjectListener(IObjectListener* l) {
		objectListeners->insert(l);
	}

	void DataProcessor::removeObjectListener(IObjectListener* l) {
		set<IObjectListener*>::iterator i;

		i = objectListeners->find(l);
		if (i != objectListeners->end()) {
			objectListeners->erase(i);
		}
	}

	void* DataProcessor::notifySEListener(void* ptr) {
		struct notifyData* data;
		IStreamEventListener* listener;
		IStreamEvent* se;
		pthread_mutex_t* mtx;

		data = (struct notifyData*)ptr;
		se = data->se;
		listener = data->listener;
		mtx = data->mutex;

		delete data;
		data = NULL;

		pthread_mutex_unlock(mtx);

		listener->receiveStreamEvent(se);
		return NULL;
	}

	void DataProcessor::notifySEListeners(IStreamEvent* se) {
		map<string, set<IStreamEventListener*>*>::iterator i;
		set<IStreamEventListener*>* listeners;
		set<IStreamEventListener*>::iterator j;
		pthread_t notifyThreadId_;
		struct notifyData* data = NULL;
		string eventName = se->getEventName();

		cout << "DataProcessor::notifySEListeners for eventName '";
		cout << eventName << "'" << endl;
		if (eventListeners->count(eventName) != 0) {
			listeners = (*eventListeners)[eventName];
			j = listeners->begin();
			while (j != listeners->end()) {
				//(*j)->receiveStreamEvent(se);
				pthread_mutex_lock(&mutex);
				data = new struct notifyData;
				data->listener = *j;
				data->se = se;
				data->mutex = &mutex;

				pthread_create(
						&notifyThreadId_,
						0, DataProcessor::notifySEListener, (void*)data);

				pthread_detach(notifyThreadId_);

				++j;
			}
		}
	}

	void DataProcessor::notifyEitListeners(set<IEventInfo*>* events) {
		set<IEPGListener*>::iterator i;

		i = epgListeners->begin();
		while (i != epgListeners->end()) {
			(*i)->receiveEventInfo(events);
			++i;
		}
	}

	void DataProcessor::receiveSection(ITransportSection* section) {
		IStreamEvent* se;
		string sectionName;
		set<IEventInfo*>* eit;
		set<unsigned int>::iterator i;
		char* payload;
		short tableId;

		//TODO: clean this mess
		DSMCCSectionPayload* dsmccSection;

		if (section != NULL) {
			tableId = section->getTableId();

			//stream event
			if (tableId == STE_TID) {

				//filterManager->addProcessedSection(section->getSectionName());

				payload = (char*)(section->getPayload());

				if ((payload[0] & 0xFF) == IStreamEvent::SE_DESCRIPTOR_TAG ||
						(payload[0] & 0xFF) == 0x1a) {

					se = new StreamEvent(
							section->getPayload(), section->getPayloadSize());

					//i = processedIds->find(se->getId());
					//if (i == processedIds->end()) {
						cout << "DataProcessor::receiveSection STE" << endl;

						processedIds->insert(se->getId());
						//TODO: get stream event object from oc
						se->setEventName("gingaEditingCommands");

						// notify event listeners
						notifySEListeners(se);

					//} else {
						//delete se;
					//}

				} else {
					//TODO: we have to organize this mess
					dsmccSection = new DSMCCSectionPayload(
							payload, section->getPayloadSize());

					nptProcessor->decodeNPT(
							dsmccSection->getDsmccDescritorList());
				}

				delete section;
				section = NULL;

			//object carousel 0x3B = MSG, 0x3C = DDB
			} else if (tableId == OCI_TID || tableId == OCD_TID) {
				lock();
				//cout << "DataProcessor::receiveSection OC" << endl;
				sections->push_back(section);
				unlock();
				unlockConditionSatisfied();

			//SDT
			} else if (tableId == SDT_TID) {
				cout << "DataProcessor::receiveSection SDT" << endl;
				sectionName = section->getSectionName();
				if (filterManager->processSection(section)) {
					EPGProcessor::decodeSdt(sectionName);
					cout << "SDT MONTADA!!!" << endl;
				}

			//EIT
			} else if (tableId == EIT_TID) {
				cout << "DataProcessor::receiveSection EIT" << endl;
				sectionName = section->getSectionName();
				if (filterManager->processSection(section)) {
					// notify eit listeners
					cout << "decoding EIT" << endl;
					eit = EPGProcessor::decodeEit(sectionName);
					if (eit != NULL) {
						notifyEitListeners(eit);
						delete eit;
					}
					cout << "EIT MONTADA" << endl;
				}

			//CDT
			} else if (tableId == CDT_TID) {
				cout << "DataProcessor::receiveSection CDT" << endl;
				sectionName = section->getSectionName();
				if (filterManager->processSection(section)) {
					EPGProcessor::decodeCdt(sectionName);
					cout << "CDT MONTADA!!!" << endl;
				}
			}
		}
	}

	void DataProcessor::run() {
		int pid;
		ITransportSection* section;
		DsmccMessageHeader* message;
		ServiceDomain* sd = NULL;
		string sectionName;
		MessageProcessor* processor;
		bool hasSection;

		running = true;
		while (running) {
			//cout << "DataProcessor::run checking tasks" << endl;
			lock();
			if (sections->empty()) {
				unlock();
				waitForUnlockCondition();

			} else {
				section = *(sections->begin());
				sections->erase(sections->begin());
				unlock();

				do {
					//cout << "DataProcessor::run call FM->processSec" << endl;

					//we must to acquire pid and section name before process
					//the section
					pid         = section->getESId();
					sectionName = section->getSectionName();

					if (filterManager->processSection(section)) {
						message     = new DsmccMessageHeader(sectionName, pid);
						if (processors->count(pid) == 0) {
							processor = new MessageProcessor();
							(*processors)[pid] = processor;

						} else {
							processor = (*processors)[pid];
						}

						sd = processor->pushMessage(message);
						if (sd != NULL) {
							sd->setObjectsListeners(objectListeners);
							sd->setServiceDomainListener(this);
							processor->checkTasks();
						}
					}

					lock();
					hasSection = !sections->empty();
					if (hasSection) {
						section = *(sections->begin());
						sections->erase(sections->begin());
					}
					unlock();

				} while (hasSection);
			}
		}

		cout << "DataProcessor::run all done!" << endl;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::dataprocessing::IDataProcessor*
		createDP() {

	return (new ::br::pucrio::telemidia::ginga::core::dataprocessing::
			DataProcessor());
}

extern "C" void destroyDP(::br::pucrio::telemidia::ginga::core::
		dataprocessing::IDataProcessor* dp) {

	delete dp;
}
