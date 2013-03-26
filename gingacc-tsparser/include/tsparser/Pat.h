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

#ifndef PAT_H_
#define PAT_H_

#include "Pmt.h"
#include "TransportSection.h"

#include <map>
#include <vector>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
	class Pat : public TransportSection {
		private:
			static int defaultProgramNumber;
			static int defaultProgramPid;

		protected:
			unsigned int currentPid;
			unsigned int currentCarouselId;
			unsigned int carouselComponentTag;
			unsigned int tsPid;

			//Program association table (pmt pid x program number)
			// if i->first == 0 i->second = networkPid
			map<unsigned int, unsigned int> pat;

			vector<unsigned int> unprocessedPmts;

			//PMT Pid TO PMT
			map<unsigned int, Pmt*> programs;

			char* stream;

		public:
			Pat();
			virtual ~Pat();

			bool hasStreamType(short streamType);
			int getPidByStreamType(short streamType);
			unsigned int getNextPid();
			unsigned int getNextCarouselId();
			unsigned int getCarouselComponentTag();
			bool hasProgramNumber(unsigned int programNumber);
			bool hasPid(unsigned int pid);
			Pmt* getPmtByProgramNumber(unsigned int programNumber);
			unsigned int getTSId();
			void setTSId(unsigned int newTSId);
			unsigned int getNumOfPrograms();
			void addProgram(unsigned int pid, unsigned int programNumber);
			void addPmt(Pmt* program);
			void replacePmt(unsigned int pid, Pmt* newPmt);
			unsigned int getFirstProgramNumber();
			unsigned int getProgramNumberByPid(unsigned int pid);
			short getStreamType(unsigned int pid);
			bool isSectionType(unsigned int pid);

			static void resetPayload(char* payload, int size);

			bool processSectionPayload();
			bool hasUnprocessedPmt();
			vector<unsigned int>* getUnprocessedPmtPids();
			map<unsigned int, Pmt*>* getProgramsInfo();
			void checkConsistency();
			unsigned int getDefaultProgramPid();
			unsigned int getDefaultMainVideoPid();
			unsigned int getDefaultMainAudioPid();

			unsigned short createPatStreamByProgramPid(unsigned short pid, char** dataStream);

			void print();
	 };
}
}
}
}
}
}
}

#endif /*PAT_H_*/
