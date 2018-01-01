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

#include "ginga.h"
#include "Tuner.h"
#include "ISDBTProvider.h"
#include "RingBuffer.h"

#include "util/functions.h"
using namespace ::ginga::util;

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)
GINGA_PRAGMA_DIAG_IGNORE (-Wformat)

GINGA_TUNER_BEGIN

const string ISDBTProvider::iniFileName (GINGA_ISDBT_PATH);

ISDBTProvider::ISDBTProvider (long freq)
{
  this->frontend = NULL;
  this->fileName = "";
  this->feDescriptor = -1;
  this->channels = new vector<Channel *>;
  this->currentChannel = channels->end ();
  this->listener = NULL;
  this->initialFrequency = 0;
  this->capabilities = (DPC_CAN_FETCHDATA | DPC_CAN_CTLSTREAM);
  /* | DPC_CAN_DEMUXBYHW |
				DPC_CAN_FILTERPID |
				DPC_CAN_FILTERTID); */ // TODO: implement
                                                       // these
                                                       // capabilities...

  // -1 means scan
  if (freq == -1)
    {
      scanChannels ();
      return;
    }

  if (freq >= 0)
    {
      this->initialFrequency = freq;
    }

  pthread_mutex_init (&output_mutex, NULL);
  pthread_cond_init (&output_cond, NULL);
  ring_buffer_create (&output_buffer, 28);
  keep_reading = 1;
  // start the thread which reads the data from the tuner
  pthread_create (&output_thread_id, NULL, ISDBTProvider::output_thread,
                  this);
}

ISDBTProvider::~ISDBTProvider ()
{
  keep_reading = 0;
  close ();
}

void *
ISDBTProvider::output_thread (void *ptr)
{
  ISDBTProvider *obj = (ISDBTProvider *)ptr;

  void *addr;
  int bytes_read;
  char buffer[INPUT_BUFFER_SIZE];

  while (obj->keep_reading)
    {
      if (obj->frontend == NULL || obj->frontend->dvrFd <= 0)
        {
          // wait until we get the dvr device opened
          usleep (100000);
          continue;
        }

      bytes_read = read (obj->frontend->dvrFd, buffer, INPUT_BUFFER_SIZE);
      if (bytes_read <= 0)
        {
          struct pollfd fds[1];
          fds[0].fd = obj->frontend->dvrFd;
          fds[0].events = POLLIN;
          poll (fds, 1, -1);
          continue;
        }

    try_again_write:
      if (ring_buffer_count_free_bytes (&(obj->output_buffer))
          >= (unsigned long) bytes_read)
        {
          pthread_mutex_lock (&(obj->output_mutex));
          addr = ring_buffer_write_address (&(obj->output_buffer));
          memcpy (addr, buffer, bytes_read);
          ring_buffer_write_advance (&(obj->output_buffer), bytes_read);
          pthread_cond_signal (&(obj->output_cond));
          pthread_mutex_unlock (&(obj->output_mutex));
        }
      else
        {
          clog << "Input buffer full, nich gut... " << endl;
          pthread_mutex_lock (&(obj->output_mutex));
          pthread_cond_wait (&(obj->output_cond), &(obj->output_mutex));
          pthread_mutex_unlock (&(obj->output_mutex));
          goto try_again_write;
        }
    }

  return NULL;
}

void
ISDBTProvider::setListener (ITProviderListener *listener)
{
  this->listener = listener;
}

short
ISDBTProvider::getCaps ()
{
  return capabilities;
}

void
ISDBTProvider::attachFilter (IFrontendFilter *filter)
{
  if (frontend != NULL)
    {
      frontend->attachFilter (filter);
    }
}

void
ISDBTProvider::removeFilter (IFrontendFilter *filter)
{
  if (frontend != NULL)
    {
      frontend->removeFilter (filter);
    }
}

bool
ISDBTProvider::scanChannels ()
{
  struct dvb_frontend_parameters params;
  struct dvb_frontend_info info;
  fe_status_t feStatus;

  int feFd;
  int dmFd;
  int dvrFd;

  clog << "ISDBTProvider::scanChannels init.";

  memset (&params, 0, sizeof (dvb_frontend_parameters));

  string file_name = string (g_get_tmp_dir ()) + "/ginga"
                     + SystemCompat::getIUriD () + "channels.txt";

  FILE *fp = fopen (file_name.c_str (), "w");

  int first_pass = 1;
  int progress = 0;

  // Let's assume just UHF channels for now
  for (int channel_counter = 14; channel_counter <= 69; channel_counter++)
    {
      clog << "Channel = " << channel_counter << " Frequency = "
           << (unsigned long long)tv_channels_list[channel_counter] << " "
           << endl;

      params.frequency = tv_channels_list[channel_counter];
      params.inversion = (info.caps & FE_CAN_INVERSION_AUTO)
                             ? INVERSION_AUTO
                             : INVERSION_OFF;
      params.u.ofdm.code_rate_HP = FEC_AUTO;
      params.u.ofdm.code_rate_LP = FEC_AUTO;
      params.u.ofdm.constellation = QAM_AUTO;
      params.u.ofdm.transmission_mode = TRANSMISSION_MODE_AUTO;
      params.u.ofdm.guard_interval = GUARD_INTERVAL_AUTO;
      params.u.ofdm.hierarchy_information = HIERARCHY_NONE;
      params.u.ofdm.bandwidth = BANDWIDTH_6_MHZ;
      progress = (channel_counter - 14) * 100 / 55;
      cout << "cmd::0::tunerscanprogress::" << progress << "%" << endl;

      if ((feFd = open (ISDBTFrontend::IFE_FE_DEV_NAME.c_str (), O_RDWR))
          < 0)
        {
          cout << "cmd::1::tuner::Unable to tune." << endl;
          clog << "ISDBTProvider::scanChannels failed opening FrontEnd DVB "
                  "device."
               << endl;
          return false;
        }

      if (ioctl (feFd, FE_SET_FRONTEND, &params) == -1)
        {
          cout << "cmd::1::tuner::Unable to tune." << endl;
          clog << "ISDBTProvider:: ioctl error with arg FE_SET_FRONTEND"
               << endl;
        }

      if ((dmFd = open (ISDBTFrontend::IFE_DEMUX_DEV_NAME.c_str (), O_RDWR))
          < 0)
        {
          cout << "cmd::1::tuner::Unable to tune." << endl;
          clog << "ISDBTProvider::scanChannels failed opening DeMux DVB "
                  "device."
               << endl;
          return false;
        }

      struct dmx_pes_filter_params filter_dmx;
      filter_dmx.pid = 8192;
      filter_dmx.input = DMX_IN_FRONTEND;
      filter_dmx.output = DMX_OUT_TS_TAP;
      filter_dmx.pes_type = DMX_PES_OTHER;
      filter_dmx.flags = DMX_IMMEDIATE_START;

      if (ioctl (dmFd, DMX_SET_PES_FILTER, &filter_dmx) == -1)
        {
          clog << "ISDBTFrontend::updateIsdbtFrontendParameters: ioctl "
                  "error "
                  "with arg IFE_DEMUX_DEV_NAME"
               << endl;
        }

      // opening DVR device (non-blocking mode), we read TS data in this fd
      if ((dvrFd = open (ISDBTFrontend::IFE_DVR_DEV_NAME.c_str (),
                         O_RDONLY | O_NONBLOCK))
          < 0)
        {
          cout << "cmd::1::tuner::Unable to tune." << endl;
          clog << "ISDBTProvider::scanChannels failed to open DVR DVB "
                  "device. "
               << endl;
          return false;
        }

      int i, value, signal, has_signal_lock = 0;
      clog << "ISDBTProvider::scanChannels Tuning" << endl;

      // retry 2 times...
      for (i = 0; (i < 2) && (has_signal_lock == 0); i++)
        {
          if (ioctl (feFd, FE_READ_STATUS, &feStatus) == -1)
            {
              cout << "cmd::1::tuner::Unable to tune." << endl;
              clog << "ISDBTProvider::scanChannels FE_READ_STATUS failed"
                   << endl;
              return false;
            }

          if (feStatus & FE_HAS_LOCK)
            {
              if (ioctl (feFd, FE_READ_SIGNAL_STRENGTH, &value) == -1)
                {
                  clog << "ISDBTProvider::scanChannels "
                          "FE_READ_SIGNAL_STRENGTH failed"
                       << endl;
                }
              else
                {
                  signal = value * 100 / 65535;
                  cout << "ISDBTProvider::scanChannels Signal locked, "
                          "received power level is "
                       << signal << "%" << endl;
                }
              has_signal_lock = 1;
            }
          usleep (300000);
        }

      if (has_signal_lock)
        {
          clog << "Channel " << channel_counter << " found." << endl;
          cout << "cmd::0::channelfound::"
               << "Phy_Channel_" << channel_counter << endl;
          if (first_pass)
            {
              fprintf (fp, "1;%d;%d;Phy_Channel_%d\n",
                       tv_channels_list[channel_counter] / 1000,
                       channel_counter, channel_counter);
              first_pass = 0;
            }
          else
            {
              fprintf (fp, "0;%d;%d;Phy_Channel_%d\n",
                       tv_channels_list[channel_counter] / 1000,
                       channel_counter, channel_counter);
            }
        }

      ::close (dvrFd);
      ::close (dmFd);
      ::close (feFd);
    }

  cout << "cmd::0::tunerscanprogress::100%" << endl;
  fclose (fp);
  return true;
}

void
ISDBTProvider::initializeChannels ()
{
  ifstream fis;
  string token, id, name, freq, seg;
  bool skipLine = false;
  Channel *channel;

  channels->clear ();

  fis.open (iniFileName.c_str (), ifstream::in);

  if (!fis.is_open ())
    {
      clog << "ISDBTProvider::initializeChannels ";
      clog << "can't open '" << iniFileName << "'" << endl;
    }
  else
    {
      fis >> token;
      while (fis.good ())
        {
          if (token.substr (0, 1) == "#")
            {
              skipLine = true;
            }
          else
            {
              skipLine = false;
            }

          if (token == "id")
            {
              fis >> token;
              id = token;
              fis >> token;
              if (token == "name")
                {
                  fis >> token;
                  name = token;
                  fis >> token;
                  if (token == "frequency")
                    {
                      fis >> token;
                      freq = token;
                      fis >> token;
                      if (token == "segment")
                        {
                          fis >> token;
                          seg = token;

                          if (!skipLine)
                            {
                              channel = new Channel ();
                              channel->setId ((short)(xstrto_int (id)));
                              channel->setName (name);
                              channel->setFrequency ((unsigned int)(xstrto_int (freq)));
                              if (seg == "FULLSEG")
                                {
                                  channel->setSegment (true);
                                }
                              else
                                {
                                  channel->setSegment (false);
                                }

                              channels->push_back (channel);
                            }
                        }
                      else if (!skipLine)
                        {
                          clog << "ISDBTProvider::initializeChannels ";
                          clog << " token segment not found";
                          clog << " current token is '" << token;
                          clog << "'" << endl;
                        }
                    }
                  else if (!skipLine)
                    {
                      clog << "ISDBTProvider::initializeChannels token";
                      clog << " frequency not found";
                      clog << " current token is '" << token;
                      clog << "'" << endl;
                    }
                }
              else if (!skipLine)
                {
                  clog << "ISDBTProvider::initializeChannels token";
                  clog << " name not found";
                  clog << " current token is '" << token;
                  clog << "'" << endl;
                }
            }
          else if (!skipLine)
            {
              clog << "ISDBTProvider::initializeChannels token";
              clog << " id not found";
              clog << " current token is '" << token;
              clog << "'" << endl;
            }
          fis >> token;
        }
      fis.close ();
    }

  if (channels->empty ())
    {
      frontend->scanFrequencies (channels);
    }
}

bool
ISDBTProvider::tune ()
{
  bool tuned = false;
  Channel *channel;

  clog << "ISDBTProvider::tune enter" << endl;

  if (frontend != NULL)
    {
      clog << "ISDBTProvider::tune returning TRUE early" << endl;
      return true;
    }

  if ((feDescriptor
       = open (ISDBTFrontend::IFE_FE_DEV_NAME.c_str (), O_RDWR))
      < 0)
    {
      cout << "cmd::1::tuner::Unable to tune." << endl;
      clog << "ISDBTProvider::tune failed" << endl;
      return false;
    }

  frontend = new ISDBTFrontend (feDescriptor);
  if (frontend->hasFrontend ())
    {
      if (initialFrequency)
        {
          clog << "ISDBTProvider::tune frequency set " << initialFrequency
               << "Hz" << endl;
          tuned = frontend->changeFrequency (initialFrequency * 1000);
          if (!tuned)
            {
              clog << "ISDBTProvider::tune frequency set "
                   << initialFrequency << "Hz failed!" << endl;
            }
        }
      else
        {
          initializeChannels ();
          if (channels->empty ())
            {
              clog << "ISDBTProvider::tune no frequencies found";
              clog << endl;
              return false;
            }
          currentChannel = channels->begin ();
          while (!tuned)
            {
              channel = *currentChannel;
              tuned = frontend->changeFrequency (channel->getFrequency ());
              if (!tuned)
                {
                  ++currentChannel;
                  if (currentChannel == channels->end ())
                    {
                      clog << "ISDBTProvider::tune all frequencies failed";
                      clog << endl;
                      break;
                    }
                }
              else
                {
                  clog << "ISDBTProvider::tune tuned at '";
                  clog << channel->getFrequency () << "' - ";
                  clog << channel->getName () << endl;
                }
            }
        }

      return tuned;
    }
  else
    {
      close ();
      return false;
    }
}

Channel *
ISDBTProvider::getCurrentChannel ()
{
  Channel *channel = NULL;

  if (currentChannel != channels->end ())
    {
      channel = *currentChannel;
    }

  return channel;
}

bool
ISDBTProvider::getSTCValue (uint64_t *stc, int *valueType)
{
  if (frontend == NULL)
    {
      return false;
    }

  return frontend->getSTCValue (stc, valueType);
}

bool
ISDBTProvider::changeChannel (int factor)
{
  int freq = 0;
  Channel *channel;

  if (channels->empty () || frontend == NULL)
    {
      clog << "ISDBTProvider::changeChannel return false: ";
      clog << "number of channels is " << channels->size ();
      clog << " and frontend address is " << frontend << endl;
      return false;
    }

  if (factor == 1)
    {
      ++currentChannel;
      if (currentChannel == channels->end ())
        {
          currentChannel = channels->begin ();
        }
    }
  else if (factor == -1)
    {
      if (currentChannel == channels->begin ())
        {
          currentChannel = channels->end ();
        }
      --currentChannel;
    }

  channel = *currentChannel;
  freq = (*currentChannel)->getFrequency ();

  if (frontend->changeFrequency (freq))
    {
      clog << "ISDBTProvider::changeChannel tuned at '" << freq;
      clog << "' - " << channel->getName () << "" << endl;
      return true;
    }

  return false;
}

bool
ISDBTProvider::setChannel (const string &channelValue)
{
  return frontend->changeFrequency (xstrto_uint (channelValue));
}

int
ISDBTProvider::createPesFilter (int pid, int pesType, bool compositeFiler)
{
  if (frontend != NULL)
    {
      return frontend->createPesFilter (pid, pesType, compositeFiler);
    }

  return -1;
}

string
ISDBTProvider::getPesFilterOutput ()
{
  return ISDBTFrontend::IFE_DVR_DEV_NAME;
}

void
ISDBTProvider::close ()
{
  if (feDescriptor > 0)
    {
      ::close (feDescriptor);
      feDescriptor = 0;
    }

  if (frontend != NULL)
    {
      delete frontend;
      frontend = NULL;
    }
}

char *
ISDBTProvider::receiveData (int *len)
{
  void *addr;
  char *buff = NULL;

  //    clog << "ISDBTProvider::receiveData enter " <<
  //    ring_buffer_count_bytes(&output_buffer) << endl;

  if (ring_buffer_count_bytes (&output_buffer) >= BUFFSIZE)
    {
      pthread_mutex_lock (&output_mutex);
      addr = ring_buffer_read_address (&output_buffer);

      buff = new char[BUFFSIZE];
      memcpy (buff, addr, BUFFSIZE);
      *len = BUFFSIZE;
      ring_buffer_read_advance (&output_buffer, BUFFSIZE);
      pthread_cond_signal (&output_cond);
      pthread_mutex_unlock (&output_mutex);
    }
  else
    {
      pthread_mutex_lock (&output_mutex);
      pthread_cond_wait (&output_cond, &output_mutex);
      pthread_mutex_unlock (&output_mutex);
      *len = 0;
    }

  return buff;
}

GINGA_TUNER_END
