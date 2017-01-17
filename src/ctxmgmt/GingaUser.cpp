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

#include "config.h"
#include <stdio.h>

#include "GingaUser.h"
#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

BR_PUCRIO_TELEMIDIA_GINGA_CORE_CONTEXTMANAGER_BEGIN

	GingaUser::GingaUser(int userId, string userName, string passwd) {
		this->userId       = userId;
		this->userName     = userName;
		this->userPasswd   = passwd;
		this->userLocation = "";
		this->userAge      = 0;
		this->userGenre    = 'm';
	}

	GingaUser::~GingaUser() {

	}

	int GingaUser::getUserId() {
		return userId;
	}

	string GingaUser::getUserName() {
		return userName;
	}

	int GingaUser::getUserAge() {
		return userAge;
	}

	string GingaUser::getUserLocation() {
		return userLocation;
	}

	char GingaUser::getUserGenre() {
		return userGenre;
	}

	bool GingaUser::isValidPassword(string passwd) {
		return passwd == userPasswd;
	}

	bool GingaUser::setPassword(string oldPasswd, string newPasswd) {
		if (oldPasswd == userPasswd) {
			userPasswd = newPasswd;
			return true;
		}
		return false;
	}

	void GingaUser::setUserName(string passwd, string userName) {
		if (passwd == userPasswd) {
			this->userName = userName;
		}
	}

	void GingaUser::setUserAge(string passwd, int userAge) {
		if (passwd == userPasswd) {
			this->userAge = userAge;
		}
	}

	void GingaUser::setUserLocation(string passwd, string userLocation) {
		if (passwd == userPasswd) {
			this->userLocation = userLocation;
		}
	}

	void GingaUser::setUserGenre(string passwd, char userGenre) {
		if (passwd == userPasswd) {
			this->userGenre = userGenre;
		}
	}

	void GingaUser::saveTo(FILE* fd) {
		string genre;

		if (userGenre == 'f') {
			genre = "f";
		} else {
			genre = "m";
		}

		saveString(fd, "|| =");
		saveString(fd, itos(userId));
		saveString(fd, userName);
		saveString(fd, userPasswd);
		saveString(fd, itos(userAge));
		saveString(fd, userLocation);
		saveString(fd, genre);
	}

	void GingaUser::saveString(FILE* fd, string bytesToSave) {
		fwrite(bytesToSave.c_str(), 1, bytesToSave.length(), fd);
		fwrite(" ", 1, 1, fd);
	}

BR_PUCRIO_TELEMIDIA_GINGA_CORE_CONTEXTMANAGER_END
