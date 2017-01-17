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

BR_PUCRIO_TELEMIDIA_GINGA_CORE_CONTEXTMANAGER_BEGIN

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
  GingaUser(int userId, string userName, string passwd);
  ~GingaUser();

  int getUserId();
  string getUserName();
  int getUserAge();
  string getUserLocation();
  char getUserGenre();
  bool isValidPassword(string passwd);

  bool setPassword(string oldPasswd, string newPasswd);
  void setUserName(string passwd, string userName);
  void setUserAge(string passwd, int userAge);
  void setUserLocation(string passwd, string userLocation);
  void setUserGenre(string passwd, char userGenre);

  void saveTo(FILE* fd);
  static void saveString(FILE* fd, string bytesToSave);
};

BR_PUCRIO_TELEMIDIA_GINGA_CORE_CONTEXTMANAGER_END

#endif /* GINGA_USER_H*/
