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

#include "ginga.h"
#include "GingaUser.h"

#include "util/functions.h"
using namespace ::ginga::util;

GINGA_CTXMGMT_BEGIN

GingaUser::GingaUser (int id, const string &name, const string &passwd)
{
  this->userId = id;
  this->userName = name;
  this->userPasswd = passwd;
  this->userLocation = "";
  this->userAge = 0;
  this->userGenre = 'm';
}

GingaUser::~GingaUser () {}

int
GingaUser::getUserId ()
{
  return userId;
}

string
GingaUser::getUserName ()
{
  return userName;
}

int
GingaUser::getUserAge ()
{
  return userAge;
}

string
GingaUser::getUserLocation ()
{
  return userLocation;
}

char
GingaUser::getUserGenre ()
{
  return userGenre;
}

bool
GingaUser::isValidPassword (const string &passwd)
{
  return passwd == userPasswd;
}

bool
GingaUser::setPassword (const string &oldPasswd, const string &newPasswd)
{
  if (oldPasswd != userPasswd)
    return false;
  userPasswd = newPasswd;
  return true;
}

void
GingaUser::setUserName (const string &passwd, const string &name)
{
  if (passwd != userPasswd)
    return;
  this->userName = name;
}

void
GingaUser::setUserAge (const string &passwd, int age)
{
  if (passwd != userPasswd)
    return;
  this->userAge = age;
}

void
GingaUser::setUserLocation (const string &passwd, const string &location)
{
  if (passwd != userPasswd)
    return;
  this->userLocation = location;
}

void
GingaUser::setUserGenre (const string &passwd, char genre)
{
  if (passwd != userPasswd)
    return;
  this->userGenre = genre;
}

void
GingaUser::saveTo (FILE *fd)
{
  string genre;

  if (userGenre == 'f')
    genre = "f";
  else
    genre = "m";

  saveString (fd, "|| =");
  saveString (fd, xstrbuild ("%d", userId));
  saveString (fd, userName);
  saveString (fd, userPasswd);
  saveString (fd, xstrbuild ("%d", userAge));
  saveString (fd, userLocation);
  saveString (fd, genre);
}

void
GingaUser::saveString (FILE *fd, const string &bytesToSave)
{
  fwrite (bytesToSave.c_str (), 1, bytesToSave.length (), fd);
  fwrite (" ", 1, 1, fd);
}

GINGA_CTXMGMT_END
