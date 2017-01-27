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

#ifndef CURL_INTERACTIVE_CHANNEL_H
#define CURL_INTERACTIVE_CHANNEL_H

#include "ginga.h"
#include "IInteractiveChannelListener.h"

GINGA_IC_BEGIN

class CurlInteractiveChannel
{
private:
  FILE *fd;
  char *buffer;
  short type;
  double rate;
  string localUri;
  string defaultServer;
  string uri;
  string userAgent;
  static CURL *curl;
  static pthread_mutex_t mutex;
  static bool mutexInit;
  IInteractiveChannelListener *listener;

public:
  CurlInteractiveChannel ();
  ~CurlInteractiveChannel ();
  bool hasConnection ();
  void setSourceTarget (const string &url);
  void setTarget (FILE *fd);
  short getType ();
  double getRate ();
  void setListener (IInteractiveChannelListener *listener);
  bool reserveUrl (const string &uri,
                   IInteractiveChannelListener *listener = NULL,
                   const string &userAgent = "");

  bool performUrl ();
  bool releaseUrl ();

private:
  static size_t writeCallBack (void *ptr, size_t size, size_t nmemb,
                               void *stream);
  bool positiveResponse (long *respCode);
  FILE *getLocalFileDescriptor ();
  IInteractiveChannelListener *getListener ();
};

GINGA_IC_END

#endif /* CURL_INTERACTIVE_CHANNEL_H */
