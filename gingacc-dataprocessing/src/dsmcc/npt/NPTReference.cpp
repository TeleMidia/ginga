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

#include "dataprocessing/dsmcc/npt/NPTReference.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace dsmcc {
namespace npt {
NPTReference::NPTReference() : MpegDescriptor(0x01) {
	postDiscontinuityIndicator = 0;
	contentId = 0;
	stcRef = 0;
	nptRef = 0;
	scaleNumerator = 1;
	scaleDenominator = 1;
	descriptorLength = 18;
}

NPTReference::~NPTReference() {

}

int NPTReference::process() {
	int pos = MpegDescriptor::process();

	postDiscontinuityIndicator = (stream[pos] >> 7) & 0x01;
	contentId = stream[pos++] & 0x3F;

	stcRef = 0;
	stcRef = (stream[pos++] & 0x01);
	stcRef = stcRef << 8;
	stcRef = stcRef | (stream[pos++] & 0xFF);
	stcRef = stcRef << 8;
	stcRef = stcRef | (stream[pos++] & 0xFF);
	stcRef = stcRef << 8;
	stcRef = stcRef | (stream[pos++] & 0xFF);
	stcRef = stcRef << 8;
	stcRef = stcRef | (stream[pos++] & 0xFF);

	pos = pos + 3;

	nptRef = 0;
	nptRef = (stream[pos++] & 0x01);
	nptRef = nptRef << 8;
	nptRef = nptRef | (stream[pos++] & 0xFF);
	nptRef = nptRef << 8;
	nptRef = nptRef | (stream[pos++] & 0xFF);
	nptRef = nptRef << 8;
	nptRef = nptRef | (stream[pos++] & 0xFF);
	nptRef = nptRef << 8;
	nptRef = nptRef | (stream[pos++] & 0xFF);

	scaleNumerator = ((stream[pos] & 0xFF) << 8) |
						(stream[pos + 1] & 0xFF);
	pos = pos + 2;
	scaleDenominator = ((stream[pos] & 0xFF) << 8) |
						(stream[pos + 1] & 0xFF);
	pos = pos + 2;

	return pos;
}

int NPTReference::updateStream() {
	MpegDescriptor::updateStream();
	stream[2] = stream[2] & 0x7F;
	stream[2] = stream[2] |	((postDiscontinuityIndicator << 7) & 0x80);
	stream[2] = stream[2] & 0x80;
	stream[2] = stream[2] |	(contentId & 0x7F);

	stream[3] = 0xFE;

	stream[3] = stream[3] |	(((stcRef >> 16) >> 16) & 0x01);
	stream[4] = ((stcRef >> 16) >> 8) & 0xFF;
	stream[5] = (stcRef >> 16) & 0xFF;
	stream[6] = (stcRef >> 8) & 0xFF;
	stream[7] = stcRef & 0xFF;

	stream[8] = 0xFF;
	stream[9] = 0xFF;
	stream[10] = 0xFF;
	stream[11] = 0xFE;

	stream[11] = stream[11] | (((nptRef >> 16) >> 16) & 0x01);
	stream[12] = ((nptRef >> 16) >> 8) & 0xFF;
	stream[13] = (nptRef >> 16) & 0xFF;
	stream[14] = (nptRef >> 8) & 0xFF;
	stream[15] = nptRef & 0xFF;
	stream[16] = (scaleNumerator << 8) & 0xFF;
	stream[17] = scaleNumerator & 0xFF;
	stream[18] = (scaleDenominator << 8) & 0xFF;
	stream[19] = scaleDenominator & 0xFF;

	return 20;
}

unsigned int NPTReference::calculateDescriptorSize() {
	int pos = MpegDescriptor::calculateDescriptorSize();
	return pos + 18;
}

char NPTReference::getPostDiscontinuityIndicator() {
	return postDiscontinuityIndicator;
}

unsigned char NPTReference::getContentId() {
	return contentId;
}

uint64_t NPTReference::getStcRef() {
	return stcRef;
}

uint64_t NPTReference::getNptRef() {
	return nptRef;
}

short NPTReference::getScaleNumerator() {
	return scaleNumerator;
}

unsigned short NPTReference::getScaleDenominator() {
	return scaleDenominator;
}

void NPTReference::setPostDiscontinuityIndicator(char indicator) {
	postDiscontinuityIndicator = indicator;
}

void NPTReference::setContentId(unsigned char id) {
	contentId = (id & 0x7F);
}

void NPTReference::setStcRef(uint64_t stc) {
	stcRef = stc;
}

void NPTReference::setNptRef(uint64_t npt) {
	nptRef = npt;
}

void NPTReference::setScaleNumerator(short numerator) {
	scaleNumerator = numerator;
}

void NPTReference::setScaleDenominator(unsigned short denominator) {
	scaleDenominator = denominator;
}

}
}
}
}
}
}
}
}
