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

#ifndef IDEMUXER_H_
#define IDEMUXER_H_

#include "tuner/ITuner.h"
#include "tuner/providers/frontends/IFrontendFilter.h"
using namespace ::br::pucrio::telemidia::ginga::core::tuning;

#include "ITSFilter.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
	// specific packet ids
	static const short PAT_PID  = 0x0000;
	static const short CAT_PID  = 0x0001;
	static const short TSDT_PID = 0x0001; //TS Description Table
	static const short NIT_PID  = 0x0010;
	static const short SDT_PID  = 0x0011;
	static const short EIT_PID  = 0x0012;
	static const short CDT_PID  = 0x0029;

	// specific section ids
	static const short PAT_TID  = 0x00;
	static const short CAT_TID  = 0x01;
	static const short PMT_TID  = 0x02;
	static const short TSDT_TID = 0x03; //TS Description Section
	static const short OCI_TID  = 0x3B; //Object Carousel Info
	static const short OCD_TID  = 0x3C; //Object Carousel Data
	static const short DDE_TID  = 0x3D; //DSM-CC Descriptors
	static const short NIT_TID  = 0x40;
	static const short SDT_TID  = 0x42;
	static const short EIT_TID  = 0x4E;
	static const short CDT_TID  = 0xC8;
	static const short AIT_TID  = 0x74;
	static const short TOT_TID  = 0x73;

	// specific stream types
	static const short STREAM_TYPE_VIDEO_MPEG1     = 0x01;
	static const short STREAM_TYPE_VIDEO_MPEG2     = 0x02;
	static const short STREAM_TYPE_AUDIO_MPEG1     = 0x03;
	static const short STREAM_TYPE_AUDIO_MPEG2     = 0x04;
	static const short STREAM_TYPE_PRIVATE_SECTION = 0x05;
	static const short STREAM_TYPE_PRIVATE_DATA    = 0x06;
	static const short STREAM_TYPE_MHEG            = 0x07;
	static const short STREAM_TYPE_DSMCC_TYPE_A    = 0x0A;
	static const short STREAM_TYPE_DSMCC_TYPE_B    = 0x0B;
	static const short STREAM_TYPE_DSMCC_TYPE_C    = 0x0C;
	static const short STREAM_TYPE_DSMCC_TYPE_D    = 0x0D;
	static const short STREAM_TYPE_AUDIO_AAC       = 0x0F;
	static const short STREAM_TYPE_VIDEO_MPEG4     = 0x10;
	static const short STREAM_TYPE_AUDIO_MPEG4     = 0x11;
	static const short STREAM_TYPE_VIDEO_H264      = 0x1B;
	static const short STREAM_TYPE_AUDIO_AC3       = 0x81;
	static const short STREAM_TYPE_AUDIO_DTS       = 0x8A;

  class IDemuxer : public ITunerListener {
	public:
		virtual ~IDemuxer(){};
		virtual unsigned int getTSId()=0;

		static int getTableIdFromStreamType(short streamType) {
			switch (streamType) {
				case STREAM_TYPE_DSMCC_TYPE_B:
					return OCI_TID;

				case STREAM_TYPE_DSMCC_TYPE_C:
					return OCD_TID;

				case STREAM_TYPE_DSMCC_TYPE_D:
					return DDE_TID;

				case STREAM_TYPE_PRIVATE_SECTION:
					return AIT_TID;

				default:
					return -1;
			}
		};

		static int getStreamTypeFromTableId(short tid) {
			switch (tid) {
				case OCI_TID:
					return STREAM_TYPE_DSMCC_TYPE_B;

				case OCD_TID:
					return STREAM_TYPE_DSMCC_TYPE_C;

				case DDE_TID:
					return STREAM_TYPE_DSMCC_TYPE_D;

				case AIT_TID:
					return STREAM_TYPE_PRIVATE_SECTION;

				default:
					return -1;
			}
		};

		virtual void setDestination(short streamType)=0; //debug purpose only
		virtual void removeFilter(ITSFilter* tsFilter)=0;
		virtual void addFilter(ITSFilter* tsFilter, int pid, int tid)=0;
		virtual void addFilter(IFrontendFilter* filter)=0;
		virtual void receiveSection(
				char* section, int secLen, IFrontendFilter* filter)=0;

		virtual void addPidFilter(unsigned int pid, ITSFilter* filter)=0;
		virtual void addSectionFilter(unsigned int tid, ITSFilter* filter)=0;
		virtual void addStreamTypeFilter(short streamType, ITSFilter* filter)=0;
		virtual void addPesFilter(short type, ITSFilter* filter)=0;

		virtual int getDefaultMainVideoPid()=0;
		virtual int getDefaultMainAudioPid()=0;

		virtual void receiveData(char* buff, unsigned int size)=0;
		virtual void updateChannelStatus(short newStatus, IChannel* channel)=0;

		virtual short getCaps()=0;

		virtual bool waitProgramInformation()=0;
		virtual bool waitBuffers()=0;
	 };
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::tsparser::IDemuxer* demCreator(
		ITuner* tuner);

typedef void demDestroyer(
		::br::pucrio::telemidia::ginga::core::tsparser::IDemuxer* demuxer);

#endif /*IDEMUXER_H_*/
