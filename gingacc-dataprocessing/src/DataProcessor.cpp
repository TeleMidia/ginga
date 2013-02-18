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

#include "dataprocessing/DataProcessor.h"
#include "dataprocessing/dsmcc/StreamEvent.h"

//TODO: clean this mess
#include "dataprocessing/dsmcc/npt/DSMCCSectionPayload.h"

#include "config.h"

#if HAVE_COMPSUPPORT
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#include "tsparser/AIT.h"
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

	DataProcessor::DataProcessor() : Thread() {
		filterManager   = new FilterManager();
		nptProcessor    = NULL;
		sdl             = NULL;
		removeOCFilter  = false;
		demux           = NULL;
		ait             = NULL;

		Thread::mutexInit(&mutex, true);

		startThread();

		SystemCompat::makeDir("carousel", 0777);
		SystemCompat::makeDir("carousel/modules", 0777);

		//TODO: remove all garbage from epg processor before start using it
		//epgProcessor = EPGProcessor::getInstance();
		//epgProcessor->setDataProcessor(this);
	}

	DataProcessor::~DataProcessor() {
		running = false;

		eventListeners.clear();
		objectListeners.clear();

		if (filterManager != NULL) {
			delete filterManager;
			filterManager = NULL;
		}

		processors.clear();
		processedIds.clear();

		if (nptProcessor != NULL) {
			delete nptProcessor;
			nptProcessor = NULL;
		}

		sections.clear();

		if (ait != NULL) {
			delete ait;
			ait = NULL;
		}

		//TODO: remove all garbage from epg processor before start using it
		/*if (epgProcessor != NULL) {
			epgProcessor->release();
			epgProcessor = NULL;
		}*/

		Thread::mutexDestroy(&mutex);
	}

	void DataProcessor::applicationInfoMounted(IAIT* ait) {
		if (sdl != NULL) {
			sdl->applicationInfoMounted(ait);
		}
	}

	void DataProcessor::serviceDomainMounted(
			string mountPoint,
			map<string, string>* names,
			map<string, string>* paths) {

		if (removeOCFilter && demux != NULL) {
			clog << "DataProcessor::run requesting ";
			clog << "OC filter destroy" << endl;
			filterManager->destroyFilter(demux, STREAM_TYPE_DSMCC_TYPE_B);
		}

		if (sdl != NULL) {
			sdl->serviceDomainMounted(mountPoint, names, paths);
		}
	}

	void DataProcessor::setDemuxer(IDemuxer* demux) {
		this->demux = demux;
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

	void DataProcessor::createStreamTypeSectionFilter(short streamType) {
		clog << "DataProcessor::createStreamTypeSectionFilter '";
		clog << streamType << "'" << endl;
		filterManager->createStreamTypeSectionFilter(streamType, demux, this);
	}

	void DataProcessor::createPidSectionFilter(int pid) {
		clog << "DataProcessor::createPidSectionFilter '";
		clog << pid << "'" << endl;

		filterManager->createPidSectionFilter(pid, demux, this);
	}

	void DataProcessor::addSEListener(
			string eventType, IStreamEventListener* listener) {

		map<string, set<IStreamEventListener*>*>::iterator i;
		set<IStreamEventListener*>* listeners;

		clog << "DataProcessor::addSEListener" << endl;
		i = eventListeners.find(eventType);
		if (i != eventListeners.end()) {
			listeners = i->second;
			if (listeners == NULL) {
				listeners = new set<IStreamEventListener*>;
				eventListeners[eventType] = listeners;
			}

			listeners->insert(listener);

		} else {
			listeners = new set<IStreamEventListener*>;
			listeners->insert(listener);
			eventListeners[eventType] = listeners;
		}
	}

	void DataProcessor::removeSEListener(
			string eventType, IStreamEventListener* listener) {

		map<string, set<IStreamEventListener*>*>::iterator i;
		set<IStreamEventListener*>::iterator j;
		set<IStreamEventListener*>* listeners;

		i = eventListeners.find(eventType);
		if (i != eventListeners.end()) {
			listeners = i->second;
			if (listeners != NULL) {
				j = listeners->find(listener);
				if (j != listeners->end()) {
					listeners->erase(j);
				}
			}
		}
	}

	void DataProcessor::setServiceDomainListener(
			IServiceDomainListener* listener) {

		sdl = listener;
	}

	void DataProcessor::addObjectListener(IObjectListener* l) {
		objectListeners.insert(l);
	}

	void DataProcessor::removeObjectListener(IObjectListener* l) {
		set<IObjectListener*>::iterator i;

		i = objectListeners.find(l);
		if (i != objectListeners.end()) {
			objectListeners.erase(i);
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

		Thread::mutexUnlock(mtx);

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

		clog << "DataProcessor::notifySEListeners for eventName '";
		clog << eventName << "'" << endl;
		if (eventListeners.count(eventName) != 0) {
			listeners = eventListeners[eventName];
			j = listeners->begin();
			while (j != listeners->end()) {
				//(*j)->receiveStreamEvent(se);
				Thread::mutexLock(&mutex);
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

	void DataProcessor::receiveSection(ITransportSection* section) {
		IStreamEvent* se;
		string sectionName;
		set<unsigned int>::iterator i;
		char* payload;
		short tableId;

		//TODO: clean this mess
		DSMCCSectionPayload* dsmccSection;

		if (section != NULL) {
			tableId = section->getTableId();

			//stream event
			if (tableId == DDE_TID) {
				//filterManager->addProcessedSection(section->getSectionName());

				payload = (char*)(section->getPayload());

				/*clog << "DataProcessor::receiveSection DSM-CC descriptor";
				clog << "tag = '" << (payload[0] & 0xFF) << "'" << endl;*/

				if ((payload[0] & 0xFF) == IMpegDescriptor::STR_EVENT_TAG ||
						(payload[0] & 0xFF) == 0x1a) {

					se = new StreamEvent(
							section->getPayload(), section->getPayloadSize());

					//i = processedIds.find(se->getId());
					//if (i == processedIds.end()) {377
						//clog << "DataProcessor::receiveSection STE" << endl;

						processedIds.insert(se->getId());
						//TODO: get stream event object from oc
						se->setEventName("gingaEditingCommands");

						// notify event listeners
						notifySEListeners(se);

					//} else {
						//delete se;
					//}

				} else if ((payload[0] & 0xFF) ==
								IMpegDescriptor::NPT_REFERENCE_TAG ||

						(payload[0] & 0xFF) ==
								IMpegDescriptor::NPT_ENDPOINT_TAG) {

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
				//clog << "DataProcessor::receiveSection OC" << endl;
				sections.push_back(section);
				unlock();
				unlockConditionSatisfied();

			//AIT
			} else if (tableId == AIT_TID) {
				//clog << "DataProcessor::receiveSection AIT" << endl;

				if (ait != NULL &&
						ait->getSectionName() == section->getSectionName()) {

					return;
				}

				if (ait != NULL) {
					delete ait;
					ait = NULL;
				}

#if HAVE_COMPSUPPORT
				ait = ((AITCreator*)(cm->getObject("AIT")))();
#else
				ait = new AIT();
#endif

				ait->setSectionName(section->getSectionName());
				ait->setApplicationType(section->getExtensionId());
				clog << "DataProcessor::receiveSection AIT calling process";
				clog << endl;
				ait->process(section->getPayload(), section->getPayloadSize());

				applicationInfoMounted(ait);

			//SDT
			} else if (tableId == SDT_TID) {
				//clog << "DataProcessor::receiveSection SDT" << endl;
				//TODO: remove all garbage from epg processor before start using it
				//epgProcessor->decodeSdtSection(section);
				delete section;
				section = NULL;

			// EIT present/following and schedule 
			} else if ( tableId == EIT_TID || //EIT present/following in actual TS
						(tableId >= 0x50 && tableId <= 0x5F)) { //EIT schedule in actual TS
					
				/*TODO: TS files don't have EITs p/f and sched in other TS.
				 tableId == 0x4F (p/f) and tableId >= 0x60 && tableId <= 0x6F
				 (schedule)
				*/
				
				//TODO: remove all garbage from epg processor before start using it
				//epgProcessor->decodeEitSection(section);
				delete section;
				section = NULL;

			//CDT
			} else if (tableId == CDT_TID) {
				clog << "DataProcessor::receiveSection CDT" << endl;
				sectionName = section->getSectionName();
				//TODO: TS files don't have any CDT sections.

			} else if (tableId == 0x73) {
				clog << "DataProcessor::receiveSection TOT FOUND!!!" << endl;
				//TODO: remove all garbage from epg processor before start using it
				//epgProcessor->decodeTot(section);
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
			//clog << "DataProcessor::run checking tasks" << endl;
			lock();
			if (sections.empty()) {
				unlock();
				waitForUnlockCondition();

			} else {
				section = *(sections.begin());
				sections.erase(sections.begin());
				unlock();

				do {
					//clog << "DataProcessor::run call FM->processSec" << endl;

					//we must to acquire pid and section name before process
					//the section
					pid         = section->getESId();
					sectionName = section->getSectionName();

					if (filterManager->processSection(section)) {
						message = new DsmccMessageHeader(sectionName, pid);
						if (processors.count(pid) == 0) {
							processor = new MessageProcessor();
							processors[pid] = processor;

						} else {
							processor = processors[pid];
						}

						sd = processor->pushMessage(message);
						if (sd != NULL) {
							sd->setObjectsListeners(&objectListeners);
							sd->setServiceDomainListener(this);
							processor->checkTasks();
						}
					}

					lock();
					hasSection = !sections.empty();
					if (hasSection) {
						section = *(sections.begin());
						sections.erase(sections.begin());
					}
					unlock();

				} while (hasSection);
			}
		}

		clog << "DataProcessor::run all done!" << endl;
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
