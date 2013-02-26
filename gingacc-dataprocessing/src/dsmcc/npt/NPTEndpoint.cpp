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

#include "dataprocessing/dsmcc/npt/NPTEndpoint.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace dsmcc {
namespace npt {

NPTEndpoint::NPTEndpoint() : MpegDescriptor(0x02) {
	descriptorLength = 14;
	startNPT = 0;
	stopNPT = 0;
}

NPTEndpoint::~NPTEndpoint() {

}

int NPTEndpoint::process() {
	int pos = MpegDescriptor::process();

	pos = pos + 1;

	startNPT = 0;
	startNPT = (stream[pos++] & 0x01);
	startNPT = startNPT << 8;
	startNPT = startNPT | (stream[pos++] & 0xFF);
	startNPT = startNPT << 8;
	startNPT = startNPT | (stream[pos++] & 0xFF);
	startNPT = startNPT << 8;
	startNPT = startNPT | (stream[pos++] & 0xFF);
	startNPT = startNPT << 8;
	startNPT = startNPT | (stream[pos++] & 0xFF);

	pos = pos + 3;

	stopNPT = 0;
	stopNPT = (stream[pos++] & 0x01);
	stopNPT = stopNPT << 8;
	stopNPT = stopNPT | (stream[pos++] & 0xFF);
	stopNPT = stopNPT << 8;
	stopNPT = stopNPT | (stream[pos++] & 0xFF);
	stopNPT = stopNPT << 8;
	stopNPT = stopNPT | (stream[pos++] & 0xFF);
	stopNPT = stopNPT << 8;
	stopNPT = stopNPT | (stream[pos++] & 0xFF);

	return pos;
}

int NPTEndpoint::updateStream() {
	MpegDescriptor::updateStream();

	stream[2] = 0xFF;
	stream[3] = 0xFE;

	stream[3] = stream[3] |	(((startNPT >> 16) >> 16) & 0x01);
	stream[4] = ((startNPT >> 16) >> 8) & 0xFF;
	stream[5] = (startNPT >> 16) & 0xFF;
	stream[6] = (startNPT >> 8) & 0xFF;
	stream[7] = startNPT & 0xFF;

	stream[8] = 0xFF;
	stream[9] = 0xFF;
	stream[10] = 0xFF;
	stream[11] = 0xFE;

	stream[11] = stream[11] | (((stopNPT >> 16) >> 16) & 0x01);
	stream[12] = ((stopNPT >> 16) >> 8) & 0xFF;
	stream[13] = (stopNPT >> 16) & 0xFF;
	stream[14] = (stopNPT >> 8) & 0xFF;
	stream[15] = stopNPT & 0xFF;

	return 16;
}

unsigned int NPTEndpoint::calculateDescriptorSize() {
	int pos = MpegDescriptor::calculateDescriptorSize();
	return pos + 14;
}

uint64_t NPTEndpoint::getStartNPT() {
	return startNPT;
}

uint64_t NPTEndpoint::getStopNPT() {
	return stopNPT;
}

void NPTEndpoint::setStartNPT(uint64_t npt) {
	startNPT = npt;
}

void NPTEndpoint::setStopNPT(uint64_t npt) {
	stopNPT = npt;
}

}
}
}
}
}
}
}
}
