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

#ifndef PREFETCH_MANAGER_H
#define PREFETCH_MANAGER_H

#include "ginga.h"

#include "ic/InteractiveChannelManager.h"
using namespace ::ginga::ic;

GINGA_FORMATTER_BEGIN

class PrefetchManager : public IInteractiveChannelListener
{
private:
  map<string, string> *localToRemoteUris;
  map<string, string> *urisToLocalRoots;
  set<string> *scheduledRemoteUris;
  set<string> *scheduledLocalUris;
  InteractiveChannelManager *icm;
  string prefetchRoot;
  bool synch;

  int kbytes;
  int filesDown;
  int filesSched;

  PrefetchManager ();
  ~PrefetchManager ();
  static PrefetchManager *_instance;

public:
  static PrefetchManager *getInstance ();

private:
  void createDirectory (const string &newDir);

public:
  void release ();
  void releaseContents ();
  string createDocumentPrefetcher (const string &remoteDocUri);
  string createSourcePrefetcher (const string &localDocUri, const string &srcUri);
  bool hasIChannel ();
  bool hasRemoteLocation (const string &localUri);
  string getRemoteLocation (const string &localUri);
  string getLocalRoot (const string &remoteUri);

private:
  void getContent (const string &remoteUri, const string &localUri);

public:
  void setSynchPrefetch (bool synch);

private:
  void scheduleContent (const string &remoteUri, const string &localUri);
  void receiveCode (arg_unused (long respCode)){};
  void receiveDataStream (arg_unused (char *buffer),
                          arg_unused (int size)){};
  void receiveDataPipe (FILE *fd, int size);
  void downloadCompleted (arg_unused (const char *localUri)){};

public:
  void getScheduledContent (const string &clientUri);
  void getScheduledContents ();
};

GINGA_FORMATTER_END

#endif /* PREFETCH_MANAGER_H */
