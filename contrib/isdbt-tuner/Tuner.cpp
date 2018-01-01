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

#include "mb/Display.h"
using namespace ::ginga::mb;

GINGA_TUNER_BEGIN

Tuner::Tuner (const string &network, const string &protocol, const string &address)
    : Thread ()
{
  receiving = false;
  currentInterface = -1;
  firstTune = true;
  listener = NULL;
  loopListener = NULL;
  currentSpec = "";

  Ginga_Display->registerKeyEventListener(this);

  interfaces.clear ();

  if (network == "" && protocol == "" && address == "")
    {
      initializeInterfaces ();
    }
  else
    {
      createInterface (network, protocol, address);
    }

  clog << "Tuner::Tuner all done" << endl;
}

Tuner::~Tuner ()
{
  clog << "Tuner::~Tuner" << endl;

//  Ginga_Display->getInputManager ()->removeInputEventListener (this);
  listener = NULL;
  loopListener = NULL;

  clearInterfaces ();
  clog << "Tuner::~Tuner all done" << endl;
}

void
Tuner::setLoopListener (ITunerListener *loopListener)
{
  this->loopListener = loopListener;
}

void
Tuner::keyInputCallback (SDL_EventType evtType, SDL_Keycode key){

  if(evtType == SDL_KEYDOWN)return;


  map<int, NetworkInterface *>::iterator i;

//  clog << "Tuner::keyEventReceived" << endl;
  if (key == SDLK_ESCAPE)
    {
      // CLOSE ALL TUNER INTERFACE/PROVIDER
      this->lock ();
      notifyStatus (TS_TUNER_POWEROFF, NULL);
      i = interfaces.begin ();
      while (i != interfaces.end ())
        {
          i->second->close ();
          ++i;
        }
       this->unlock ();
    }
}

void
Tuner::clearInterfaces ()
{
  map<int, NetworkInterface *>::iterator i;
  NetworkInterface *ni;

  lock ();
  i = interfaces.begin ();
  while (i != interfaces.end ())
    {
      ni = i->second;
      delete ni;
      interfaces.erase (i);
      i = interfaces.begin ();
    }
  interfaces.clear ();
  currentInterface = -1;
  unlock ();
}

void
Tuner::receiveSignal (short signalCode)
{
  switch (signalCode)
    {
    case PST_LOOP:
      if (loopListener != NULL)
        {
          loopListener->updateChannelStatus (TS_LOOP_DETECTED, NULL);
        }
      break;

    default:
      break;
    }
}

void
Tuner::initializeInterface (const string &_niSpec)
{
  string niSpec = _niSpec;
  size_t pos;
  int fo = 0;

  if (niSpec.length () > 3 && niSpec.substr (0, 1) != "#")
    {
      if (niSpec.substr (0, 5) == "file:")
        {
          createInterface ("file", "local",
                           niSpec.substr (5, niSpec.length () - 5));

          return;
        }
      else if (niSpec.find ("ip:") != string::npos)
        {
          niSpec = niSpec.substr (3, niSpec.length () - 3);
        }
      else
        {
          pos = niSpec.find_first_of ("//");
          if (pos != string::npos)
            {
              niSpec
                  = niSpec.substr (pos + 2, niSpec.length () - (pos + 2));
            }
        }

      sscanf (niSpec.c_str (), "%d", &fo);

      if (fo >= 224 && fo <= 239)
        {
          createInterface ("ip", "udp_multicast", niSpec);
        }
      else if ((fo >= 001 && fo <= 223) || (fo >= 240 && fo <= 254))
        {
          createInterface ("ip", "udp_unicast", niSpec);
        }
      else if (niSpec.substr (0, 6) == "isdbt:")
        {
          createInterface ("sbtvd", "terrestrial",
                           niSpec.substr (6, niSpec.length () - 6));
        }
      else if (niSpec.substr (0, 7) == "sbtvdt:")
        {
          createInterface ("sbtvd", "terrestrial",
                           niSpec.substr (7, niSpec.length () - 7));
        }
      else if (niSpec == "isdbt" || niSpec == "sbtvdt")
        {
          createInterface ("sbtvd", "terrestrial", niSpec);
        }
      else
        {
          clog << "Tuner::initializeInterface can't initialize";
          clog << niSpec << "': unknown specification" << endl;
        }
    }
  else if (niSpec.substr (0, 1) != "#")
    {
      clog << "Tuner::initializeInterface can't initialize";
      clog << niSpec << "': invalid specification" << endl;
    }
}

void
Tuner::initializeInterfaces ()
{
  ifstream fis;
  string addr;

  string tunerpath
      = SystemCompat::getGingaBinPath () + "/" + GINGA_TUNER_PATH;

  fis.open (tunerpath.c_str (), ifstream::in);

  if (!fis.is_open ())
    {
      clog << "can't open " << tunerpath;
      clog << endl;
    }
  else
    {
      currentInterface = 0;
      fis >> addr;
      while (fis.good ())
        {
          initializeInterface (addr);
          fis >> addr;
        }
      fis.close ();
    }
}

void
Tuner::createInterface (const string &network, const string &protocol, const string &address)
{
  currentInterface++;
  NetworkInterface *newInterface
      = new NetworkInterface (currentInterface, network, protocol, address);

  lock ();
  interfaces[currentInterface] = newInterface;
  unlock ();
}

bool
Tuner::listenInterface (NetworkInterface *nInterface)
{
  IDataProvider *provider;
  bool tuned = false;

  provider = nInterface->tune ();
  if (provider != NULL)
    {
      tuned = true;
      provider->setListener (this);
    }

  return tuned;
}

void
Tuner::receiveInterface (NetworkInterface *nInterface)
{
  int rval;
  char *buff;

  /*int debugStream = fopen(
                  "debugStream.ts", "w+b");*/

  receiving = true;

  do
    {
      buff = nInterface->receiveData (&rval);
      if (rval > 0 && buff != NULL)
        {
          notifyData (buff, (unsigned int)rval);
        }
    }
  while (receiving);

  // close(debugStream);

  nInterface->close ();
  unlockConditionSatisfied ();
  clog << "Tuner::receive ALL DONE!" << endl;
}

bool
Tuner::hasSignal ()
{
  map<int, NetworkInterface *>::iterator i;
  NetworkInterface *ni;

  ni = getCurrentInterface ();
  if (ni != NULL)
    {
      return ni->hasSignal ();
    }

  return false;
}

void
Tuner::setSpec (const string &niName, const string &ch)
{
  clearInterfaces ();

  currentSpec = niName + ":" + ch;
  initializeInterface (currentSpec);

  clog << "Tuner::setSpec NI = '" << niName << "'";
  clog << ", channel = '" << ch << "' all done!" << endl;
}

void
Tuner::tune ()
{
  if (currentSpec.find ("scan") == std::string::npos)
    {
      startThread ();
    }
  else
    {
      run ();
    }
}

NetworkInterface *
Tuner::getCurrentInterface ()
{
  map<int, NetworkInterface *>::iterator i;
  NetworkInterface *ni;

  lock ();
  i = interfaces.find (currentInterface);
  if (i != interfaces.end ())
    {
      ni = i->second;
      unlock ();
      return ni;
    }
  unlock ();

  return NULL;
}

void
Tuner::channelDown ()
{
  changeChannel (-1);
}

void
Tuner::channelUp ()
{
  changeChannel (1);
}

void
Tuner::changeChannel (int factor)
{
  NetworkInterface *nInterface;
  Channel *channel;

  if (receiving)
    {
      receiving = false;
      clog << "Tuner::changeChannel waiting" << endl;
      waitForUnlockCondition ();
    }

  nInterface = getCurrentInterface ();
  if (nInterface != NULL)
    {
      channel = nInterface->getCurrentChannel ();
      notifyStatus (TS_SWITCHING_CHANNEL, channel);
      if (!nInterface->changeChannel (factor))
        {
          clog << "Tuner::changeChannel can't find channel '";
          clog << factor << "'" << endl;
        }
      else
        {
          channel = nInterface->getCurrentChannel ();
          notifyStatus (TS_NEW_CHANNEL_TUNED, channel);
        }
    }

  clog << "Tuner::changeChannel start new thread" << endl;
  startThread ();
}

void
Tuner::setTunerListener (ITunerListener *listener)
{
  this->listener = listener;
}

void
Tuner::notifyData (char *buff, unsigned int val)
{
  if (listener != NULL)
    {
      listener->receiveData (buff, val);
    }
  else
    {
      delete[] buff;
    }
}

void
Tuner::notifyStatus (short newStatus, Channel *channel)
{
  if (listener != NULL)
    {
      listener->updateChannelStatus (newStatus, channel);
    }
}

void
Tuner::waitForListeners ()
{
  while (listener == NULL)
    g_usleep (10000);
  while (!listener->isReady ())
    g_usleep (10000);
}

void
Tuner::run ()
{
  NetworkInterface *nInterface = NULL, *curInt = NULL;
  map<int, NetworkInterface *>::iterator i;
  bool tuned = false;
  Channel *channel;

  clog << "Tuner::run tuning... " << endl;

  curInt = getCurrentInterface ();
  if (curInt == NULL)
    {
      clog << "Tuner::run current interface not found" << endl;
    }
  else
    {
      tuned = listenInterface (curInt);
      if (tuned)
        {
          nInterface = curInt;
        }
    }

  if (!tuned)
    {
      clog << "Tuner::run current interface can't tune...";
      clog << " trying other interfaces" << endl;

      lock ();
      i = interfaces.begin ();
      while (i != interfaces.end ())
        {
          nInterface = i->second;

          if (curInt != nInterface)
            {
              tuned = listenInterface (nInterface);
              if (tuned)
                {
                  break;
                }
            }

          ++i;
        }
      unlock ();
    }

  if (tuned && firstTune)
    {
      firstTune = false;
      channel = nInterface->getCurrentChannel ();
      notifyStatus (TS_NEW_CHANNEL_TUNED, channel);
    }

  if (tuned && nInterface != NULL
      && !(nInterface->getCaps () & DPC_CAN_DEMUXBYHW))
    {
      clog << "Tuner::run() call receive" << endl;

      waitForListeners ();
      receiveInterface (nInterface);
    }

  if (!tuned && listener != NULL)
    {
      notifyStatus (TS_TUNER_POWEROFF, NULL);
    }
  clog << "Tuner::run done " << endl;
}

GINGA_TUNER_END
