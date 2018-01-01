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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef FILESYSTEMPROVIDER_H_
#define FILESYSTEMPROVIDER_H_

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "IDataProvider.h"

GINGA_TUNER_BEGIN

class FileSystemProvider : public IDataProvider
{
protected:
  string fileName;
  FILE *fileDescriptor;
  short capabilities;
  ITProviderListener *listener;

  bool checkPossiblePacket (char *buff, const int &pos);
  int nextPacket (char *buff);
  int synchBuffer (char *buff, int diff);

public:
  FileSystemProvider (const string &fileName);
  virtual ~FileSystemProvider ();
  void setListener (ITProviderListener *listener);
  void attachFilter (unused (IFrontendFilter *filter)){};
  void removeFilter (unused (IFrontendFilter *filter)){};
  short getCaps ();
  bool tune ();
  Channel *getCurrentChannel ();
  bool getSTCValue (guint64 *stc, int *valueType);
  bool changeChannel (int factor);
  bool setChannel (const string &channelValue);
  int createPesFilter (int pid, int pesType, bool compositeFiler);
  string getPesFilterOutput ();
  void close ();

  char *receiveData (int *len);
};

GINGA_TUNER_END

#endif /*FILESYSTEMPROVIDER_H_*/
