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
#include "../../gingacc-tsparser/include/IShortEventDescriptor.h"
#include "../../gingacc-tsparser/include/LogoTransmissionDescriptor.h"
#endif

#include "tsparser/IServiceInfo.h"
#include "tsparser/IEventInfo.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace epg {
	set<string>* EPGProcessor::cdt = new set<string>;
	int EPGProcessor::files = 0;
#if HAVE_COMPSUPPORT
	static IComponentManager* cm = IComponentManager::getCMInstance();
#endif

	/*EPGProcessor::EPGProcessor() {
		EPGProcessor::eit = new set<EventInfo*>;
		EPGProcessor::cdt = new set<string>;
		EPGProcessor::files = 0;
	}*/

	void EPGProcessor::decodeSdt(string fileName) {
		IServiceInfo* srvi;
		ILogoTransmissionDescriptor* ltd;
		char data[4084];
		int fd, rval;
		unsigned short originalNetworkId;
		size_t pos, remainingBytesDescriptor, value;

		cout << "Decoding SDT stream..." << endl << endl;

		fd = open(fileName.c_str(), O_RDONLY|O_LARGEFILE);
		rval = read(fd, (void*)&(data[0]), 4084);
		while (rval > 0) {
			originalNetworkId = (((data[0] << 8) & 0xFF00) | (data[1] & 0xFF));
			cout << "OriginalNetworkId: " << originalNetworkId << endl;
			cout << endl;

			pos = 3; //jumping reserved... it points to service_id

			while (pos < rval) {
				//there's at least one serviceinfo
#if HAVE_COMPSUPPORT
				srvi = ((ServiceInfoCreator*)(cm->getObject("ServiceInfo")))();
#else
				srvi = new ServiceInfo();
#endif
				if (srvi == NULL) {
					continue;
				}

				srvi->setServiceId((((data[pos] << 8) & 0xFF00) |
						(data[pos+1] & 0xFF)));

				pos += 2;
				srvi->setEitScheduleFlag((data[pos] & 0x02) >> 1);
				srvi->setEitPresentFollowingFlag(data[pos] & 0x01);
				pos++;
				srvi->setRunningStatus((data[pos] & 0xE0) >> 5);
				srvi->setFreeCAMode((data[pos] & 0x10) >> 4);
				srvi->setDescriptorsLoopLength(
						(((data[pos] & 0x0F) << 8) & 0xFF00) |
						(data[pos+1] & 0xFF));

				pos += 2;

				cout << "ServiceId: " << srvi->getServiceId() << endl;
				cout << "EitSchecudeFlag: ";
				if (srvi->getEitScheduleFlag()) {
					cout << "Yes" << endl;
				} else {
					cout << "No" << endl;
				}

				cout << "EitPresentFollowingFlag: ";
				if (srvi->getEitPresentFollowingFlag()) {
					cout << "Yes" << endl;
				} else {
					cout << "No" << endl;
				}

				cout << "RunningStatus: ";
				if (srvi->getRunningStatus()) {
					cout << "Yes" << endl;
				} else {
					cout << "No" << endl;
				}

				cout << "FreeCAMode: ";
				if (srvi->getRunningStatus()) {
					cout << "Yes" << endl;
				} else {
					cout << "No" << endl;
				}

				cout << "DescriptorsLoopLength: ";
				cout << srvi->getDescriptorsLoopLength() << endl;
				cout << endl;

				remainingBytesDescriptor = srvi->getDescriptorsLoopLength();

				while (remainingBytesDescriptor) {
					//there's at least one descriptor
					value = data[pos+1] + 2;
					remainingBytesDescriptor -= value;
					switch (data[pos]) {
					case 0xCF: //Logo Transmission Descriptor
#if HAVE_COMPSUPPORT
						ltd = ((LTDCreator*)(cm->getObject(
								"LogoTransmissionDescriptor")))();
#else
						ltd = new LogoTransmissionDescriptor();
#endif
						pos++;
						ltd->setDescriptorLength(data[pos]);
						cout << "Descriptor length: ";
						cout << (ltd->getDescriptorLength() & 0xFF) << endl;
						pos++;
						ltd->setType(data[pos]);
						cout << "Type: " << (ltd->getType() & 0xFF) << endl;
						pos++;
						if (data[pos-1] == 0x01) { // scheme 1
							ltd->setLogoId(((data[pos] << 8) & 0xFF00) |
									(data[pos+1] & 0xFF));

							pos += 2;
							ltd->setLogoVersion(((data[pos] << 8) &
										0x0100) | (data[pos+1] & 0xFF));
							pos += 2;
							ltd->setDownloadDataId(((data[pos] << 8) &
										0xFF00) | (data[pos+1] & 0xFF));
							pos += 2;
						}
						else if (data[pos-1] == 0x02) { // scheme 2
							ltd->setLogoId(((data[pos] << 8) & 0xFF00) |
												(data[pos+1] & 0xFF));
							pos += 2;
						}
						else if (data[pos-1] == 0x03) { // simple logo system
							char str[ltd->getDescriptorLength()];
							memcpy(str, data + pos, ltd->getDescriptorLength()-1);
							str[ltd->getDescriptorLength()-1] = 0;
							ltd->setName((string) str);
							pos += (ltd->getDescriptorLength()-1);
							cout << "Simple logo system: " << ltd->getName() << endl;
						}
						else {
							pos += (ltd->getDescriptorLength()-1);
							cout << "Unrecognized Logo Transmission Type: "
										<< ltd->getType() << endl;
						}

						srvi->insertDescriptor(ltd);
						break;

					default: //Unrecognized Descriptor
						pos += value;
						break;
					}
				}
			}
			rval = read(fd, (void*)&(data[0]), 4093);
		}
		cout << "Stream decoded successfully." << endl;
	}

	set<IEventInfo*>* EPGProcessor::decodeEit(string fileName) {
		set<IEventInfo*>* eit;
		IEventInfo* ei;
		IShortEventDescriptor* se;
		int fd, rval;
		time_t time;
		size_t pos, remainingBytesDescriptor, value;
		char str[256];
		char data[4084];

		int transportStreamId, originalNetworkId, segmentLastSectionNumber;
		int lastTableId;
		cout << "Decoding EIT stream..." << endl << endl;

		eit = new set<IEventInfo*>;
		fd = open(fileName.c_str(), O_RDONLY|O_LARGEFILE);
		rval = read(fd, (void*)&(data[0]), 4084);
		while (rval > 0) {
			transportStreamId = (((data[0] << 8) & 0xFF00) |
					(data[1] & 0xFF));

			originalNetworkId = (((data[2] << 8) & 0xFF00) |
					(data[3] & 0xFF));

			segmentLastSectionNumber = data[4];
			//TODO: works only for short
			lastTableId = data[5];

			cout << "TransportStreamId: " << transportStreamId << endl;
			cout << "OriginalNetworkId: " << originalNetworkId << endl;
			cout << endl;

			pos = 6;

			while ((pos) < rval) {
				//there's at least one eventinfo
#if HAVE_COMPSUPPORT
				ei = ((EICreator*)(cm->getObject("EventInfo")))();
#else
				ei = new EventInfo();
#endif
				if (ei == NULL) {
					continue;
				}

				ei->setEventId((((data[pos] << 8) & 0xFF00) |
						(data[pos+1] & 0xFF)));

				pos += 2;
				ei->setStartTimeEncoded(data+pos);

				pos += 5;
				ei->setDurationEncoded(data+pos);

				pos += 3;
				ei->setRunningStatus((data[pos] & 0xE0) >> 5);
				ei->setFreeCAMode((data[pos] & 0x10) >> 4);
				ei->setDescriptorsLoopLength(
						(((data[pos] & 0x0F) << 8) & 0xFF00) |
						(data[pos+1] & 0xFF));

				pos += 2;

				cout << "EventId: " << ei->getEventId() << endl;
				time = ei->getStartTime();
				if (time) {
					cout << "StartTime: " << time << endl;
				} else {
					cout << "StartTime: undefined." << endl;
				}
				time = ei->getDuration();
				cout << "Duration: " << time << endl;
				cout << "RunningStatus: ";
				if (ei->getRunningStatus()) {
					cout << "Yes" << endl;
				} else {
					cout << "No" << endl;
				}
				cout << "FreeCAMode: ";
				if (ei->getRunningStatus()) {
					cout << "Yes" << endl;
				} else {
					cout << "No" << endl;
				}
				cout << "DescriptorsLoopLength: ";
				cout << ei->getDescriptorsLoopLength() << endl;
				cout << endl;

				remainingBytesDescriptor = ei->getDescriptorsLoopLength();

				while (remainingBytesDescriptor) {
					//there's at least one descriptor
					value = ((data[pos+1] & 0xFF) + 2);
					remainingBytesDescriptor -= value;
					switch (data[pos]) {
					case 0x4D: //Short Event Descriptor
#if HAVE_COMPSUPPORT
						se = ((SEDCreator*)(cm->getObject(
								"ShortEventDescriptor")))();
#else
						se = new ShortEventDescriptor();
#endif
						se->setDescriptorLength(data[pos+1]);
						pos += 2;
						se->setLanguageCode(data+pos);
						pos += 3;
						se->setEventName(data+pos+1, data[pos]);
						pos += (se->getEventNameLength()+1);
						se->setDescription(data+pos+1, data[pos]);
						pos += (se->getDescriptionLength() + 1);

						cout << "Descriptor: Short Event Descriptor" << endl;
						memcpy(str, se->getLanguageCode(), 3); str[3] = 0;
						cout << "LanguageCode: " << str << endl;
						memcpy(str, se->getEventName(),
								se->getEventNameLength());
						str[se->getEventNameLength()] = 0;
						cout << "EventName: " << str << endl;
						memcpy(str, se->getDescription(),
								se->getDescriptionLength());
						str[se->getDescriptionLength()] = 0;
						cout << "Description: " << str << endl;
						cout << endl;

						ei->insertDescriptor(se);
						break;

					default: //Unrecognized Descriptor
						pos += value;
						break;
					}
				}
				eit->insert(ei);
			}
			rval = read(fd, (void*)&(data[0]), 4093);
		}

		if (eit->empty()) {
			delete eit;
			eit = NULL;
		}

		cout << "EIT Stream decoded successfully." << endl;
		return eit;
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

		savePNG(pngData, pngSize);
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
