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

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "ginga.h"

#ifndef PATH_MAX
#define PATH_MAX 512
#endif

GINGA_UTIL_BEGIN

double strUTCToSec (string utcValue);
string cvtPercentual (string value, bool *isPercentual);
vector<string> *split (string str, string token, string pos_delimiter = "");
double getPercentualValue (string value);
bool isPercentualValue (string value);
bool fileExists (string filename);
bool isDirectory (const char *path);
double NaN ();
double infinity ();
bool isNaN (double value);
bool isInfinity (double value);

double getNextStepValue (double currentStepValue, double value, int factor,
                         double time, double initTime, double dur,
                         int stepSize);

bool readPPMFile (char *fn, int &X, int &Y, unsigned char *&result);
bool readBMPFile (char *fn, int &X, int &Y, unsigned char *&result);

bool ppmToJpeg (char *ppmfile, char *jpegfile, int quality);
bool bmpToJpeg (char *bmpfile, char *jpegfile, int quality);

void str_replace_all (string &str, const string &find_what,
                      const string &replace_with);

GINGA_UTIL_END

#endif /* FUNCTIONS_H */
