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

#include "../../../include/dsmcc/npt/Stc.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace dsmcc {
namespace npt {
Stc::Stc() {
	reference        = 0;
	stc              = 0;
	clockRef.tv_sec  = 0;
	clockRef.tv_usec = 0;
}

Stc::~Stc() {

}

int Stc::timevalSubtract(
		struct timeval *result, struct timeval *x, struct timeval *y) {

	int nsec;

	if (x->tv_usec < y->tv_usec) {
		nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec  += nsec;

	}

	if (x->tv_usec - y->tv_usec > 1000000) {
		nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec  -= nsec;
	}

	result->tv_sec  = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;

	return x->tv_sec < y->tv_sec;
}

int Stc::userClock(struct timeval* usrClk) {
	struct rusage usage;

	if (getrusage(RUSAGE_SELF, &usage) != 0) {
		cout << "Stc::userClock getrusage error." << endl;
		return -1;
	}

	usrClk->tv_sec  = usage.ru_utime.tv_sec;
	usrClk->tv_usec = usage.ru_utime.tv_usec;
	return 0;
}

uint64_t Stc::baseExtToStc(uint64_t base, uint64_t ext) {
	return ((base * 300) + ext);
}

uint64_t Stc::stcToBase(uint64_t stc) {
	uint64_t value = 0;
	value = 1 << 16;
	value = value << 17;
	return (((uint64_t)(stc / 300)) % value);
}

uint64_t Stc::stcToExt(uint64_t stc) {
	return (((uint64_t) stc) % 300);
}

double Stc::stcToSecond(uint64_t stc) {
	double ret = stc;
	ret = ret / SYSTEM_CLOCK_FREQUENCY;
	return ret;
}

double Stc::baseToSecond(uint64_t base) {
	double ret = base;
	ret = ret / SYSTEM_CLOCK_FREQUENCY_90;
	return ret;
}

uint64_t Stc::secondToStc(double seconds) {
	return (uint64_t)(seconds * SYSTEM_CLOCK_FREQUENCY);
}

uint64_t Stc::secondToBase(double seconds) {
	return (uint64_t) (seconds * SYSTEM_CLOCK_FREQUENCY_90);
}

void Stc::refreshStcSample() {
	struct timeval currentRef;
	struct timeval result;
	uint64_t clockedSec, clockedUsec;

	userClock(&currentRef);
	timevalSubtract(&result, &currentRef, &clockRef);
	clockedSec  = result.tv_sec * 27000000;
	clockedUsec = result.tv_usec * 27;
	stc = clockedSec + clockedUsec;
}

uint64_t Stc::getReference() {
	return reference;
}

void Stc::setReference(uint64_t pcr) {
	reference = pcr;
	userClock(&clockRef);
}

void Stc::setReference(uint64_t base, uint64_t ext) {
	reference = ((base * 300) + ext);
	userClock(&clockRef);
}

uint64_t Stc::getStc() {
	refreshStcSample();
	stc = stc + reference;
	return stc;
}

uint64_t Stc::getStcBase() {
	refreshStcSample();
	stc = stc + reference;
	uint64_t value = 0;
	value = 1 << 16;
	value = value << 17;
	return (((uint64_t)(stc / 300)) % value);
}

uint64_t Stc::getStcExt() {
	refreshStcSample();
	stc = stc + reference;
	return (((uint64_t) stc) % 300);
}

double Stc::getBaseToSecond() {
	return baseToSecond(getStcBase());
}

}
}
}
}
}
}
}
}
