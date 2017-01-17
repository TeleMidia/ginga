/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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

#include "config.h"
#include "ncl/interfaces/IntervalAnchor.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

BR_PUCRIO_TELEMIDIA_NCL_INTERFACES_BEGIN

	//if the representation changes, update isObjectDuration method
	const double IntervalAnchor::OBJECT_DURATION = infinity();

	IntervalAnchor::IntervalAnchor(string id, double begin, double end)
		    : ContentAnchor(id) {

		typeSet.insert("IntervalAnchor");
		this->begin = 0;
		setEnd(end);
		setBegin(begin);
	}

	void IntervalAnchor::setStrValues(string begin, string end) {
		this->strBegin = begin;
		this->strEnd   = end;
	}

	string IntervalAnchor::getStrBegin() {
		return this->strBegin;
	}

	string IntervalAnchor::getStrEnd() {
		return this->strEnd;
	}

	double IntervalAnchor::getBegin() {
		return begin;
	}

	double IntervalAnchor::getEnd() {
		return end;
	}

	void IntervalAnchor::setBegin(double b) {
		bool isBDur = isObjectDuration(b);
		bool isEDur = isObjectDuration(end);

		if (b < 0 && !isBDur) {
			begin = 0;

		} else if ((!isBDur && !isEDur && b > end) ||
				(isBDur && !isEDur)) {

			begin = end;

		} else {
			begin = b;
		}
	}

	void IntervalAnchor::setEnd(double e) {
		bool isEDur = isObjectDuration(e);

		if (e < 0 && !isEDur) {
			end = IntervalAnchor::OBJECT_DURATION;

		} else if ((!isEDur && !isObjectDuration(begin) && e < begin)) {
			end = begin;

		} else {
			end = e;
		}
	}

	bool IntervalAnchor::isObjectDuration(double value) {
		return isInfinity(value);
	}

BR_PUCRIO_TELEMIDIA_NCL_INTERFACES_END
