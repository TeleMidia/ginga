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

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <iostream>
using namespace std;

int main() {
	double initTime;
	string hms;

	clog << "telemidia-util tests" << endl;

	initTime = getCurrentTimeMillis();
	clog << "initTime = '" << initTime << "'";
	clog << endl;

	hms = "0:2:3.4";
	clog << "str hms = '" << hms << "' converted to '" << strUTCToSec(hms);
	clog << "' seconds" << endl;

	while (true) {
#ifndef WIN32
		::usleep(1000000);
#else
		Sleep(1000);
#endif
		clog << "elapsedTime = '" << getCurrentTimeMillis() - initTime << "'";
		clog << endl;
	}
	//TODO: tests
	return 0;
}
