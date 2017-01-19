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

#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include "ginga.h"

GINGA_CTXMGMT_BEGIN

class SystemInfo
{
private:
  double clockSpeed;
  map<string, string> *sysTable;

public:
  SystemInfo ();
  ~SystemInfo ();

private:
  void initializeClockSpeed ();
  string getValue (string attribute);
  void printSysTable ();

public:
  void setSystemTable (map<string, string> *sysTable);
  string getSystemLanguage ();
  string getCaptionLanguage ();
  string getSubtitleLanguage ();
  double getReturnBitRate ();
  void getScreenSize (GingaScreenID screenId, int *width, int *height);
  void getScreenGraphicSize (GingaScreenID screenId, int *width,
                             int *height);
  string getAudioType ();
  double getCPUClock ();
  double getMemorySize ();
  string getJavaConfiguration ();
  string getJavaProfile ();
  string getLuaVersion ();
};

GINGA_CTXMGMT_END

#endif /* SYSTEM_INFO_H */
