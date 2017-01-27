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

#ifndef GINGA_USER_H
#define GINGA_USER_H

#include "ginga.h"

GINGA_CTXMGMT_BEGIN

class GingaUser
{
private:
  int userId;
  string userName;
  string userPasswd;
  int userAge;
  string userLocation;
  char userGenre;

public:
  GingaUser (int userId, const string &userName, const string &passwd);
  ~GingaUser ();
  int getUserId ();
  string getUserName ();
  int getUserAge ();
  string getUserLocation ();
  char getUserGenre ();
  bool isValidPassword (const string &passwd);
  bool setPassword (const string &oldPasswd, const string &newPasswd);
  void setUserName (const string &passwd, const string &userName);
  void setUserAge (const string &passwd, int userAge);
  void setUserLocation (const string &passwd, const string &userLocation);
  void setUserGenre (const string &passwd, char userGenre);
  void saveTo (FILE *fd);
  static void saveString (FILE *fd, const string &bytesToSave);
};

GINGA_CTXMGMT_END

#endif /* GINGA_USER_H*/
