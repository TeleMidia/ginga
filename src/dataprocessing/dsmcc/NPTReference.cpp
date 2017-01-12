/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#include "NPTReference.h"

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
