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

#include "../../include/Pmt.h"
#include "../../include/IDemuxer.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
	Pmt::Pmt(unsigned int pid, unsigned int programNumber) : TSSection() {
		streams = new map<unsigned int, short>;
		streamTypeNames = new map<short, string>;
		//aits = new map<unsigned int, AITInfo*>;
		processed = false;

		(*streamTypeNames)[STREAM_TYPE_VIDEO_MPEG1] = "ISO/IEC 11172-2 Video";
		(*streamTypeNames)[STREAM_TYPE_VIDEO_MPEG2] = "ISO/IEC 13818-2 Video";
		(*streamTypeNames)[STREAM_TYPE_AUDIO_MPEG1] = "ISO/IEC 11172-3 Audio";
		(*streamTypeNames)[STREAM_TYPE_AUDIO_MPEG2] = "ISO/IEC 13818-3 Audio";
		(*streamTypeNames)[STREAM_TYPE_PRIVATE_SECTION] =
			    "ISO/IEC 13818 Private Sections";

		(*streamTypeNames)[STREAM_TYPE_PRIVATE_DATA] =
			    "ISO/IEC 13818 Private Data";

		(*streamTypeNames)[STREAM_TYPE_MHEG] = "ISO/IEC 13522 MHEG";
		(*streamTypeNames)[STREAM_TYPE_DSMCC_TYPE_A] = "ISO/IEC 13818-6 type A";
		(*streamTypeNames)[STREAM_TYPE_DSMCC_TYPE_B] = "ISO/IEC 13818-6 type B";
		(*streamTypeNames)[STREAM_TYPE_DSMCC_TYPE_C] = "ISO/IEC 13818-6 type C";
		(*streamTypeNames)[STREAM_TYPE_DSMCC_TYPE_D] = "ISO/IEC 13818-6 type D";
		(*streamTypeNames)[STREAM_TYPE_AUDIO_AAC] = "ISO/IEC 13818-7 Audio AAC";
		(*streamTypeNames)[STREAM_TYPE_VIDEO_MPEG4] = "ISO/IEC 14496-2 Visual";
		(*streamTypeNames)[STREAM_TYPE_AUDIO_MPEG4] = "ISO/IEC 14496-3 Audio";
		(*streamTypeNames)[STREAM_TYPE_VIDEO_H264] = "ISO/IEC 14496-10 h.264";
		(*streamTypeNames)[STREAM_TYPE_AUDIO_AC3] = "audio AC3";
		(*streamTypeNames)[STREAM_TYPE_AUDIO_DTS] = "audio DTS";

		this->pid = pid;
		this->programNumber = programNumber;
		this->pcrPid = 0;
	}

	void Pmt::addElementaryStream(unsigned int pid, short esType) {
		cout << "pid = '" << pid << "' esType = '" << esType;
		cout << "' " << endl;
		(*streams)[pid] = esType;
	}

/*
	void Pmt::addAITInfo(unsigned int pid, AITInfo* info) {
		(*aits)[pid] = info;
	}
*/
	unsigned int Pmt::getPid() {
		return this->pid;
	}

	vector<unsigned int>* Pmt::getPidsByTid(unsigned int tid) {
		vector<unsigned int>* pids;
		map<unsigned int, short>::iterator i;
		short streamType;

		streamType = IDemuxer::getStreamTypeFromTableId(tid);

		pids = new vector<unsigned int>;
		i = streams->begin();
		while (i != streams->end()) {
			if (streamType == i->second) {
				pids->push_back(i->first);
			}
			++i;
		}

		return pids;
	}

	void Pmt::setPid(unsigned int pid) {
		this->pid = pid;
	}

	unsigned int Pmt::getProgramNumber() {
		return this->programNumber;
	}

	void Pmt::setProgramNumber(unsigned int programNumber) {
		this->programNumber = programNumber;
	}

	bool Pmt::hasPid(unsigned int somePid) {
		if (somePid == this->pid) {
			return true;
		}

		return (streams->count(somePid) != 0);
	}

	short Pmt::getStreamTypeValue(unsigned int somePid) {
		if (streams->count(somePid) == 0) {
			return -1;
		}

		return (*streams)[somePid];
	}

	string Pmt::getStreamType(unsigned int somePid) {
		if (streams->count(somePid) == 0) {
			return "";
		}

		return getStreamTypeName((*streams)[somePid]);
	}

	bool Pmt::isSectionType(unsigned int pid) {
		short value;
		value = getStreamTypeValue(pid);

		if (value == STREAM_TYPE_PRIVATE_SECTION ||
			    value == STREAM_TYPE_PRIVATE_DATA ||
			    (value >= STREAM_TYPE_DSMCC_TYPE_A &&
			    value <= STREAM_TYPE_DSMCC_TYPE_D)) {

			return true;
		}
		return false;
	}

	string Pmt::getStreamTypeName(short streamType) {
		if (streamTypeNames == NULL ||
			    streamTypeNames->count(streamType) == 0) {

			return "unknown type '" + intToStrHexa(streamType) + "'";
		}
		return (*streamTypeNames)[streamType];
	}

	bool Pmt::processSectionPayload() {
		if (processed) {
			cout << "Warning! Pmt::processSectionPayload() - Try to process a "
				 << "already processed PMT " << endl;
			return false;
		}

		if (tableId != (unsigned int)PMT_TID || !isConsolidated()) {
			cout << "Pmt::processSectionPayload Warning! inconsistency found";
			cout << " tableId = " << tableId << " and isConsolidated = ";
			cout << isConsolidated() << endl;
			return false;
		}

		if (programNumber != idExtention) {
			cout << "Pmt::processSectionPayload Warning! programNumber !=";
			cout << " idExtension." << endl;
		}

		char sectionPayload[sectionLength - 9];
		memcpy((void*)(&sectionPayload[0]), getPayload(), sectionLength - 9);

		pcrPid = ((sectionPayload[0] & 0x1F) << 8) |
			    (sectionPayload[1] & 0xFF);

		unsigned int programInfoLength;
		programInfoLength = ((sectionPayload[2] & 0x0F) << 8) |
			    (sectionPayload[3] & 0xFF);

		unsigned int i;
		short streamType;
		unsigned int elementaryPid;
		unsigned int esInfoLength;

		i = 4 + programInfoLength;//jumping descriptors
		while (i < (sectionLength - 9)) {
			streamType = (short)(sectionPayload[i] & 0xFF);
			i++;
			elementaryPid = ((sectionPayload[i] & 0x1F) << 8) |
			    (sectionPayload[i + 1] & 0xFF);

			(*streams)[elementaryPid] = streamType;

			i += 2;
			esInfoLength = ((sectionPayload[i] & 0x0F) << 8) |
			    (sectionPayload[i + 1] & 0xFF);

			i = i + 2 + esInfoLength;
		}

		processed = true;
		return processed;
	}

	bool Pmt::hasProcessed() {
		return processed;
	}

	map<unsigned int, short>* Pmt::getStreamsInformation() {
		return streams;
	}

	unsigned int Pmt::getPCRPid() {
		return pcrPid;
	}

	unsigned int Pmt::getDefaultMainVideoPid() {
		map<unsigned int, short>::iterator i;

		i = streams->begin();
		while (i != streams->end()) {
			switch (i->second) {
				case STREAM_TYPE_VIDEO_MPEG1:
				case STREAM_TYPE_VIDEO_MPEG2:
				case STREAM_TYPE_VIDEO_MPEG4:
				case STREAM_TYPE_VIDEO_H264:
					return i->first;

				default:
					break;
			}
			++i;
		}

		return 0;
	}

	unsigned int Pmt::getDefaultMainAudioPid() {
		map<unsigned int, short>::iterator i;

		i = streams->begin();
		while (i != streams->end()) {
			switch (i->second) {
				case STREAM_TYPE_AUDIO_MPEG1:
				case STREAM_TYPE_AUDIO_MPEG2:
				case STREAM_TYPE_AUDIO_MPEG4:
				case STREAM_TYPE_AUDIO_AAC:
				case STREAM_TYPE_AUDIO_AC3:
				case STREAM_TYPE_AUDIO_DTS:
					return i->first;

				default:
					break;
			}
			++i;
		}

		return 0;
	}

	void Pmt::print() {
		cout << "Pmt::print" << endl;
		cout << "Program number = " << idExtention << endl;
		cout << "streams:" << endl;
		map<unsigned int, short>::iterator i;
		for (i = streams->begin(); i != streams->end(); ++i) {
			cout << "streamType '" << getStreamTypeName(i->second) << "' ";
			cout << " has pid = '" << i->first << "'" << endl;
		}
	}
}
}
}
}
}
}
}
