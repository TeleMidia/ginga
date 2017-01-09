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

#include "dataprocessing/dsmcc/npt/TimeBaseClock.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace dsmcc {
namespace npt {
TimeBaseClock::TimeBaseClock() : Stc() {
	numerator   = 1;
	denominator = 1;
	endpointAvailable = false;
}

TimeBaseClock::~TimeBaseClock() {

}

int64_t TimeBaseClock::convertToNpt(
		int64_t base,
		short numerator,
		unsigned short denominator) {

	double scale, ret;

	scale = ((double) numerator) / denominator;
	ret   = base;
	ret   = ret * scale;

	return (int64_t) ret;
}

unsigned char TimeBaseClock::getContentId() {
	return contentId;
}

void TimeBaseClock::setContentId(unsigned char id) {
	contentId = id;
}

short TimeBaseClock::getScaleNumerator() {
	return numerator;
}

unsigned short TimeBaseClock::getScaleDenominator() {
	return denominator;
}

void TimeBaseClock::setScaleNumerator(short num) {
	numerator = num;
}

void TimeBaseClock::setScaleDenominator(unsigned short den) {
	denominator = den;
}

uint64_t TimeBaseClock::getStartNpt() {
	return startNpt;
}

uint64_t TimeBaseClock::getStopNpt() {
	return stopNpt;
}

void TimeBaseClock::setStartNpt(uint64_t start) {
	startNpt = start;
}

void TimeBaseClock::setStopNpt(uint64_t stop) {
	stopNpt = stop;
}

void TimeBaseClock::setEndpointAvailable(bool epa) {
	endpointAvailable = epa;
}

bool TimeBaseClock::getEndpointAvailable() {
	return endpointAvailable;
}

void TimeBaseClock::refreshStcSample() {
	// future problem: never returns a negative value!
	double doubleStc;
	double scale;

	Stc::refreshStcSample();

	scale     = ((double) numerator) / denominator;
	doubleStc = stc;
	doubleStc = doubleStc * scale;
	stc       = (uint64_t) doubleStc;
}

}
}
}
}
}
}
}
}
