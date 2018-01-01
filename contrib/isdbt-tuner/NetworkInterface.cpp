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

#include "ginga.h"
#include "NetworkInterface.h"

#include "FileSystemProvider.h"
#include "NetworkProvider.h"
#ifdef _MSC_VER
#include "BDAProvider.h"
#endif

#if WITH_LINUXDVB
#include "ISDBTProvider.h"
#endif

GINGA_TUNER_BEGIN

NetworkInterface::NetworkInterface (int id, const string &name, const string &protocol,
                                    const string &addr)
{
  this->provider = NULL;
  this->id = id;
  this->name = name;
  this->protocol = protocol;
  this->address = addr;
  this->tuned = false;
}

NetworkInterface::~NetworkInterface () { close (); }

short
NetworkInterface::getCaps ()
{
  if (provider != NULL)
    {
      return provider->getCaps ();
    }
  else
    {
      clog << "NetworkInterface::getCaps return 0 (NULL provider)";
      clog << endl;
    }

  return 0;
}

int
NetworkInterface::getId ()
{
  return id;
}

string
NetworkInterface::getName ()
{
  return name;
}

string
NetworkInterface::getProtocol ()
{
  return protocol;
}

string
NetworkInterface::getAddress ()
{
  return address;
}

void
NetworkInterface::attachFilter (IFrontendFilter *filter)
{
  if (provider == NULL)
    {
      return;
    }

  provider->attachFilter (filter);
}

void
NetworkInterface::removeFilter (IFrontendFilter *filter)
{
  if (provider == NULL)
    {
      return;
    }

  provider->removeFilter (filter);
}

void
NetworkInterface::setDataProvider (IDataProvider *provider)
{
  this->provider = provider;
}

bool
NetworkInterface::createProvider ()
{
  string ip, portNumber;

  close ();

  if (name == "ip")
    {
      ip = address.substr (0, address.find (":"));
      portNumber
          = address.substr (address.find (":") + 1, address.length ());

      provider = new NetworkProvider (ip, xstrto_int (portNumber), protocol);
      return true;
    }
  else if (name == "file")
    {
      provider = new FileSystemProvider (address);
      return true;
    }
  else if (name == "sbtvd" && protocol == "terrestrial")
    {
      long freq;

      if (address == "scan")
        {
          freq = -1;
        }
      else if (address == "current")
        {
          freq = 0;
        }
      else
        {
          freq = xstrto_int (address);
          if (freq < 1)
            return false;
        }
#if defined _MSC_VER
      provider = new BDAProvider (freq);
#elif WITH_LINUXDVB
      provider = new ISDBTProvider (freq);
#endif

      return true;
    }

  return false;
}

bool
NetworkInterface::hasSignal ()
{
  return tuned;
}

IDataProvider *
NetworkInterface::tune ()
{
  if (provider == NULL)
    {
      if (createProvider ())
        {
          tuned = provider->tune ();
        }
    }
  else
    {
      tuned = provider->tune ();
    }

  if (tuned)
    {
      return provider;
    }

  return NULL;
}

bool
NetworkInterface::changeChannel (int factor)
{
  if (provider != NULL)
    {
      return provider->changeChannel (factor);
    }

  return false;
}

bool
NetworkInterface::setChannel (const string &channelValue)
{
  if (provider != NULL)
    {
      return provider->setChannel (channelValue);
    }

  return false;
}

bool
NetworkInterface::getSTCValue (guint64 *stc, int *valueType)
{
  if (provider != NULL)
    {
      return provider->getSTCValue (stc, valueType);
    }

  return false;
}

int
NetworkInterface::createPesFilter (int pid, int pesType,
                                   bool compositeFiler)
{
  if (provider != NULL)
    {
      return provider->createPesFilter (pid, pesType, compositeFiler);
    }

  return -1;
}

string
NetworkInterface::getPesFilterOutput ()
{
  if (provider == NULL)
    {
      return "";
    }

  return provider->getPesFilterOutput ();
}

Channel *
NetworkInterface::getCurrentChannel ()
{
  return provider->getCurrentChannel ();
}

char *
NetworkInterface::receiveData (int *len)
{
  return provider->receiveData (len);
}

void
NetworkInterface::close ()
{
  if (provider != NULL)
    {
      provider->close ();
      delete provider;
      provider = NULL;
    }
}

GINGA_TUNER_END
