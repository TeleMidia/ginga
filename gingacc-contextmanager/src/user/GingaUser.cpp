/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "../../include/user/GingaUser.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <fcntl.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace contextmanager {
	GingaUser::GingaUser(int userId, string userName, string passwd) {
		this->userId = userId;
		this->userName = userName;
		this->userPasswd = passwd;
		this->userLocation = "";
		this->userAge = 0;
		this->userGenre = 'm';
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
			userPasswd == newPasswd;
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

	void GingaUser::saveTo(int fd) {
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

	void GingaUser::saveString(int fd, string bytesToSave) {
		write(fd, bytesToSave.c_str(), bytesToSave.length());
		write(fd, " ", 1);
	}
}
}
}
}
}
}
