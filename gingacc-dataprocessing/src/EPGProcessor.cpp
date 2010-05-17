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

#include "../include/EPGProcessor.h"

#include "../config.h"

#if HAVE_COMPSUPPORT
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#include "../../gingacc-tsparser/include/ServiceInfo.h"
#include "../../gingacc-tsparser/include/EventInfo.h"
#include "../../gingacc-tsparser/include/ShortEventDescriptor.h"
#include "../../gingacc-tsparser/include/LogoTransmissionDescriptor.h"
#endif

#include "tsparser/IServiceInfo.h"
#include "tsparser/IEventInfo.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser::si;

#include "tsparser/IMpegDescriptor.h"
using namespace::br::pucrio::telemidia::ginga::core::tsparser;

#include "tsparser/IShortEventDescriptor.h"
#include "tsparser/IExtendedEventDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser::si::descriptors;

#include "../include/IEPGListener.h"
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace epg {
	//set<string>* EPGProcessor::cdt = new set<string>;
	//int EPGProcessor::files = 0;

#if HAVE_COMPSUPPORT
	static IComponentManager* cm = IComponentManager::getCMInstance();
#endif

//TODO: test CDT

	EPGProcessor::EPGProcessor() {
		set<string>* cdt   = new set<string>;
		int files          = 0;
		eventPresent       = new map<unsigned int, IEventInfo*>;
		eventSchedule      = new map<unsigned int, IEventInfo*>;
		//epgListeners       = new map<string, set<IEPGListener*>*>;
		epgListeners       = NULL;
		dataProcessor      = NULL;
		processedSections  = new set<string>;
		presentMapReady    = false;
		scheduleMapReady   = false;
	}

	EPGProcessor::~EPGProcessor() {
		/*TODO:
		 * delete listeners vector/set/map, but do NOT delete all listeners!
		 */

		map<unsigned int, IEventInfo*>::iterator i;

		for (i = eventPresent->begin(); i != eventPresent->end(); ++i) {
			delete i->second;
		}

		delete eventPresent;
		eventPresent = NULL;

		for (i = eventSchedule->begin(); i != eventSchedule->end(); ++i) {
			delete i->second;
		}

		delete eventSchedule;
		eventSchedule = NULL;

		if (epgListeners != NULL) {
			delete epgListeners;
			epgListeners  = NULL;
		}

		delete processedSections;
		processedSections = NULL;

		_instance = NULL;
	}

	EPGProcessor* EPGProcessor::_instance = NULL;

	EPGProcessor* EPGProcessor::getInstance() {
		if (EPGProcessor::_instance == NULL) {
			EPGProcessor::_instance = new EPGProcessor();
		}
		return EPGProcessor::_instance;
	}

	void EPGProcessor::setDataProcessor(IDataProcessor* dataProcessor) {
		this->dataProcessor = dataProcessor;
	}

/*
 * The EPG events could only be resquested by Lua node. When a Lua node request
 * epg events, the Lua Player register himself as epgListener associated with
 * request (more than one Lua could make the same request, making 1 request to n
 * epgListener association).
 * The standard defines three general request types:
 * 1 - current event (with desired metadata fiedls)
 * 2 - next event to a specified event (with desired metadata fields)
 * 3 - schedule (with desired time range and desired metadata fields)
 *
 * The EPGProcessor only process EIT table if there is at least one epgListener.
 *
 */
	void EPGProcessor::addEPGListener(IEPGListener* listener, string request) {
		if (epgListeners == NULL) {
			epgListeners = new set<IEPGListener*>;
		}

		if (epgListeners->empty()) {
			dataProcessor->createPidSectionFilter(SDT_PID); //SDT
			dataProcessor->createPidSectionFilter(EIT_PID); //EIT
			dataProcessor->createPidSectionFilter(CDT_PID); //CDT
		}

		epgListeners->insert(listener);
	}

	void EPGProcessor::removeEPGListener(IEPGListener* listener) {
		set<IEPGListener*>::iterator i;

		i = epgListeners->find(listener);
		if (i != epgListeners->end()) {
			epgListeners->erase(i);
		}

		if (epgListeners->empty()) {
			//TODO: removePidSectionFilter
		}
	}

	void EPGProcessor::decodeSdtSection(ITransportSection* section) {
		IServiceInfo* srvi;
		//ILogoTransmissionDescriptor* ltd;
		unsigned int payloadSize;
		char* data;
		int fd, rval;
		unsigned short originalNetworkId;
		size_t pos, remainingBytesDescriptor, value;

		cout << "EPGProcessor::decodeSdtSection decoding SDT section";
		payloadSize = section->getPayloadSize();
		cout << " with payloadSize = "<< payloadSize << endl;

		data = new char[payloadSize];
		memcpy((void*)&(data[0]), section->getPayload(), payloadSize);

		pos = 0;
		//originalNetworkId = (((data[0] << 8) & 0xFF00) | (data[1] & 0xFF));
		originalNetworkId = (((data[pos] << 8) & 0xFF00) | (data[pos+1] & 0xFF));

		cout << "OriginalNetworkId: " << originalNetworkId << endl;

		//pos = 3; //jumping reserved... it points to service_id
		pos += 3;
		while (pos < payloadSize) {
			//there's at least one serviceinfo
#if HAVE_COMPSUPPORT
			srvi = ((ServiceInfoCreator*)(cm->getObject("ServiceInfo")))();
#else
			srvi = new ServiceInfo();
#endif
			pos = srvi-> process(data, pos);
			//srvi->print();
		}
		cout << "EPGProcessor::decodedSdtSection section decoded" << endl;
	}

	void EPGProcessor::addProcessedSection(ITransportSection* section) {
		string newName, sectionName;

		sectionName = section->getSectionName();
		newName =  sectionName + itos(section->getSectionNumber());
		processedSections->insert(newName);

		/* with tableId = 0x4E, when sections 0 and 1 are processed, the map is
		 * ready to go.
		 * With tableId for schedule, the sections numbers could be anyone, so
		 * is impossible to know if all sections are processed before the last
		 * section is received twice. This verification is done by checkSection
		 * function.
		 */
		if (section->getTableId() == 0x4E) {
			if (processedSections->count(sectionName + itos(0)) &&
				processedSections->count(sectionName + itos(1))) {

				cout << "EPGProcessor::addProcessedSection presentMap ready!";
				cout << endl;

				generateMap(eventPresent);
				presentMapReady = true;
			}
		}
	}

	bool EPGProcessor::checkSection(ITransportSection* section) {
		unsigned int tableId, sectionNumber, lastSectionNumber;
		string newSectionName;

		if (section->getPayloadSize() <= 6) {
			//cout << "EPGProcessor::checkSection discarding section" << endl;
			return false;
		}

		sectionNumber      = section->getSectionNumber();
		lastSectionNumber  = section->getLastSectionNumber();
		newSectionName     = section->getSectionName() + itos(sectionNumber);

		if (processedSections->count(newSectionName) > 0) {

			tableId = section->getTableId();
			/*cout << "EPGProcessor::checkSection section exists:!";
			cout << endl;

			cout << "  -TableId = " << hex    << tableId << dec;
			cout << "    -SectionVersion = "  << section->getVersionNumber();
			cout << "    -SectionNumber = "   << sectionNumber ;
			cout << "	 -LastSectionNumber=" << lastSectionNumber;
			cout << "    -SectionName="       << newSectionName << endl;*/

			if (sectionNumber == lastSectionNumber) {
				//cout << "EPGProcessor::checkSection is last version: ";

				if (tableId == 0x4E ) {
					if (!presentMapReady) {
						//cout << "presentMap ready to move" << endl;
						presentMapReady = true;

					} else {
						//cout << "presentMap is already ready" << endl;
					}

				} else if ((tableId <= 0x50 && tableId >= 0x5F) &&
						!scheduleMapReady){

					//cout << "scheduleMap ready to move" << endl;
					scheduleMapReady = true;
				}
			}
			return false;
		}
		return true;
	}

	void EPGProcessor::decodeEitSection(ITransportSection* section) {
		unsigned int payloadSize, transportStreamId, originalNetworkId;
		unsigned int segmentLastSectionNumber, lastTableId, sectionLength;
		unsigned int sectionVersion, tableId, sectionNumber, lastSectionNumber;
		string sectionName;
		IEventInfo* ei;
		set<IEventInfo*>* eit;
		char* data;
		size_t pos;
		map<unsigned int, IEventInfo*>::iterator i;

		tableId     = section->getTableId();
		payloadSize = section->getPayloadSize();

		if (!checkSection(section)) {
			return;
		}

		//cout << "EPGProcessor::decodeEitSection with tableId = " << hex ;
		//cout << tableId << dec <<  " and payloadSize = ";
		//cout << payloadSize << endl;

		sectionLength     = section->getSectionLength();
		sectionVersion    = section->getVersionNumber();
		sectionNumber     = section->getSectionNumber();
		lastSectionNumber = section->getLastSectionNumber();
		sectionName       = section->getSectionName();

		data = new char[payloadSize];
		memcpy((void*)&(data[0]), section->getPayload(), payloadSize);

		pos = 0;
		transportStreamId = (((data[pos] << 8) & 0xFF00) |
				(data[pos+1] & 0xFF));

		pos += 2;
		originalNetworkId = (((data[pos] << 8) & 0xFF00) |
				(data[pos+1] & 0xFF));

		pos += 2;
		segmentLastSectionNumber = data[pos];
		pos++;
		lastTableId = data[pos];
/*
		cout << "TransportStreamId: " << transportStreamId << endl;
		cout << "OriginalNetworkId: " << originalNetworkId << endl;
		cout << endl;

		cout << "TableId: " << hex << tableId << dec << endl;
		cout << "SectionVersion: "   << sectionVersion << endl;
		cout << "SectionNumber:"     << sectionNumber << endl;
		cout << "LastSectionNumber:" << lastSectionNumber << endl;
		cout << "SectionName: " << sectionName << endl;
*/
		pos++; //pos = 6;
		while (pos < payloadSize) {
#if HAVE_COMPSUPPORT
			ei = ((EICreator*)(cm->getObject("EventInfo")))();
#else
			ei = new EventInfo();
#endif

			pos = ei->process(data, pos);

			//TODO: clean this mess.
			//ei->setTableId(tableId);
			//ei->setSectionVersion(sectionVersion);
			//ei->setSectionNumber(sectionNumber);

			if (tableId == 0x4E) {
				(*eventPresent)[sectionNumber] = ei;

			} else if (tableId >= 0x50 && tableId <= 0x5F) {
				(*eventSchedule)[sectionNumber] = ei;
			}

			addProcessedSection(section);
			//cout << "EPGProcessor::decodedEitSection ei process";
			//cout << " finished with ";
			//cout << "pos = " << pos << endl;
		}
	}

	void EPGProcessor::generateMap(map<unsigned int, IEventInfo*>* actualMap) {
		map<string, struct Field> responseMap, data;
		IEventInfo* ei;
		map<unsigned int, IEventInfo*>::iterator i ;
		vector<IMpegDescriptor*>::iterator j;
		struct Field field, fieldMap;
		IShortEventDescriptor* sed;
		vector<IMpegDescriptor*>* descs;
		string name;
		set<IEPGListener*>::iterator k;

		//cout << "EPGProcessor::generateMap beginning" << endl;
		for (i = actualMap->begin(); i != actualMap->end(); ++i) {
			ei = i->second;
			if (ei != NULL) {

				field.str = itos(ei->getEventId());
				(responseMap)["id"] =  field;

				field.str = ei->getStartTimeSecsStr();
				(responseMap)["startTime"] = field;

				field.str = ei->getEndTimeSecsStr();
				(responseMap)["endTime"] = field;
				descs = ei->getDescriptors();

				/*cout << "EPGProcessor::generateMap printing:" << endl;
				cout << "  -id = " << (responseMap)["id"].str;
				cout << "  -startTime = " << (responseMap)["startTime"].str;
				cout << "  -endTime = " << (responseMap)["endTime"].str;*/

				if (descs != NULL) {
					for (j = descs->begin(); j != descs->end(); ++j) {
						switch ((*j)->getDescriptorTag()) {
							case IEventInfo::SHORT_EVENT:
								sed = (IShortEventDescriptor*)(*j);
								field.str = sed->getEventName();
								(responseMap)["name"] = field;
								//cout << "  -name = ";
								//cout << (responseMap)["name"].str;

								field.str = sed->getTextChar();
								(responseMap)["shortDescription"] = field;
								//cout << "  -shortDescription = ";
								//cout << (responseMap)["shortDescription"].str;
								break;

							case IEventInfo::PARENTAL_RATING:
								break;

							default:
								break;
						}
					}
				}
				//cout << endl;
				name = "evt" + itos(ei->getEventId());
				fieldMap.table = responseMap;
				(data)[name] = fieldMap;
				//cout << "(evt belongs to data[" << name << "])" << endl;
			}
		}

		//printFieldMap(&data);
		if (epgListeners != NULL && !epgListeners->empty()) {
			for (k = epgListeners->begin(); k != epgListeners->end(); ++k) {
				(*k)->pushEPGEvent(data);
			}

		} else {
			cout << "EPGProcessor::generateMap there is no listener to notify";
			cout << endl;
		}
	}

	void EPGProcessor::printFieldMap(map<string, struct Field>* fieldMap) {
		map<string, struct Field>::iterator i;

		cout << "EPGProcesor::printFieldMap printing..." << endl;
		for (i = fieldMap->begin(); i!= fieldMap->end(); ++i){
			cout << i->first << " = ";
			if (i->second.str.empty()){
				if (i->second.table.empty()){
					cout << "all empty"<< endl;

				} else {
					cout << "map: { " << endl;
					printFieldMap(&(i->second.table));
					cout << "}" << endl;
				}

			} else {
				cout << i->second.str << endl;
			}
		}
	}

	struct Field* EPGProcessor::handleFieldStr(string str) {
		struct Field* field;
	//	cout << "EPGProcessor::handleFieldstr with str = " << str << endl;
		field = new struct Field;

		if (str == "") {
			field->str = "0";
			return field;
		}

		field->str = str;
		return field;
	}

	void EPGProcessor::decodeCdt(string fileName) {
		char data[4084];
		int fd, rval, pngSize, totalSize, times, remainder, pos;
		int originalNetworkId/*, descriptorsLoopLength*/;

		cout << "Decoding CDT stream..." << endl << endl;

		fd = open(fileName.c_str(), O_RDONLY|O_LARGEFILE);
		pngSize = 0;
		pos = 0;

		totalSize = (int)lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);

		remainder = totalSize % 4084;
		times = ((totalSize - remainder) / 4084);

		pngSize = (times * 4079) + (remainder - 5);

		char pngData[pngSize];

		cout << "CDT reading first blocks. pngSize is '" << pngSize << "'";
		cout << endl;
		for (int i = 0; i < times; i++) {
			rval = read(fd, (void*)&(data[0]), 4084);
			if (rval == 4084) {
				originalNetworkId = (((data[0] << 8) & 0xFF00) |
					(data[1] & 0xFF));

				//TODO: check data_type!!!

				//descriptorsLoopLength = ((((data[3] & 0x0F) << 8) & 0xFF00) |
					//	(data[4] & 0xFF));

				memcpy(pngData+pos, data+5, 4079);
				pos += 4079;

			} else {
				cout << "CDT Warning! Can't read 4079 block of bytes.";
				cout << " pos = '" << pos << "'" << endl;
				return;
			}
		}

		cout << "CDT reading remainder." << endl;
		rval = read(fd, (void*)&(data[0]), remainder);
		if (rval == remainder) {
			memcpy(pngData+pos, data+5, remainder);

		} else {
			cout << "CDT Warning! Can't read remaining of '" << remainder;
			cout << "' bytes. pos = '" << pos << "'" << endl;
			return;
		}

		close(fd);

		this->savePNG(pngData, pngSize);
		cout << "Stream decoded successfully. PngSize is '" << pngSize;
		cout << "', pos is '" << pos + remainder << "'" << endl;
	}

	int EPGProcessor::savePNG(char* pngData, int pngSize) {
		FILE* png;
		size_t wc;
		string path;

		path = itos(files) + ".png";
		files++;
		if (pngData == NULL) {
			cout << "No data to save." << endl;
			return 1;
		}

		cout << "Creating png file." << endl;
		png = fopen(path.c_str(), "wb");
		if (png == NULL) {
			cout << "cannot open PNG file. (" << path << ")" << endl;
			return 2;
		}

		cout << "Writing data to png file." << endl;
		wc = fwrite(pngData, 1, pngSize, png);
		if (wc != pngSize) {
			cout << "Writing error." << endl;
			return 3;
		}

		cout << "Closing png file." << endl;
		cdt->insert(path);

		fclose(png);
		return 0;
	}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::dataprocessing::epg::IEPGProcessor*
		createEPGP() {

	return (::br::pucrio::telemidia::ginga::core::dataprocessing::epg::
			EPGProcessor::getInstance());
}

extern "C" void destroyEPGP(::br::pucrio::telemidia::ginga::core::
		dataprocessing::epg::IEPGProcessor* epgp) {

	delete epgp;
}
