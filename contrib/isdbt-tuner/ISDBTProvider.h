/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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

#ifndef ISDBTPROVIDER_H_
#define ISDBTPROVIDER_H_

#define INPUT_BUFFER_SIZE 4096

//GINGA_PRAGMA_DIAG_PUSH ()
GINGA_PRAGMA_DIAG_IGNORE (-Wunused-variable)
extern "C" {
// Latin America channel assignments for ISDB-T International
static uint64_t tv_channels_list[] = {
  /* 0 */ 0, /* index placeholders... */
  /* 1 */ 0, // never used for broadcasting
  /* 2 */ 0, // what to do?
  /* 3 */ 0, // what to do?
  /* 4 */ 0, // what to do?
  /* 5 */ 0, // future allocation for radio broadcasting
  /* 6 */ 0, // future allocation for radio broadcasting
  /* 7 */ 177142000,
  /* 8 */ 183142000,
  /* 9 */ 189142000,
  /* 10 */ 195142000,
  /* 11 */ 201142000,
  /* 12 */ 207142000,
  /* 13 */ 213142000,
  /* 14 */ 473142000,
  /* 15 */ 479142000,
  /* 16 */ 485142000,
  /* 17 */ 491142000,
  /* 18 */ 497142000,
  /* 19 */ 503142000,
  /* 20 */ 509142000,
  /* 21 */ 515142000,
  /* 22 */ 521142000,
  /* 23 */ 527142000,
  /* 24 */ 533142000,
  /* 25 */ 539142000,
  /* 26 */ 545142000,
  /* 27 */ 551142000,
  /* 28 */ 557142000,
  /* 29 */ 563142000,
  /* 30 */ 569142000,
  /* 31 */ 575142000,
  /* 32 */ 581142000,
  /* 33 */ 587142000,
  /* 34 */ 593142000,
  /* 35 */ 599142000,
  /* 36 */ 605142000,
  /* 37 */ 611142000, // radio astronomy reserved (we are in rx mode anyway,
                      // so
                      // no harm here...
  /* 38 */ 617142000,
  /* 39 */ 623142000,
  /* 40 */ 629142000,
  /* 41 */ 635142000,
  /* 42 */ 641142000,
  /* 43 */ 647142000,
  /* 44 */ 653142000,
  /* 45 */ 659142000,
  /* 46 */ 665142000,
  /* 47 */ 671142000,
  /* 48 */ 677142000,
  /* 49 */ 683142000,
  /* 50 */ 689142000,
  /* 51 */ 695142000,
  /* 52 */ 701142000,
  /* 53 */ 707142000,
  /* 54 */ 713142000,
  /* 55 */ 719142000,
  /* 56 */ 725142000,
  /* 57 */ 731142000,
  /* 58 */ 737142000,
  /* 59 */ 743142000,
  /* 60 */ 749142000,
  /* 61 */ 755142000,
  /* 62 */ 761142000,
  /* 63 */ 767142000,
  /* 64 */ 773142000,
  /* 65 */ 779142000,
  /* 66 */ 785142000,
  /* 67 */ 791142000,
  /* 68 */ 797142000,
  /* 69 */ 803142000,
};
}
//GINGA_PRAGMA_DIAG_POP ()

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "IDataProvider.h"
#include "Channel.h"

#include "ISDBTFrontend.h"
#include "RingBuffer.h"

GINGA_TUNER_BEGIN

class ISDBTProvider : public IDataProvider
{
protected:
  string fileName;
  int feDescriptor;
  int initialFrequency;
  static const string iniFileName;
  ISDBTFrontend *frontend;
  vector<Channel *> *channels;
  vector<Channel *>::iterator currentChannel;
  short capabilities;
  ITProviderListener *listener;

  // thread and ring buffer variables...
  int keep_reading;
  pthread_t output_thread_id;
  pthread_mutex_t output_mutex;
  pthread_cond_t output_cond;
  struct ring_buffer output_buffer;
  static void *output_thread (void *nothing);

public:
  ISDBTProvider (long freq);
  virtual ~ISDBTProvider ();

  short getCaps ();
  void setListener (ITProviderListener *listener);
  void attachFilter (IFrontendFilter *filter);
  void removeFilter (IFrontendFilter *filter);

private:
  bool scanChannels ();
  void initializeChannels ();

public:
  bool tune ();
  Channel *getCurrentChannel ();
  bool getSTCValue (uint64_t *stc, int *valueType);
  bool changeChannel (int factor);
  bool setChannel (const string &channelValue);
  int createPesFilter (int pid, int pesType, bool compositeFiler);
  string getPesFilterOutput ();
  void close ();
  char *receiveData (int *len);
};

GINGA_TUNER_END

#endif /*FILESYSTEMPROVIDER_H_*/
